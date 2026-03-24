#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:26429)
#pragma warning(disable:26430)
#pragma warning(disable:26462)
#pragma warning(disable:26800)
#endif

namespace iron::unsafe::btree {

	template <typename _KEY,typename _MAPPED,typename _LT,std::size_t _ORDER,typename _A,bool _MULTI,bool _HAS_MAPPED>
	class raw_table final {
	public:
		using key_t = _KEY;
		using mapped_t = _MAPPED;
		using allocator_t = _A;
		using key_less_t = _LT;
		using size_t = std::size_t;
		static constexpr size_t order=_ORDER;
		static_assert(order > 1, "raw btree table require order > 1");
		static constexpr bool is_multi = _MULTI;
		static constexpr bool has_mapped = _HAS_MAPPED;
		using index_t=iron::unsafe::btree::index<key_t,mapped_t,key_less_t,order,is_multi,has_mapped>;
		using value_t = typename index_t::value_t;
		using exported_value_t = typename index_t::exported_value_t;
		static constexpr size_t min_data_size = index_t::min_data_size;
		static constexpr size_t max_data_size = index_t::max_data_size;
		static constexpr size_t max_child_size = index_t::max_child_size;
		using ptr_t = value_t*;
		using pointer = ptr_t;
		using cptr_t = const value_t*;
		using const_pointer = cptr_t;
		using diff_t = std::ptrdiff_t;
		using difference_type = diff_t;
		using error_handler_t = iron::error_handler;
		using export_allocator_t=typename std::allocator_traits<allocator_t>::template rebind_alloc<exported_value_t>;
		using adapter_box_t = iron::unsafe::raw_vector<exported_value_t, export_allocator_t>;
		using tree_node_t = typename index_t::tree_node_t;
		using rollback_t = iron::unsafe::raw_vector<index_t, allocator_t>;
		using data_node_t = typename index_t::data_node_t;
		using data_iter_t = typename index_t::data_iter_t;
		using iter_value_t = std::conditional_t<has_mapped, iron::proxy_pair<const key_t, mapped_t>, const key_t>;
		using iter_t = iron::unsafe::iterators::btree_iterator<iter_value_t,index_t>;
		using const_iter_value_t = std::conditional_t<has_mapped, iron::proxy_pair<const key_t,const mapped_t>, const key_t>;
		using const_iter_t = iron::unsafe::iterators::btree_iterator<const_iter_value_t,index_t>;
		using riter_t = std::reverse_iterator<iter_t>;
		using const_riter_t = std::reverse_iterator<const_iter_t>;
		using common_t = iron::unsafe::btree::common<index_t>;
		struct raw_table_tag { raw_table_tag() = delete; };
		[[nodiscard]] static constexpr size_t max_size()noexcept { return iron::unsafe::raw_storage<value_t>::max_size(); }
		constexpr raw_table()noexcept
		:_m_root_node()
		,_m_sz()
		,_m_a()
		{}
		constexpr explicit raw_table(allocator_t _a)noexcept
			:_m_root_node()
			, _m_sz()
			, _m_a(_a)
		{}
		constexpr raw_table(raw_table&& _v)noexcept
			:_m_root_node()
			, _m_sz()
			, _m_a()
		{
			_swap(_v);
		}
		constexpr raw_table(const raw_table&) = delete;
		constexpr raw_table& operator=(raw_table&& _v)noexcept {
			_swap(_v);
			return *this;
		}

		constexpr raw_table& operator=(const raw_table&) = delete;
		constexpr ~raw_table() {
			_reset();
		}
		constexpr void swap(raw_table& _v)noexcept {
			_swap(_v);
		}

