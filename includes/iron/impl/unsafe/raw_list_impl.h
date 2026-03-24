#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable:26429)
#pragma warning (disable:26481)
#endif

namespace iron::unsafe {

	template <typename _NODE, typename _A>
	class raw_list final {
	public:
		using node_t=_NODE;
		using allocator_t = _A;
		using value_t = typename node_t::value_t;
		using ptr_t = node_t*;
		using pointer = ptr_t;
		using cptr_t = const node_t*;
		using const_pointer = cptr_t;
		using size_t = typename node_t::size_t;
		using diff_t = std::ptrdiff_t;
		using difference_type = diff_t;
		using error_handler_t = typename node_t::error_handler_t;
		using reference = value_t&;
		using const_reference = const value_t&;
		static constexpr auto is_bidirectional = node_t::is_bidirectional;
		using value_allocator_t = typename std::allocator_traits<allocator_t>::template rebind_alloc<value_t>;
		using adapter_box_t = iron::unsafe::raw_vector<value_t,value_allocator_t>;
		[[nodiscard]] static constexpr size_t max_size()noexcept { return node_t::raw_storage_node_t::max_size(); }
		struct raw_list_tag { raw_list_tag() = delete; };
		[[nodiscard]] constexpr static iron::result<raw_list, error_handler_t> from(adapter_box_t&& _box)noexcept {
			return _from(std::move(_box));
		}
		constexpr raw_list()noexcept = default;
		constexpr explicit  raw_list(allocator_t _a)noexcept : _m_a(_a) {}
		constexpr raw_list(ptr_t _head, allocator_t _a)noexcept
		requires(!is_bidirectional)
			: _m_head(_head)
			, _m_a(_a)
		{
		}

		constexpr raw_list(ptr_t _head, ptr_t _tail, size_t _sz, allocator_t _a)noexcept
		requires(is_bidirectional)
			: _m_head(_head)
			, _m_tail(_tail,_sz)
			, _m_a(_a)
		{
		}
		raw_list(const raw_list&) = delete;
		raw_list& operator=(const raw_list&) = delete;
		constexpr raw_list(raw_list&& _v)noexcept
		{
			_swap(_v);
		}
		constexpr raw_list& operator=(raw_list&& _v)noexcept {
			_swap(_v);
			return *this;
		}
		constexpr ~raw_list() { _reset(); }

		constexpr void swap(raw_list& _v)noexcept {
			_swap(_v);
		}
		[[nodiscard]] constexpr size_t size()const noexcept
		requires(is_bidirectional)
		{
			return _count();
		}
		[[nodiscard]] constexpr size_t count()const noexcept {
			return _count();
		}
		[[nodiscard]] constexpr bool empty()const noexcept {
			return _m_head ? false : true;
		}
		[[nodiscard]] constexpr explicit operator bool()const noexcept { return !empty(); }
		constexpr raw_list& reset()noexcept {
			_reset();
			return *this;
		}
		[[nodiscard]] constexpr allocator_t allocator()const noexcept {
			return _m_a;
		}
		[[nodiscard]] constexpr iron::result<adapter_box_t,error_handler_t> release()noexcept {
			return _release();
		}
		constexpr void push_front(ptr_t _node)noexcept
		{
			DEBUG_ASSERT(_node);
			_push_front(_node);
		}
		template <bool _TRANSACTIONAL,typename _ITER>
		[[nodiscard]] constexpr iron::result<size_t, error_handler_t> insert_range_before(node_t* _node, _ITER& _b, const _ITER& _e)noexcept
		{
			return _insert_range<_TRANSACTIONAL,false>(_node, _b, _e);
		}
		template <bool _TRANSACTIONAL,typename _ITER>
		[[nodiscard]] constexpr iron::result<size_t, error_handler_t> insert_range_after(node_t* _node, _ITER& _b, const _ITER& _e)noexcept
		{
			return _insert_range<_TRANSACTIONAL,true>(_node, _b, _e);
		}
		template <bool _TRANSACTIONAL, typename _ITER>
		[[nodiscard]] constexpr iron::result<size_t,error_handler_t> push_front_range(_ITER& _b, const _ITER& _e)noexcept {
			return _push_range<_TRANSACTIONAL, false>(_b, _e);
		}
		template <bool _TRANSACTIONAL, typename _ITER>
		[[nodiscard]] constexpr iron::result<size_t, error_handler_t> push_back_range(_ITER& _b, const _ITER& _e)noexcept {
			return _push_range<_TRANSACTIONAL,true>(_b, _e);
		}
		constexpr void push_back(ptr_t _node)noexcept
		requires (is_bidirectional)
		{
			DEBUG_ASSERT(_node);
			_push_back(_node);
		}
		constexpr void push_back(ptr_t _prev,ptr_t _node)noexcept
		requires (!is_bidirectional)
		{
			DEBUG_ASSERT(_node);
			_push_back(_prev,_node);
		}

