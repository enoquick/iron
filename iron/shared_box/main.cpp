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
#include "iron/shared_box.h"
#include "iron/collector.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <mutex>
#endif 


class myclass final {
public:
	constexpr myclass()noexcept = default;
	constexpr void push_back(int v)noexcept {
		for (auto& a : m_a) {
			if (!a) {
				a = v;
				return;
			}
		}
		PANIC("not slots free");
	}
	void print(std::ostream& os )const {
		for (auto& a : m_a) {
			if (!a) {
				break;
			}
			os << a << ' ';
		}
	}
	constexpr bool operator==(const myclass& v)const noexcept {
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable:26459)
#endif

		return std::equal(std::begin(m_a), std::end(m_a),std::begin(v.m_a));

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

	}

private:
	int m_a[2]{};
};

inline std::ostream& operator <<(std::ostream& os, const myclass& m) {
	m.print(os);
	return os;
}

class base_test {
public:
	constexpr explicit base_test(int v)noexcept : m_v(v) {}
	constexpr virtual ~base_test() = default;
	constexpr int value()const noexcept { return m_v; }
private:
	int m_v;
};

class c1_test : public base_test {
public:
	constexpr explicit c1_test(int v)noexcept : base_test(v) {}
	constexpr ~c1_test(){}
};

class c2_test : public base_test {
public:
	constexpr explicit c2_test(int v)noexcept : base_test(v) {}
	constexpr ~c2_test() {}

};

template <typename T>
constexpr T convert(int v)noexcept {
	if constexpr (std::is_same_v<T,int>) {
		return v;
	}
	else if constexpr (std::is_same_v<T,myclass>) {
		T m;
		m.push_back(v);
		return m;
	}
	else {
		static_assert(iron::unsafe::asserts::always_false<T>, "internal error");
	}
}


template <typename SH>
constexpr bool f_box()noexcept {
	using sh_t = SH;
	static_assert(iron::concepts::shared_box<sh_t>, "is not a shared box");
	using v_t = typename sh_t::value_t;
	constexpr int value = 123;
	constexpr auto x = convert<v_t>(value);
	{
		sh_t sh;
		ASSERT(!sh);
		ASSERT(!sh.use_count());
		ASSERT(!sh.use_weak_count());
		ASSERT(!sh.unsafe_data());
		{
			auto w = sh.make_weak();
			ASSERT(!sh.use_count());
			ASSERT(!sh.use_weak_count());
			ASSERT(!w);
			const auto lk = w.lock();
			ASSERT(!lk);
		}
		sh = sh_t::from(iron::make_box<v_t>(convert<v_t>(value)));
		ASSERT_EQ(sh.use_count(), 1);
		ASSERT(!sh.use_weak_count());
		ASSERT(sh);
		auto sh1 = sh;
		ASSERT_EQ(sh.use_count(), 2);
		ASSERT_EQ(sh1.use_count(), 2);
		ASSERT_EQ(sh.value().unwrap(), x);
		ASSERT_EQ(*sh1, x);
		auto weak = sh.make_weak();
		ASSERT_EQ(weak.use_count(), 2);
		ASSERT_EQ(weak.use_weak_count(), 1);
		ASSERT(weak);
		ASSERT_EQ(weak.lock().value().unwrap(), x);
		sh.reset();
		ASSERT(!sh.unsafe_data());
		ASSERT(!sh.use_count());
		ASSERT_EQ(sh1.use_count(), 1);
		sh1.reset();
		ASSERT(!weak);
		sh1 = weak.lock();
		ASSERT(!sh1);
	}
	{
		auto csh = sh_t::from(iron::make_box<v_t>(convert<v_t>(value)));
		ASSERT_EQ(csh.use_count(), 1);
		ASSERT_EQ(*csh, x);
		csh = sh_t::from(iron::make_box<v_t>(convert<v_t>(value)));
		const auto csh1 = csh;
		ASSERT_EQ(csh.use_count(), 2);
		ASSERT_EQ(csh1.use_count(), 2);
	}
	{
		auto csh = sh_t::from(iron::make_box<v_t>(convert<v_t>(value)));
		auto box = csh.release();
		ASSERT(box);
		ASSERT(!csh);
		ASSERT_EQ(*box, x);
	} 
	return true;
}


