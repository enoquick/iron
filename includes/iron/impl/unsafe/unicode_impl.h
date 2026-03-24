
namespace iron::unsafe {

	class unicode final {
	public:
		using u32_t = std::uint32_t;
		using u8_t = std::uint8_t;
		unicode() = delete;
		typedef enum class attribute : u32_t {
			numeric
			, alphabetic
			, lowercase
			, uppercase
			, whitespace
			, alphanumeric
			, control
			, ascii
			, ascii_digit
			, ascii_hexdigit
			, ascii_punctuation
			, ascii_graphic
			, not_unicode
			, first = numeric
			, last = not_unicode
		} attr_t;
		using version_t = std::tuple<u8_t, u8_t, u8_t>;
		using iter_t = iron::unsafe::iterators::contiguous_iterator<const u32_t>;
		using seq_t = std::pair<iter_t, iter_t>;
		[[nodiscard]] static constexpr u32_t min()noexcept { return {}; }
		[[nodiscard]] static constexpr u32_t max()noexcept { return 0x0010fffe;}
		[[nodiscard]] static constexpr version_t unicode_version()noexcept { return std::make_tuple(u8_t{ 14U }, u8_t{}, u8_t{}); }
		[[nodiscard]] static bool is(u32_t _v, attr_t _a)noexcept { return _is(_v, _a); }
		[[nodiscard]] static constexpr bool is_ascii(u32_t _v)noexcept {
			const auto _b = _is_ascii(_v);
			if (!std::is_constant_evaluated()) {
				DEBUG_ASSERT_EQ(_is(_v, attr_t::ascii), _b);
			}
			return _b;
		}
		[[nodiscard]] static constexpr bool is_ascii_digit(u32_t _v)noexcept {
			const auto _b = _v > 0x2F && _v < 0x3A;
			if (!std::is_constant_evaluated()) {
				DEBUG_ASSERT_EQ(_is(_v, attr_t::ascii_digit), _b);
			}
			return _b;
		}
		[[nodiscard]] static constexpr bool is_ascii_hexdigit(u32_t _v)noexcept {
			const auto _b = is_ascii_digit(_v)
				|| (_v > 0x40 && _v < 0x47)
				|| (_v > 0x60 && _v < 0x67)
				;
			if (!std::is_constant_evaluated()) {
				DEBUG_ASSERT_EQ(_is(_v, attr_t::ascii_hexdigit), _b);
			}
			return _b;
		}
		[[nodiscard]] static constexpr bool is_ascii_punctuation(u32_t _v)noexcept {
			const auto _b = (_v > 0x20 && _v < 0x30)
				|| (_v > 0x39 && _v < 0x41)
				|| (_v > 0x5A && _v < 0x61)
				|| (_v > 0x7A && _v < 0x7F)
				;
			if (!std::is_constant_evaluated()) {
				DEBUG_ASSERT_EQ(_is(_v, attr_t::ascii_punctuation), _b);
			}
			return _b;
		}
		[[nodiscard]] static constexpr bool is_ascii_graphic(u32_t _v)noexcept {
			const auto _b = _v > 0x20 && _v < 0x7F;
			if (!std::is_constant_evaluated()) {
				DEBUG_ASSERT_EQ(_is(_v, attr_t::ascii_graphic), _b);
			}
			return _b;
		}
		[[nodiscard]] static constexpr bool is_ascii_whitespace(u32_t _v)noexcept {
			const auto _b = _v == 0x20 || (_v > 0x08 && _v < 0x0E);
			if (!std::is_constant_evaluated()) {
				DEBUG_ASSERT_EQ(is_ascii(_v) && _is(_v, attr_t::whitespace), _b);
			}
			return _b;
		}
		[[nodiscard]] static constexpr bool is_ascii_lowercase(u32_t _v)noexcept {
			const auto _b = _v > 0x60 && _v < 0x7B;
			if (!std::is_constant_evaluated()) {
				DEBUG_ASSERT_EQ(is_ascii(_v) && _is(_v, attr_t::lowercase), _b);
			}
			return _b;
		}
		[[nodiscard]] static constexpr bool is_ascii_uppercase(u32_t _v)noexcept {
			const auto _b = _v > 0x40 && _v < 0x5B;
			if (!std::is_constant_evaluated()) {
				DEBUG_ASSERT_EQ(is_ascii(_v) && _is(_v, attr_t::uppercase), _b);
			}
			return _b;
		}
		[[nodiscard]] static constexpr bool is_ascii_alphabetic(u32_t _v)noexcept {
			const auto _b = is_ascii_lowercase(_v) || is_ascii_uppercase(_v);
			if (!std::is_constant_evaluated()) {
				DEBUG_ASSERT_EQ(is_ascii(_v) && _is(_v, attr_t::alphabetic), _b);
			}
			return _b;
		}
		[[nodiscard]] static constexpr bool is_ascii_alphanumeric(u32_t _v)noexcept {
			const auto _b = is_ascii_alphabetic(_v) || is_ascii_digit(_v);
			if (!std::is_constant_evaluated()) {
				DEBUG_ASSERT_EQ(is_ascii(_v) && _is(_v, attr_t::alphanumeric), _b);
			}
			return _b;
		}
		[[nodiscard]] static constexpr bool is_ascii_control(u32_t _v)noexcept {
			const auto _b = _v < 0x20 || _v == 0x7F;
			if (!std::is_constant_evaluated()) {
				DEBUG_ASSERT_EQ(is_ascii(_v) && _is(_v, attr_t::control), _b);
			}
			return _b;
		}
		[[nodiscard]] static seq_t uppers(u32_t _v)noexcept { return _uppers(_v); }
		[[nodiscard]] static seq_t lowers(u32_t _v)noexcept { return _lowers(_v); }
	private:
		[[nodiscard]] _IRON_DLL_EXPORT static bool _is(u32_t, attr_t)noexcept;
		[[nodiscard]] _IRON_DLL_EXPORT static seq_t _uppers(u32_t)noexcept;
		[[nodiscard]] _IRON_DLL_EXPORT static seq_t _lowers(u32_t)noexcept;
		[[nodiscard]] static constexpr bool _is_ascii(u32_t _v)noexcept { return _v < 128U; }
		class pimp;
	};

}

