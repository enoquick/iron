
namespace iron::unsafe::iterators {

	template<typename _ITER>
	class take_iterator final {
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
		using size_type = std::size_t;
		static_assert(std::forward_iterator<iter_type>,"ITER parameter is not a fwd iterator");
		constexpr take_iterator()noexcept = default;
		constexpr take_iterator(iter_type _b, iter_type _e,size_type _take)noexcept : _m_b(_take ? _b : _e), _m_e(_e), _m_take(_take) {}
		constexpr take_iterator(iter_type _e)noexcept: _m_b(_e), _m_e(_e) {}
		constexpr take_iterator(const take_iterator&)noexcept = default;
		constexpr take_iterator& operator=(const take_iterator&)noexcept = default;
		constexpr take_iterator(take_iterator&&)noexcept = default;
		constexpr take_iterator& operator=(take_iterator&&)noexcept = default;
		constexpr ~take_iterator() = default;
		[[nodiscard]] constexpr reference operator*()const noexcept { return *_m_b;}
		[[nodiscard]] constexpr pointer operator->()const noexcept { return &(*_m_b);}
		constexpr take_iterator& operator++()noexcept {
			if (!(_m_b == _m_e)) {
				if (_m_take) {
					++_m_b;
					--_m_take;
				}
				if (!_m_take) {
					_m_b = _m_e;
				}
			}
			return *this;
		}
		constexpr take_iterator operator++(int)noexcept {
			const auto temp = *this;
			++(*this);
			return temp;
		}
		[[nodiscard]] constexpr bool operator==(const take_iterator& _other)const noexcept {
			return _m_b == _other._m_b;
		}
		[[nodiscard]] constexpr bool operator!=(const take_iterator& other)const noexcept {
			return !(*this == other);
		}
		[[nodiscard]] constexpr iter_type& base()const noexcept { return _m_b; }
	private:
		iter_type _m_b{};
		iter_type _m_e{};
		size_type _m_take{};
	};

	static_assert(std::forward_iterator<take_iterator<contiguous_iterator<int>>> , "internal error");
	static_assert(std::forward_iterator<take_iterator<contiguous_iterator<const int>>> , "internal error");

} //ns