template <typename SH>
void f_th(SH sh,std::size_t thn) {
	static_assert(iron::concepts::shared_box<SH>, "is not a shared box");
	const auto sh1 = sh;
	static std::mutex m;
	std::lock_guard<std::mutex> lk(m);
	std::cout << thn << " value: " << sh1.value().unwrap().value() << " use count: " << sh1.use_count() << std::endl;
}

template <typename T> 
constexpr void test_make_t(T& sh)noexcept {
	static_assert(iron::concepts::shared_box<T>, "is not a shared box");
	ASSERT_EQ(sh.use_count(), 1);
	auto w = sh.make_weak();
	ASSERT(w);
	sh.reset();
	ASSERT(!w);
	//auto refv = w.value(); //error
	sh = w.lock();
	ASSERT(!sh);
}

constexpr bool test_make()noexcept {
	using v_t = myclass;
	constexpr int value = 123;
	auto shrc = iron::make_rc<v_t>(convert<v_t>(value));
	ASSERT_EQ(shrc.use_count(), 1);
	test_make_t(shrc);
	if (!std::is_constant_evaluated()) {
		auto sharc = iron::make_arc<v_t>(convert<v_t>(value));
		ASSERT_EQ(sharc.use_count(), 1);
		test_make_t(sharc);
	}
	return true;
}

template <typename T>
constexpr bool test_weak()noexcept {
	static_assert(iron::concepts::shared_box<T>, "is not a shared box");
	using v_t = typename T::value_t;
	static_assert(std::is_same_v<v_t, int>,"illegal type");
	using w_t = typename T::weak_t;
	w_t w;
	static_assert(iron::concepts::weak_box<w_t>, "internal error");
	{
		constexpr int v = 20;
		auto sh = T::from(iron::make_box<v_t>(v));
		w = sh.make_weak();
		ASSERT_EQ(sh.use_count(), 1);
		ASSERT_EQ(sh.use_weak_count(), 1);
		ASSERT_EQ(*sh,v);
		ASSERT(w);
	}
	ASSERT(!w);
	{
		constexpr int v = 20;
		auto sh = T::from(iron::make_box<v_t>(v));
		w = sh.make_weak();
		ASSERT_EQ(sh.use_count(), 1);
		ASSERT_EQ(sh.use_weak_count(), 1);
		ASSERT_EQ(*sh, v);
		ASSERT(w);
		const auto box = sh.release();
		ASSERT(box);
		ASSERT(!sh);
		ASSERT(!w);
	}
	ASSERT(!w);
	return true;
}

void test_threads() {
	constexpr int value = 123;
	using v_t = myclass;
	auto v = std::vector<std::thread>();
	constexpr std::size_t n = 10;
	v.reserve(n);
	const auto sh = iron::arc<v_t>::from(iron::make_box<v_t>(convert<v_t>(value)));
	for (std::size_t i = 0; i < n; ++i) {
		using sh_t = std::decay_t<decltype(sh)>;
		v.push_back(std::thread(f_th<sh_t>,sh,i));
	}
	for (auto& th : v) {
		th.join();
	}
}

class test_not_set final {
public:
	test_not_set()noexcept;
	test_not_set(const test_not_set&)noexcept;
	test_not_set& operator=(const test_not_set&)noexcept;
	test_not_set(test_not_set&&)noexcept;
	test_not_set& operator=(test_not_set&&)noexcept;
	~test_not_set();
private:
	class not_set;
	iron::rc<not_set> m_sh;
};


iron::rc<base_test> make_base()noexcept;

