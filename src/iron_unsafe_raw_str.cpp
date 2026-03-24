
#ifdef USE_MODULES
#else
#include "iron/unsafe/raw_str.h"
#endif 

namespace iron::unsafe::str {

	template <> auto raw_str<char, iron::unsafe::allocator<char>, iron::str_default_fixedsize, false>::_reserve_nc(size_t _abs_capacity)noexcept -> error_handler_t {
		return _reserve_c(_abs_capacity);
	}
	template <> auto raw_str<char, iron::unsafe::allocator<char>, iron::str_default_fixedsize, true>::_reserve_nc(size_t _abs_capacity)noexcept -> error_handler_t {
		return _reserve_c(_abs_capacity);
	}

	template <> auto raw_str<char8_t, iron::unsafe::allocator<char8_t>, iron::str_default_fixedsize, false>::_reserve_nc(size_t _abs_capacity)noexcept -> error_handler_t {
		return _reserve_c(_abs_capacity);
	}
	template <> auto raw_str<char8_t, iron::unsafe::allocator<char8_t>, iron::str_default_fixedsize, true>::_reserve_nc(size_t _abs_capacity)noexcept -> error_handler_t {
		return _reserve_c(_abs_capacity);
	}

	template <> auto raw_str<char16_t, iron::unsafe::allocator<char16_t>, iron::str_default_fixedsize, false>::_reserve_nc(size_t _abs_capacity)noexcept -> error_handler_t {
		return _reserve_c(_abs_capacity);
	}
	template <> auto raw_str<char16_t, iron::unsafe::allocator<char16_t>, iron::str_default_fixedsize, true>::_reserve_nc(size_t _abs_capacity)noexcept -> error_handler_t {
		return _reserve_c(_abs_capacity);
	}

	template <> auto raw_str<wchar_t, iron::unsafe::allocator<wchar_t>, iron::str_default_fixedsize, false>::_reserve_nc(size_t _abs_capacity)noexcept -> error_handler_t {
		return _reserve_c(_abs_capacity);
	}
	template <> auto raw_str<wchar_t, iron::unsafe::allocator<wchar_t>, iron::str_default_fixedsize, true>::_reserve_nc(size_t _abs_capacity)noexcept -> error_handler_t {
		return _reserve_c(_abs_capacity);
	}

	template <> auto raw_str<char32_t, iron::unsafe::allocator<char32_t>, iron::str_default_fixedsize, false>::_reserve_nc(size_t _abs_capacity)noexcept -> error_handler_t {
		return _reserve_c(_abs_capacity);
	}
	template <> auto raw_str<char32_t, iron::unsafe::allocator<char32_t>, iron::str_default_fixedsize, true>::_reserve_nc(size_t _abs_capacity)noexcept -> error_handler_t {
		return _reserve_c(_abs_capacity);
	}

}