		[[nodiscard]] constexpr result<raw_table, error_handler_t> try_clone(size_t _n=1)const noexcept {
			raw_table _t;
			if (_n) IRON_LIKELY 
			{
				if constexpr (!is_multi) {
					_n = 1;
				}
				rollback_t _rollback;
				while (_n--) IRON_LIKELY 
				{
					constexpr bool _transactional{};
					constexpr bool _move{};
					auto _b = begin();
					auto _r = _t.template insert_range<_transactional,_move>(_b, end(),_rollback); RR(_r);
				}
			}
			return iron::ok(std::move(_t));
		}
		[[nodiscard]] constexpr raw_table clone(size_t _n=1)const noexcept {
			auto _r = try_clone(_n); RP(_r);
		}
		[[nodiscard]] constexpr size_t size()const noexcept {
			return _m_sz;
		}
		[[nodiscard]] constexpr bool empty()const noexcept { return !size(); }

		[[nodiscard]] constexpr explicit operator bool()const noexcept {
			return _m_sz ? true : false;
		}
		[[nodiscard]] constexpr allocator_t allocator()const noexcept {
			return 	_m_a;
		}
		constexpr void reset()noexcept {
			_reset();
		}
		[[nodiscard]] constexpr iter_t find(const key_t& _k)noexcept {
			if (_m_root_node) IRON_LIKELY 
			{
				const auto _index=common_t::recursive_search(*_m_root_node, _k);
				if (_index.data_node) IRON_LIKELY
				{
					return iter_t{_index};
				}
			}
			return end();
		}
		[[nodiscard]] constexpr const_iter_t find(const key_t& _k)const noexcept {
			if (_m_root_node) IRON_LIKELY
			{
				const auto _index = common_t::recursive_search(*_m_root_node, _k);
				if (_index.data_node) IRON_LIKELY
				{
					return const_iter_t{ _index };
				}
			}
			return end();
		}
		template <typename _K, typename _M>
		[[nodiscard]] constexpr iron::result<std::pair<iter_t, bool>,error_handler_t> insert(_K&& _k, _M&& _m)noexcept {
			if (!_m_root_node) IRON_UNLIKELY
			{
				DEBUG_ASSERT(!_m_sz);
				auto _h = tree_node_t::from(_m_root_node, _m_a); RR(_h);
				return _insert_unique_key(_m_root_node,_m_root_node->data_end(), std::forward<_K>(_k), std::forward<_M>(_m));
			}
			if (_m_root_node->data_full()) IRON_LIKELY 
			{
				tree_node_t* _pnode{};
				auto _h = tree_node_t::from(_pnode, _m_a); RR(_h);
				const auto _iter=_pnode->child_push_back(_m_root_node);
				_h = common_t::split_child(
					*_pnode
					, _iter
					, _m_a
				);
				if (!_h) IRON_UNLIKELY
				{
					_m_root_node = _pnode->child_pop_back(); //roolback
					tree_node_t::destroy(_pnode,_m_a);
					tree_node_t::deallocate(_pnode, _m_a);
					return iron::err(std::move(_h));
				}
				_m_root_node = _pnode;
			}
			auto _pnode = _m_root_node;
			while (true) {
				const key_t& _key = _k;
				auto _data_iter = _pnode->data_begin();
				if (common_t::search(_data_iter, _pnode->data_end(), _key)) IRON_UNLIKELY  //key already exists
				{
					if constexpr (is_multi) { //insert the key
						return _insert_multi_key(_pnode, _data_iter, std::forward<_K>(_k), std::forward<_M>(_m));
					}
					else {
						return iron::ok(_inserted(_pnode, _data_iter,*_data_iter, false));
					}
				}
				if (_pnode->child_empty()) IRON_LIKELY // simple insertion into leaf
				{
					return _insert_unique_key(_pnode, _data_iter, std::forward<_K>(_k), std::forward<_M>(_m));
				}
				auto _child_iter = _pnode->child_begin() + (_data_iter - _pnode->data_begin());
				auto _pch = *_child_iter;
				DEBUG_ASSERT(_pch);
				if (_pch->data_full()) IRON_UNLIKELY  
				{
					auto _h=common_t::split_child(
						*_pnode
						, _child_iter
						, _m_a
					); RR(_h);
					auto& _middle_key = common_t::get_key((*_data_iter)->value());
					switch (const auto _c = common_t::compare(_middle_key, _key)) {
						using t_t = decltype(_c);
						case t_t::lt: {
							_pch = *(_child_iter + 1);
							DEBUG_ASSERT(_pch);
							break;
						}
						case t_t::gt: {
							break;
						}
						case t_t::eq: { //key already exists
							if constexpr (is_multi) { //insert
								return _insert_multi_key(_pnode, _data_iter, std::forward<_K>(_k), std::forward<_M>(_m));
							}
							else {
								return iron::ok(_inserted(_pnode, _data_iter, *_data_iter, false));
							}
						}
					}
				}
				_pnode = _pch;
			} //while
			DEBUG_ASSERT(0);
		}
		[[nodiscard]] constexpr iron::result<std::pair<iter_t, bool>, error_handler_t> insert(exported_value_t&& _value)noexcept {
			if constexpr (has_mapped) {
				return insert(std::move(_value.first), std::move(_value.second));
			}
			else {
				return insert(std::move(_value), mapped_t{});
			}
		}
		template <bool _TRANSACTIONAL,bool _MOVE,typename _ITER>
		[[nodiscard]] constexpr iron::result<size_t, error_handler_t> insert_range(_ITER& _b, const _ITER& _e, rollback_t& _roolback)noexcept {
			return _insert_range<_TRANSACTIONAL,_MOVE>(_b, _e, _roolback);
		}
		[[nodiscard]] constexpr  data_node_t* detach(iter_t& _iter)noexcept {
			if (_m_sz) IRON_LIKELY
			{
				const auto & _index = common_t::index(_iter);
				auto _data_node = common_t::detach_data(_index, _m_a);
				DEBUG_ASSERT(_data_node);
				const auto _c = common_t::count_data(_data_node);
				DEBUG_ASSERT(_c <= _m_sz);
				_m_sz -= _c;
				_destroy_if();
				return _data_node;
			}
			return {};
		}

