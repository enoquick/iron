
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable:26429)
#pragma warning (disable:26472)
#pragma warning (disable:26481)
#endif

namespace iron {

	class chr final {
	private:
		using _uc_t = iron::unsafe::unicode;
		using _ucc_t = iron::unsafe::unicode_conversions;
	public:
		struct chr_tag { chr_tag() = delete; };
		using u8_t = std::uint8_t;
		using u32_t = std::uint32_t;
		using size_t = std::size_t;
		using radix_t = u32_t;
		using unicode_version_t = _uc_t::version_t;
		static constexpr char32_t max_chr=_uc_t::max();
		static constexpr char32_t default_replacement_chr=_ucc_t::default_replacement_chr;
		template <typename _T>
		[[nodiscard]] static constexpr bool is_unicode(_T _v)noexcept 
		requires(iron::concepts::utf32_charset<_T>)
		{
			return _is_unicode(static_cast<char32_t>(_v));
		}
		template <typename _T>
		[[nodiscard]] static constexpr bool is_ascii(_T _v)noexcept 
		requires(iron::concepts::utf32_charset<_T>)
		{ 
			return _ucc_t::is_basic_charset(_v); 
		}
		[[nodiscard]] static constexpr unicode_version_t unicode_version() { return _uc_t::unicode_version(); }
		static constexpr radix_t default_radix = 10;
		static constexpr radix_t min_radix = 2;
		static constexpr radix_t max_radix = 36;
		[[nodiscard]] static constexpr bool is_radix(radix_t _r)noexcept { return _r >= min_radix && _r <= max_radix; }
		constexpr chr()noexcept = default;
		constexpr chr(const chr& _v)noexcept
		: _m_c(_v._m_c)
		{
			IRON_SECURITY_CHECK_TEMPORARY(_v);
		}
		constexpr chr(chr&& _v)noexcept
			: _m_c(_v._m_c)
		{
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			IRON_SECURITY_MARK_TEMPORARY(_v);
		}
		constexpr chr& operator=(const chr& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			auto _t = _v;
			_swap(_t);
			return *this;
		}
		constexpr chr& operator=(chr&& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_swap(_v);
			IRON_SECURITY_MARK_TEMPORARY(_v);
			return *this;
		}
		constexpr ~chr() = default;
		constexpr void swap(chr& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_swap(_v);
		}

