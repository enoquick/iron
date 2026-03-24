
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
#include <iostream>
#include "iron/option.h"
#include <string> 
#endif 

iron::option<int> f()noexcept {
	return iron::none();
}

constexpr bool test_str() {
	constexpr auto i = 1;
	auto opt=iron::some(i);
	ASSERT(opt);
	const auto wv1=opt.unwrap();
	auto& v1=*wv1;
	ASSERT_EQ(v1,1);
	return true;
}
constexpr bool test_opt() {
	{
		const auto n = iron::none();
		const auto b = n.is_none();
		ASSERT(b);
	}
	{
		constexpr auto v = 10;
		const auto b = iron::some(v).is_some();
		ASSERT(b);
		auto opt=iron::some(v);
		const auto i = opt.unwrap();
		ASSERT_EQ(i, v);
		auto& x = opt.unchecked_cunwrap();
		ASSERT_EQ(x, v);
		static_assert(std::is_const_v<std::remove_reference_t<decltype(x)>>, "x is const");
		auto& y = opt.unchecked_unwrap();
		static_assert(!std::is_const_v<std::remove_reference_t<decltype(y)>>, "y is not const");
		y = 11; //OK
	}
	{
		constexpr auto v = 10;
		const auto opt = iron::some_emplace<int>(v);
		const auto ref_i = opt.unwrap();
		static_assert(iron::concepts::ref_wrapper<decltype(ref_i)>, "ref_i must be a ref_wrapper");
		ASSERT_EQ(ref_i, v);
	}
#if 0 //error - not pass concept  
	{
		const std::string s("abc");
		const auto r = iron::some_emplace<std::string>(s);
	}
#endif 
	{
		//none.unwrap(); PANIC
	}
	{
		using v_t = std::vector<int>;
		v_t v{ 1, 2, 4 };
		const auto opt = iron::some(std::move(v));
		ASSERT_EQ(opt.unwrap()->front(), 1);
	}
	{
		const auto  x = iron::some(int{ 2 });
		auto b = x.is_some_and([](const int& y)noexcept->bool { return y > 1; });
		ASSERT(b);
		b = x.is_some_and([](const int& y)noexcept { return y > 2; });
		ASSERT(!b);
	}
	{
		auto v = iron::none<int>().unwrap_or_default();
		ASSERT_EQ(v, 0);
	}
	{
		const auto v1 = iron::none<int>();
		auto v2 = v1.clone();
		static_assert(std::is_nothrow_copy_constructible_v<int>, "ccc");
		iron::option<int> x(v1);
	}
	{
		const auto opt = iron::some(1);
		const auto x = opt.clone();
	}
	{
		const auto opt = iron::some(iron::option<int>());
		const auto x = opt.clone();
	}


	{
		using opt_t = iron::option<iron::ref_wrapper<int>>;
		opt_t x;
		ASSERT(x.is_none());
		int i = 5;
#if IRON_SECURITY_CHECKER		
		auto ck = x.checker();
		//iron::security_checker ck;
#endif 
		auto opt = iron::some(iron::make_ref(x, i));
		auto& cref = opt.unchecked_cunwrap();
		static_assert(std::is_same_v<const int, std::remove_reference_t<decltype(cref)>>, "cref must be const int");
		auto& ref = opt.unchecked_unwrap();
		static_assert(std::is_same_v<int, std::remove_reference_t<decltype(ref)>>, "ref must be int");
		ASSERT_EQ(ref, i);
		const auto k = opt.cunwrap();
		ASSERT_EQ(k, i);
		ref = i;
	}
	{
		iron::option<const int*> x;
		const int* p{};
		x = iron::some(p);
	}
	if (std::is_constant_evaluated()) {
#ifndef __clang__ //in clang++15 std::string failed with constexpr
		const auto opt = iron::some(std::string("abc"));
		const auto x = opt.clone();
#endif 
	}
	else {
		const auto opt = iron::some(std::string("abc"));
		const auto x = opt.clone();
	}
	return true;
}

int main() {
	test_opt();
	[[maybe_unused]] constexpr bool b1 = test_opt();
	test_str();
	[[maybe_unused]] constexpr bool b2 = test_str();

	static_assert(iron::concepts::option<iron::option<int>>);
	return 0;

}