		template <typename _K, typename... _ARGS >
		constexpr size_t erase_keys(const _K& _key, const _ARGS&... _args)noexcept
		{
			const auto _sz = _m_sz;
			_erase_keys(_key, _args...);
			_destroy_if();
			DEBUG_ASSERT(_sz >= _m_sz);
			return _sz - _m_sz;
		}

		template <typename _ITER>
		constexpr size_t erase(_ITER _iter)noexcept
		{
			DEBUG_ASSERT(_iter != end());
			return _erase(_iter);
		}

		template <typename... _ARGS >
		[[nodiscard]] constexpr iron::result<adapter_box_t, error_handler_t> release_list(const _ARGS&... _keys)noexcept
		requires(sizeof...(_ARGS) > 0)
		{
			return _release(_keys...);
		}
		[[nodiscard]] constexpr iron::result<adapter_box_t, error_handler_t> release_all()noexcept
		{
			return _release_all();
		}
		[[nodiscard]] constexpr const tree_node_t* data()const noexcept {
			return _m_root_node;
		}
		[[nodiscard]] constexpr tree_node_t* data()noexcept {
			return _m_root_node;
		}
		constexpr void rollback(rollback_t& _rb)noexcept {
			_rollback(_rb);
		}
		constexpr iter_t begin()noexcept {
			if (_m_root_node) IRON_LIKELY
			{
				DEBUG_ASSERT(_m_sz);
				auto _start_node = common_t::first_tree_node(_m_root_node);
				DEBUG_ASSERT(_start_node && _start_node->data_size());
				auto _b = _start_node->data_begin();
				return iter_t(index_t{_start_node,_b,*_b});
			}
			else {
				return iter_t(index_t{});
			}
		}
		constexpr iter_t end()noexcept {
			if (_m_root_node) IRON_LIKELY 
			{
				return iter_t(index_t{_m_root_node,_m_root_node->data_end(),nullptr});
			}
			else {
				return iter_t(index_t{});
			}
		}
		constexpr const_iter_t begin()const noexcept {
			if (_m_root_node) IRON_LIKELY
			{
				DEBUG_ASSERT(_m_sz);
				auto _start_node = common_t::first_tree_node(_m_root_node);
				DEBUG_ASSERT(_start_node && _start_node->data_size());
				auto _b = _start_node->data_begin();
				return const_iter_t(index_t{_start_node,_b,*_b});
			}
			else {
				return const_iter_t(index_t{});
			}
		}
		constexpr const_iter_t end()const noexcept {
			if (_m_root_node) IRON_LIKELY
			{
				return const_iter_t(index_t{_m_root_node,_m_root_node->data_end(),nullptr});
			}
			else {
				return const_iter_t(index_t{});
			}
		}

