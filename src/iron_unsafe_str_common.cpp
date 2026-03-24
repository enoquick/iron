
#ifdef USE_MODULES
#else
#include "iron/unsafe/raw_str.h"
#endif

namespace iron::unsafe::str {

	template <bool _SAFE,typename _T, typename _U>
	[[nodiscard]] inline bool common::_icopy(_T* _p, size_t& _sz,const _U* _b,const _U* _e)noexcept {
		return unicode_cv_t::template copy<_SAFE>(_p, _sz, _b, _e);
	}

	template <> bool common::_copy(char* _p, size_t& _sz, const char* _b, const char* _e)noexcept { return _icopy<false>(_p, _sz, _b, _e); }
	template <> bool common::_copy(char* _p, size_t& _sz, const char8_t* _b, const char8_t* _e)noexcept { return _icopy<false>(_p, _sz, _b, _e); }
	template <> bool common::_copy(char* _p, size_t& _sz, const char16_t* _b, const char16_t* _e)noexcept { return _icopy<false>(_p, _sz, _b, _e); }
	template <> bool common::_copy(char* _p, size_t& _sz, const wchar_t* _b, const wchar_t* _e)noexcept { return _icopy<false>(_p, _sz, _b, _e); }
	template <> bool common::_copy(char* _p, size_t& _sz, const char32_t* _b, const char32_t* _e)noexcept { return _icopy<false>(_p, _sz, _b, _e); }

	template <> bool common::_copy(char8_t* _p, size_t& _sz, const char* _b, const char* _e)noexcept { return _icopy<false>(_p, _sz, _b, _e); }
	template <> bool common::_copy(char8_t* _p, size_t& _sz, const char8_t* _b, const char8_t* _e)noexcept { return _icopy<false>(_p, _sz, _b, _e); }
	template <> bool common::_copy(char8_t* _p, size_t& _sz, const char16_t* _b, const char16_t* _e)noexcept { return _icopy<false>(_p, _sz, _b, _e); }
	template <> bool common::_copy(char8_t* _p, size_t& _sz, const wchar_t* _b, const wchar_t* _e)noexcept { return _icopy<false>(_p, _sz, _b, _e); }
	template <> bool common::_copy(char8_t* _p, size_t& _sz, const char32_t* _b, const char32_t* _e)noexcept { return _icopy<false>(_p, _sz, _b, _e); }

	template <> bool common::_copy(char16_t* _p, size_t& _sz, const char* _b, const char* _e)noexcept { return _icopy<false>(_p, _sz, _b, _e); }
	template <> bool common::_copy(char16_t* _p, size_t& _sz, const char8_t* _b, const char8_t* _e)noexcept { return _icopy<false>(_p, _sz, _b, _e); }
	template <> bool common::_copy(char16_t* _p, size_t& _sz, const char16_t* _b, const char16_t* _e)noexcept { return _icopy<false>(_p, _sz, _b, _e); }
	template <> bool common::_copy(char16_t* _p, size_t& _sz, const wchar_t* _b, const wchar_t* _e)noexcept { return _icopy<false>(_p, _sz, _b, _e); }
	template <> bool common::_copy(char16_t* _p, size_t& _sz, const char32_t* _b, const char32_t* _e)noexcept { return _icopy<false>(_p, _sz, _b, _e); }

	template <> bool common::_copy(wchar_t* _p, size_t& _sz, const char* _b, const char* _e)noexcept { return _icopy<false>(_p, _sz, _b, _e); }
	template <> bool common::_copy(wchar_t* _p, size_t& _sz, const char8_t* _b, const char8_t* _e)noexcept { return _icopy<false>(_p, _sz, _b, _e); }
	template <> bool common::_copy(wchar_t* _p, size_t& _sz, const char16_t* _b, const char16_t* _e)noexcept { return _icopy<false>(_p, _sz, _b, _e); }
	template <> bool common::_copy(wchar_t* _p, size_t& _sz, const wchar_t* _b, const wchar_t* _e)noexcept { return _icopy<false>(_p, _sz, _b, _e); }
	template <> bool common::_copy(wchar_t* _p, size_t& _sz, const char32_t* _b, const char32_t* _e)noexcept { return _icopy<false>(_p, _sz, _b, _e); }

	template <> bool common::_copy(char32_t* _p, size_t& _sz, const char* _b, const char* _e)noexcept { return _icopy<false>(_p, _sz, _b, _e); }
	template <> bool common::_copy(char32_t* _p, size_t& _sz, const char8_t* _b, const char8_t* _e)noexcept { return _icopy<false>(_p, _sz, _b, _e); }
	template <> bool common::_copy(char32_t* _p, size_t& _sz, const char16_t* _b, const char16_t* _e)noexcept { return _icopy<false>(_p, _sz, _b, _e); }
	template <> bool common::_copy(char32_t* _p, size_t& _sz, const wchar_t* _b, const wchar_t* _e)noexcept { return _icopy<false>(_p, _sz, _b, _e); }
	template <> bool common::_copy(char32_t* _p, size_t& _sz, const char32_t* _b, const char32_t* _e)noexcept { return _icopy<false>(_p, _sz, _b, _e); }


