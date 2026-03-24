
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable:26429)
#pragma warning (disable:26467)
#pragma warning (disable:26481)
#pragma warning (disable:26492)
#pragma warning (disable:26494)
#pragma warning (disable:26800)
#endif

namespace iron::unsafe {

	template <typename _KEY,typename _MAPPED,typename _EQ,typename _HASHER,typename _A,bool _MULTI,bool _HAS_MAPPED>
	class raw_hash_table final {
	public:
		using key_t = _KEY;
		using mapped_t = _MAPPED;
		using allocator_t = _A;
		using key_equal_t = _EQ;
		using hasher_t = _HASHER;
		static constexpr bool is_multi = _MULTI;
		static constexpr bool has_mapped = _HAS_MAPPED;
		using value_t = std::conditional_t<has_mapped,std::pair<key_t, mapped_t>,key_t>;
		using exported_value_t = value_t; // std::conditional_t<has_mapped, std::pair<key_t, mapped_t>, key_t>;
		using size_t = std::size_t;
		using ptr_t = value_t*;
		using pointer = ptr_t;
		using cptr_t = const value_t*;
		using const_pointer = cptr_t;
		using diff_t = std::ptrdiff_t;
		using difference_type = diff_t;
		using reference = value_t&;
		using const_reference = const value_t&;
		using error_handler_t = iron::error_handler;
		using load_factor_t = float;
		using node_value_t = std::conditional_t<has_mapped, std::pair<key_t, mapped_t>, key_t>;
		using node_t = iron::unsafe::list_node<node_value_t, false>;
		using list_t = iron::unsafe::raw_list<node_t, allocator_t>;
		using buckets_t = iron::unsafe::raw_vector<list_t, allocator_t>;
		using export_allocator_t=typename std::allocator_traits<allocator_t>::template rebind_alloc<exported_value_t>;
		using adapter_box_t = iron::unsafe::raw_vector<exported_value_t, export_allocator_t>;
		using nodes_t = iron::unsafe::raw_vector<std::pair<list_t*,node_t*>,allocator_t>;
		struct raw_hash_table_tag { raw_hash_table_tag() = delete; };
		using iter_value_t = std::conditional_t<has_mapped, iron::proxy_pair<const key_t, mapped_t>, const key_t>;
		using iter_t = iron::unsafe::iterators::hash_iterator<iter_value_t,list_t>;
		using const_iter_value_t = std::conditional_t<has_mapped, iron::proxy_pair<const key_t, const mapped_t>, const key_t>;
		using citer_t = iron::unsafe::iterators::hash_iterator<const_iter_value_t, const list_t>;
		[[nodiscard]] static constexpr size_t max_size()noexcept { return iron::unsafe::raw_storage<node_t>::max_size(); }
		[[nodiscard]] static constexpr size_t max_bucket_count()noexcept { return iron::unsafe::raw_storage<buckets_t>::max_size(); }
		[[nodiscard]] static constexpr load_factor_t max_load_factor()noexcept { return static_cast<load_factor_t>(max_size() / 100); }
		constexpr raw_hash_table()noexcept
			: raw_hash_table(0.0F,allocator_t{})
		{}
		constexpr explicit raw_hash_table(load_factor_t _lf,allocator_t _a)noexcept
			: _m_lf(_lf)
			, _m_sz()
			, _m_bs(_a)
		{}
		raw_hash_table(const raw_hash_table&) = delete;
		raw_hash_table& operator=(const raw_hash_table&) = delete;
		constexpr raw_hash_table(raw_hash_table&& _v)noexcept
		: _m_lf()
		, _m_sz()
		, _m_bs(_v._m_bs.allocator())
		{
			_swap(_v);
		}
		constexpr raw_hash_table& operator=(raw_hash_table&& _v)noexcept {
			_swap(_v);
			return *this;
		}
		constexpr ~raw_hash_table() = default;

		constexpr void load_factor_threshold(load_factor_t _lf)noexcept {
			_m_lf = _lf;
		}
		[[nodiscard]] constexpr load_factor_t load_factor_threshold()const noexcept {
			return _m_lf;
		}

