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
#include <vector>
#include <utility>
#include <tuple>
#include <iostream>
#include "iron/clone.h"
#endif 

class test_cloneable final {
public:
	constexpr explicit test_cloneable(int v=0)noexcept : m_v(v) {}
	constexpr test_cloneable(test_cloneable&&)noexcept = default;
	constexpr test_cloneable& operator=(test_cloneable&&)noexcept = default;
	constexpr test_cloneable(const test_cloneable&)= delete;
	constexpr test_cloneable& operator=(const test_cloneable&) = delete;
	constexpr ~test_cloneable() = default;
	[[nodiscard]] constexpr iron::result<test_cloneable, iron::error_handler> try_clone()const noexcept { return iron::ok(test_cloneable(m_v)); }
	[[nodiscard]] constexpr test_cloneable clone()const noexcept { auto r = try_clone(); RP(r); }
	[[nodiscard]] constexpr int get()const noexcept { return m_v; }
	[[nodiscard]] constexpr bool operator==(const test_cloneable& v)const noexcept { return m_v == v.m_v; }
private:
	int m_v;
};

static_assert(iron::concepts::cloneable<test_cloneable>, "failed concept cloneable");

class test_copythrow final {
public:
	explicit test_copythrow(int v)noexcept : m_v(v) {}
	test_copythrow(const test_copythrow&) { throw std::bad_alloc(); }
	test_copythrow& operator=(const test_copythrow&)noexcept = default;
	test_copythrow& operator=(test_copythrow&&)noexcept = default;
	test_copythrow(test_copythrow&&)noexcept = default;
	~test_copythrow() = default;
	[[nodiscard]] int get()const noexcept { return m_v;}
private:
	int m_v;
};

constexpr bool f() {
	const test_cloneable c(3);
	const auto cc = iron::clone::clone_from(c);
	ASSERT_EQ(cc.get(), 3);

	const std::vector<int> v { 1, 2, 3, 4};
	using v_t = decltype(v);
	static_assert(!iron::concepts::std_pair<v_t>, "error - is a pair");
	static_assert(!iron::concepts::std_tuple<v_t>, "error - is a tuple");
	const auto vv = iron::clone::clone_from(v);
	ASSERT_EQ(v, vv);

	{
		const int a[] = { 1,2,3,4 };
		using a_t = decltype(a);
		static_assert(!iron::concepts::std_pair<a_t>, "error - is a pair");
		static_assert(!iron::concepts::std_tuple<a_t>, "error - is a tuple");
		//const auto aa = iron::clone::clone_from(a); error - deleted function
	}
	{
		constexpr int x{ 3 };
		static_assert(!iron::concepts::std_tuple<decltype(x)>, "error - is a tuple");
		constexpr auto xx = iron::clone::clone_from(x);
		ASSERT_EQ(x, xx);
	}
	{
		const auto p = std::make_pair(1, v);
		using p_t = decltype(p);
		static_assert(iron::concepts::std_pair<p_t>, "error - is not a pair");
		static_assert(iron::concepts::std_tuple<p_t>, "error - is not a tuple"); //the pair is a tuple with 2 args
		constexpr auto pair_sz = std::tuple_size_v<p_t>;
		ASSERT_EQ(pair_sz, 2);
		const auto p0 = std::get<0>(p);
		ASSERT_EQ(p0, 1);
		const auto p1 = std::get<1>(p);
		ASSERT_EQ(p1, v);
		const auto pp = iron::clone::clone_from(p);
		ASSERT_EQ(p, pp);
	}
	{ 
		const auto t0 = std::make_tuple(1, 'a', v);
		using tt_t = decltype(t0);
		static_assert(iron::concepts::std_tuple<tt_t>, "error - is not a tuple");
		static_assert(iron::concepts::std_tuple_copyable_or_cloneable<tt_t>, "error - tuple not copyable or cloneable");
		const auto t1 = iron::clone::clone_from(t0);
		ASSERT_EQ(t0, t1);
	}
	{
		const auto t0 = std::make_tuple();
		using tt_t = decltype(t0);
		static_assert(iron::concepts::std_tuple<tt_t>, "error - is not a tuple");
		static_assert(iron::concepts::std_tuple_copyable_or_cloneable<tt_t>, "error - tuple not copyable or cloneable");
		ASSERT_EQ(std::tuple_size<decltype(t0)>::value, 0);
		const auto t1 = iron::clone::clone_from(t0);
		ASSERT_EQ(t0, t1);
	}
	{
		const auto t0 = std::make_tuple(1,test_cloneable(2), test_cloneable(3));
		using tt_t = decltype(t0);
		static_assert(iron::concepts::std_tuple<tt_t>, "error - is not a tuple");
		static_assert(iron::concepts::std_tuple_copyable_or_cloneable<tt_t>, "error - tuple not copyable or cloneable");
		const auto t1 = iron::clone::clone_from(t0);
		ASSERT_EQ(t0, t1);
	}
	return true;
}

int main() {
	f();
	[[maybe_unused]] constexpr auto b = f();
	{
		const auto t0 = std::make_tuple(3,test_copythrow(3),4);
		const auto r = iron::clone::try_clone_from(t0);
		ASSERT(!r);
		const auto h = r.unwrap_err();
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable:26490)
#endif 
		std::cerr << "catch error: " << h->unsafe_code() << ' ' << reinterpret_cast<const char*>(h->unsafe_description()) << std::endl;
#if defined(_MSC_VER)
#pragma warning(pop)
#endif 
	}
}