		[[nodiscard]] constexpr ptr_t front()noexcept {
			return _m_head;
		}
		[[nodiscard]] constexpr const ptr_t front()const noexcept {
			return _m_head;
		}
		[[nodiscard]] constexpr ptr_t back()noexcept
		requires(is_bidirectional)
		{
			return _m_tail._m_node;
		}
		[[nodiscard]] constexpr const ptr_t back()const noexcept
		requires(is_bidirectional)
		{
			return _m_tail._m_node;
		}
		constexpr ptr_t pop_front()noexcept
		{
			return _pop_front();
		}

		constexpr ptr_t pop_back()noexcept
		requires (is_bidirectional)
		{
			DEBUG_ASSERT(size());
			return _pop_back();
		}
		constexpr void insert(ptr_t _cn, ptr_t _node)noexcept {
			DEBUG_ASSERT(_cn);
			DEBUG_ASSERT(_node);
			_insert(_cn, _node);
		}
		constexpr raw_list& clear()noexcept {
			_reset();
			return *this;
		}
		[[nodiscard]] constexpr iron::result<raw_list, error_handler_t> try_clone(size_t _n=1)const noexcept
		requires(iron::concepts::copyable_or_cloneable<value_t>)
		{
			return _clone(_n);
		}
		[[nodiscard]] constexpr raw_list clone(size_t _n = 1)const noexcept
			requires(iron::concepts::copyable_or_cloneable<value_t>)
		{
			auto _r = try_clone(_n); RP(_r);
		}
		constexpr raw_list& reverse()noexcept {
			if (_m_head) {
				_reverse();
			}
			return *this;
		}
		constexpr raw_list& detach(node_t* _node)noexcept
		requires (is_bidirectional)
		{
			DEBUG_ASSERT(_node);
			_detach(_node);
			return *this;
		}
		constexpr raw_list& detach(node_t* _prev,node_t* _node)noexcept
		requires (!is_bidirectional)
		{
			DEBUG_ASSERT(_node);
			_detach(_prev,_node);
			return *this;
		}
		template <bool _TRANSACTIONAL>
		[[nodiscard]] constexpr error_handler_t resize(size_t _sz,const value_t& _v)noexcept
		{
			auto _h=_resize<_TRANSACTIONAL>(_sz,_v);
			_check();
			return _h;
		}
		constexpr void check()noexcept {
			_check();
		}
	private:
		[[nodiscard]] constexpr iron::result<adapter_box_t,error_handler_t> _release()noexcept {
			const auto _a = typename std::allocator_traits<allocator_t>::template rebind_alloc<value_t>(_m_a);
			adapter_box_t _v{ _a };
			if (auto _node=_m_head) IRON_LIKELY
			{
				const auto _sz = _count();
				DEBUG_ASSERT(_sz);
				auto _h = _v.reserve(_sz); RR(_h);
				while (_node) IRON_LIKELY
				{
					_h = _v.push_back(std::move(_node->value()));
					DEBUG_ASSERT(_h);
					auto _p = _node->next();
#ifndef NDEBUG
					_node->reset_links();
#endif
					node_t::destroy(_node);
					node_t::deallocate(_node, _m_a);
					_node = _p;
				}
				_m_head = {};
				if constexpr (is_bidirectional) {
					_m_tail = {};
				}
			}
			return iron::ok(std::move(_v));
		}
		constexpr void _swap(raw_list& _v)noexcept {
			std::swap(_m_head, _v._m_head);
			if constexpr (is_bidirectional) {
				std::swap(_m_tail, _v._m_tail);
			}
			std::swap(_m_a, _v._m_a);
			IRON_SECURITY_SWAP(_v);
		}
		constexpr void _push_front(ptr_t _node)noexcept {
			if (_m_head) IRON_LIKELY 
			{
				_node->next() = _m_head;
				if constexpr (is_bidirectional) {
					_m_head->prev() = _node;
					if (!_m_tail._m_node->prev()) IRON_UNLIKELY {
						_m_tail._m_node->prev() = _node;
					}
				}
			}
			else {
				if constexpr (is_bidirectional) {
					_m_tail._m_node = _node;
				}
			}
			_m_head = _node;
			if constexpr (is_bidirectional) {
				++_m_tail._m_sz;
			}
			_check();
		}

