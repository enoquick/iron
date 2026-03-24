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
#include "iron/bit_set.h"
#include "iron/enum_set.h"
#include "iron/collector.h"
#endif 

constexpr bool test_bitset() {
	using bitset_t = iron::bit_set<100>;
	bitset_t b;
	ASSERT(b.none());
	for (bitset_t::size_t i = 0; i < bitset_t::size(); ++i) {
		b.set(i);
		ASSERT(b.test(i));
	}
	ASSERT(b.all());
	auto b1 = b;
	ASSERT(b1.all());
	for (bitset_t::size_t i = 0; i < bitset_t::size(); ++i) {
		b.reset(i);
		ASSERT(!b.test(i));
	}
	ASSERT(b.none());
	b1 = b;
	ASSERT(b1.none());
	b.set_all();
	b.reset_all();
	auto x = iron::make_bit_set<100>();
	ASSERT(!x);
	x = iron::make_bit_set<100>(2,3);
	ASSERT(!x.test(1));
	ASSERT(x.test(2));
	ASSERT(x.test(3));
	ASSERT(!x.test(4));
	return true;
}

template <typename T>
constexpr bool test_enum_set() {
	using value_t = T;
	using enumset_t = iron::enum_set<value_t>;
	enumset_t s;
	s.set(value_t::a);
	ASSERT(s.test(value_t::a));
	s.reset(value_t::a);
	ASSERT(!s.test(value_t::a));
	s.set(value_t::b);
	ASSERT(s.test(value_t::b));
	s.reset(value_t::b);
	ASSERT(!s.test(value_t::b));
	ASSERT(!enumset_t::is_bit(value_t::d));
	const auto x = iron::make_enum_set(value_t::a,value_t::b);
	ASSERT(x.test(value_t::a));
	ASSERT(x.test(value_t::b));
	ASSERT(x.test_or({ value_t::a }));
	ASSERT(!x.test(value_t::c));
	ASSERT(x.test_and({ value_t::a,value_t::b }));
	ASSERT(!x.test_and({ value_t::a,value_t::c }));
	return true;
}

typedef enum class enum_1 : unsigned char {
	a
	, b
	, c
	, d = 200 //invalid bit
} enum_1_t;


int main() {
	test_bitset();
	[[maybe_unused]] constexpr auto b = test_bitset();
	test_enum_set<enum_1_t>();
	[[maybe_unused]] constexpr auto b1 = test_enum_set<enum_1_t>();
}