		constexpr riter_t rbegin()noexcept {
			return riter_t(end());
		}
		constexpr riter_t rend()noexcept {
			return riter_t(begin());
		}
		constexpr const_riter_t rbegin()const noexcept {
			return const_riter_t(end());
		}
		constexpr const_riter_t rend()const noexcept {
			return const_riter_t(begin());
		}

		template <bool _TRANSACTIONAL>
		[[nodiscard]] constexpr iron::result<size_t,error_handler_t> merge(raw_table& _ht,rollback_t& _rollback)noexcept {
			DEBUG_ASSERT(this != &_ht && _ht.size() && this->size());
			return _merge<_TRANSACTIONAL>(_ht, _rollback);
		}
		template <typename _KEY1, typename _MAPPED1, typename _LT1, std::size_t _ORDER1, typename _A1, bool _MULTI1, bool _HAS_MAPPED1>
		[[nodiscard]] constexpr bool operator ==(const raw_table<_KEY1, _MAPPED1, _LT1, _ORDER1, _A1, _MULTI1, _HAS_MAPPED1>& _r)const noexcept {
			return size() == _r.size() && common_t::eq(begin(), end(), _r.begin());
		}

		template <typename _KEY1, typename _MAPPED1, typename _LT1, std::size_t _ORDER1, typename _A1, bool _MULTI1, bool _HAS_MAPPED1>
		[[nodiscard]] constexpr std::strong_ordering operator <=>(const raw_table<_KEY1, _MAPPED1, _LT1, _ORDER1, _A1, _MULTI1, _HAS_MAPPED1>& _r)const noexcept {
			switch (const auto _t = common_t::compare_seq(begin(), end(), _r.begin(), _r.end())) {
				using _t_t = decltype(_t);
				using _s_t = std::strong_ordering;
				case _t_t::eq: return _s_t::equal;
				case _t_t::lt: return _s_t::less;
				case _t_t::gt: return _s_t::greater;
			}
			PANIC("internal error");
		}