	template <> bool common::_copy_safe(char* _p, size_t& _sz, const char* _b, const char* _e)noexcept { return _icopy<true>(_p, _sz, _b, _e); }
	template <> bool common::_copy_safe(char* _p, size_t& _sz, const char8_t* _b, const char8_t* _e)noexcept { return _icopy<true>(_p, _sz, _b, _e); }
	template <> bool common::_copy_safe(char* _p, size_t& _sz, const char16_t* _b, const char16_t* _e)noexcept { return _icopy<true>(_p, _sz, _b, _e); }
	template <> bool common::_copy_safe(char* _p, size_t& _sz, const wchar_t* _b, const wchar_t* _e)noexcept { return _icopy<true>(_p, _sz, _b, _e); }
	template <> bool common::_copy_safe(char* _p, size_t& _sz, const char32_t* _b, const char32_t* _e)noexcept { return _icopy<true>(_p, _sz, _b, _e); }

	template <> bool common::_copy_safe(char8_t* _p, size_t& _sz, const char* _b, const char* _e)noexcept { return _icopy<true>(_p, _sz, _b, _e); }
	template <> bool common::_copy_safe(char8_t* _p, size_t& _sz, const char8_t* _b, const char8_t* _e)noexcept { return _icopy<true>(_p, _sz, _b, _e); }
	template <> bool common::_copy_safe(char8_t* _p, size_t& _sz, const char16_t* _b, const char16_t* _e)noexcept { return _icopy<true>(_p, _sz, _b, _e); }
	template <> bool common::_copy_safe(char8_t* _p, size_t& _sz, const wchar_t* _b, const wchar_t* _e)noexcept { return _icopy<true>(_p, _sz, _b, _e); }
	template <> bool common::_copy_safe(char8_t* _p, size_t& _sz, const char32_t* _b, const char32_t* _e)noexcept { return _icopy<true>(_p, _sz, _b, _e); }

	template <> bool common::_copy_safe(char16_t* _p, size_t& _sz, const char* _b, const char* _e)noexcept { return _icopy<true>(_p, _sz, _b, _e); }
	template <> bool common::_copy_safe(char16_t* _p, size_t& _sz, const char8_t* _b, const char8_t* _e)noexcept { return _icopy<true>(_p, _sz, _b, _e); }
	template <> bool common::_copy_safe(char16_t* _p, size_t& _sz, const char16_t* _b, const char16_t* _e)noexcept { return _icopy<true>(_p, _sz, _b, _e); }
	template <> bool common::_copy_safe(char16_t* _p, size_t& _sz, const wchar_t* _b, const wchar_t* _e)noexcept { return _icopy<true>(_p, _sz, _b, _e); }
	template <> bool common::_copy_safe(char16_t* _p, size_t& _sz, const char32_t* _b, const char32_t* _e)noexcept { return _icopy<true>(_p, _sz, _b, _e); }

	template <> bool common::_copy_safe(wchar_t* _p, size_t& _sz, const char* _b, const char* _e)noexcept { return _icopy<true>(_p, _sz, _b, _e); }
	template <> bool common::_copy_safe(wchar_t* _p, size_t& _sz, const char8_t* _b, const char8_t* _e)noexcept { return _icopy<true>(_p, _sz, _b, _e); }
	template <> bool common::_copy_safe(wchar_t* _p, size_t& _sz, const char16_t* _b, const char16_t* _e)noexcept { return _icopy<true>(_p, _sz, _b, _e); }
	template <> bool common::_copy_safe(wchar_t* _p, size_t& _sz, const wchar_t* _b, const wchar_t* _e)noexcept { return _icopy<true>(_p, _sz, _b, _e); }
	template <> bool common::_copy_safe(wchar_t* _p, size_t& _sz, const char32_t* _b, const char32_t* _e)noexcept { return _icopy<true>(_p, _sz, _b, _e); }

	template <> bool common::_copy_safe(char32_t* _p, size_t& _sz, const char* _b, const char* _e)noexcept { return _icopy<true>(_p, _sz, _b, _e); }
	template <> bool common::_copy_safe(char32_t* _p, size_t& _sz, const char8_t* _b, const char8_t* _e)noexcept { return _icopy<true>(_p, _sz, _b, _e); }
	template <> bool common::_copy_safe(char32_t* _p, size_t& _sz, const char16_t* _b, const char16_t* _e)noexcept { return _icopy<true>(_p, _sz, _b, _e); }
	template <> bool common::_copy_safe(char32_t* _p, size_t& _sz, const wchar_t* _b, const wchar_t* _e)noexcept { return _icopy<true>(_p, _sz, _b, _e); }
	template <> bool common::_copy_safe(char32_t* _p, size_t& _sz, const char32_t* _b, const char32_t* _e)noexcept { return _icopy<true>(_p, _sz, _b, _e); }


	template <> bool common::_is_chr_boundary_varcharset_nc(const char8_t* _p, size_t _idx, size_t _sz)noexcept {
		return _is_chr_boundary_varcharset_c(_p, _idx, _sz);
	}

