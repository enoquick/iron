#ifndef _IRON__INTERNAL_STACK_AREA_H
#define _IRON__INTERNAL_STACK_AREA_H

namespace iron::_internal {
	template <std::size_t _SZ>
	struct _stack_area {
		using _size_t = std::size_t;
		static constexpr _size_t _area_size = _SZ;
		union {
			int _dummy; //for align
			unsigned char _area[_area_size];
		};
	};
}

#endif
