
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
#include "iron/box.h"
#include "iron/collector.h"
#include <string>
#include <map>
#endif

class base {
public:
	constexpr virtual ~base()= default;
protected:
	constexpr base()noexcept = default;
	base(const base&) = delete;
	base& operator=(const base&) = delete;
	constexpr base(base&&)noexcept = default;
	constexpr base& operator=(base&&)noexcept = default;
};

class derived : public base {
public:
	class included { constexpr included()noexcept {} };
	constexpr derived() = default;
	derived(const derived&) = delete;
	derived& operator=(const derived&) = delete;
	constexpr derived(derived&&)noexcept = default;
	constexpr derived& operator=(derived&&)noexcept = default;
	constexpr ~derived() = default;
	explicit derived(int i) { throw int{ 3 }; } //not constexpr
	constexpr int getint()const noexcept { return m_i; }
private:
	int m_i{4};
};

iron::box<base> make_base()noexcept;

static constexpr void test_derived()noexcept {
	auto box_derived = iron::make_box<derived>();
	auto box_base = box_derived.template upcast<base>();
	ASSERT(box_base);
	ASSERT(!box_derived);
	box_derived = box_base.template downcast<derived>();
	ASSERT(box_derived);
	ASSERT(!box_base);
	const auto v = box_derived.release_value();
	ASSERT(!box_derived); 
	if (!std::is_constant_evaluated()) {
		auto b = make_base();
		ASSERT(b);
		auto d = b.template downcast<derived>();
		ASSERT(d);
	}
}

class box_included final {
public:
	box_included()noexcept;
	box_included(const box_included&) = delete;
	box_included& operator=(const box_included&) = delete;
	box_included(box_included&&)noexcept;
	box_included& operator=(box_included&&)noexcept;
	~box_included();
private:
	class not_set;
	iron::box<not_set> m_x;

};

static constexpr bool f()noexcept {
	auto r = iron::make_box<int>();
	ASSERT_EQ(r.value().unwrap(), 0);

	if (!std::is_constant_evaluated()) {
		auto r1 = iron::try_make_box<derived>(3); //throw exception
		ASSERT(!r1); //failure
		//auto rfailure1 = iron::memory::try_make_box<test>(std::string{}); //FAILURE - the construction is not valid
		static_assert(!iron::concepts::constructible_at<derived, std::string>,"failure check constraint");
	}
	if (std::is_constant_evaluated()) {
#ifdef __clang__ 
	//the test_derived in clang++ <= 19 failed for a compiler bug in virtual destructor

#elif defined(__GNUC__)
	//the test_derived in g++ <= 14 failed for a compiler bug in dynamic cast
#elif defined(_MSC_VER) 
		test_derived(); //in MS MSVC pass 
#endif 
	}
	else {
		test_derived();
	}
	auto box_empty_str = iron::make_box<std::string>();
	ASSERT(box_empty_str);
	auto box_str = iron::make_box<std::string>(std::string("abc"));
	ASSERT_EQ(**box_str,std::string("abc"));
	const auto box_str_clone = box_str.clone();
	ASSERT_EQ(**box_str_clone,std::string("abc"));
	const auto s = box_str.release_value();
	ASSERT_EQ(s, std::string("abc"));
	ASSERT(!box_str);
	const auto s1 = box_str.release_value_or_default();
	ASSERT(!box_str);
	ASSERT(s1.empty());
	const auto ebx = iron::make_empty_box<int>();
	ASSERT(!ebx);
	return true;
}

int main() {
	f();
	[[maybe_unused]] constexpr auto b = f();
	box_included x;
}

class box_included::not_set {};
box_included::box_included()noexcept {}
box_included::box_included(box_included&&)noexcept = default;
box_included::~box_included() = default;

iron::box<base> make_base()noexcept {
	auto box = iron::make_box<derived>();
	return box.template cast<base>();
}