template <typename SH>
bool test_cast()noexcept {
	static_assert(iron::concepts::shared_box<SH>, "is not a shared box");
//	using sh_based_t = SH:: template rebind_t<base_test>;
	using sh_c1_t = SH:: template rebind_t<c1_test>;
	auto x=iron::make_box<c1_test>(1234);
	auto sh_c1 = sh_c1_t::from(std::move(x));
	auto sh_base = sh_c1. template upcast<base_test>();
	ASSERT(sh_base);
	{
		const auto data = sh_base.unsafe_data();
		static_assert(std::is_same_v<std::decay_t<decltype(*data)>, base_test>, "internal error");
		const auto value = *sh_base;
		static_assert(std::is_same_v<std::decay_t<decltype(*value)>, base_test>, "internal error");
	}
	{
		const auto sh_c1c = sh_base. template downcast<c1_test>();
		const auto data = sh_c1c.unsafe_data();
		static_assert(std::is_same_v<std::decay_t<decltype(*data)>, c1_test>, "internal error");
		const auto value = *sh_c1c;
		static_assert(std::is_same_v<std::decay_t<decltype(*value)>, c1_test>, "internal error");

	}
	{
		const auto sh_c2c = sh_base. template downcast<c2_test>();
		ASSERT(!sh_c2c);
	}
	return true;
}

constexpr bool test_compare()noexcept {
	iron::rc<int> rc1;
	iron::rc<int> rc2;
	ASSERT_EQ(rc1,rc2);
	rc1 = iron::make_rc<int>(1);
	rc2 = iron::make_rc<int>(1);
	ASSERT_NE(rc1,rc2);
	rc2 = rc1;
	ASSERT_EQ(rc1, rc2);
	const auto w = rc2.make_weak();
	ASSERT_EQ(w.lock(), rc1);
	ASSERT_EQ(w.lock(), rc2);
	rc1.reset();
	rc2.reset();
	ASSERT_EQ(w.lock(), rc1);
	ASSERT_EQ(w.lock(), rc2);
	auto sh_c1 = iron::make_rc<c1_test>(1); //OK - it's compatible
	if (!std::is_constant_evaluated()) {
		const auto sh_b1 = sh_c1.template upcast<base_test>();
		ASSERT_EQ(sh_c1, sh_b1);
	}
	return true;
}

int main() {
	{
		f_box<iron::arc<int>>();
//		[[maybe_unused]] constexpr auto b1 = f_box<iron::arc<int>>(); //error - arc cannot be constexpr 

		f_box<iron::rc<int>>();
		[[maybe_unused]] constexpr auto b0 = f_box<iron::rc<int>>();
	}

	{
		f_box<iron::arc<myclass>>();
		f_box<iron::rc<myclass>>();
		[[maybe_unused]] constexpr auto b2 = f_box<iron::rc<myclass>>();
	}

	{
		test_make();
		[[maybe_unused]] constexpr auto b5 = test_make();

	}
	{
		test_weak<iron::rc<int>>();
		[[maybe_unused]] constexpr auto b=test_weak<iron::rc<int>>();
		test_weak<iron::arc<int>>();
	}
	test_threads();
	{
		test_cast<iron::rc<c1_test>>();
		test_cast<iron::arc<c1_test>>();

	}
	{
		test_compare();
#ifndef __clang__
		//for a bug in clang19 not execute in modality constexpr
		[[maybe_unused]] constexpr auto _b = test_compare();
#endif 
	}
	{
		test_not_set t;
	}
	{
		auto sh= make_base();
		ASSERT(sh);
	}
}


class test_not_set::not_set {  };
test_not_set::test_not_set()noexcept = default;
test_not_set::~test_not_set()= default;
test_not_set::test_not_set(test_not_set&&)noexcept = default;
test_not_set& test_not_set::operator=(test_not_set&&)noexcept = default;


iron::rc<base_test> make_base()noexcept {
	auto sh=iron::make_rc<c1_test>(1);
	return sh.template upcast<base_test>();
}