		[[nodiscard]] constexpr load_factor_t load_factor()const noexcept {
			return _load_factor(_m_sz, _m_bs.size());
		}
		[[nodiscard]] constexpr bool empty()const noexcept {
			return !_m_sz;
		}
		[[nodiscard]] constexpr explicit operator bool()const noexcept {
			return _m_sz ? true : false;
		}
		[[nodiscard]] constexpr size_t size()const noexcept {
			return _m_sz;
		}
		[[nodiscard]] constexpr size_t bucket_count()const noexcept {
			return _m_bs.size();
		}
		constexpr void swap(raw_hash_table& _v)noexcept {
			_swap(_v);
		}
		[[nodiscard]] constexpr iron::result<raw_hash_table, error_handler_t> try_clone()const noexcept
		{
			return _clone();
		}
		[[nodiscard]] constexpr raw_hash_table clone()const noexcept
		{
			auto _r = try_clone(); RP(_r);
		}
		[[nodiscard]] constexpr iron::result<raw_hash_table, error_handler_t> clone(load_factor_t _lf)const noexcept
		{
			return _clone(_lf);
		}
		[[nodiscard]] constexpr error_handler_t rehash(size_t _buckets_count,load_factor_t _lf)noexcept {
			return _rehash(_buckets_count,_lf);
		}
		[[nodiscard]] static constexpr size_t compute_bucket_count(load_factor_t _load_factor, size_t _addictional, size_t _sz,size_t _bucket_count)noexcept {
			DEBUG_ASSERT(_load_factor);
			DEBUG_ASSERT(_addictional);
			constexpr size_t _initial_bc = 128;
			constexpr size_t _second_bc = 512;
			auto _x = (static_cast<load_factor_t>(_addictional) + static_cast<load_factor_t>(_sz)) / _load_factor;
			if (!std::is_constant_evaluated()) { //std::ceil is not constexpr in all compilers c++23
				_x = std::ceil(_x);
			}
			const auto _new_bc = static_cast<size_t>(_x);
			if (_new_bc > _bucket_count) IRON_UNLIKELY 			//apply correction factor
			{
				if (_new_bc < _initial_bc) IRON_LIKELY 
				{
					return _initial_bc;
				}
				if (_new_bc < _second_bc) IRON_LIKELY
				{
					return _new_bc * 8;
				}
				if (_new_bc < max_bucket_count() / 2) IRON_LIKELY 
				{
					return _new_bc * 2;
				}
				if (_new_bc > max_bucket_count()) IRON_UNLIKELY 
				{
					return max_bucket_count();
				}
			}
			return _new_bc;
		}

		[[nodiscard]] constexpr error_handler_t reserve(size_t _addictional)noexcept {
			DEBUG_ASSERT(load_factor_threshold());
			const auto _new_bc = compute_bucket_count(_m_lf, _addictional, _m_sz,_m_bs.size());
			if (_new_bc > _m_bs.size()) IRON_UNLIKELY 
			{
				if (!_m_bs.size()) IRON_UNLIKELY 
				{
					DEBUG_ASSERT(!_m_sz);
					return _m_bs.resize(_new_bc);
				}
				return _rehash(_new_bc,_m_lf);
			}
			return {};
		}
		[[nodiscard]] constexpr error_handler_t rehash(raw_hash_table& _from)noexcept {
			DEBUG_ASSERT(size() && _from.size());
			return _rehash(_from);
		}
		[[nodiscard]] constexpr allocator_t allocator()const noexcept {
			return _m_bs.allocator();
		}