		constexpr void _push_back(ptr_t _node)noexcept
		requires(is_bidirectional)
		{
			DEBUG_ASSERT(_node);
			if (_m_tail._m_node) IRON_LIKELY 
			{
				_node->prev() = _m_tail._m_node;
				_m_tail._m_node->next() = _node;
				if (!_m_head->next()) IRON_UNLIKELY 
				{
					_m_head->next() = _node;
				}
			}
			else {
				_m_head = _node;
			}
			_m_tail._m_node = _node;
			++_m_tail._m_sz;
			_check();
		}
		constexpr void _push_back(ptr_t _prev, ptr_t _node)noexcept
		requires(!is_bidirectional)
		{
			DEBUG_ASSERT(_node);
			if (_prev) IRON_LIKELY {
				DEBUG_ASSERT(!_prev->next());
				_prev->next() = _node;
			}
			else {
				DEBUG_ASSERT(!_m_head);
				_m_head = _node;
			}
		}

		constexpr void _insert(ptr_t _cn, ptr_t _node)noexcept {
			DEBUG_ASSERT(_cn);
			DEBUG_ASSERT(_node);
			_node->next() = _cn->next();
			_cn->next() = _node;
			if constexpr (is_bidirectional) {
				if (auto& _node_next = _node->next()) {
					_node_next->prev() = _node;
				}
				else {
					_m_tail._m_node = _node;
				}
				_node->prev() = _cn;
				++_m_tail._m_sz;
			}
			_check();
		}
		[[nodiscard]] constexpr ptr_t _pop_front()noexcept {
			auto _node = _m_head;
			DEBUG_ASSERT(_node);
			_m_head = _m_head->next();
			_node->reset_links();
			if constexpr (is_bidirectional) {
				auto& _sz = _m_tail._m_sz;
				DEBUG_ASSERT(_sz);
				--_sz;
				if (!_sz) IRON_UNLIKELY 
				{
					_m_tail._m_node = {};
					DEBUG_ASSERT(!_m_head);
				}
				else {
					if (_sz == 1) IRON_UNLIKELY
					{
						_m_tail._m_node = _m_head;
					}
					_m_head->prev() = {};
				}
			}
			_check();
			return _node;
		}

