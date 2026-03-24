
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable:26481)
#endif

namespace iron::unsafe::str {
	class common;
}

namespace iron::unsafe::iterators {

	template <typename _T>
	class unicode_iterator final {
	public:
		static_assert(!std::is_reference_v<_T>, "iterator template parameter must not be a reference type");
		static_assert(!std::is_array_v<_T>, "iterator template parameter must not be an array type");
		using value_type = std::decay_t<_T>;
		using value_t = value_type;
		static_assert(iron::concepts::varying_charset<value_type>, "iterator template parameter must be a varlen character encoding");
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using diff_t = difference_type;
		using iterator_concept = std::forward_iterator_tag;
		using pointer = _T*;
		using reference = char32_t;
		using size_t = std::size_t;
		using size_type = size_t;
		struct unicode_iterator_tag { unicode_iterator_tag()=delete;};
	private:
		using _uc_t = iron::unsafe::unicode_conversions;
	public:
		static constexpr char32_t default_replacement_chr = _uc_t::default_replacement_chr;
		constexpr unicode_iterator() noexcept = default;
		constexpr explicit unicode_iterator(pointer _b,pointer _e)noexcept
		:_m_b(_b)
		,_m_e(_e)
		{
			_decode(); //set _m_current and _m_sz
		}
		constexpr unicode_iterator(const unicode_iterator&) noexcept = default;
		constexpr unicode_iterator(unicode_iterator&&)noexcept = default;
		constexpr unicode_iterator& operator=(const unicode_iterator& _oth)noexcept = default;
		constexpr unicode_iterator& operator=(unicode_iterator&&)noexcept = default;
		constexpr ~unicode_iterator() = default;
		[[nodiscard]] constexpr explicit operator bool()const noexcept { return !(_m_b == _m_e); }
		[[nodiscard]] constexpr const reference* operator->()const noexcept {
			DEBUG_ASSERT(*this);
			return &_m_current;
		}
		[[nodiscard]] constexpr reference operator*()const noexcept {
			DEBUG_ASSERT(*this);
			return _m_current;
		}
		constexpr unicode_iterator& operator++()noexcept {
			DEBUG_ASSERT(*this);
			_next();
			return *this;
		}
		constexpr unicode_iterator operator++(int)noexcept {
			auto _temp = *this;
			++(*this);
			return _temp;
		}
		[[nodiscard]] constexpr bool operator==(const unicode_iterator& _other) const noexcept {
			return _m_b == _other._m_b;
		}
		[[nodiscard]] constexpr bool operator!=(const unicode_iterator& _other) const noexcept {
			return _m_b != _other._m_b;
		}
		[[nodiscard]] constexpr bool operator<(const unicode_iterator& _other) const noexcept {
			return _m_b < _other._m_b;
		}
		[[nodiscard]] constexpr bool operator<=(const unicode_iterator& _other) const noexcept {
			return _m_b <= _other._m_b;
		}
		[[nodiscard]] constexpr bool operator>(const unicode_iterator& _other) const noexcept {
			return _m_b > _other._m_b;
		}
		[[nodiscard]]constexpr bool operator>=(const unicode_iterator& _other) const noexcept {
			return _m_b >= _other._m_b;
		}
		[[nodiscard]] constexpr auto operator<=>(const unicode_iterator& _other) const noexcept {
			return _m_b <=> _other._m_b;
		}
	private:
		friend class iron::unsafe::str::common;
		constexpr void _decode() {
			auto _b = _m_b;
			if constexpr (iron::concepts::utf8_charset<value_t>) {
				if (!_uc_t::utf8_to_utf32(_m_current, _b, _m_e)) {
					_m_current = default_replacement_chr;
				}
			}
			else if constexpr (iron::concepts::utf16_charset<value_t>) {
				if (!_uc_t::utf16_to_utf32(_m_current, _b, _m_e)) {
					_m_current = default_replacement_chr;
				}
			}
			else {
				static_assert(iron::unsafe::asserts::always_false<value_t>, "internal error");
			}
			_m_sz = _b - _m_b;
		}
		constexpr void _next() {
			_m_b += _m_sz;
			_decode();
		}
		pointer _m_b{};
		pointer _m_e{};
		char32_t _m_current = default_replacement_chr;
		diff_t _m_sz{};
	};
}

#if defined(_MSC_VER)
#pragma warning(pop)
#endif