		[[nodiscard]] constexpr auto operator<=>(const chr& _v)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			return _m_c <=> _v._m_c;
		}
		[[nodiscard]] constexpr bool operator==(const chr& _v)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			return _m_c == _v._m_c;
		}

		template <typename _T>
		[[nodiscard]] static constexpr iron::option<chr> from(_T _v)noexcept 
		requires(iron::concepts::utf32_charset<_T>)
		{
			return from_u32(static_cast<u32_t>(_v));
		}
		[[nodiscard]] static constexpr iron::option<chr> from(char _c)noexcept {
#ifdef IRON_EBCDIC
#error _EBCDIC not implemented
#else 
			const auto _ch = static_cast<char32_t>(_c);
			if (is_ascii(_ch)) IRON_LIKELY
			{
				return iron::some(chr(_ch));
			}
			return {};
#endif 
		}

		[[nodiscard]] static constexpr iron::option<chr> from_u32(u32_t _u)noexcept {
			if (_is_unicode(static_cast<char32_t>(_u))) IRON_LIKELY
			{
				return iron::some(unchecked_from_u32(_u));
			}
			return {};
		}

		template <typename _T>
		[[nodiscard]] static constexpr chr unchecked_from(_T _c)noexcept
		requires(iron::concepts::utf32_charset<_T>)
		{
			return chr(static_cast<char32_t>(_c));
		}
		[[nodiscard]] static constexpr chr unchecked_from(char _c)noexcept
		{
			return chr(static_cast<char32_t>(_c));
		}

		[[nodiscard]] static constexpr chr unchecked_from_u32(u32_t _v)noexcept { 
			return chr(_v); 
		}
		[[nodiscard]] static constexpr chr from_u32(u32_t _v, u32_t _v1)noexcept {
			if (_is_unicode(static_cast<char32_t>(_v))) IRON_LIKELY
			{
				return unchecked_from_u32(_v);
			}
			PANIC_UNLESS(_is_unicode(static_cast<char32_t>(_v1)), U"invalid utf32 char");
			return unchecked_from_u32(_v1);
		}

		template <typename _T1,typename _T2>
		[[nodiscard]] static constexpr chr from(_T1 _c1,_T2 _c2)noexcept 
		requires(iron::concepts::utf32_charset<_T1> && iron::concepts::utf32_charset<_T2>)
		{ 
			return from_u32(static_cast<u32_t>(_c1), static_cast<u32_t>(_c2)); 
		}

		[[nodiscard]] static constexpr iron::option<chr> from_digit(u32_t _num, radix_t _radix)noexcept;
		[[nodiscard]] static constexpr iron::option<chr> from_digit(radix_t _r)noexcept {
			if (std::is_constant_evaluated()) {
				return _from_digit_c(_r);
			}
			else {
				return _from_digit_nc(_r);
			}
		}
	
		[[nodiscard]] constexpr u32_t to_u32()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _to_u32();
		}
		template <typename _T=char32_t>
		[[nodiscard]] constexpr _T to_utf32()const noexcept 
		requires(iron::concepts::utf32_charset<_T>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return static_cast<_T>(_m_c);
		}
		[[nodiscard]] constexpr iron::option<char> to_basic()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			if (_is_ascii()) IRON_LIKELY
			{
#ifdef IRON_EBCDIC
#error EBCDIC not implemented
//convert ascii in ebcdic 
#else 

				return iron::some(static_cast<char>(_m_c));
#endif 
			}
			return{};
		}
		[[nodiscard]] constexpr iron::option<u32_t> to_digit(radix_t _radix)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			PANIC_IF(_radix < min_radix, "chr::from_digit() : radix is too low(minimun 2)");
			PANIC_IF(_radix > max_radix, "chr::from_digit(): radix is too high (maximum 36)");
			if (std::is_constant_evaluated()) {
				return _to_digit_c(_m_c, _radix);
			}
			else {
				return _to_digit_nc(_m_c, _radix);
			}
		}

		[[nodiscard]] constexpr bool is_unicode()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return is_unicode(_m_c);
		}
		[[nodiscard]] constexpr bool is_digit(radix_t _radix)const noexcept { 
			return to_digit(_radix).is_some(); 
		}
		[[nodiscard]] bool is_numeric()const noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _uc_t::is(_to_u32(), _uc_t::attr_t::numeric); 
		}
		[[nodiscard]] bool is_alphabetic()const noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _uc_t::is(_to_u32(), _uc_t::attr_t::alphabetic); 
		}
		[[nodiscard]] bool is_lowercase()const noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _uc_t::is(_to_u32(), _uc_t::attr_t::lowercase); 
		}
		[[nodiscard]] bool is_uppercase()const noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _uc_t::is(_to_u32(), _uc_t::attr_t::uppercase); 
		}
		[[nodiscard]] bool is_whitespace()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _uc_t::is(_to_u32(), _uc_t::attr_t::whitespace); 
		}
		[[nodiscard]] bool is_alphanumeric()const noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _uc_t::is(_to_u32(), _uc_t::attr_t::alphanumeric); 
		}
		[[nodiscard]] bool is_control()const noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _uc_t::is(_to_u32(), _uc_t::attr_t::control); 
		}
		[[nodiscard]] constexpr bool is_ascii()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _is_ascii();
		}
		[[nodiscard]] constexpr bool is_ascii_alphabetic()const noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _uc_t::is_ascii_alphabetic(_to_u32()); 
		}
		[[nodiscard]] constexpr bool is_ascii_uppercase()const noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _uc_t::is_ascii_uppercase(_to_u32()); 
		}
		[[nodiscard]] constexpr bool is_ascii_lowercase()const noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _uc_t::is_ascii_lowercase(_to_u32()); 
		}
		[[nodiscard]] constexpr bool is_ascii_alphanumeric()const noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _uc_t::is_ascii_alphanumeric(_to_u32()); 
		}
		[[nodiscard]] constexpr bool is_ascii_digit()const noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _uc_t::is_ascii_digit(_to_u32()); 
		}
		[[nodiscard]] constexpr bool is_ascii_hexdigit()const noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _uc_t::is_ascii_hexdigit(_to_u32()); 
		}
		[[nodiscard]] constexpr bool is_ascii_punctuation()const noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _uc_t::is_ascii_punctuation(_to_u32()); 
		}
		[[nodiscard]] constexpr bool is_ascii_graphic()const noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _uc_t::is_ascii_graphic(_to_u32()); 
		}
		[[nodiscard]] constexpr bool is_ascii_whitespace()const noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _uc_t::is_ascii_whitespace(_to_u32()); 
		}
		[[nodiscard]] constexpr bool is_ascii_control()const noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _uc_t::is_ascii_control(_to_u32()); 
		}
		[[nodiscard]] constexpr size_t  size_utf8()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			if (std::is_constant_evaluated()) {
				return _size_utf8_c(_m_c);
			}
			else {
				return _size_utf8_nc(_m_c);
			}
		}
		template <size_t _N>
		[[nodiscard]] constexpr size_t encode_utf8(char8_t(&_buffer)[_N])const noexcept
		requires(_N > 4)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _encode_utf8(_buffer, _m_c);
		}

		[[nodiscard]] constexpr size_t  size_utf16()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			if (std::is_constant_evaluated()) {
				return _size_utf16_c(_m_c);
			}
			else {
				return _size_utf16_nc(_m_c);
			}
		}
		template <typename _T,size_t _N>
		[[nodiscard]] constexpr size_t encode_utf16(_T(&_buffer)[_N])const noexcept
		requires (_N > 2 && iron::concepts::utf16_charset<_T>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _encode_utf16(_buffer, _m_c);
		}
		[[nodiscard]] constexpr chr to_ascii_uppercase()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _to_ascii_uppercase();
		}
		[[nodiscard]] constexpr chr to_ascii_lowercase()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _to_ascii_lowercase();
		}
		[[nodiscard]] constexpr bool eq_ignore_ascii_case(const chr& _v)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			return _m_c == _v._m_c || _to_ascii_lowercase() == _v._to_ascii_lowercase();
		}
		[[nodiscard]] constexpr bool lt_ignore_ascii_case(const chr& _v)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			return _to_ascii_lowercase()._m_c < _v._to_ascii_lowercase()._m_c;
		}
		constexpr chr& make_ascii_uppercase()noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			_m_c = _to_ascii_uppercase()._m_c; 
			return *this; 
		}
		constexpr chr& make_ascii_lowercase()noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			_m_c = _to_ascii_lowercase()._m_c; 
			return *this; 
		}
		[[nodiscard]] constexpr const chr max(const chr& _v)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			return _m_c > _v._m_c ? *this : _v;
		}
		[[nodiscard]] constexpr const chr min(const chr& _v)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			return _m_c < _v._m_c ? *this : _v;
		}
		[[nodiscard]] constexpr bool eq_ignore_case(const chr& _v)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			return _m_c == _v._m_c || (_is_ascii() && _v.is_ascii() ? _to_ascii_uppercase() == _v._to_ascii_uppercase() : _equal_case_insensitive(*this, _v));
		}
		[[nodiscard]] constexpr bool lt_ignore_case(const chr& _v)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			return _m_c != _v._m_c && (_is_ascii() && _v.is_ascii() ? _to_ascii_uppercase() < _v._to_ascii_uppercase() : _less_case_insensitive(*this, _v));
		}
		class sequence;
		using seq_t = sequence;
		[[nodiscard]] seq_t to_lowercase()const noexcept;
		[[nodiscard]] seq_t to_uppercase()const noexcept;
		[[nodiscard]] seq_t escape_unicode()const noexcept;
		[[nodiscard]] seq_t escape_debug()const noexcept;
		[[nodiscard]] seq_t escape_default()const noexcept;
		IRON_SECURITY_METHOD()
	private:
		[[nodiscard]] static constexpr bool _is_unicode(char32_t _v)noexcept {
			if (std::is_constant_evaluated()) {
				return _size_utf8_c(_v) > 0;
			}
			else {
				return _size_utf8_nc(_v) > 0;
			}
		}
		[[nodiscard]] constexpr u32_t _to_u32()const noexcept {
			return static_cast<u32_t>(_m_c);
		}
		[[nodiscard]] constexpr bool _is_ascii()const noexcept {
			return _uc_t::is_ascii(_to_u32());
		}
		[[nodiscard]] static constexpr iron::option<u32_t> _to_digit_c(char32_t, radix_t)noexcept;
		[[nodiscard]] _IRON_DLL_EXPORT static iron::option<u32_t> _to_digit_nc(char32_t, radix_t)noexcept;
		[[nodiscard]] static constexpr size_t _size_utf8_c(char32_t _ch)noexcept {
			return iron::unsafe::unicode_conversions::size_utf8(_ch);
		}
		[[nodiscard]] _IRON_DLL_EXPORT static size_t _size_utf8_nc(char32_t)noexcept;
		template <size_t _N>
		[[nodiscard]] static constexpr size_t _encode_utf8_c(char8_t(&_buffer)[_N], char32_t _ch)noexcept {
			auto _sz = iron::unsafe::unicode_conversions::utf32_to_utf8(_buffer, _ch);
			if (!_sz) IRON_UNLIKELY
			{
				_sz = iron::unsafe::unicode_conversions::utf32_to_utf8(_buffer, default_replacement_chr);
				DEBUG_ASSERT(_sz);
			}
			return _sz;
		}
		[[nodiscard]] _IRON_DLL_EXPORT static size_t _encode_utf8_nc(char8_t*, char32_t)noexcept;
		template <size_t _N>
		[[nodiscard]] static constexpr size_t _encode_utf8(char8_t(&_buffer)[_N], char32_t _ch)noexcept {
			if (std::is_constant_evaluated()) {
				return _encode_utf8_c(_buffer, _ch);
			}
			else {
				return _encode_utf8_nc(&_buffer[0], _ch);
			}
		}
		[[nodiscard]] static constexpr size_t  _size_utf16_c(char32_t _ch)noexcept {
			return iron::unsafe::unicode_conversions::size_utf16(_ch);
		}
		[[nodiscard]] _IRON_DLL_EXPORT static size_t  _size_utf16_nc(char32_t)noexcept;

		template <typename _T, size_t _N>
		[[nodiscard]] static constexpr size_t _encode_utf16_c(_T(&_buffer)[_N], char32_t _ch)noexcept {
			auto _sz = iron::unsafe::unicode_conversions::utf32_to_utf16(_buffer, _ch);
			if (!_sz) IRON_UNLIKELY
			{
				_sz = iron::unsafe::unicode_conversions::utf32_to_utf16(_buffer, default_replacement_chr);
				DEBUG_ASSERT(_sz);
			}
			return _sz;
		}
		template <typename _T>
		[[nodiscard]] static size_t _encode_utf16_nc(_T*, char32_t)noexcept;

		template <typename _T, size_t _N>
		[[nodiscard]] static constexpr size_t _encode_utf16(_T(&_buffer)[_N],char32_t _ch)noexcept
		{
			if (std::is_constant_evaluated()) {
				return _encode_utf16_c(_buffer, _ch);
			}
			else {
				return _encode_utf16_nc(&_buffer[0], _ch);
			}
		}

		[[nodiscard]] _IRON_DLL_EXPORT static seq_t _to_lowercase(char32_t)noexcept;
		[[nodiscard]] _IRON_DLL_EXPORT static seq_t _to_uppercase(char32_t)noexcept;
		[[nodiscard]] _IRON_DLL_EXPORT static bool _equal_case_insensitive(const chr&, const chr&)noexcept;
		[[nodiscard]] _IRON_DLL_EXPORT static bool _less_case_insensitive(const chr&, const chr&)noexcept;
		[[nodiscard]] constexpr chr _to_ascii_uppercase()const noexcept;
		[[nodiscard]] constexpr chr _to_ascii_lowercase()const noexcept;
		[[nodiscard]] static constexpr iron::option<chr> _from_digit_c(radix_t _r)noexcept;
		[[nodiscard]] _IRON_DLL_EXPORT static iron::option<chr> _from_digit_nc(radix_t _r)noexcept;
		class pimp;
		constexpr chr(char32_t _c)noexcept : _m_c(_c) {}
		constexpr void _swap(chr& _v)noexcept {
			std::swap(_m_c, _v._m_c);
			IRON_SECURITY_SWAP(_v);
		}
		char32_t _m_c{};
		IRON_SECURITY_DCL();
	};

	template <> _IRON_DLL_EXPORT auto  chr::_encode_utf16_nc(char16_t*, char32_t)noexcept -> size_t;