	template <> bool common::_is_chr_boundary_varcharset_nc(const char16_t* _p, size_t _idx, size_t _sz)noexcept {
		return _is_chr_boundary_varcharset_c(_p, _idx, _sz);
	}
#ifdef _WIN32
	template <> bool common::_is_chr_boundary_varcharset_nc(const wchar_t* _p, size_t _idx, size_t _sz)noexcept {
		return _is_chr_boundary_varcharset_c(_p, _idx, _sz);
	}
#endif

	template <>  iron::option<char32_t> common::_get_char_nc(const char8_t* _p, size_t _sz, size_t _idx)noexcept {
		return _get_char_c(_p, _sz, _idx);
	}
	template <> iron::option<char32_t> common::_get_char_nc(const char16_t* _p, size_t _sz, size_t _idx)noexcept {
		return _get_char_c(_p, _sz, _idx);
	}
#ifdef _WIN32
	template <> iron::option<char32_t> common::_get_char_nc(const wchar_t* _p, size_t _sz, size_t _idx)noexcept {
		return _get_char_c(_p, _sz, _idx);
	}
#endif
	template <> iron::option<char32_t> common::_get_char_back_nc(const char8_t* _p, size_t _sz)noexcept {
		return _get_char_back_c(_p, _sz);
	}
	template <> iron::option<char32_t> common::_get_char_back_nc(const char16_t* _p, size_t _sz)noexcept {
		return _get_char_back_c(_p, _sz);
	}
#ifdef _WIN32
	template <> iron::option<char32_t> common::_get_char_back_nc(const wchar_t* _p, size_t _sz)noexcept {
		return _get_char_back_c(_p, _sz);
	}
#endif

	template <> std::pair<char8_t*, size_t> common::_slice_var_nc(char8_t* _p, size_t _sz, size_t  _start, size_t _len)noexcept {
		return _slice_var_c(_p, _sz, _start, _len);
	}
	template <> std::pair<const char8_t*, size_t> common::_slice_var_nc(const char8_t* _p, size_t _sz, size_t  _start, size_t _len)noexcept {
		return _slice_var_c(_p, _sz, _start, _len);
	}

	template <> std::pair<char16_t*, size_t> common::_slice_var_nc(char16_t* _p, size_t _sz, size_t  _start, size_t _len)noexcept {
		return _slice_var_c(_p, _sz, _start, _len);
	}
	template <> std::pair<const char16_t*, size_t> common::_slice_var_nc(const char16_t* _p, size_t _sz, size_t  _start, size_t _len)noexcept {
		return _slice_var_c(_p, _sz, _start, _len);
	}

#ifdef _WIN32
	template <> std::pair<wchar_t*, size_t> common::_slice_var_nc(wchar_t* _p, size_t _sz, size_t  _start, size_t _len)noexcept {
		return _slice_var_c(_p, _sz, _start, _len);
	}
	template <> std::pair<const wchar_t*, size_t> common::_slice_var_nc(const wchar_t* _p, size_t _sz, size_t  _start, size_t _len)noexcept {
		return _slice_var_c(_p, _sz, _start, _len);
	}
#endif

	template <> std::pair<char*, size_t> common::_ltrim_nc(char* _p, size_t _sz)noexcept {
		return _ltrim_c(_p, _sz);
	}
	template <> std::pair<const char*, size_t> common::_ltrim_nc(const char* _p, size_t _sz)noexcept {
		return _ltrim_c(_p, _sz);
	}
	template <> std::pair<char8_t*, size_t> common::_ltrim_nc(char8_t* _p, size_t _sz)noexcept {
		return _ltrim_c(_p, _sz);
	}
	template <> std::pair<const char8_t*, size_t> common::_ltrim_nc(const char8_t* _p, size_t _sz)noexcept {
		return _ltrim_c(_p, _sz);
	}
	template <> std::pair<char16_t*, size_t> common::_ltrim_nc(char16_t* _p, size_t _sz)noexcept {
		return _ltrim_c(_p, _sz);
	}
	template <> std::pair<const char16_t*, size_t> common::_ltrim_nc(const char16_t* _p, size_t _sz)noexcept {
		return _ltrim_c(_p, _sz);
	}
	template <> std::pair<wchar_t*, size_t> common::_ltrim_nc(wchar_t* _p, size_t _sz)noexcept {
		return _ltrim_c(_p, _sz);
	}
	template <> std::pair<const wchar_t*, size_t> common::_ltrim_nc(const wchar_t* _p, size_t _sz)noexcept {
		return _ltrim_c(_p, _sz);
	}
	template <> std::pair<char32_t*, size_t> common::_ltrim_nc(char32_t* _p, size_t _sz)noexcept {
		return _ltrim_c(_p, _sz);
	}
	template <> std::pair<const char32_t*, size_t> common::_ltrim_nc(const char32_t* _p, size_t _sz)noexcept {
		return _ltrim_c(_p, _sz);
	}