	private:
		constexpr void _swap(raw_table& _v)noexcept {
			std::swap(_m_root_node, _v._m_root_node);
			std::swap(_m_sz, _v._m_sz);
			_m_a.swap(_v._m_a);
		}
		[[nodiscard]] static constexpr std::pair<iter_t,bool>  _inserted(tree_node_t* _node,data_iter_t _iter,data_node_t* _listnode,bool _inserted)noexcept {
			DEBUG_ASSERT(_node && _iter && _listnode);
			return std::make_pair(iter_t(index_t{ _node,_iter,_listnode }), _inserted);
		}
		template <typename _K, typename _M>
		[[nodiscard]] constexpr iron::result<std::pair<iter_t,bool>, error_handler_t> _insert_unique_key(tree_node_t* _tree_node,data_iter_t _data_iter,_K&& _k, _M&& _m)noexcept {
			DEBUG_ASSERT(_tree_node && _data_iter);
			data_node_t* _data_node{};
			auto _h = tree_node_t::make_data(_data_node, _m_a, std::forward<_K>(_k), std::forward<_M>(_m)); RR(_h);
			auto _insert_iter = _tree_node->data_insert(_data_iter, _data_node);
			++_m_sz;
			auto _head = *_insert_iter;
			DEBUG_ASSERT(_head);
			return iron::ok(_inserted(_tree_node, _data_iter,_head, true));
		}
		template <typename _K,typename _M>
		[[nodiscard]] constexpr iron::result<std::pair<iter_t, bool>, error_handler_t> _insert_multi_key(tree_node_t* _tree_node, data_iter_t _data_iter,_K&& _k, _M&& _m)noexcept
		requires(is_multi)
		{
			DEBUG_ASSERT(_tree_node && _data_iter);
			data_node_t* _data_node{};
			auto _h = tree_node_t::make_data(_data_node, _m_a, std::forward<_K>(_k), std::forward<_M>(_m)); RR(_h);
			auto _head = *_data_iter;
			DEBUG_ASSERT(_head);
			while (_head) IRON_LIKELY 
			{
				auto& _next = _head->next();
				if (!_next) IRON_UNLIKELY 
				{
					_next = _data_node;
					break;
				}
				_head = _next;
			}
			++_m_sz;
			return iron::ok(_inserted(_tree_node, _data_iter, _data_node,true));
		}
		static constexpr void _reset(tree_node_t* _p,allocator_t& _a)noexcept {
			DEBUG_ASSERT(_p);
			while(_p->child_size()) IRON_LIKELY 
			{
				auto _ch = _p->child_pop_back();
				_reset(_ch,_a);
			}
			tree_node_t::destroy(_p,_a);
			tree_node_t::deallocate(_p, _a);
		}
		constexpr void _reset()noexcept {
			if (_m_root_node) IRON_LIKELY
			{
				_reset(_m_root_node,_m_a);
				_m_root_node = {};
				_m_sz = {};
			}
			else {
				DEBUG_ASSERT(!_m_sz);
			}
		}
		constexpr void _release_key(adapter_box_t& _box, const key_t& _k)noexcept {
			DEBUG_ASSERT(_m_root_node);
			const auto _index=common_t::recursive_search(*_m_root_node,_k);
			if (_index.tree_node) IRON_LIKELY 
			{
				auto _list = common_t::detach_data(_index, _m_a);
				const auto _c = _release_list(_box,_list);
				DEBUG_ASSERT(_c <= _m_sz);
				_m_sz -= _c;
			}
		}

		template <typename _K, typename... _ARGS>
		constexpr void _release_keys(adapter_box_t& _box, const _K& _k, const _ARGS&... _args)noexcept {
			DEBUG_ASSERT(_m_sz);
			const key_t& _key = _k;
			_release_key(_box, _key);
			if constexpr (sizeof...(_ARGS) != 0) {
				if (_m_sz) IRON_LIKELY {
					_release_keys(_box, _args...);
				}
			}
		}

		template <typename... _ARGS >
		[[nodiscard]] constexpr iron::result<adapter_box_t, error_handler_t> _release(const _ARGS&... _args)noexcept
		{
			static_assert(sizeof...(_ARGS) != 0, "internal error");
			adapter_box_t _a(_m_a);
			if (_m_sz) IRON_LIKELY 
			{
				if constexpr (is_multi) {
					auto _h = _a.reserve(_m_sz); RR(_h);
				}
				else {
					auto _h = _a.reserve(sizeof...(_ARGS)); RR(_h);
				}
				_release_keys(_a, _args...);
				_destroy_if();
			}
			return iron::ok(std::move(_a));
		}

