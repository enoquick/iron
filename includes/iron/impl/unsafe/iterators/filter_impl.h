
namespace iron::unsafe::iterators {

	template<typename _ITER, typename _PREDICATE>
	class filter_iterator final {
	public:
		using iterator_concept = std::forward_iterator_tag;
		using iterator_category = std::forward_iterator_tag;
		using iter_type = _ITER;
		using iter_t = iter_type;
		using value_type = typename std::iterator_traits<iter_type>::value_type;
		using value_t = value_type;
		using difference_type = typename std::iterator_traits<iter_type>::difference_type;
		using diff_t = difference_type;
		using pointer = typename std::iterator_traits<iter_type>::pointer;
		using reference = typename std::iterator_traits<iter_type>::reference;
		using size_t = std::size_t;
		using size_type = size_t;
		using predicate_type = _PREDICATE;
		using predicate_t = predicate_type;
		static_assert(std::forward_iterator<iter_type>,"ITER parameter is not a fwd iterator");
		constexpr filter_iterator()noexcept = default;
		constexpr filter_iterator(iter_type _b, iter_type _e,predicate_type _pred)noexcept : _m_b(_b), _m_e(_e), _m_predicate(_pred)
		{
			_advance_to_valid();
		}
		constexpr filter_iterator(iter_type _e,predicate_type _pred)noexcept: _m_b(_e), _m_e(_e), _m_predicate(_pred) {}
		constexpr filter_iterator(const filter_iterator&)noexcept = default;
		constexpr filter_iterator& operator=(const filter_iterator&)noexcept = default;
		constexpr filter_iterator(filter_iterator&&)noexcept = default;
		constexpr filter_iterator& operator=(filter_iterator&&)noexcept = default;
		constexpr ~filter_iterator() = default;
		[[nodiscard]] constexpr reference operator*()const noexcept { return *_m_b;}
		[[nodiscard]] constexpr pointer operator->()const noexcept { return &(*_m_b);}
		constexpr filter_iterator& operator++()noexcept {
			++_m_b;
			_advance_to_valid();
			return *this;
		}
		constexpr filter_iterator operator++(int)noexcept {
			const auto temp = *this;
			++(*this);
			return temp;
		}
		[[nodiscard]] constexpr bool operator==(const filter_iterator& _other)const noexcept {
			return _m_b == _other._m_b;
		}
		[[nodiscard]] constexpr bool operator!=(const filter_iterator& other)const noexcept {
			return !(*this == other);
		}
		[[nodiscard]] constexpr iter_type& base()const noexcept { return _m_b; }
	private:
		constexpr void _advance_to_valid()noexcept {
			while (_m_b != _m_e && !_m_predicate(*_m_b))
			{
				++_m_b;
			}
		}
		iter_type _m_b{};
		iter_type _m_e{};
		predicate_type _m_predicate;
	};

	static_assert(std::forward_iterator<filter_iterator<contiguous_iterator<int>, decltype([](const int&)noexcept { return true; })>> , "internal error");
	static_assert(std::forward_iterator<filter_iterator<contiguous_iterator<const int>, decltype([](const int&)noexcept { return true; })>> , "internal error");

} //ns