	template <> std::pair<char*, size_t> common::_rtrim_nc(char* _p, size_t _sz)noexcept {
		return _rtrim_c(_p, _sz);
	}
	template <> std::pair<const char*, size_t> common::_rtrim_nc(const char* _p, size_t _sz)noexcept {
		return _rtrim_c(_p, _sz);
	}
	template <> std::pair<char8_t*, size_t> common::_rtrim_nc(char8_t* _p, size_t _sz)noexcept {
		return _rtrim_c(_p, _sz);
	}
	template <> std::pair<const char8_t*, size_t> common::_rtrim_nc(const char8_t* _p, size_t _sz)noexcept {
		return _rtrim_c(_p, _sz);
	}
	template <> std::pair<char16_t*, size_t> common::_rtrim_nc(char16_t* _p, size_t _sz)noexcept {
		return _rtrim_c(_p, _sz);
	}
	template <> std::pair<const char16_t*, size_t> common::_rtrim_nc(const char16_t* _p, size_t _sz)noexcept {
		return _rtrim_c(_p, _sz);
	}
	template <> std::pair<wchar_t*, size_t> common::_rtrim_nc(wchar_t* _p, size_t _sz)noexcept {
		return _rtrim_c(_p, _sz);
	}
	template <> std::pair<const wchar_t*, size_t> common::_rtrim_nc(const wchar_t* _p, size_t _sz)noexcept {
		return _rtrim_c(_p, _sz);
	}
	template <> std::pair<char32_t*, size_t> common::_rtrim_nc(char32_t* _p, size_t _sz)noexcept {
		return _rtrim_c(_p, _sz);
	}
	template <> std::pair<const char32_t*, size_t> common::_rtrim_nc(const char32_t* _p, size_t _sz)noexcept {
		return _rtrim_c(_p, _sz);
	}

	template <> std::pair<char*, size_t> common::_find_char_nc(char* _p, size_t _sz, std::uint32_t _ch)noexcept {
		return _find_char_c(_p, _sz, _ch);
	}
	template <> std::pair<const char*, size_t> common::_find_char_nc(const char* _p, size_t _sz, std::uint32_t _ch)noexcept {
		return _find_char_c(_p, _sz, _ch);
	}
	template <> std::pair<char8_t*, size_t> common::_find_char_nc(char8_t* _p, size_t _sz, std::uint32_t _ch)noexcept {
		return _find_char_c(_p, _sz, _ch);
	}
	template <> std::pair<const char8_t*, size_t> common::_find_char_nc(const char8_t* _p, size_t _sz, std::uint32_t _ch)noexcept {
		return _find_char_c(_p, _sz, _ch);
	}
	template <> std::pair<char16_t*, size_t> common::_find_char_nc(char16_t* _p, size_t _sz, std::uint32_t _ch)noexcept {
		return _find_char_c(_p, _sz, _ch);
	}
	template <> std::pair<const char16_t*, size_t> common::_find_char_nc(const char16_t* _p, size_t _sz, std::uint32_t _ch)noexcept {
		return _find_char_c(_p, _sz, _ch);
	}
	template <> std::pair<wchar_t*, size_t> common::_find_char_nc(wchar_t* _p, size_t _sz, std::uint32_t _ch)noexcept {
		return _find_char_c(_p, _sz, _ch);
	}
	template <> std::pair<const wchar_t*, size_t> common::_find_char_nc(const wchar_t* _p, size_t _sz, std::uint32_t _ch)noexcept {
		return _find_char_c(_p, _sz, _ch);
	}
	template <> std::pair<char32_t*, size_t> common::_find_char_nc(char32_t* _p, size_t _sz, std::uint32_t _ch)noexcept {
		return _find_char_c(_p, _sz, _ch);
	}
	template <> std::pair<const char32_t*, size_t> common::_find_char_nc(const char32_t* _p, size_t _sz, std::uint32_t _ch)noexcept {
		return _find_char_c(_p, _sz, _ch);
	}

	template <> std::pair<char*, size_t> common::_find_nc(char* _p, size_t _sz, const char* _pc, size_t _szc)noexcept {
		return _find_c(_p, _sz, _pc, _szc);
	}
	template <> std::pair<const char*, size_t> common::_find_nc(const char* _p, size_t _sz, const char* _pc, size_t _szc)noexcept {
		return _find_c(_p, _sz, _pc, _szc);
	}
	template <> std::pair<char8_t*, size_t> common::_find_nc(char8_t* _p, size_t _sz, const char8_t* _pc, size_t _szc)noexcept {
		return _find_c(_p, _sz, _pc, _szc);
	}
	template <> std::pair<const char8_t*, size_t> common::_find_nc(const char8_t* _p, size_t _sz, const char8_t* _pc, size_t _szc)noexcept {
		return _find_c(_p, _sz, _pc, _szc);
	}
	template <> std::pair<char16_t*, size_t> common::_find_nc(char16_t* _p, size_t _sz, const char16_t* _pc, size_t _szc)noexcept {
		return _find_c(_p, _sz, _pc, _szc);
	}
	template <> std::pair<const char16_t*, size_t> common::_find_nc(const char16_t* _p, size_t _sz, const char16_t* _pc, size_t _szc)noexcept {
		return _find_c(_p, _sz, _pc, _szc);
	}
	template <> std::pair<wchar_t*, size_t> common::_find_nc(wchar_t* _p, size_t _sz, const wchar_t* _pc, size_t _szc)noexcept {
		return _find_c(_p, _sz, _pc, _szc);
	}
	template <> std::pair<const wchar_t*, size_t> common::_find_nc(const wchar_t* _p, size_t _sz, const wchar_t* _pc, size_t _szc)noexcept {
		return _find_c(_p, _sz, _pc, _szc);
	}
	template <> std::pair<char32_t*, size_t> common::_find_nc(char32_t* _p, size_t _sz, const char32_t* _pc, size_t _szc)noexcept {
		return _find_c(_p, _sz, _pc, _szc);
	}
	template <> std::pair<const char32_t*, size_t> common::_find_nc(const char32_t* _p, size_t _sz, const char32_t* _pc, size_t _szc)noexcept {
		return _find_c(_p, _sz, _pc, _szc);
	}

