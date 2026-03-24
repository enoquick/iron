
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
#include "iron/result.h"
#endif 


using myr_t=iron::result<int,char>;
constexpr myr_t fok() {
	return iron::ok(0);
}

constexpr myr_t ferr() {
	return iron::err('b');
}

constexpr iron::result<int, std::vector<int>> f1()noexcept {
	return iron::ok(1);
}

constexpr myr_t f2() {
	iron::result<int, std::vector<int>> r = f1();
	if (r) return iron::ok(std::move(r.unwrap().value()));
	return iron::err('a');
}

constexpr iron::result<int, std::vector<int>> f3() {
	std::vector<int> v;
	return iron::err(std::move(v));
}

constexpr iron::result<double,int> f4() {
	iron::result<int, std::vector<int>> r = f3();
	if (!r) {
		return iron::err(99);
	}
	return iron::ok(3.0);
}

constexpr bool f0()noexcept {
	myr_t r;
	ASSERT(r);
	ASSERT_EQ(r.unwrap(), int{});
	r = iron::ok(1);
	ASSERT_EQ(r.unwrap(), 1);
	r = iron::err('b');
	ASSERT_EQ(r.unwrap_err(),'b');
	r = f2();
	const auto r0 = f4();
	[[maybe_unused]] const auto xu = r0.unwrap_or_default();
	const auto xe = r0.unwrap_err_or_default();
	ASSERT_EQ(xe, 99);
	myr_t r1 = iron::ok(1);
	myr_t r2 = iron::err('c');
	r1.swap(r2);
	ASSERT(!r1);
	ASSERT_EQ(r1.unwrap_err(),'c');
	ASSERT(r2);
	ASSERT_EQ(r2.unwrap(), 1);
	return true;
}

constexpr bool f_same()noexcept {
	iron::result<int, int> r;
	r = iron::ok(1);
	DEBUG_ASSERT(r);
	r = iron::err(2);
	DEBUG_ASSERT(!r);
	return true;
}
//#define constexpr const

class copy_throw {
public:
	copy_throw()noexcept = default;
	copy_throw(const copy_throw&) { throw int{ 0 }; }
	copy_throw(copy_throw&&)noexcept = default;
	copy_throw& operator=(const copy_throw&) { throw int{ 0 }; }
	copy_throw& operator=(copy_throw&&)noexcept = default;
	~copy_throw() = default;
};
class copy_nothrow {
public:
	copy_nothrow()noexcept = default;
	copy_nothrow(const copy_nothrow&)noexcept = default;
	copy_nothrow(copy_nothrow&&)noexcept = default;
	copy_nothrow& operator=(const copy_nothrow&)noexcept = default;
	copy_nothrow& operator=(copy_nothrow&&)noexcept = default;
	~copy_nothrow() = default;
};

#if 0 //in error
void f_copy() {
	using r_t = iron::result<copy_throw,int>;
	r_t r; 
	copy_throw c;
	r = iron::ok(c); //error
}
#endif 

constexpr void f_copynothrow()noexcept {
	using r_t = iron::result<copy_nothrow, int>;
	r_t r;
	copy_nothrow c;
	r = iron::ok(c);
}

int main() {
	f0();
	[[maybe_unused]] constexpr auto b = f_same();
	f_copynothrow();
	return 0;

}
