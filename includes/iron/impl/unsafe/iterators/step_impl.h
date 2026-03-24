
namespace iron::unsafe::iterators {

	template<typename _ITER>
	class step_iterator final {
	public:
		using iterator_concept = std::forward_iterator_tag;
		using iterator_category = std::forward_iterator_tag;
		using iter_type = _ITER;
		using value_type = typename std::iterator_traits<iter_type>::value_type;
		using difference_type = typename std::iterator_traits<iter_type>::difference_type;
		using pointer = typename std::iterator_traits<iter_type>::pointer;
		using reference = typename std::iterator_traits<iter_type>::reference;
		using size_type = std::size_t;
		using size_t = size_type;
		static_assert(std::forward_iterator<iter_type>,"ITER parameter is not a fwd iterator");
		constexpr step_iterator()noexcept = default;
		constexpr step_iterator(iter_type _b, iter_type _e,size_type _step)noexcept : _m_b(_b), _m_e(_e), _m_step(_step - 1)
		{
			DEBUG_ASSERT(_step);
		//	_advance_to_valid();
		}
		constexpr step_iterator(iter_type _e)noexcept: _m_b(_e), _m_e(_e) {}
		constexpr step_iterator(const step_iterator&)noexcept = default;
		constexpr step_iterator& operator=(const step_iterator&)noexcept = default;
		constexpr step_iterator(step_iterator&&)noexcept = default;
		constexpr step_iterator& operator=(step_iterator&&)noexcept = default;
		constexpr ~step_iterator() = default;
		[[nodiscard]] constexpr reference operator*()const noexcept { return *_m_b;}
		[[nodiscard]] constexpr pointer operator->()const noexcept { return &(*_m_b);}
		constexpr step_iterator& operator++()noexcept {
			++_m_b;
			_advance_to_valid();
			return *this;
		}
		constexpr step_iterator operator++(int)noexcept {
			const auto temp = *this;
			++(*this);
			return temp;
		}
		[[nodiscard]] constexpr bool operator==(const step_iterator& _other)const noexcept {
			return _m_b == _other._m_b;
		}
		[[nodiscard]] constexpr bool operator!=(const step_iterator& other)const noexcept {
			return !(*this == other);
		}
		[[nodiscard]] constexpr iter_type& base()const noexcept { return _m_b; }
	private:
		constexpr void _advance_to_valid()noexcept {
			auto _step=_m_step;
			while (_step-- && _m_b != _m_e) {
				++_m_b;
			}
		}
		iter_type _m_b{};
		iter_type _m_e{};
		size_type _m_step{};
	};

	static_assert(std::forward_iterator<step_iterator<contiguous_iterator<int>>> , "internal error");
	static_assert(std::forward_iterator<step_iterator<contiguous_iterator<const int>>> , "internal error");

} //ns