	template <> bool common::_ends_with_char_var_nc(const char8_t* _p, size_t _sz, std::uint32_t _ch)noexcept {
		return _ends_with_char_var_c(_p, _sz, _ch);
	}
	template <> bool common::_ends_with_char_var_nc(const char16_t* _p, size_t _sz, std::uint32_t _ch)noexcept {
		return _ends_with_char_var_c(_p, _sz, _ch);
	}
#ifdef _WIN32
	template <> bool common::_ends_with_char_var_nc(const wchar_t* _p, size_t _sz, std::uint32_t _ch)noexcept {
		return _ends_with_char_var_c(_p, _sz, _ch);
	}
#endif

	template <> bool common::_ends_with_nc(const char* _p, size_t _sz, const char* _pc, size_t _szc)noexcept {
		return _ends_with_c(_p, _sz, _pc, _szc);
	}
	template <> bool common::_ends_with_nc(const char8_t* _p, size_t _sz, const char8_t* _pc, size_t _szc)noexcept {
		return _ends_with_c(_p, _sz, _pc, _szc);
	}
	template <> bool common::_ends_with_nc(const char16_t* _p, size_t _sz, const char16_t* _pc, size_t _szc)noexcept {
		return _ends_with_c(_p, _sz, _pc, _szc);
	}
	template <> bool common::_ends_with_nc(const wchar_t* _p, size_t _sz, const wchar_t* _pc, size_t _szc)noexcept {
		return _ends_with_c(_p, _sz, _pc, _szc);
	}
	template <> bool common::_ends_with_nc(const char32_t* _p, size_t _sz, const char32_t* _pc, size_t _szc)noexcept {
		return _ends_with_c(_p, _sz, _pc, _szc);
	}

	template <> bool common::_starts_with_nc(const char* _p, size_t _sz, const char* _pc, size_t _szc)noexcept {
		return _starts_with_c(_p, _sz, _pc, _szc);
	}
	template <> bool common::_starts_with_nc(const char8_t* _p, size_t _sz, const char8_t* _pc, size_t _szc)noexcept {
		return _starts_with_c(_p, _sz, _pc, _szc);
	}
	template <> bool common::_starts_with_nc(const char16_t* _p, size_t _sz, const char16_t* _pc, size_t _szc)noexcept {
		return _starts_with_c(_p, _sz, _pc, _szc);
	}
	template <> bool common::_starts_with_nc(const wchar_t* _p, size_t _sz, const wchar_t* _pc, size_t _szc)noexcept {
		return _starts_with_c(_p, _sz, _pc, _szc);
	}
	template <> bool common::_starts_with_nc(const char32_t* _p, size_t _sz, const char32_t* _pc, size_t _szc)noexcept {
		return _starts_with_c(_p, _sz, _pc, _szc);
	}

	template <> auto common::_chr_index_nc(const char8_t* _p, size_t _sz, size_t _idx)noexcept -> iron::option<size_t> {
		return _chr_index_c(_p, _sz, _idx);
	}
	template <> auto common::_chr_index_nc(const char16_t* _p, size_t _sz, size_t _idx)noexcept -> iron::option<size_t> {
		return _chr_index_c(_p, _sz, _idx);
	}
#ifdef _WIN32
	template <> auto common::_chr_index_nc(const wchar_t* _p, size_t _sz, size_t _idx)noexcept -> iron::option<size_t> {
		return _chr_index_c(_p, _sz, _idx);
	}
#endif

	template <> auto  common::_raw_in_nc(const char* _p, size_t _sz, const char* _pc, size_t _szc)noexcept -> iron::option<size_t> {
		return _raw_in_c(_p, _sz, _pc, _szc);
	}
	template <> auto  common::_raw_in_nc(const char8_t* _p, size_t _sz, const char8_t* _pc, size_t _szc)noexcept -> iron::option<size_t> {
		return _raw_in_c(_p, _sz, _pc, _szc);
	}
	template <> auto  common::_raw_in_nc(const char16_t* _p, size_t _sz, const char16_t* _pc, size_t _szc)noexcept -> iron::option<size_t> {
		return _raw_in_c(_p, _sz, _pc, _szc);
	}
	template <> auto  common::_raw_in_nc(const wchar_t* _p, size_t _sz, const wchar_t* _pc, size_t _szc)noexcept -> iron::option<size_t> {
		return _raw_in_c(_p, _sz, _pc, _szc);
	}
	template <> auto  common::_raw_in_nc(const char32_t* _p, size_t _sz, const char32_t* _pc, size_t _szc)noexcept -> iron::option<size_t> {
		return _raw_in_c(_p, _sz, _pc, _szc);
	}

