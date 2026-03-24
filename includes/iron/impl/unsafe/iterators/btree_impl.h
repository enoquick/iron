
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable:26429)
#pragma warning (disable:26472)
#pragma warning (disable:26481)
#endif

namespace iron::concepts {

	template <typename _T>
	concept _btree_struct_iter = requires(_T) {
		typename std::decay_t<_T>::struct_iter_tag;
	};

}
//NOTE: not use list_node for not multi btree?

namespace iron::unsafe::btree {
	template <typename _INDEX>
	class common;
}

namespace iron::unsafe::iterators {


	template <typename _T, typename _INDEX>
	class btree_iterator final {
	public:
		using index_t=_INDEX;
	private:
		using tree_node_t = typename index_t::tree_node_t;
		using data_iter_t = typename index_t::data_iter_t;
		using data_node_t = typename index_t::data_node_t;
		using common_t = iron::unsafe::btree::common<index_t>;
	public:
		using value_t = _T;
		using value_type = value_t;
		using iterator_category = std::bidirectional_iterator_tag;
		using iterator_concept = iterator_category;
		using difference_type = std::ptrdiff_t;
		using diff_t = difference_type;
		using pointer = value_type*;
		using reference = std::conditional_t<iron::concepts::proxy_pair<value_t>, value_t, value_t&>;
		using size_t = std::size_t;
		using size_type = size_t;
		struct btree_table_iterator_tag { btree_table_iterator_tag() = delete; };
		static constexpr bool is_multi = index_t::is_multi;
		static constexpr bool has_mapped = index_t::has_mapped;
		constexpr btree_iterator()noexcept = default;
		constexpr explicit btree_iterator(const index_t& _index)noexcept
		: _m_index(_index)
		{
		}
		constexpr btree_iterator(const btree_iterator&)noexcept=default;
		constexpr btree_iterator(btree_iterator&&)noexcept=default;
		constexpr btree_iterator& operator=(const btree_iterator&)noexcept=default;
		constexpr btree_iterator& operator=(btree_iterator&&)noexcept=default;
		constexpr ~btree_iterator()=default;
		constexpr void swap(btree_iterator& _v)noexcept {
			std::swap(_m_index,_v._m_index);
		}
		constexpr btree_iterator& operator++()noexcept { _next(); return *this; }
		constexpr btree_iterator operator++(int)noexcept { auto _t = *this; ++(*this); return _t; }
		constexpr btree_iterator& operator--()noexcept { _prev(); return *this; }
		constexpr btree_iterator operator--(int)noexcept { auto _t = *this; --(*this); return _t; }
		[[nodiscard]] constexpr reference operator->()const noexcept
		requires(has_mapped)
		{
			DEBUG_ASSERT(_m_index.data_node);
			return operator*();
		}
		[[nodiscard]] constexpr pointer operator->()const noexcept
		requires(!has_mapped)
		{
			DEBUG_ASSERT(_m_index.data_node);
			auto& _v = _m_index.data_node->value();
			return &_v.first;
		}
		[[nodiscard]] constexpr reference operator*()const noexcept
		{
			DEBUG_ASSERT(_m_index.data_node);
			auto& _v = _m_index.data_node->value();
			if constexpr (has_mapped) {
				return reference::unchecked_from(_v.first, _v.second);
			}
			else {
				return _v;
			}
		}
		[[nodiscard]] constexpr bool operator==(const btree_iterator& _other) const noexcept {
			return _m_index == _other._m_index;
		}
		[[nodiscard]] constexpr bool operator!=(const btree_iterator& _other) const noexcept {
			return !operator==(_other);
		}
	private:
		friend class iron::unsafe::btree::common<index_t>;
		constexpr void _next()noexcept { common_t::next(_m_index);}
		constexpr void _prev()noexcept { common_t::prev(_m_index);}
		index_t _m_index;
	};

}

#if defined(_MSC_VER)
#pragma warning(pop)
#endif
