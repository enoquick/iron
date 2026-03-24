
namespace iron::unsafe::iterators {

	template<typename _ITER,typename _ENUMERATE,typename _REFERENCE>
	class enumerate_iterator final {
	public:
		using iterator_concept = std::forward_iterator_tag;
		using iterator_category = std::forward_iterator_tag;
		using iter_type = _ITER;
		using iter_t = iter_type;
		using enumerate_type = _ENUMERATE;
		using enumerate_t = enumerate_type;
		using value_type = _REFERENCE;
		using value_t = value_type;
		using difference_type = typename std::iterator_traits<iter_type>::difference_type;
		using diff_t = difference_type;
		using pointer = value_type*;
		using reference = _REFERENCE;
		using size_type = std::size_t;
		using size_t = size_type;
		static constexpr bool is_ref = std::is_lvalue_reference_v<decltype(*std::declval<iter_type>())>;
		static_assert(std::forward_iterator<iter_type>,"ITER parameter is not a fwd iterator");
		constexpr enumerate_iterator()noexcept = default;
		template <typename _MASTER>
		constexpr enumerate_iterator(const _MASTER& _master,iter_type _iter, enumerate_type _start)noexcept
#if IRON_SECURITY_CHECKER
		requires(iron::concepts::has_security_checker<_MASTER>)
#endif 
		: _m_iter(_iter)
		, _m_start(_start)
		{
			IRON_SECURITY_MAKE_REF(_master);
		}
		constexpr enumerate_iterator(iter_type _iter)noexcept : _m_iter(_iter) {}
		constexpr enumerate_iterator(const enumerate_iterator&)noexcept = default;
		constexpr enumerate_iterator& operator=(const enumerate_iterator&)noexcept = default;
		constexpr enumerate_iterator(enumerate_iterator&&)noexcept = default;
		constexpr enumerate_iterator& operator=(enumerate_iterator&&)noexcept = default;
		constexpr ~enumerate_iterator() = default;
		[[nodiscard]] constexpr reference operator*()const noexcept {
			if constexpr (is_ref) {
				return reference(_m_start, iron::make_ref(*this, *_m_iter));
			}
			else {
				return reference(_m_start, *_m_iter);
			}

		}
		constexpr enumerate_iterator& operator++()noexcept {
			++_m_iter;
			if (_m_start==std::numeric_limits<enumerate_type>::max()) {
				_m_start=std::numeric_limits<enumerate_type>::min();
			}
			else {
				++_m_start;
			}
			return *this;
		}
		constexpr enumerate_iterator operator++(int)noexcept {
			const auto temp = *this;
			++(*this);
			return temp;
		}
		[[nodiscard]] constexpr bool operator==(const enumerate_iterator& _other)const noexcept {
			return _m_iter == _other._m_iter;
		}
		[[nodiscard]] constexpr bool operator!=(const enumerate_iterator& other)const noexcept {
			return !(*this == other);
		}
		[[nodiscard]] constexpr iter_type& base()const noexcept { return _m_iter; }
		IRON_SECURITY_METHOD()
	private:
		iter_type _m_iter{};
		enumerate_type _m_start{};
		IRON_SECURITY_DCL();
	};

} //ns