	template <> auto  common::_raw_index_nc(const char8_t* _p, size_t _sz, size_t _idx)noexcept-> iron::option<size_t> {
		return _raw_index_c(_p, _sz, _idx);
	}
	template <> auto  common::_raw_index_nc(const char16_t* _p, size_t _sz, size_t _idx)noexcept-> iron::option<size_t> {
		return _raw_index_c(_p, _sz, _idx);
	}
#ifdef _WIN32
	template <> auto  common::_raw_index_nc(const wchar_t* _p, size_t _sz, size_t _idx)noexcept-> iron::option<size_t> {
		return _raw_index_c(_p, _sz, _idx);
	}
#endif

	template <> bool common::_eq_nc(const char* _pl, size_t _szl, const char8_t* _pr, size_t _szr)noexcept {
		return _eq_c(_pl, _szl, _pr, _szr);
	}
	template <> bool common::_eq_nc(const char* _pl, size_t _szl, const char16_t* _pr, size_t _szr)noexcept {
		return _eq_c(_pl, _szl, _pr, _szr);
	}
	template <> bool common::_eq_nc(const char* _pl, size_t _szl, const wchar_t* _pr, size_t _szr)noexcept {
		return _eq_c(_pl, _szl, _pr, _szr);
	}
	template <> bool common::_eq_nc(const char* _pl, size_t _szl, const char32_t* _pr, size_t _szr)noexcept {
		return _eq_c(_pl, _szl, _pr, _szr);
	}
	template <> bool common::_eq_nc(const char8_t* _pl, size_t _szl, const char16_t* _pr, size_t _szr)noexcept {
		return _eq_c(_pl, _szl, _pr, _szr);
	}
	template <> bool common::_eq_nc(const char8_t* _pl, size_t _szl, const wchar_t* _pr, size_t _szr)noexcept {
		return _eq_c(_pl, _szl, _pr, _szr);
	}
	template <> bool common::_eq_nc(const char8_t* _pl, size_t _szl, const char32_t* _pr, size_t _szr)noexcept {
		return _eq_c(_pl, _szl, _pr, _szr);
	}
	template <> bool common::_eq_nc(const wchar_t* _pl, size_t _szl, const char16_t* _pr, size_t _szr)noexcept {
		return _eq_c(_pl, _szl, _pr, _szr);
	}
	template <> bool common::_eq_nc(const wchar_t* _pl, size_t _szl, const char32_t* _pr, size_t _szr)noexcept {
		return _eq_c(_pl, _szl, _pr, _szr);
	}
	template <> bool common::_eq_nc(const char16_t* _pl, size_t _szl, const char32_t* _pr, size_t _szr)noexcept {
		return _eq_c(_pl, _szl, _pr, _szr);
	}

	template <> bool common::_lt_nc(const char* _pl, size_t _szl, const char* _pr, size_t _szr)noexcept { return _lt_c(_pl, _szl, _pr, _szr); }
	template <> bool common::_lt_nc(const char* _pl, size_t _szl, const char8_t* _pr, size_t _szr)noexcept { return _lt_c(_pl, _szl, _pr, _szr); }
	template <> bool common::_lt_nc(const char* _pl, size_t _szl, const char16_t* _pr, size_t _szr)noexcept { return _lt_c(_pl, _szl, _pr, _szr); }
	template <> bool common::_lt_nc(const char* _pl, size_t _szl, const wchar_t* _pr, size_t _szr)noexcept { return _lt_c(_pl, _szl, _pr, _szr); }
	template <> bool common::_lt_nc(const char* _pl, size_t _szl, const char32_t* _pr, size_t _szr)noexcept { return _lt_c(_pl, _szl, _pr, _szr); }

	template <> bool common::_lt_nc(const char8_t* _pl, size_t _szl, const char* _pr, size_t _szr)noexcept { return _lt_c(_pl, _szl, _pr, _szr); }
	template <> bool common::_lt_nc(const char8_t* _pl, size_t _szl, const char8_t* _pr, size_t _szr)noexcept { return _lt_c(_pl, _szl, _pr, _szr); }
	template <> bool common::_lt_nc(const char8_t* _pl, size_t _szl, const char16_t* _pr, size_t _szr)noexcept { return _lt_c(_pl, _szl, _pr, _szr); }
	template <> bool common::_lt_nc(const char8_t* _pl, size_t _szl, const wchar_t* _pr, size_t _szr)noexcept { return _lt_c(_pl, _szl, _pr, _szr); }
	template <> bool common::_lt_nc(const char8_t* _pl, size_t _szl, const char32_t* _pr, size_t _szr)noexcept { return _lt_c(_pl, _szl, _pr, _szr); }