		[[nodiscard]] constexpr ptr_t _pop_back()noexcept
		requires(is_bidirectional)
		{
			auto _node = _m_tail._m_node;
			DEBUG_ASSERT(_node);
			_m_tail._m_node = _node->prev();
			_node->reset_links();
			auto& _sz = _m_tail._m_sz;
			--_sz;
			if (!_sz) IRON_UNLIKELY
			{
				_m_head = {};
				DEBUG_ASSERT(!_m_tail._m_node);
			}
			else {
				if (_sz == 1) IRON_UNLIKELY
				{
					_m_head = _m_tail._m_node;
				}
				_m_tail._m_node->next() = {};
			}
			_check();
			return _node;
		}
		constexpr void _reset()noexcept {
			while (_m_head) {
				auto _p = _m_head->next();
#ifndef NDEBUG
				_m_head->reset_links();
#endif
				node_t::destroy(_m_head);
				node_t::deallocate(_m_head, _m_a);
				_m_head = _p;
			}
			if constexpr (is_bidirectional) {
				_m_tail = {};
			}
			_check();
		}
		constexpr void _reverse()noexcept {
			DEBUG_ASSERT(_m_head);
			if constexpr (is_bidirectional) {
				node_t* _node{};
				_m_tail._m_node = _m_head;
				while (_m_head) IRON_LIKELY 
				{
					_node = _m_head->prev();
					_m_head->prev() = _m_head->next();
					_m_head->next() = _node;
					_m_head = _m_head->prev();
				}
				DEBUG_ASSERT(_node);
				_m_head = _node->prev();
				_check();
			}
			else {
				node_t* _prev{};
				auto& _node = _m_head;
				while(true)  {
					const auto _next = _node->next();
					_node->next() = _prev;
					if (!_next) IRON_UNLIKELY 
					{
						break;
					}
					_prev = _node;
					_node = _next;
				}
				_check();
			}
		}
		[[nodiscard]] constexpr iron::result<raw_list, error_handler_t> _clone(size_t _n)const noexcept {
			raw_list _t(std::allocator_traits<allocator_t>::select_on_container_copy_construction(_m_a));
			if (_m_head && _n) {
				[[maybe_unused]] node_t* _prev{}; //only for !bidirectional
				while (_n--) IRON_LIKELY
				{
					auto _p = _m_head;
					while (_p) IRON_LIKELY
					{
						auto& _v = _p->value();
						node_t* _node{};
						auto _h = node_t::from_iter(_node, _t.allocator(), &_v); RR(_h);
						if constexpr (is_bidirectional) {
							_t._push_back(_node);
						}
						else {
							_t._push_back(_prev, _node);
							_prev = _node;
						}
						_p = _p->next();
					}
				}
			}
			_t.check();
			return iron::ok(std::move(_t));
		}
		[[nodiscard]] constexpr size_t _count()const noexcept {
			if constexpr (is_bidirectional) {
				return _m_tail._m_sz;
			}
			else {
				size_t _c{};
				auto _p = _m_head;
				while (_p) IRON_LIKELY
				{
#ifndef NDEBUG
					PANIC_IF(_c == max_size(), "attept to use a big list");
#endif
					++_c;
					_p = _p->next();
				}
				return _c;
			}
		}
		[[nodiscard]] constexpr static iron::result<raw_list, error_handler_t> _from(adapter_box_t&& _box)noexcept {
			raw_list _list{ _box.allocator() };
			auto _sz = _box.size();
			auto _p = _box.data();
			while (_sz--) IRON_LIKELY 
			{
				node_t* _node{};
				auto _h = node_t::from_value(_node, _list._m_a, std::move(*_p++)); RR(_h);
				if constexpr (is_bidirectional) {
					_list.push_back(_node);
				}
				else {
					_list.push_front(_node);
				}
			}
			_list._check();
			return iron::ok(std::move(_list));
		}
		constexpr void _detach(node_t* _node)noexcept
		requires(is_bidirectional)
		{
			DEBUG_ASSERT(_node);
			if (auto _p = _node->next()) {
				_p->prev() = _node->prev();
			}
			if (auto _p = _node->prev()) {
				_p->next() = _node->next();
			}
			if (_node == _m_head) {
				_m_head = _node->next();
			}
			if (_node == _m_tail._m_node) {
				_m_tail._m_node = _node->prev();
			}
			--_m_tail._m_sz;
			_check();
#ifndef NDEBUG
			_node->reset_links();
#endif
		}

		constexpr void _detach(node_t* _prev,node_t* _node)noexcept
		requires(!is_bidirectional)
		{
			DEBUG_ASSERT(_node);
			if (_prev) IRON_LIKELY
			{
				DEBUG_ASSERT(_prev->next() == _node);
				_prev->next() = _node->next();
			}
			else {
				DEBUG_ASSERT(_node == _m_head);
				_m_head = _node->next();
			}
			_check();
		}
		[[nodiscard]] constexpr std::pair<node_t*, size_t> _count_n(const size_t _n)const noexcept
		{
			if constexpr (is_bidirectional) {
				const auto _sz = _m_tail._m_sz;
				auto _node_tail = _m_tail._m_node;
				if (_n >= _sz) IRON_UNLIKELY
				{
					return std::make_pair(_node_tail, _sz);
				}
			}
			size_t _c{};
			auto _p = _m_head;
			while (_p) IRON_LIKELY
			{
				++_c;
				if (_c == _n || !_p->next()) IRON_UNLIKELY 
				{
					break;
				}
				_p = _p->next();
			}
			return std::make_pair(_p, _c);
		}