		[[nodiscard]] constexpr iron::result<adapter_box_t, error_handler_t> _release_all()noexcept {
			adapter_box_t _t(_m_a);
			if (_m_sz) IRON_LIKELY
			{
				auto _h = _t.reserve(_m_sz); RR(_h);
				auto _b = begin();
				auto _index = common_t::index(_b);
				while (_index.data_node) IRON_LIKELY
				{
					_h = _t.push_back(common_t::move(_index.data_node->value()));
					DEBUG_ASSERT(_h);
					common_t::next(_index);
				}
				_reset();
			}
			return iron::ok(std::move(_t));
		}
		constexpr void _rollback(rollback_t& _rb)noexcept {
			auto _b = _rb.data();
			const auto _e = _rb.data() + _rb.size();
			DEBUG_ASSERT(_m_sz >= _rb.size());
			while (_b != _e) IRON_LIKELY
			{
				auto& _index = *_b++;
				auto _data_node = common_t::detach_data_single(_index,_m_a);
				DEBUG_ASSERT(_data_node && !_data_node->next());
				data_node_t::destroy(_data_node);
				data_node_t::deallocate(_data_node, _m_a);
			}
			_m_sz -= _rb.size();
			if (!_m_sz) IRON_UNLIKELY
			{
				_reset();
			}
		}
		template <bool _TRANSACTIONAL,bool _MOVE,typename _ITER>
		[[nodiscard]] constexpr iron::result<size_t, error_handler_t> _insert_range(_ITER& _b, const _ITER& _e, rollback_t& _rb)noexcept {
			size_t _count{};
			std::pair<iter_t, bool> _pair{};
			while (_b != _e) IRON_LIKELY
			{
				if constexpr (_MOVE) {
					DEBUG_ASSERT(!_TRANSACTIONAL);
					auto _r = insert(std::move(*_b)); RR(_r);
					_pair.swap(_r.unchecked_unwrap());
				}
				else if constexpr (std::is_same_v<std::decay_t<decltype(*std::declval<_ITER>())>,value_t>) {
					auto _r = common_t::clone(*_b); RR(_r);
					auto _r1 = insert(std::move(_r.unchecked_unwrap())); RR(_r);
					_pair.swap(_r1.unchecked_unwrap());
				}
				else if constexpr (iron::concepts::proxy_pair<decltype(*_b)>) {
					auto _r = common_t::clone(*_b); RR(_r);
					auto _r1 = insert(std::move(_r.unchecked_unwrap())); RR(_r1);
					_pair.swap(_r1.unchecked_unwrap());
				}
				else {
					PANIC("internal error");
				}
				[[maybe_unused]] auto& [_iter, _inserted] = _pair;
				if (_inserted) IRON_LIKELY 
				{
					if constexpr (_TRANSACTIONAL) {
						[[maybe_unused]] const auto _h = _rb.push_back(common_t::index(_iter));
						DEBUG_ASSERT(_h); //_rb must be preallocate
#if 0 //is only for test rollback
						if (common_t::get_key(_rb.back()->data_node->value()) == key_t{40}) {
							return iron::err(ERROR_HANDLER(1000));
						}
#endif 
					}
					++_count;
				}
				++_b;
			} //while
			return iron::ok(_count);
		}
		template <bool _TRANSACTIONAL>
		[[nodiscard]] constexpr iron::result<size_t, error_handler_t> _merge(raw_table& _ht, rollback_t& _rollback)noexcept {
			auto _b = _ht.begin();
			auto _e = _ht.end();
			size_t _count{};
			while (_b != _e) IRON_LIKELY 
			{
				if constexpr (!is_multi) {
					DEBUG_ASSERT(_m_root_node);
					if (common_t::recursive_search(*_m_root_node, common_t::get_key(*_b)).data_node) IRON_UNLIKELY //key already exists
					{
						++_b;
						continue;
					}
				}
				if constexpr (_TRANSACTIONAL) {
					PANIC("todo");

//					auto _index = common_t::index(_b);
//					const auto _h = _rollback.push_back(std::make_pair(index_t{},_index)); //store source
//					DEBUG_ASSERT(_h); //must be preallocate
				}
				else {
					auto _next_iter = _b;
					if constexpr (is_multi) { //skip the multi key 
						const auto& _k=common_t::get_key(*_next_iter);
						while (_next_iter != _e && common_t::compare(common_t::get_key(*_next_iter),_k) == common_t::compare_t::eq) IRON_LIKELY 
						{
							++_next_iter;
						}
					}
					else {
						++_next_iter;
					}
					const key_t* _next_key= _next_iter == _e ? nullptr : &(common_t::get_key(*_next_iter));
					auto _data_node = _ht.detach(_b);
					while (_data_node) IRON_LIKELY 
					{
						auto _r = insert(common_t::move(_data_node->value())); RR(_r);
						DEBUG_ASSERT(_r.unchecked_unwrap().second); //check inserted
						++_count;
						auto _next = _data_node->next();
#ifndef NDEBUG
						_data_node->reset_links();
#endif 
						data_node_t::destroy(_data_node);
						data_node_t::deallocate(_data_node, _ht.allocator());
						_data_node = _next;
					}
					_e = _ht.end(); //_e can be changed
					_b = _next_key ? _ht.find(*_next_key) : _e; //not use next_iter - can be not valid
					DEBUG_ASSERT(!_next_key || _b != _e);
				}
			}
			return iron::ok(_count);
		}