#ifdef _WIN32 
	template <> _IRON_DLL_EXPORT auto  chr::_encode_utf16_nc(wchar_t*, char32_t)noexcept -> size_t;
#endif 

#define _IRON_TR(_v) return iron::some(chr(U##_v))

	inline constexpr auto chr::_from_digit_c(radix_t _n)noexcept -> iron::option<chr> {
		switch (_n) {
			case 0: _IRON_TR('0');
			case 1: _IRON_TR('1');
			case 2: _IRON_TR('2');
			case 3: _IRON_TR('3');
			case 4: _IRON_TR('4');
			case 5: _IRON_TR('5');
			case 6: _IRON_TR('6');
			case 7: _IRON_TR('7');
			case 8: _IRON_TR('8');
			case 9: _IRON_TR('9');
			case 10: _IRON_TR('A');
			case 11: _IRON_TR('B');
			case 12: _IRON_TR('C');
			case 13: _IRON_TR('D');
			case 14: _IRON_TR('E');
			case 15: _IRON_TR('F');
			case 16: _IRON_TR('G');
			case 17: _IRON_TR('H');
			case 18: _IRON_TR('I');
			case 19: _IRON_TR('J');
			case 20: _IRON_TR('K');
			case 21: _IRON_TR('L');
			case 22: _IRON_TR('M');
			case 23: _IRON_TR('N');
			case 24: _IRON_TR('O');
			case 25: _IRON_TR('P');
			case 26: _IRON_TR('Q');
			case 27: _IRON_TR('R');
			case 28: _IRON_TR('S');
			case 29: _IRON_TR('T');
			case 30: _IRON_TR('U');
			case 31: _IRON_TR('V');
			case 32: _IRON_TR('W');
			case 33: _IRON_TR('X');
			case 34: _IRON_TR('Y');
			case 35: _IRON_TR('Z');
			default:break;
		}
		return {};
	}

