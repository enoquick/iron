
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable:26481)
#endif

namespace iron::unsafe::iterators {

	template <typename _T, typename _RAW_LIST>
	class hash_iterator {
	public:
		using raw_list_t = _RAW_LIST;
		using value_t = _T;
		using value_type = value_t;
	private:
		using _node_t = typename raw_list_t::node_t;
		using _value_t = typename _node_t::value_t;
	public:
		using node_t = std::conditional_t<std::is_const_v<raw_list_t>, const _node_t, _node_t>;
		using iterator_category = std::forward_iterator_tag;
		using iterator_concept = iterator_category;
		using difference_type = std::ptrdiff_t;
		using diff_t = difference_type;
		using pointer = value_t*;
		using reference = std::conditional_t<iron::concepts::proxy_pair<value_t>,value_t,value_t&>;
		using size_t = typename raw_list_t::size_t;
		using size_type = size_t;
		struct hash_table_iterator_tag { hash_table_iterator_tag() = delete; };
		constexpr hash_iterator()noexcept = default;
	public:
		constexpr hash_iterator(raw_list_t* _b,raw_list_t* _e,node_t* _node)noexcept
		: _m_b(_b)
		, _m_e(_e)
		, _m_node(_node)
		{
			if (!_m_node && _m_b != _m_e)
			{
				_m_node = _m_b->front();
			}
			_next_unless();
		}
		constexpr hash_iterator(const hash_iterator&)noexcept=default;
		constexpr hash_iterator(hash_iterator&&)noexcept=default;
		constexpr hash_iterator& operator=(const hash_iterator&)noexcept=default;
		constexpr hash_iterator& operator=(hash_iterator&&)noexcept=default;
		constexpr ~hash_iterator()=default;
		constexpr void swap(hash_iterator& _v)noexcept {
			std::swap(_m_b,_v._m_b);
			std::swap(_m_e, _v._m_e);
			std::swap(_m_node,_v._m_node);
		}
		constexpr hash_iterator& operator++()noexcept { _next(); return *this; }
		constexpr hash_iterator operator++(int)noexcept { auto _t=*this; ++(*this); return _t; }
		[[nodiscard]] constexpr pointer operator->()const noexcept {
			PANIC("todo");
			//return &(_m_node->value().value());
		}
		[[nodiscard]] constexpr reference operator*()const noexcept
		{
			DEBUG_ASSERT(_m_node);
			auto& _v = _m_node->value();
			if constexpr (iron::concepts::proxy_pair<value_t>) {
				return reference::unchecked_from(_v.first, _v.second);
			}
			else {
				return _v;
			}
		}
		[[nodiscard]] constexpr bool operator==(const hash_iterator& _other) const noexcept {
			return _m_node == _other._m_node;
		}
		[[nodiscard]] constexpr bool operator!=(const hash_iterator& _other) const noexcept {
			return _m_node != _other._m_node;
		}
	private:
		constexpr void _next_unless()noexcept {
			if (!_m_node && _m_b != _m_e) IRON_LIKELY
			{
				++_m_b;
				while (_m_b != _m_e) IRON_LIKELY
				{
					_m_node = _m_b->front();
					if (_m_node)
					{
						break;
					}
					++_m_b;
				}
			}
		}
		constexpr void _next()noexcept
		{
			_m_node = _m_node->next();
			_next_unless();
		}
		raw_list_t* _m_b{};
		const raw_list_t* _m_e{};
		node_t* _m_node{};
	};

}

#if defined(_MSC_VER)
#pragma warning(pop)
#endif