		[[nodiscard]] constexpr std::pair<list_t*,node_t*> find(const key_t& _k)noexcept {
			const auto _idx = _compute_idx(_k, _m_bs.size());
			auto _p = _m_bs.data() + _idx;
			if (auto _node = _find_key(*_p, _k)) IRON_LIKELY
			{
				return std::make_pair(_p, _node);
			}
			return {};
		}
		[[nodiscard]] constexpr std::pair<const list_t*,const node_t*> find(const key_t& _k)const noexcept {
			const auto _idx = _compute_idx(_k, _m_bs.size());
			auto _p = _m_bs.data() + _idx;
			if (auto _node = _find_key(*_p, _k)) IRON_LIKELY 
			{
				return std::make_pair(_p, _node);
			}
			return {};
		}
		template <typename _K,typename _M>
		[[nodiscard]] constexpr std::tuple<list_t*,node_t*,bool, error_handler_t> insert(_K&& _k,_M&& _m)noexcept
		{
#ifndef NDEBUG
			DEBUG_ASSERT(max_load_factor());
			const auto _lf=_load_factor(_m_sz, _m_bs.size());
			DEBUG_ASSERT(_lf < _m_lf);
#endif
			return _insert(std::forward<_K>(_k),std::forward<_M>(_m));
		}
		template <bool _TRANSACTIONAL,bool _MOVE,typename _ITER>
		[[nodiscard]] constexpr iron::result<size_t, error_handler_t> insert_range(_ITER& _b, const _ITER& _e,nodes_t& _roolback_nodes)noexcept {
			return _range_insert<_TRANSACTIONAL,_MOVE>(_b, _e, _roolback_nodes);
		}
		constexpr raw_hash_table& clear()noexcept {
			_clear();
			return *this;
		}
		constexpr raw_hash_table& reset(load_factor_t _lf)noexcept {
			_reset(_lf);
			return *this;
		}

		template <typename _K,typename... _ARGS >
		constexpr size_t erase(const _K& _key,const _ARGS&... _args)noexcept
		{
			size_t _c{};
			_erase(_c,_key, _args...);
			return _c;
		}
		[[nodiscard]] constexpr size_t merge(raw_hash_table& _ht)noexcept {
			DEBUG_ASSERT(this != &_ht && _ht.size() && this->size());
			DEBUG_ASSERT(_ht._m_sz && _m_sz);
			DEBUG_ASSERT(_load_factor(_m_sz + _ht._m_sz, _m_bs.size()) <= _m_lf); //if failed reserve is required
			DEBUG_ASSERT(_m_sz < max_size());
			return _merge(_ht);
		}
		template <typename... _ARGS >
		[[nodiscard]] constexpr iron::result<adapter_box_t, error_handler_t> release_list(const _ARGS&... _args)noexcept
		requires(sizeof...(_ARGS) > 0)
		{
			return _release(_args...);
		}
		[[nodiscard]] constexpr iron::result<adapter_box_t, error_handler_t> release_all()noexcept
		{
			return _release_all();
		}
		[[nodiscard]] constexpr const list_t* data()const noexcept {
			return _m_bs.data();
		}
		[[nodiscard]] constexpr list_t* data()noexcept {
			return _m_bs.data();
		}
		constexpr void rollback(nodes_t& _nodes)noexcept {
			_rollback(_nodes);
		}
	private:
		[[nodiscard]] static constexpr load_factor_t _load_factor(size_t _sz, size_t _bucket_count)noexcept {
			return _bucket_count ? (static_cast<load_factor_t>(_sz) / static_cast<load_factor_t>(_bucket_count)) : load_factor_t{};
		}
		static constexpr void _reash_nodes(buckets_t& _out, buckets_t& _in)noexcept {
			DEBUG_ASSERT(_out.size() && _in.size());
			const auto _outsz = _out.size();
			auto _p = _in.data();
			auto _n = _in.size();
			while (_n--) IRON_LIKELY 
			{
				auto& _list = *_p++;
				while (_list.front()) IRON_LIKELY 
				{
					auto _node = _list.pop_front();
					const key_t* _key;
					if constexpr (has_mapped) {
						_key = &(_node->value().first);
					}
					else {
						_key = &(_node->value());
					}
					const auto _idx = _compute_idx(*_key, _outsz);
					auto _q = _out.data() + _idx;
					_q->push_front(std::move(_node));
				}
			}
		}
		[[nodiscard]] constexpr error_handler_t _reash(size_t _bucket_count)noexcept {
			DEBUG_ASSERT(_m_sz);
			buckets_t _bs(_m_bs.allocator());
			auto _h = _bs.resize(_bucket_count); RE(_h);
			_reash_nodes(_bs, _m_bs);
			_m_bs.swap(_bs);
			return {};
		}
		[[nodiscard]] constexpr error_handler_t _rehash(size_t _bucket_count, load_factor_t _current_lf)noexcept {
			DEBUG_ASSERT(_bucket_count);
			DEBUG_ASSERT(_current_lf > 0.0F);
			buckets_t _bs(_m_bs.allocator());
			auto _h = _bs.resize(_bucket_count); RE(_h);
			_reash_nodes(_bs, _m_bs);
			_m_bs.swap(_bs);
			_m_lf = _current_lf;
			return {};
		}
		[[nodiscard]] static constexpr size_t _compute_idx(const key_t& _k, size_t _sz)noexcept {
			DEBUG_ASSERT(_sz);
			size_t _idx = hasher_t()(_k);
			_idx %= _sz;
			DEBUG_ASSERT(_idx < _sz);
			return _idx;
		}
		[[nodiscard]] static constexpr bool _compare(const value_t& _d, const key_t& _k)noexcept {
			if constexpr (has_mapped) {
				return key_equal_t()(_d.first, _k);
			}
			else {
				return key_equal_t()(_d, _k);
			}
		}
		[[nodiscard]] constexpr static std::pair<node_t*,node_t*> _find_key(list_t& _list, const key_t& _k)noexcept
		{
			auto _node = _list.front();
			node_t* _pred{};
			while (_node) IRON_LIKELY 
			{
				const auto& _data = _node->value();
				if (_compare(_data, _k)) IRON_UNLIKELY
				{
					break;
				}
				_pred = _node;
				_node = _node->next();
			}
			return std::make_pair(_pred,_node);
		}
		[[nodiscard]] constexpr static const node_t* _find_key(const list_t& _list, const key_t& _k)noexcept
		{
			auto _node = _list.front();
			while (_node) IRON_LIKELY
			{
				const auto& _data = _node->value();
				if (_compare(_data, _k)) IRON_UNLIKELY
				{
					break;
				}
				_node = _node->next();
			}
			return _node;
		}
		[[nodiscard]] constexpr static std::pair<node_t*, node_t*> _find_node(list_t& _list, const node_t* _n)noexcept
		{
			auto _node = _list.front();
			node_t* _pred{};
			while (_node) IRON_LIKELY 
			{
				if (_node == _n) IRON_UNLIKELY
				{
					break;
				}
				_pred = _node;
				_node = _node->next();
			}
			return std::make_pair(_pred, _node);
		}