	template <> bool common::_lt_nc(const char16_t* _pl, size_t _szl, const char* _pr, size_t _szr)noexcept { return _lt_c(_pl, _szl, _pr, _szr); }
	template <> bool common::_lt_nc(const char16_t* _pl, size_t _szl, const char8_t* _pr, size_t _szr)noexcept { return _lt_c(_pl, _szl, _pr, _szr); }
	template <> bool common::_lt_nc(const char16_t* _pl, size_t _szl, const char16_t* _pr, size_t _szr)noexcept { return _lt_c(_pl, _szl, _pr, _szr); }
	template <> bool common::_lt_nc(const char16_t* _pl, size_t _szl, const wchar_t* _pr, size_t _szr)noexcept { return _lt_c(_pl, _szl, _pr, _szr); }
	template <> bool common::_lt_nc(const char16_t* _pl, size_t _szl, const char32_t* _pr, size_t _szr)noexcept { return _lt_c(_pl, _szl, _pr, _szr); }

	template <> bool common::_lt_nc(const wchar_t* _pl, size_t _szl, const char* _pr, size_t _szr)noexcept { return _lt_c(_pl, _szl, _pr, _szr); }
	template <> bool common::_lt_nc(const wchar_t* _pl, size_t _szl, const char8_t* _pr, size_t _szr)noexcept { return _lt_c(_pl, _szl, _pr, _szr); }
	template <> bool common::_lt_nc(const wchar_t* _pl, size_t _szl, const char16_t* _pr, size_t _szr)noexcept { return _lt_c(_pl, _szl, _pr, _szr); }
	template <> bool common::_lt_nc(const wchar_t* _pl, size_t _szl, const wchar_t* _pr, size_t _szr)noexcept { return _lt_c(_pl, _szl, _pr, _szr); }
	template <> bool common::_lt_nc(const wchar_t* _pl, size_t _szl, const char32_t* _pr, size_t _szr)noexcept { return _lt_c(_pl, _szl, _pr, _szr); }

	template <> bool common::_lt_nc(const char32_t* _pl, size_t _szl, const char* _pr, size_t _szr)noexcept { return _lt_c(_pl, _szl, _pr, _szr); }
	template <> bool common::_lt_nc(const char32_t* _pl, size_t _szl, const char8_t* _pr, size_t _szr)noexcept { return _lt_c(_pl, _szl, _pr, _szr); }
	template <> bool common::_lt_nc(const char32_t* _pl, size_t _szl, const char16_t* _pr, size_t _szr)noexcept { return _lt_c(_pl, _szl, _pr, _szr); }
	template <> bool common::_lt_nc(const char32_t* _pl, size_t _szl, const wchar_t* _pr, size_t _szr)noexcept { return _lt_c(_pl, _szl, _pr, _szr); }
	template <> bool common::_lt_nc(const char32_t* _pl, size_t _szl, const char32_t* _pr, size_t _szr)noexcept { return _lt_c(_pl, _szl, _pr, _szr); }

	template <> auto common::_compare_nc(const char* _pl, size_t _szl, const char* _pr, size_t _szr)noexcept -> compare_t { return _compare_c(_pl, _szl, _pr, _szr); }
	template <> auto common::_compare_nc(const char* _pl, size_t _szl, const char8_t* _pr, size_t _szr)noexcept -> compare_t { return _compare_c(_pl, _szl, _pr, _szr); }
	template <> auto common::_compare_nc(const char* _pl, size_t _szl, const char16_t* _pr, size_t _szr)noexcept -> compare_t { return _compare_c(_pl, _szl, _pr, _szr); }
	template <> auto common::_compare_nc(const char* _pl, size_t _szl, const wchar_t* _pr, size_t _szr)noexcept -> compare_t { return _compare_c(_pl, _szl, _pr, _szr); }
	template <> auto common::_compare_nc(const char* _pl, size_t _szl, const char32_t* _pr, size_t _szr)noexcept -> compare_t { return _compare_c(_pl, _szl, _pr, _szr); }

	template <> auto common::_compare_nc(const char8_t* _pl, size_t _szl, const char* _pr, size_t _szr)noexcept -> compare_t { return _compare_c(_pl, _szl, _pr, _szr); }
	template <> auto common::_compare_nc(const char8_t* _pl, size_t _szl, const char8_t* _pr, size_t _szr)noexcept -> compare_t { return _compare_c(_pl, _szl, _pr, _szr); }
	template <> auto common::_compare_nc(const char8_t* _pl, size_t _szl, const char16_t* _pr, size_t _szr)noexcept -> compare_t { return _compare_c(_pl, _szl, _pr, _szr); }
	template <> auto common::_compare_nc(const char8_t* _pl, size_t _szl, const wchar_t* _pr, size_t _szr)noexcept -> compare_t { return _compare_c(_pl, _szl, _pr, _szr); }
	template <> auto common::_compare_nc(const char8_t* _pl, size_t _szl, const char32_t* _pr, size_t _szr)noexcept -> compare_t { return _compare_c(_pl, _szl, _pr, _szr); }