#undef _IRON_TR

	inline constexpr auto chr::_to_digit_c(char32_t _ch,radix_t _radix)noexcept-> iron::option<u32_t> {
		const auto _d1 = static_cast<u32_t>(_ch);
		constexpr auto _zero = static_cast<u32_t>(U'0');
		constexpr auto _ten = default_radix;
		if (_d1 < _zero) IRON_UNLIKELY 
		{
			return {};
		}
		auto _digit = _d1 - _zero;
		if (_radix > _ten) IRON_UNLIKELY 
		{
			if (_digit < _ten) IRON_LIKELY 
			{
				return iron::some(std::move(_digit));
			}
			constexpr auto _la = static_cast<std::uint32_t>(U'a');
			constexpr u32_t seven_bit = 0b100000;
			// force the 6th bit to be set to ensure ascii is lower case.
			_digit = base_integral<u32_t>((_d1 | seven_bit) - _la).saturating_add(_ten).value();
		}
		if (_digit < _radix) IRON_LIKELY 
		{
			return iron::some(std::move(_digit));
		}
		return {};
	}

	inline constexpr auto chr::from_digit(u32_t _num, radix_t _radix)noexcept -> iron::option<chr> {
		constexpr auto _zero = unchecked_from(U'0')._to_u32();
		constexpr auto _ten = default_radix;
		constexpr auto _la = unchecked_from(U'a')._to_u32();
		PANIC_IF(_radix < min_radix, "chr::from_digit() : radix is too low(minimun 2)");
		PANIC_IF(_radix > max_radix,"chr::from_digit(): radix is too high (maximum 36)");
		if (_num > _radix) IRON_UNLIKELY
		{
			return {};
		}
		if (_num < _ten) IRON_LIKELY
		{
			return iron::some(unchecked_from_u32(_num + _zero));
		}
		return iron::some(unchecked_from_u32(_num - _ten + _la));
	}

	class chr::sequence  final {
	public:
		using value_t = chr;
		using iter_t = const value_t*;
		struct chr_sequence_tag { chr_sequence_tag() = delete; };
		[[nodiscard]] bool empty()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _empty();
		}
		[[nodiscard]] size_t size()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			DEBUG_ASSERT(_m_sz >= _m_current);
			return _m_sz - _m_current;
		}
		[[nodiscard]] explicit operator bool()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return !_empty();
		}
		[[nodiscard]] iter_t begin()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return std::begin(_m_v) + _m_current;
		}
		[[nodiscard]] iter_t end()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return std::begin(_m_v) + _m_sz;
		}
		[[nodiscard]] iron::option<value_t> next()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			if (!_empty()) IRON_LIKELY
			{
				const auto _c = _peek();
				++_m_current;
				return iron::some(_c);
			}
			return {};
		}
		sequence(const sequence& _v)noexcept
		: _m_current(_v._m_current)
		, _m_sz(_v._m_sz)
		{
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_copy<true>(_m_v, _v._m_v);
		}
		sequence(sequence&& _v)noexcept
		: _m_current(_v._m_current)
		, _m_sz(_v._m_sz)
		{
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_copy<true>(_m_v, _v._m_v);
			IRON_SECURITY_MARK_TEMPORARY(_v);
		}
		sequence& operator=(const sequence& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_m_current = _v._m_current;
			_m_sz = _v._m_sz;
			_copy<false>(_m_v, _v._m_v);
			return *this;
		}
		sequence& operator=(sequence&& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_m_current = _v._m_current;
			_m_sz = _v._m_sz;
			_copy<false>(_m_v, _v._m_v);
			IRON_SECURITY_MARK_TEMPORARY(_v);
			return *this;
		}
		[[nodiscard]] iron::option<value_t> peek()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			if (!_empty()) IRON_LIKELY
			{
				const auto _c = _peek();
				return iron::some(_c);
			}
			return {};
		}
		[[nodiscard]] value_t operator*()const noexcept {
			return peek().unwrap();
		}
		sequence& operator++()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			if (!_empty()) IRON_LIKELY
			{
				++_m_current;
			}
			return *this;
		}
		sequence operator++(int)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			auto t = *this;
			++(*this);
			return t;
		}
		~sequence() = default;
		IRON_SECURITY_METHOD()
	private:
		friend class chr;
		typedef enum class escape_type {
			debug
			, def
			, unicode
		} escape_t;
		explicit sequence(_uc_t::iter_t,_uc_t::iter_t)noexcept;
		[[nodiscard]] value_t _peek()const noexcept {
			return *(std::begin(_m_v) + _m_current);
		}

		template <bool _init,std::size_t _N>
		static void _copy(value_t(&_out)[_N], const value_t(&_in)[_N])noexcept {
			value_t* _pout = &_out[0];
			const value_t* _pin = &_in[0];
			auto _sz = _N;
			while (_sz--) IRON_LIKELY
			{
				if constexpr (_init) {
					std::construct_at(_pout++, *_pin++);
				}
				else {
					*_pout++ = *_pin++;
				}
			}
		}
		_IRON_DLL_EXPORT explicit sequence(const value_t&, escape_t)noexcept;
		[[nodiscard]] bool _empty()const noexcept {
			return _m_current == _m_sz;
		}
		size_t _m_current{},_m_sz{};
		static constexpr size_t _sm_sz = 20;
		value_t _m_v[_sm_sz];
		IRON_SECURITY_DCL();
	};

	inline constexpr auto chr::_to_ascii_uppercase()const noexcept -> chr {
		if (_is_ascii()) IRON_LIKELY 
		{
			constexpr auto _la = unchecked_from(U'a')._to_u32();
			constexpr auto _lz = unchecked_from(U'z')._to_u32();
			constexpr auto _lA = unchecked_from(U'A')._to_u32();
			if (_to_u32() >= _la && _to_u32() <= _lz) IRON_LIKELY 
			{
				return chr(_to_u32() - (_la - _lA));
			}
		}
		return *this;
	}

	inline constexpr auto chr::_to_ascii_lowercase()const noexcept -> chr {
		if (_is_ascii()) IRON_LIKELY 
		{
			constexpr auto _lA = unchecked_from(U'A')._to_u32();
			constexpr auto _lZ = unchecked_from(U'Z')._to_u32();
			constexpr auto _la = unchecked_from(U'a')._to_u32();
			if (_to_u32() >= _lA && _to_u32() <= _lZ) IRON_LIKELY 
			{
				return chr(_to_u32() + (_la - _lA));
			}
		}
		return *this;
	}

	inline auto chr::to_lowercase()const noexcept-> seq_t {
		IRON_SECURITY_CHECK_TEMPORARY_THIS();
		return _to_lowercase(_m_c);
	}

	inline auto chr::to_uppercase()const noexcept-> seq_t {
		IRON_SECURITY_CHECK_TEMPORARY_THIS();
		return _to_uppercase(_m_c);
	}

	inline auto chr::escape_unicode()const noexcept->seq_t { 
		IRON_SECURITY_CHECK_TEMPORARY_THIS();
		return seq_t(*this, seq_t::escape_t::unicode); 
	}

	inline auto chr::escape_debug()const noexcept -> seq_t {
		IRON_SECURITY_CHECK_TEMPORARY_THIS();
		return seq_t(*this, seq_t::escape_t::debug);
	}

	inline auto chr::escape_default()const noexcept ->seq_t { 
		IRON_SECURITY_CHECK_TEMPORARY_THIS();
		return seq_t(*this, seq_t::escape_t::def); 
	}


} //ns

static_assert(iron::concepts::chr<iron::chr>, "internal error");

#if defined(_MSC_VER)
#pragma warning(pop)
#endif