		template <typename... _ARGS>
		[[nodiscard]] constexpr iron::result<adapter_box_t, error_handler_t> _release_all()noexcept {
			adapter_box_t _box(_m_bs.allocator());
			auto _p = _m_bs.data();
			auto _sz = _m_bs.size();
			auto _h = _box.reserve(_m_sz); RR(_h);
			while (_sz--) IRON_LIKELY
			{
				while(_p->front()) IRON_LIKELY 
				{
					auto _node = _p->pop_front();
					auto& _value = _node->value();
					key_t* _k;
					if constexpr (has_mapped) {
						_k = &(_value.first);
					}
					else {
						_k = &_value;
					}
					if constexpr (has_mapped) {
						_h = _box.push_back(std::make_pair(std::move(*_k), std::move(_value.second)));
						DEBUG_ASSERT(_h);
					}
					else {
						_h = _box.push_back(std::move(*_k));
						DEBUG_ASSERT(_h);
					}
					node_t::destroy(_node);
					node_t::deallocate(_node, _m_bs.allocator());
				}
				++_p;
			}
			_m_sz = {};
			return iron::ok(std::move(_box));
		}

		constexpr void _release_key(adapter_box_t& _box, const key_t& _k)noexcept {
			const auto _idx = _compute_idx(_k, _m_bs.size());
			auto _p = _m_bs.data() + _idx;
			while (true) {
				auto [_pred, _node] = _find_key(*_p, _k);
				if (!_node) IRON_UNLIKELY
				{
					break;
				}
				DEBUG_ASSERT(_m_sz);
				_p->detach(_pred, _node);
				auto& _value = _node->value();
				key_t* _key;
				if constexpr (has_mapped) {
					_key = &(_value.first);
				}
				else {
					_key = &_value;
				}
				if constexpr (has_mapped) {
					[[maybe_unused]] const auto _h = _box.push_back(std::make_pair(std::move(*_key), std::move(_value.second)));
					DEBUG_ASSERT(_h);
				}
				else {
					[[maybe_unused]] const auto _h = _box.push_back(std::move(*_key));
					DEBUG_ASSERT(_h);
				}
				node_t::destroy(_node);
				node_t::deallocate(_node, _m_bs.allocator());
				--_m_sz;
				if constexpr (!is_multi) {
					break;
				}
			}
		}
		template <typename _K,typename... _ARGS>
		constexpr void _release_into(adapter_box_t& _box, const _K& _k, const _ARGS&... _args)noexcept {
			const key_t& _key = _k;
			_release_key(_box, _key);
			if constexpr (sizeof...(_ARGS) != 0) {
				_release_into(_box,_args...);
			}
		}
		constexpr void _erase_key(size_t& _c, const key_t& _k)noexcept {
			const auto _idx = _compute_idx(_k, _m_bs.size());
			auto _p = _m_bs.data() + _idx;
			while (true) {
				auto [_pred, _node] = _find_key(*_p, _k);
				if (!_node) IRON_UNLIKELY
				{
					break;
				}
				DEBUG_ASSERT(_m_sz);
				_p->detach(_pred, _node);
#ifndef NDEBUG
				_node->reset_links();
#endif
				node_t::destroy(_node);
				node_t::deallocate(_node, _m_bs.allocator());
				--_m_sz;
				++_c;
				if constexpr (!is_multi) {
					break;
				}
			}
		}
		template <typename _K,typename... _ARGS>
		constexpr void _erase(size_t& _c,const _K& _k,const _ARGS&... _args)noexcept {
			const key_t& _key = _k;
			_erase_key(_c, _key);
			if constexpr (sizeof...(_ARGS) != 0) {
				_erase(_c, _args...);
			}
		}
		template <typename... _ARGS>
		[[nodiscard]] constexpr iron::result<adapter_box_t, error_handler_t> _release(const _ARGS&... _args)noexcept {
			static_assert(sizeof...(_ARGS) != 0, "internal error");
			adapter_box_t _a(_m_bs.allocator());
			if constexpr (is_multi) {
				auto _h = _a.reserve(_m_sz); RR(_h);
			}
			else {
				auto _h = _a.reserve(sizeof...(_ARGS)); RR(_h);
			}
			_release_into(_a, _args...);
			return iron::ok(std::move(_a));
		}
		constexpr void _clear()noexcept {
			DEBUG_ASSERT(_m_sz);
			auto _p = _m_bs.data();
			auto _sz = _m_bs.size();
			while (_sz--) IRON_LIKELY 
			{
				_p++->reset();
			}
			_m_sz = {};
		}
		constexpr void _reset(load_factor_t _lf)noexcept {
			_m_bs.reset();
			_m_sz = {};
			_m_lf = _lf;
		}
		constexpr void _swap(raw_hash_table& _v)noexcept {
			std::swap(_m_lf, _v._m_lf);
			std::swap(_m_sz, _v._m_sz);
			_m_bs.swap(_v._m_bs);
		}
		[[nodiscard]] constexpr iron::result<raw_hash_table, error_handler_t> _clone()const {
			raw_hash_table _t(_m_lf, _m_bs.allocator());
			auto _h = _t._m_bs.reserve(_m_bs.size()); RR(_h);
			auto _p = _m_bs.data();
			auto _sz = _m_bs.size();
			while (_sz--) IRON_LIKELY 
			{
				list_t _dlist(_m_bs.allocator());
				auto& _slist = *_p++;
				if (auto _node = _slist.front()) IRON_LIKELY 
				{
					node_t* _pred{}, * _head{};
					while (_node) IRON_LIKELY 
					{
						auto& _v = _node->value();
						node_t* _dnode{};
						const key_t* _key;
						if constexpr (has_mapped) {
							_key = &(_v.first);
						}
						else {
							_key = &_v;
						}
						auto _rk = iron::clone::try_clone_from(*_key); RR(_rk);
						if constexpr (has_mapped) {
							auto _rv = iron::clone::try_clone_from(_v.second); RR(_rv);
							_h = node_t::from_pair(_dnode, _m_bs.allocator(), std::move(_rk.unchecked_unwrap()), std::move(_rv.unchecked_unwrap())); RR(_h);
						}
						else {
							_h = node_t::from_value(_dnode, _m_bs.allocator(), std::move(_rk.unchecked_unwrap())); RR(_h);
						}
						if (!_head) IRON_UNLIKELY
						{
							_head = _pred = _dnode;
						}
						else {
							_pred->next() = _dnode;
							_pred = _dnode;
						}
						_node = _node->next();
					} //while(_node)
					_dlist = list_t(_head, _m_bs.allocator());
				} //if (auto _node = _slist.front())
				_h = _t._m_bs.push_back(std::move(_dlist));
				DEBUG_ASSERT(_h);
			} //while
			_t._m_sz = _m_sz;
			return iron::ok(std::move(_t));
		}

