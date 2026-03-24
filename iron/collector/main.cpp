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
#include "iron/collector.h"
#include "iron/vector.h"
#endif 
 
class not_copiable {
public:
	constexpr not_copiable(int x)noexcept : m_i(x) { }
	constexpr not_copiable(const not_copiable&) = delete;
	constexpr not_copiable(not_copiable&&)noexcept = default;
	constexpr not_copiable& operator=(const not_copiable&) = delete;
	constexpr not_copiable& operator=(not_copiable&&)noexcept = default;
	constexpr not_copiable clone()const noexcept { return not_copiable(m_i); }
	constexpr iron::result<not_copiable, iron::error_handler> try_clone()const noexcept { return iron::ok(clone()); }
	constexpr bool operator==(const not_copiable& v)noexcept { return m_i == v.m_i; }
private:
	int m_i;
};

#if 0
constexpr bool adapter() {
	auto v = iron::make_vector(1, 2, 3, 4);
	const auto sz = v.size();
	auto box = v.release();
	ASSERT(v.empty());
	v = iron::make_vector(std::move(box));
	ASSERT_EQ(sz, v.size());
	return true;
}
#endif 

template <typename T>
constexpr bool f()noexcept {
	static_assert(std::is_integral_v<T>);
	{
		auto v = iron::collector::make_vector<T>(1, 2, 3, 4);
		ASSERT_EQ(v.size(), 4);
		ASSERT_EQ(v.front().unwrap(), 1);
		ASSERT_EQ(v.back().unwrap(), 4);
		auto seq = v.seq();
		auto v1 = iron::collector::make_vector<T>(1, 2, 3, 4,seq);
		seq = {};
		ASSERT_EQ(v1.size(), 8);
		ASSERT_EQ(v1.back().unwrap(), 4);
		v1 = iron::collector::make_vector<T>(v.release());
		ASSERT_EQ(v1.size(), 4);
	}

	return true;
}



int main() {
	f<int>();
//	[[maybe_unused]] constexpr bool b = f<int>();
//	adapter();
//	[[maybe_unused]] constexpr bool b1 = adapter();
}

