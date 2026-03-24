#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:26429)
#pragma warning(disable:26446)
#pragma warning(disable:26461)
#pragma warning(disable:26465)
#pragma warning(disable:26481)
#pragma warning(disable:26482)
#pragma warning(disable:26490)
#pragma warning(disable:26492)
#endif

namespace iron::unsafe {

	template <typename _NODE,std::size_t _N>
	class raw_array_list {
	public:
		using node_t=_NODE;
		using node_type=node_t;
		using size_t=std::size_t;
		using size_type = size_t;
		using diff_t = std::ptrdiff_t;
		using difference_type = diff_t;
		using iter_t = node_t**;
		using iter_type = iter_t;
		using const_iter_t = const node_t*const *;
		using const_iter_type = const_iter_t;
		using raw_storage_t=iron::unsafe::raw_storage<node_t*>;
		using raw_storage_type = raw_storage_t;
		static_assert(_N > 0,"raw_array_list require size must be > 0");
		static_assert(!std::is_reference_v<node_t>, "raw_array_list node_type must not be a reference type");
		static_assert(!std::is_array_v<node_t>, "raw_array_list node_type must not be an array type");
		static_assert(!std::is_const_v<node_t>, "raw_array_list node_type must not be const");
	private:
	public:
		constexpr raw_array_list()noexcept
			: _m_nodes()
			, _m_sz()
		{
			[[maybe_unused]] const auto _h=raw_storage_t::uninitialized_fill(&_m_nodes[0],_N,nullptr);
			DEBUG_ASSERT(_h);
		}
		raw_array_list(const raw_array_list&)=delete;
		raw_array_list& operator=(const raw_array_list&)=delete;

		constexpr raw_array_list(raw_array_list&& _v)noexcept
		: _m_nodes()
		, _m_sz()
		{
			[[maybe_unused]] const auto _h=raw_storage_t::uninitialized_fill(&_m_nodes[0],_N,nullptr);
			DEBUG_ASSERT(_h);
			_swap(_v);
		}
		constexpr raw_array_list& operator=(raw_array_list&& _v)noexcept
		{
			_swap(_v);
			return *this;
		}
		constexpr ~raw_array_list() {
			DEBUG_ASSERT(!_m_sz);
		}
		constexpr void swap(raw_array_list& _v)noexcept
		{
			_swap(_v);
		}
		[[nodiscard]] static constexpr size_t max_size()noexcept { return _N;}
		[[nodiscard]] constexpr size_t size()const noexcept { return _m_sz;}
		[[nodiscard]] constexpr bool empty()const noexcept { return !_m_sz; }
		[[nodiscard]] constexpr bool full()const noexcept { return _m_sz == max_size(); }
		constexpr iter_t push_back(node_t* _node)noexcept {
			DEBUG_ASSERT(!full() && _node);
			return _push_back(_node);
		}
		constexpr iter_t push_front(node_t* _node)noexcept {
			DEBUG_ASSERT(!full() && _node);
			return _push_front(_node);
		}