		constexpr void _rollback_resize(node_t* _node_tail)noexcept
		{
			if (_node_tail) IRON_LIKELY
			{
				while (auto _p = _node_tail->next()) IRON_LIKELY
				{
					if constexpr (is_bidirectional) {
						_detach(_p);
					}
					else {
						_detach(_node_tail, _p);
					}
#ifndef NDEBUG
					_p->reset_links();
#endif
					node_t::destroy(_p);
					node_t::deallocate(_p, _m_a);
				}
			}
			else {
				_reset();
			}
		}
		template <bool _TRANSACTIONAL>
		constexpr error_handler_t _resize(const size_t _sz,const value_t& _v)noexcept
		{
			const auto [_node_tail, _count] = _count_n(_sz);
			if (!_node_tail || !_node_tail->next()) IRON_UNLIKELY
			{ //added
				DEBUG_ASSERT(_sz >= _count);
				auto _p = _node_tail;
				auto _diff = _sz - _count;
				while (_diff--) IRON_LIKELY
				{
					node_t* _node{};
					error_handler_t _h;
					if constexpr (std::is_nothrow_copy_constructible_v<value_t>) {
						_h = node_t::from_value(_node, _m_a, _v);
					}
					else {
						if (auto _r = iron::clone::try_clone_from(_v)) IRON_LIKELY
						{
							_h = node_t::from_value(_node, _m_a,std::move(_r.unchecked_unwrap()));
						}
						else {
							_h = std::move(_r.unchecked_unwrap_err());
						}
					}
					if (!_h) IRON_UNLIKELY
					{
						if constexpr (_TRANSACTIONAL) {
							_rollback_resize(_node_tail);
						}
						return _h;
					}
					if constexpr (is_bidirectional) {
						_push_back(_node);
					}
					else {
						_push_back(_p, _node);
						_p = _node;
					}
				}
			}
			else { //drop
				while (auto _node=_node_tail->next()) IRON_LIKELY
				{
					if constexpr (is_bidirectional) {
						_detach(_node);
					}
					else {
						_detach(_node_tail, _node);
					}
#ifndef NDEBUG
					_node->reset_links();
#endif
					node_t::destroy(_node);
					node_t::deallocate(_node, _m_a);
				}
			}
			return {};
		}

		constexpr void _check()const noexcept {
#ifdef _IRON_CHECK_LIST
			if constexpr (is_bidirectional) {
				if (!_m_tail._m_sz) {
					ASSERT(!_m_head);
					ASSERT(!_m_tail._m_node);
				}
				else {
					ASSERT(_m_head);
					ASSERT(!_m_head->prev());
					ASSERT(_m_tail._m_node);
					ASSERT(!_m_tail._m_node->next());
					auto _p = _m_head;
					ptr_t _prev{};
					size_t _count{};
					while (_p) {
						if (auto _x=_p->next()) {
							ASSERT_EQ(_x->prev(), _p);
						}
						else {
							ASSERT_EQ(_p, _m_tail._m_node);
						}
						if (_prev) {
							ASSERT_EQ(_prev->next(), _p);
						}
						++_count;
						_prev = _p;
						_p = _p->next();
					}
					ASSERT(_m_tail._m_sz == _count);
				}
			}
			else {
				auto _p = _m_head;
				while (_p) {
					_p = _p->next();
				}
			}
#endif
		}

