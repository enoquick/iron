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
#include "iron/adapter_box.h"
#endif

using rv_t = iron::unsafe::raw_vector<int, iron::unsafe::allocator<int>>;
using va_t = iron::adapter_box<rv_t>;

constexpr bool f_vector()noexcept {
	rv_t mbox;
	auto h=mbox.reserve(2);
	ASSERT(h);
	ASSERT_EQ(mbox.capacity(), 2);
	h=mbox.push_back(1);
	ASSERT(h);
	ASSERT_EQ(mbox.size(), 1);
	h = mbox.push_back(2);
	ASSERT(h);
	ASSERT_EQ(mbox.size(), 2);
	ASSERT_EQ(*(mbox.data()), 1);
	ASSERT_EQ(*(mbox.nth(1)), 2);
	//h = box.push_back(3); failed - unsufficient space
	h = mbox.reserve(3);
	ASSERT(h);
	ASSERT_EQ(*(mbox.nth(0)), 1);
	ASSERT_EQ(*(mbox.nth(1)), 2);
	h = mbox.push_back(3);
	ASSERT(h);
	ASSERT_EQ(*(mbox.nth(2)), 3);
	ASSERT_EQ(*(mbox.front()), 1);
	ASSERT_EQ(*(mbox.back()), 3);
	auto v = mbox.pop_back();
	ASSERT_EQ(v,3);
	ASSERT_EQ(mbox.size(), 2);
	auto box = va_t::unchecked_from(std::move(mbox));
	mbox = box.unsafe_release();
	ASSERT_EQ(mbox.capacity(), 3);
	ASSERT_EQ(mbox.size(),2);
	ASSERT_EQ(*(mbox.front()), 1);
	ASSERT_EQ(*(mbox.back()),2);

	return true;
}

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable:26800)
#endif 


#if defined(_MSC_VER)
#pragma warning(pop)
#endif 

int main() {
	f_vector();
	[[maybe_unused]] constexpr auto b = f_vector();
}