		constexpr void _erase_key(const key_t& _k)noexcept {
			DEBUG_ASSERT(_m_root_node);
			const auto _index = common_t::recursive_search(*_m_root_node, _k);
			if (_index.tree_node) IRON_LIKELY
			{
				auto _list = common_t::detach_data(_index, _m_a);
				const auto _c = _erase_list(_list);
				DEBUG_ASSERT(_c <= _m_sz);
				_m_sz -= _c;
			}
		}
		template <typename _K, typename... _ARGS>
		constexpr void _erase_keys(const _K& _k, const _ARGS&... _args)noexcept {
			if (_m_sz) IRON_LIKELY 
			{
				const key_t& _key = _k;
				_erase_key(_key);
				if constexpr (sizeof...(_ARGS) != 0) {
					if (_m_sz) IRON_LIKELY 
					{
						_erase_keys(_args...);
					}
				}
			}
		}
		template <typename _ITER>
		[[nodiscard]] constexpr size_t _erase(_ITER _iter)noexcept {
			DEBUG_ASSERT(_iter != end());
			auto _data_list = detach(_iter);
			return _erase_list(_data_list);
		}

		[[nodiscard]] constexpr size_t _release_list(adapter_box_t& _box, data_node_t* _data_list)noexcept {
			size_t _c{};
			while (_data_list) IRON_LIKELY 
			{
				[[maybe_unused]] const auto _h = _box.push_back(common_t::move(_data_list->value())); //must be preallocate
				DEBUG_ASSERT(_h);
				auto _next = _data_list->next();
#ifndef NDEBUG
				_data_list->reset_links();
#endif 
				data_node_t::destroy(_data_list);
				data_node_t::deallocate(_data_list, _m_a);
				++_c;
				_data_list = _next;
			}
			return _c;
		}
		[[nodiscard]] constexpr size_t _erase_list(data_node_t* _data_list)noexcept {
			size_t _c{};
			while (_data_list) IRON_LIKELY 
			{
				auto _next = _data_list->next();
#ifndef NDEBUG
				_data_list->reset_links();
#endif 
				data_node_t::destroy(_data_list);
				data_node_t::deallocate(_data_list,_m_a);
				++_c;
				_data_list = _next;
			}
			return _c;
		}
		constexpr void _destroy_if()noexcept {
			if (!_m_sz)  IRON_UNLIKELY
			{
				DEBUG_ASSERT(_m_root_node && _m_root_node->empty());
				tree_node_t::destroy(_m_root_node,_m_a);
				tree_node_t::deallocate(_m_root_node, _m_a);
				_m_root_node = {};
			}
		}
		tree_node_t* _m_root_node;
		size_t _m_sz;
		IRON_NO_UNIQUE_ADDRESS allocator_t _m_a;
	};

} //ns

#if defined(_MSC_VER)
#pragma warning(pop)
#endif