		[[nodiscard]] constexpr iron::result<raw_hash_table, error_handler_t> _clone(load_factor_t _lf)const {
			raw_hash_table _t(_lf,_m_bs.allocator());
			auto _h = _t.reserve(_m_sz); RR(_h);
			auto _p = _m_bs.data();
			auto _sz = _m_bs.size();
			std::tuple<list_t*, node_t*, bool, error_handler_t> _tp;
			while (_sz--) IRON_LIKELY 
			{
				auto& _slist = *_p++;
				if (auto _node = _slist.front()) IRON_LIKELY 
				{
					while (_node) IRON_LIKELY 
					{
						const key_t* _key;
						if constexpr (has_mapped) {
							_key = &(_node->value().first);
						}
						else {
							_key = &(_node->value());
						}
						auto _rk = iron::clone::try_clone_from(*_key); RR(_rk);
						if constexpr (has_mapped) {
							auto _rv = iron::clone::try_clone_from(_node->value().second); RR(_rv);
							_tp  = _t._insert(std::move(_rk.unchecked_unwrap()), std::move(_rv.unchecked_unwrap())); RR(std::get<3>(_tp));
							DEBUG_ASSERT(std::get<2>(_tp)); //check for inserted
						}
						else {
							_tp = _t._insert(std::move(_rk.unchecked_unwrap()), mapped_t{}); RR(std::get<3>(_tp));
							DEBUG_ASSERT(std::get<2>(_tp)); //check for inserted
						}
						_node = _node->next();
					} //while
				} //if
			} //while
			_t._m_sz = _m_sz;
			return iron::ok(std::move(_t));
		}
		template <typename _K, typename _M>
		[[nodiscard]] constexpr std::tuple<list_t*, node_t*, bool, error_handler_t> _insert(_K&& _k, _M&& _v)noexcept
		{
			DEBUG_ASSERT(_load_factor(_m_sz + 1, _m_bs.size()) <= _m_lf); //if failed reserve is required
			DEBUG_ASSERT(_m_sz < max_size());
			const auto _idx = _compute_idx(_k, _m_bs.size());
			auto _p = _m_bs.data() + _idx;
			if constexpr (!is_multi) {
				auto _pair = _find_key(*_p, _k);
				if (auto& _node = _pair.second) IRON_UNLIKELY  //key already exists
				{
					return std::make_tuple(_p, _node, false, error_handler_t{});
				}
			}
			node_t* _node{};
			if constexpr (has_mapped) {
				auto _h = node_t::from_pair(_node, _m_bs.allocator(), std::forward<_K>(_k), std::forward<_M>(_v));
				if (!_h) IRON_UNLIKELY
				{
					return std::make_tuple(nullptr, nullptr, false, std::move(_h));
				}
			}
			else {
				auto _h = node_t::from_value(_node, _m_bs.allocator(), std::forward<_K>(_k));
				if (!_h) IRON_UNLIKELY
				{
					return std::make_tuple(nullptr, nullptr, false, std::move(_h));
				}
			}
			_p->push_front(std::move(_node));
			++_m_sz;
			_node = _p->front();
			return std::make_tuple(_p, _p->front(), true, error_handler_t{});
		}
		constexpr void _rollback(nodes_t& _nodes)noexcept {
			auto _p = _nodes.data();
			auto _n = _nodes.size();
			while (_n--) IRON_LIKELY
			{
				auto& [_list, _rnode] = *_p;
				auto [_pred, _node] = _find_node(*_list,_rnode);
				DEBUG_ASSERT(_node);
				_list->detach(_pred, _node);
#ifndef NDEBUG
				_node->reset_links();
#endif
				node_t::destroy(_node);
				node_t::deallocate(_node, _m_bs.allocator());
				DEBUG_ASSERT(_m_sz);
				--_m_sz;
				if (std::is_constant_evaluated()) {
					if (_n) {
						++_p;
					}
				}
				else {
					++_p;
				}
			}
		}