		constexpr iter_t insert(iter_t _iter,node_t* _node)noexcept {
			DEBUG_ASSERT(!full());
			DEBUG_ASSERT(_iter != end() && size());
			DEBUG_ASSERT(_node);
			return _insert(_iter,_node);
		}
		[[nodiscard]] constexpr node_t* pop_back()noexcept
		{
			DEBUG_ASSERT(!empty());
			return _pop_back();
		}
		[[nodiscard]] constexpr node_t* detach(iter_t _iter)noexcept {
			return _shift_left(_iter);
		}
		constexpr void append_from(raw_array_list& _a, size_t _start_idx, size_t _len) {
			auto _iter = _a.begin() + _start_idx;
			while (_len--) {
				auto _node = _a._shift_left(_iter);
				push_back(_node);
			}
		}
		[[nodiscard]] constexpr size_t index(const_iter_t _iter)const noexcept {
			DEBUG_ASSERT(_iter);
			const auto _idx = static_cast<size_t>(_iter - begin());
			DEBUG_ASSERT(_idx <= size());
			return _idx;
		}
		[[nodiscard]] constexpr node_t* nth(size_t _idx)noexcept {
			DEBUG_ASSERT(_idx < size());
			return *(begin() + _idx);
		}
		[[nodiscard]] constexpr iter_t find(node_t* _node)noexcept {
			DEBUG_ASSERT(_node);
			auto _b = begin();
			const auto _e = end();
			while (_b != _e) {
				if (*_b == _node) {
					return _b;
				}
				++_b;
			}
			return _b;
		}
		[[nodiscard]] constexpr iter_t begin()noexcept { return &_m_nodes[0]; }
		[[nodiscard]] constexpr iter_t end()noexcept { return &_m_nodes[0] + _m_sz; }
		[[nodiscard]] constexpr const_iter_t begin()const noexcept { return &_m_nodes[0]; }
		[[nodiscard]] constexpr const_iter_t end()const noexcept { return &_m_nodes[0] + _m_sz; }
		template <typename _A>
		static constexpr void destroy(raw_array_list& _list,_A _a) 
		requires(iron::concepts::allocator<_A>)
		{
			if constexpr (std::is_trivially_destructible_v<node_t>) {
				[[maybe_unused]] const auto _h = raw_storage_t::assign_fill(&_list._m_nodes[0], _N, nullptr);
				DEBUG_ASSERT(_h);
			}
			else if constexpr (iron::concepts::list_node<node_t>) {
				auto _b = _list.begin();
				const auto _e = _list.end();
				while (_b != _e) {
					auto _node = *_b;
					*_b++ = nullptr;
					DEBUG_ASSERT(_node);
					while (_node) {
						auto _next = _node->next();
#ifndef NDEBUG
						_node->reset_links();
#endif 
						node_t::destroy(_node);
						node_t::deallocate(_node, _a);
						_node = _next;
					}
				}
			}
			else if constexpr (iron::concepts::btree_node<node_t>) {
				auto _b = _list.begin();
				const auto _e = _list.end();
				while (_b != _e) {
					auto _node = *_b;
					node_t::destroy(_node, _a);
					node_t::deallocate(_node, _a);
					*_b++ = nullptr;
				}
			}
			else {
				static_assert(iron::unsafe::asserts::always_false<node_t>, "unknow node type");
			}
			_list._m_sz = {};
		}
	private:
		constexpr void _swap(raw_array_list& _v)noexcept {
			std::swap(_m_nodes,_v._m_nodes);
			std::swap(_m_sz, _v._m_sz);
		}
		[[nodiscard]] constexpr node_t* _shift_left(iter_t _p)noexcept { //erase the element on iter
			auto _ret=*_p;
			auto _e = &_m_nodes[0] + (--_m_sz);
			while (_p != _e) IRON_LIKELY{
				auto _d = _p++;
				*_d = *_p;
			}
			*_p=nullptr;
			return _ret;
		}
		constexpr void _shift_right(iter_t _p)noexcept { //free a place for insert
			auto _ps = &_m_nodes[0] + (_m_sz - 1); //point to last
			*(_ps + 1)=*_ps;
			while (_ps != _p) {
				*_ps = *(_ps - 1);
				--_ps;
			}
			++_m_sz;
		}
		[[nodiscard]] constexpr iter_t _push_back(node_t* _p)noexcept
		{
			auto& _ps = _m_nodes[_m_sz++];
			_ps=_p;
			return &_ps;
		}
		[[nodiscard]] constexpr iter_t _push_front(node_t* _p)noexcept {
			if (_m_sz) IRON_LIKELY {
				_shift_right(begin());
			}
			else {
				_m_sz = 1;
			}
			_m_nodes[0] = _p;
			return begin();
		}
		[[nodiscard]] constexpr iter_t _insert(iter_t _iter,node_t* _node)noexcept {
			_shift_right(_iter); //free a place for insert
			*_iter=_node;
			return _iter;
		}
		[[nodiscard]] constexpr node_t* _pop_back()noexcept {
			node_t* _p{};
			auto& _plast=_m_nodes[--_m_sz];
			std::swap(_p, _plast);
			return _p;
		}
		node_t* _m_nodes[_N];
		size_t _m_sz;
	};

} //ns

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