		constexpr void _rollback_range(node_t* _node, size_t _count)noexcept {
			if (!_node) IRON_UNLIKELY
			{
				while (_count--) IRON_LIKELY
				{
					auto _p=_pop_front();
#ifndef NDEBUG
					_p->reset_links();
#endif
					node_t::destroy(_p);
					node_t::deallocate(_p, _m_a);
				}
			}
			else {
				while (_count--) IRON_LIKELY
				{
					auto _p = _node->next();
					if constexpr (is_bidirectional) {
						_detach(_p);
					}
					else {
						_detach(_node, _p);
					}
#ifndef NDEBUG
					_p->reset_links();
#endif
					node_t::destroy(_p);
					node_t::deallocate(_p, _m_a);
				}
			}
		}
		template <bool _TRANSACTIONAL, bool _AFTER, typename _ITER>
		[[nodiscard]] constexpr iron::result<size_t, error_handler_t> _insert_range(node_t* _pos_node, _ITER& _b, const _ITER& _e)noexcept {
			DEBUG_ASSERT(_pos_node);
			if constexpr (!_AFTER) {
				_pos_node = _pos_node->prev();
			}
			size_t _count{};
			auto _h = _insert_range_x<_AFTER>(_count, _pos_node, _b, _e);
			if constexpr (_TRANSACTIONAL) {
				if (!_h && _count) IRON_UNLIKELY 
				{
					_rollback_range(_pos_node, _count);
				}
			}
			RR(_h);
			return iron::ok(_count);
		}

		template <bool _AFTER,typename _ITER>
		[[nodiscard]] constexpr error_handler_t _insert_range_x(size_t& _count,node_t* _pos_node, _ITER& _b, const _ITER& _e)noexcept
		{
			while (_b != _e) IRON_LIKELY 
			{
				node_t* _node{};
				auto _h = node_t::from_iter(_node, _m_a,_b); RE(_h);
				if constexpr (_AFTER) {
					_insert(_pos_node, _node);
				}
				else {
					if (!_pos_node) IRON_UNLIKELY 
					{
						_push_front(_node);
					}
					else {
						_insert(_pos_node, _node);
					}
				}
				_pos_node = _node;
				++_b;
				++_count;
			}
			return {};
		}
		template <bool _TRANSACTIONAL, bool _BACK, typename _ITER>
		[[nodiscard]] constexpr iron::result<size_t, error_handler_t> _push_range(_ITER& _b, const _ITER& _e)noexcept {
			[[maybe_unused]] node_t* _pos_node{}; //only for TRANSACTIONAL
			if constexpr (_TRANSACTIONAL && _BACK) {
				_pos_node = _m_tail._m_node;
			}
			size_t _count{};
			auto _h = _push_range_x<_BACK>(_count, _b, _e);
			if constexpr (_TRANSACTIONAL) {
				if (!_h && _count) IRON_UNLIKELY 
				{
					_rollback_range(_pos_node, _count);
				}
			}
			RR(_h);
			return iron::ok(_count);
		}

		template <bool _BACK, typename _ITER>
		[[nodiscard]] constexpr error_handler_t _push_range_x(size_t& _count,_ITER& _b, const _ITER& _e)noexcept {
			while (_b != _e) IRON_LIKELY 
			{
				node_t* _node{};
				auto _h = node_t::from_iter(_node, _m_a,_b); RE(_h);
				if constexpr (_BACK) {
					_push_back(_node);
				}
				else {
					_push_front(_node);
				}
				++_b;
				++_count;
			}
			return {};
		}

		node_t* _m_head{};
		typedef struct _struct_tail_empty {} _tail_empty_t;
		typedef struct _struct_tail {
			node_t* _m_node{};
			size_t _m_sz{};
		} _tail_bd_t;
		using _tail_t = std::conditional_t<is_bidirectional, _tail_bd_t, _tail_empty_t>;
		IRON_NO_UNIQUE_ADDRESS _tail_t _m_tail{};
		IRON_NO_UNIQUE_ADDRESS allocator_t _m_a{};
		IRON_SECURITY_DCL();
	};


} //ns

#ifdef _IRON_CHECK_LIST
static_assert(iron::concepts::raw_list<iron::unsafe::raw_list<iron::unsafe::list_node<int,true>, iron::unsafe::allocator<int>>>, "internal error");
#endif

#if defined(_MSC_VER)
#pragma warning(pop)
#endif
