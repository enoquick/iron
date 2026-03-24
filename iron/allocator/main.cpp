#ifdef USE_MODULES
#include "iron/macros.h"
import iron.core;
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable:5050)
#endif 
import std.core;
#if defined(_MSC_VER)
#pragma warning(pop)
#endif 
#else 
#define IRON_ALLOCATOR_ALLOC_RESET
#define IRON_ALLOCATOR_DEALLOC_RESET
#include "iron/unsafe/allocator.h"
#include <cstring>
#endif 

constexpr bool f()noexcept {
	iron::unsafe::allocator<int> a;
	auto p = a.allocate(1);
	if (p) {
		std::construct_at(p,1);
		a.deallocate(p,1);
	}
	return true;
}


int main() {
	{
		iron::unsafe::allocator<int> a;
		auto p = a.allocate(10);
		std::memset(p, 0, sizeof(int) * 10);
		a.deallocate(p,10);
	}
	{
		iron::unsafe::allocator<char> a;
		auto p = a.allocate(10);
		std::memset(p, 0, sizeof(char) * 10);
		a.deallocate(p, 10);
	}
	{
		struct S {};
		iron::unsafe::allocator<S> a;
		auto p = a.allocate(10);
		std::memset(p, 0, sizeof(S) * 10);
		a.deallocate(p, 10);
	}
	{
		constexpr auto b=f();
		static_assert(b);
	}
}