		template <bool _TRANSACTIONAL,bool _MOVE,typename _ITER>
		[[nodiscard]] constexpr iron::result<size_t, error_handler_t> _range_insert(_ITER& _b, const _ITER& _e,nodes_t& _rollback_nodes)noexcept {
			size_t _c{};
			while (_b != _e) IRON_LIKELY
			{
				std::tuple<list_t*, node_t*, bool, error_handler_t> _t;
				if constexpr(has_mapped) {
					if constexpr (_MOVE) {
						_t = _insert(std::move((*_b).first), std::move((*_b).second));
					}
					else {
						auto _rk = iron::clone::try_clone_from((*_b).first); RR(_rk);
						auto _rm = iron::clone::try_clone_from((*_b).second); RR(_rm);
						_t = _insert(std::move(_rk.unchecked_unwrap()), std::move(_rm.unchecked_unwrap()));
					}
				}
				else {
					if constexpr (_MOVE) {
						_t = _insert(std::move(*_b), mapped_t{});
					}
					else {
						auto _rk = iron::clone::try_clone_from(*_b); RR(_rk);
						_t = _insert(std::move(_rk.unchecked_unwrap()), mapped_t{});
					}
				}
				++_b;
				RR(std::get<3>(_t)); //return for error
				if (std::get<2>(_t)) IRON_LIKELY //if inserted
				{
					if constexpr (_TRANSACTIONAL) {
						[[maybe_unused]] const auto _h = _rollback_nodes.push_back(std::make_pair(std::get<0>(_t), std::get<1>(_t)));
						DEBUG_ASSERT(_h); //_rollback_nodes must be preallocated
					}
					++_c;
				}
			}
			return iron::ok(_c);
		}
		[[nodiscard]] constexpr size_t _merge(raw_hash_table& _ht)noexcept {
			size_t _c{};
			auto _ps = _ht._m_bs.data();
			auto _szs = _ht._m_bs.size();
			while (--_szs) IRON_LIKELY 
			{
				auto& _lists = *_ps++;
				auto _nodes = _lists.front();
				node_t* _preds{};
				while (_nodes) IRON_LIKELY 
				{
					const key_t* _k;
					if constexpr (has_mapped) {
						_k= &(_nodes->value().first);
					}
					else {
						_k = &(_nodes->value());
					}
					const auto _idxd = _compute_idx(*_k, _m_bs.size());
					auto _pd = _m_bs.data() + _idxd;
					if constexpr(!is_multi) {
						const auto& _listd = *_pd;
						if (_find_key(_listd,*_k)) IRON_UNLIKELY //key already exists
						{
							_preds = _nodes;
							_nodes = _nodes->next();
							continue;
						}
					}
					DEBUG_ASSERT(_ht._m_sz);
					_lists.detach(_preds, _nodes);
					_nodes->reset_links();
					--_ht._m_sz;
					_pd->push_front(_nodes);
					_nodes = _preds ? _preds->next() : _lists.front();
					++_m_sz;
					++_c;
				}
			}
			return _c;
		}
		load_factor_t _m_lf;
		size_t _m_sz;
		buckets_t _m_bs;
	};


} //ns

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

