

namespace iron::unsafe::iterators {

	template <typename _T, typename _RAW_LIST>
	class list_iterator;

	namespace _impl {
		class _node {
		public:
			template <typename _LIST_ITERATOR>
			[[nodiscard]] static constexpr auto node(_LIST_ITERATOR& _iter)noexcept {
				return _iter._node();
			}
		};
	}

	template <typename _T,typename _RAW_LIST>
	class list_iterator {
	public:
		using raw_list_t = _RAW_LIST;
		using raw_list_type = raw_list_t;
		static_assert(iron::concepts::raw_list<raw_list_t>, "list_iterator raw_list_t is not a raw_list");
		using raw_list_ref_t = iron::ref_wrapper<raw_list_t>;
		using raw_list_ref_type = raw_list_ref_t;
		using node_t = typename raw_list_t::node_t;
		using value_type = typename raw_list_t::value_t;
		using value_t = value_type;
		static_assert(std::is_same_v<std::decay_t<_T>, value_t>, "list_iterator not match value type ");
		using allocator_t = typename raw_list_t::allocator_t;
		using allocator_type = allocator_t;
		constexpr static auto is_bidirectional = raw_list_t::is_bidirectional;
		using iterator_category = std::conditional_t<is_bidirectional, std::bidirectional_iterator_tag, std::forward_iterator_tag>;
		using iterator_concept = iterator_category;
		using difference_type = std::ptrdiff_t;
		using diff_t = difference_type;
		using pointer = _T*;
		using reference = _T&;
		using size_t = typename node_t::size_t;
		using size_type = size_t;
		struct list_iterator_tag { list_iterator_tag() = delete; };
		constexpr list_iterator()noexcept = default;
		constexpr list_iterator(node_t* _node,raw_list_t& _data,bool _tail_activate)noexcept
		: _m_node(_node)
		, _m_tail()
		{
			if constexpr (is_bidirectional) {
				_m_tail._data = &_data;
				_m_tail._activate = _tail_activate;
			}
		}
		constexpr list_iterator(const list_iterator&)noexcept=default;
		constexpr list_iterator(list_iterator&&)noexcept=default;
		constexpr list_iterator& operator=(const list_iterator&)noexcept=default;
		constexpr list_iterator& operator=(list_iterator&&)noexcept=default;
		constexpr ~list_iterator()=default;
		constexpr void swap(list_iterator& _v)noexcept {
			std::swap(_m_node,_v._m_node);
			if constexpr (is_bidirectional) {
				std::swap(_m_tail, _v._m_tail);
			}
		}
		constexpr list_iterator& operator++()noexcept { _next(); return *this; }
		constexpr list_iterator operator++(int)noexcept { auto _t=*this; ++(*this); return _t; }
		constexpr list_iterator& operator--()noexcept
		requires(is_bidirectional)
		{
			_prev();
			return *this;
		}
		constexpr list_iterator operator--(int)noexcept
		requires(is_bidirectional)
		{
			auto _t=*this;
			--(*this);
			return _t;
		}
		[[nodiscard]] constexpr pointer operator->()const noexcept {
			return &(_m_node->value());
		}
		[[nodiscard]] constexpr reference operator*()const noexcept
		{
			DEBUG_ASSERT(_m_node);
			return _m_node->value();
		}
		[[nodiscard]] constexpr bool operator==(const list_iterator& _other) const noexcept {
			return _m_node == _other._m_node;
		}
		[[nodiscard]] constexpr bool operator!=(const list_iterator& _other) const noexcept {
			return _m_node != _other._m_node;
		}
	private:
		friend class _impl::_node;
		[[nodiscard]] constexpr  node_t* _node()noexcept { return _m_node; }
		[[nodiscard]] constexpr const node_t* _node()const noexcept { return _m_node; }
		constexpr void _next()noexcept
		{
			DEBUG_ASSERT(_m_node);
			_m_node = _m_node->next();
			if constexpr (is_bidirectional) {
				if (!_m_node) IRON_UNLIKELY
				{
					DEBUG_ASSERT(_m_tail._data);
					_m_tail._activate = true;
				}
			}
		}
		constexpr void _prev()noexcept
		requires(is_bidirectional)
		{
			if (_m_node) IRON_LIKELY
			{
				_m_node = _m_node->prev();
			}
			else if (_m_tail._activate) IRON_LIKELY
			{
				DEBUG_ASSERT(_m_tail._data);
				_m_node = _m_tail._data->back();
				_m_tail._activate = {};
			}
			else {
				DEBUG_ASSERT(0);
			}
		}
		node_t* _m_node{};
		typedef struct _struct_tail_empty {} _tail_empty_t;
		typedef struct _struct_tail {
			raw_list_t* _data{};
			bool _activate{};
		} _tail_bd_t;
		using _tail_t = std::conditional_t<is_bidirectional, _tail_bd_t, _tail_empty_t>;
		IRON_NO_UNIQUE_ADDRESS _tail_t _m_tail{};
	};

} //ns

static_assert(
		std::forward_iterator<
			iron::unsafe::iterators::list_iterator<
				int
				, iron::unsafe::raw_list<iron::unsafe::list_node<int,false>, iron::unsafe::allocator<int>>
			>
		>, "invalid"
	);

static_assert(
	std::bidirectional_iterator<
	iron::unsafe::iterators::list_iterator<
				int
				, iron::unsafe::raw_list<iron::unsafe::list_node<int,true>, iron::unsafe::allocator<int>>
			>
		>, "invalid"
	);