	template <> auto common::_compare_nc(const char16_t* _pl, size_t _szl, const char* _pr, size_t _szr)noexcept -> compare_t { return _compare_c(_pl, _szl, _pr, _szr); }
	template <> auto common::_compare_nc(const char16_t* _pl, size_t _szl, const char8_t* _pr, size_t _szr)noexcept -> compare_t { return _compare_c(_pl, _szl, _pr, _szr); }
	template <> auto common::_compare_nc(const char16_t* _pl, size_t _szl, const char16_t* _pr, size_t _szr)noexcept -> compare_t { return _compare_c(_pl, _szl, _pr, _szr); }
	template <> auto common::_compare_nc(const char16_t* _pl, size_t _szl, const wchar_t* _pr, size_t _szr)noexcept -> compare_t { return _compare_c(_pl, _szl, _pr, _szr); }
	template <> auto common::_compare_nc(const char16_t* _pl, size_t _szl, const char32_t* _pr, size_t _szr)noexcept -> compare_t { return _compare_c(_pl, _szl, _pr, _szr); }

	template <> auto common::_compare_nc(const wchar_t* _pl, size_t _szl, const char* _pr, size_t _szr)noexcept -> compare_t { return _compare_c(_pl, _szl, _pr, _szr); }
	template <> auto common::_compare_nc(const wchar_t* _pl, size_t _szl, const char8_t* _pr, size_t _szr)noexcept -> compare_t { return _compare_c(_pl, _szl, _pr, _szr); }
	template <> auto common::_compare_nc(const wchar_t* _pl, size_t _szl, const char16_t* _pr, size_t _szr)noexcept -> compare_t { return _compare_c(_pl, _szl, _pr, _szr); }
	template <> auto common::_compare_nc(const wchar_t* _pl, size_t _szl, const wchar_t* _pr, size_t _szr)noexcept -> compare_t { return _compare_c(_pl, _szl, _pr, _szr); }
	template <> auto common::_compare_nc(const wchar_t* _pl, size_t _szl, const char32_t* _pr, size_t _szr)noexcept -> compare_t { return _compare_c(_pl, _szl, _pr, _szr); }

	template <> auto common::_compare_nc(const char32_t* _pl, size_t _szl, const char* _pr, size_t _szr)noexcept -> compare_t { return _compare_c(_pl, _szl, _pr, _szr); }
	template <> auto common::_compare_nc(const char32_t* _pl, size_t _szl, const char8_t* _pr, size_t _szr)noexcept -> compare_t { return _compare_c(_pl, _szl, _pr, _szr); }
	template <> auto common::_compare_nc(const char32_t* _pl, size_t _szl, const char16_t* _pr, size_t _szr)noexcept -> compare_t { return _compare_c(_pl, _szl, _pr, _szr); }
	template <> auto common::_compare_nc(const char32_t* _pl, size_t _szl, const wchar_t* _pr, size_t _szr)noexcept -> compare_t { return _compare_c(_pl, _szl, _pr, _szr); }
	template <> auto common::_compare_nc(const char32_t* _pl, size_t _szl, const char32_t* _pr, size_t _szr)noexcept -> compare_t { return _compare_c(_pl, _szl, _pr, _szr); }

	template <> void common::_to_upper_nc(char* _pc, size_t& _szd, const char* _p, size_t _sz)noexcept { _to_upper_c(_pc, _szd, _p, _sz); }
	template <> void common::_to_upper_nc(char8_t* _pc, size_t& _szd, const char8_t* _p, size_t _sz)noexcept { _to_upper_c(_pc, _szd, _p, _sz); }
	template <> void common::_to_upper_nc(char16_t* _pc, size_t& _szd, const char16_t* _p, size_t _sz)noexcept { _to_upper_c(_pc, _szd, _p, _sz); }
	template <> void common::_to_upper_nc(wchar_t* _pc, size_t& _szd, const wchar_t* _p, size_t _sz)noexcept { _to_upper_c(_pc, _szd, _p, _sz); }
	template <> void common::_to_upper_nc(char32_t* _pc, size_t& _szd, const char32_t* _p, size_t _sz)noexcept { _to_upper_c(_pc, _szd, _p, _sz); }

	template <> void common::_to_lower_nc(char* _pc, size_t& _szd, const char* _p, size_t _sz)noexcept { _to_lower_c(_pc, _szd, _p, _sz); }
	template <> void common::_to_lower_nc(char8_t* _pc, size_t& _szd, const char8_t* _p, size_t _sz)noexcept { _to_lower_c(_pc, _szd, _p, _sz); }
	template <> void common::_to_lower_nc(char16_t* _pc, size_t& _szd, const char16_t* _p, size_t _sz)noexcept { _to_lower_c(_pc, _szd, _p, _sz); }
	template <> void common::_to_lower_nc(wchar_t* _pc, size_t& _szd, const wchar_t* _p, size_t _sz)noexcept { _to_lower_c(_pc, _szd, _p, _sz); }
	template <> void common::_to_lower_nc(char32_t* _pc, size_t& _szd, const char32_t* _p, size_t _sz)noexcept { _to_lower_c(_pc, _szd, _p, _sz); }

}

