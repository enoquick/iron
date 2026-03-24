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
#include "iron/sequence.h"
#endif 


class master final {
public:
IRON_SECURITY_METHOD()
private:
	IRON_SECURITY_DCL();
};

template <typename ITER_TYPE,typename ITER,typename _MASTER>
static inline constexpr auto make_seq(const ITER& b,const ITER& e,const _MASTER& _master)noexcept {
	return iron::sequence<ITER_TYPE>::unchecked_from(_master, b, e);
}

using c_t = iron::unsafe::iterators::contiguous_iterator<int>;  
using cc_t = iron::unsafe::iterators::contiguous_iterator<const int>; 

#if 1
template <typename T,std::size_t N>
constexpr bool f_unicode_iter(const T(&str)[N]) {
	const auto b = std::begin(str);
	const auto e = std::end(str);
	using iter_t = iron::unsafe::iterators::unicode_iterator<const T>;
	static_assert(std::forward_iterator<iter_t>, "internal error");
	iter_t iter(b, e);
	char32_t ch = *iter++;
	ASSERT_EQ(ch,U'A');
	ch = *iter++;
	ASSERT_EQ(ch, 0x20AC); //euro
	ch = *iter++;
	ASSERT_EQ(ch, iter_t::default_replacement_chr);
	ch = *iter++;
	if constexpr (std::is_same_v<T,char8_t>) {
		ASSERT_EQ(ch, iter_t::default_replacement_chr);
		ch = *iter++;
	}
	ASSERT_EQ(ch, U'B');
	ASSERT(!iter);
	return true;
}
#endif 

constexpr bool f() {
	int v[] = { 1,2,3,4,5,6 };
	master ms;
#if IRON_SECURITY_CHECKER
	static_assert(iron::concepts::has_security_checker<master>, "internal error");
#endif 
	static_assert(std::forward_iterator<c_t>);
	static_assert(std::bidirectional_iterator<c_t>);
	static_assert(std::random_access_iterator<c_t>);
	static_assert(std::contiguous_iterator<c_t>);

	static_assert(std::forward_iterator<cc_t>);
	static_assert(std::bidirectional_iterator<cc_t>);
	static_assert(std::random_access_iterator<cc_t>);
	static_assert(std::contiguous_iterator<cc_t>);
	
	
	const c_t b(std::begin(v));
	const c_t e(std::end(v));
	{ //while
		auto s = make_seq<cc_t>(b, e, ms);
		auto& b1 = s.begin();
		const auto e1 = s.end();
		int start = 1;
		while (b1 != e1) {
			ASSERT_EQ(start, *b1);
			++b1;
			++start;
		}
		ASSERT(!s); //OK
	}
	{ //for
		auto s = make_seq<cc_t>(b, e, ms);
		int start = 1;
		for (auto& x : s) {
			ASSERT_EQ(start, x);
			++start;
		}
		ASSERT(s); //the for cicle copy begin iter
	}
	{ //peek
		auto s=make_seq<cc_t>(b,e,ms);
		const auto sx = s;
#if IRON_SECURITY_CHECKER
		ASSERT(s.is_same_container(sx));
#endif 
		ASSERT_EQ(s.size().unwrap(), 6);
		ASSERT_EQ(s.clone().count(), 6);
		ASSERT_EQ(s.clone().count(), static_cast<std::size_t>(e - b));
		int c = v[0];
		while (const auto opt = s.peek()) {
			ASSERT(opt);
			ASSERT_EQ(opt.unwrap(), c);
			const auto r = *s++;
			ASSERT_EQ(r, c);
			++c;
		}
		ASSERT_EQ(c, 7);
	}
	{//skip
		auto s=make_seq<cc_t>(b,e,ms);
		s.skip(2).skip(0);
		ASSERT_EQ(*s, 3);
		s.skip(10);
		ASSERT(!s);
	}
	{//skip_if
		auto s = make_seq<cc_t>(b, e,ms);
		ASSERT_EQ(*s, 1);
		auto s1 = s.skip_if([](const int& v)constexpr noexcept { return v == 1; });
		ASSERT_EQ(*s1, 2);
		s = make_seq<cc_t>(b, e,ms);
		ASSERT_EQ(*s, 1);
		s1 = s.skip_if([](const int& v)constexpr noexcept { return v == 2; });
		ASSERT_EQ(*s1, 1);
	}
	{ //take && slice
		auto s = make_seq<cc_t>(b, e,ms);
		auto s1 = s.take(2);
		ASSERT(!s);
		ASSERT_EQ(*s1++, 1);
		ASSERT_EQ(*s1++, 2);
		ASSERT(!s1);
		s = make_seq<cc_t>(b, e,ms);
		s1 = s.skip(1).take(2);
		ASSERT(!s);
		ASSERT_EQ(*s1++, 2);
		ASSERT_EQ(*s1++, 3);
		ASSERT(!s1);
		s = make_seq<cc_t>(b, e,ms);
		s1 = s.skip(4).take(100);
		ASSERT(!s);
		ASSERT_EQ(*s1++, 5);
		ASSERT_EQ(*s1++, 6);
		ASSERT(!s1);
		s = make_seq<cc_t>(b, e,ms);
		s1 = s.slice(4,100); //same as skip(4).take(100) 
		ASSERT(!s);
		ASSERT_EQ(*s1++, 5);
		ASSERT_EQ(*s1++, 6);
		ASSERT(!s1);
		s = make_seq<cc_t>(b, e,ms);
		s1 = s.take(0);
		ASSERT(!s);
		ASSERT(!s1);
	}
	{//take_if
		auto s = make_seq<cc_t>(b, e,ms);
		ASSERT_EQ(*s, 1);
		auto s1 = s.take_if([](const int& v)constexpr noexcept { return v == 1; });
		ASSERT_EQ(*s1, 1);
		s = make_seq<cc_t>(b, e, ms);
		ASSERT_EQ(*s, 1);
		s1 = s.take_if([](const int& v)constexpr noexcept { return v == 3; });
		ASSERT_EQ(*s1, 2);
	}
	{//next
		auto s=make_seq<cc_t>(b,e,ms);
		int c = v[0];
		while (const auto r = s.next()) {
			ASSERT_EQ(c++, r.unwrap());
		}
		ASSERT_EQ(c, 7);
	}
	{//all 
		auto s=make_seq<cc_t>(b,e,ms);
		s.all([](const int& v)constexpr noexcept { return v < 100; }); //check if all numbers are < 100
		ASSERT(!s); //true
		s=make_seq<cc_t>(b,e,ms);
		s.all([](const int& v)constexpr noexcept { return v % 2 == 1; }); //check for	all numbers are odd
		ASSERT(s); //false
		ASSERT_EQ(s.peek().unwrap(), 2); //2 is not odd
	}
	{ //any 
		auto s=make_seq<cc_t>(b,e,ms);
		s.any([](const int& v)constexpr noexcept { return v > 100; }); //check if exist a number > 100 
		ASSERT(!s); //false
		s=make_seq<cc_t>(b,e,ms);
		s.any([](const int& v)constexpr noexcept { return v % 2 == 1; }); //check if exist an odd number
		ASSERT(s); //true
		ASSERT_EQ(s.peek().unwrap(), 1); //i1 is odd
	}
	{ //count_if
		auto s=make_seq<cc_t>(b,e,ms);
		const std::size_t c = s.count_if([](const int& v)constexpr noexcept { return v > 2; }); //count numbers > 2
		ASSERT_EQ(c, 4);
	}
#if 0 //TODO
	{ //chain
		auto s=make_seq<cc_t>(b,e,ck);
		int v1[] = { 7,8,9 };
//		static_assert(std::contiguous_iterator<cc_t>);

		const c_t b1(std::begin(v1));
		const c_t e1(std::end(v1));
		auto s1=make_seq<cc_t>(b1,e1,ck);
		auto schain = s.chain(s1);
		ASSERT(schain);
		int c = v[0];
		while (schain) {
			ASSERT_EQ(schain.next().unwrap(), c++);
		}
		ASSERT_EQ(c, 10);
	}
#endif 

#if 1
	{ //enumerate
		char vc[] = { 'a','b','c'};
		using iter_t = iron::unsafe::iterators::contiguous_iterator<char>;
		auto s = make_seq<iter_t>(iter_t(std::begin(vc)), iter_t(std::end(vc)),ms);
		auto se = s.enumerate(1);
		static_assert(std::forward_iterator<typename decltype(se)::iter_t>);
//		static_assert(std::forward_iterator<decltype(se)>);
		ASSERT(se);
		{
			const auto [n, r] = *se++;
			ASSERT(n == 1 && r == 'a');
		}
		{
			const auto [n, r] = *se++;
			ASSERT(n == 2 && r == 'b');
		}
		{
			const auto [n, r] = *se++;
			ASSERT(n == 3 && r == 'c');
		}
		ASSERT(!se);
	}
#endif 

	{ //eq 
		auto s1=make_seq<cc_t>(b,e,ms);
		int v2[] = { 1,2,3,4,5,6 };
		const c_t b2(std::begin(v2));
		const c_t e2(std::end(v2));
		auto s2 = make_seq<cc_t>(b2,e2,ms);
		const bool is = s1.eq(s2);
		ASSERT(is);
		ASSERT(!s1);
		ASSERT(!s2);
	}
	{ //ne
		auto s1=make_seq<cc_t>(b,e,ms);
		int v2[] = { 1,2,3,4,5,6 };
		const c_t b2(std::begin(v2));
		const c_t e2(std::end(v2));
		auto s2 = make_seq<cc_t>(b2,e2,ms);
		const bool is = s1.ne(s2);
		ASSERT(!s1 && !s2);
		ASSERT(!is);
	}
	{ //cmp
		auto s1=make_seq<cc_t>(b,e,ms);
		int v2[] = { 1,2,3,4,5,6 };
		const c_t b2(std::begin(v2));
		const c_t e2(std::end(v2));
		auto s2=make_seq<cc_t>(b2,e2,ms);
		const auto rseq= s1.cmp(s2, [](const int& i1, const int& i2)constexpr noexcept  { return i1 == i2; });
		ASSERT(!rseq);
	}
	{ //filter
		auto s1 = make_seq<cc_t>(b, e,ms);
		auto s2 = s1.filter([](const int& i)constexpr noexcept { return !(i % 2); });
		ASSERT(!s1);
		while (s2) {
			const auto i = s2.next().unwrap();
			ASSERT(!(i % 2));
		}
		ASSERT(!s2);
	}
	{ //for_each 
		auto s1=make_seq<cc_t>(b,e,ms);
		s1.for_each([](const int& i)constexpr noexcept { ASSERT(i > 0); return true; });
		ASSERT(!s1);
		s1=make_seq<cc_t>(b,e,ms);
		int sum{};
		s1.for_each([&sum](const int& i)constexpr noexcept { sum += i; return true; });
		ASSERT(!s1);
		ASSERT_EQ(sum, 21);
		s1=make_seq<cc_t>(b,e,ms);
		sum = {};
		s1.for_each([&sum](const int& i)constexpr noexcept { sum += i; return i != 3; });
		ASSERT(s1);
		ASSERT_EQ(sum,6);
	}
	{ //for_each_condition
		auto s1=make_seq<cc_t>(b,e,ms);
		const auto l1 = [](const int& i1, const int& i2)constexpr noexcept { return i1 < i2; };
		auto opt = s1.for_each_condition(l1); //search the max
		ASSERT(!s1);
		auto ref = opt.unwrap();
		ASSERT_EQ(ref, 6);
		s1=make_seq<cc_t>(b,e,ms);
		ref = opt.unwrap();
		ASSERT_EQ(ref, 6);
		const auto l = [](const int& i1, const int& i2)constexpr noexcept { return i1 > i2; };
		auto opt1 = s1.for_each_condition(l); //search the min
		opt = std::move(opt1);
		ASSERT(!s1);
		ASSERT_EQ(opt.unwrap(), 1);
	}
	{ //find 
		auto s1=make_seq<cc_t>(b,e,ms);
		s1.find([](const int& i)constexpr noexcept { return i == 1; });
		ASSERT(s1);
		ASSERT_EQ(*s1, 1);
		s1.find(2);
		ASSERT(s1);
		ASSERT_EQ(*s1, 2);
		s1.find([](const int& i)constexpr noexcept { return i == 99; });
		ASSERT(!s1);
	}
	{ //position - position start from 0
		auto s1=make_seq<cc_t>(b,e,ms);
		auto opt = s1.position([](const int& i)constexpr noexcept { return i == 2; });
		ASSERT_EQ(opt.unwrap(), 1);
		ASSERT_EQ(*s1, 2);
		opt = s1.position([](const int& i)constexpr noexcept { return i == 99; });
		ASSERT(!opt);
		ASSERT(!s1);
	}
	{ //last 
		auto s1=make_seq<cc_t>(b,e,ms);
		auto opt = s1.last();
		ASSERT_EQ(opt.unwrap(), 6);
		ASSERT(!s1);
	}
	{ //step_by
		auto s1 = make_seq<cc_t>(b, e,ms);
		auto s2 = s1.step_by(1);
		ASSERT(!s1);
		std::size_t c{};
		while (s2++) { ++c; }
		ASSERT(!s2);
		ASSERT_EQ(c, 6);
		s1 = make_seq<cc_t>(b, e,ms);
		s2 = s1.step_by(2);
		ASSERT(!s1);
		ASSERT_EQ(*s2++, 1);
		ASSERT_EQ(*s2++, 3);
		ASSERT_EQ(*s2++, 5);
		ASSERT(!s2);

	} 
	{//revert 
		using cr_t = std::reverse_iterator<cc_t>;
		static_assert(std::forward_iterator<cr_t>);
		static_assert(std::bidirectional_iterator<cr_t>);
		static_assert(std::random_access_iterator<cr_t>);
		static_assert(!std::contiguous_iterator<cr_t>);
		const auto rb = cr_t(e);
		const auto re = cr_t(b);
		auto s1 = make_seq<cr_t>(rb, re,ms);
		const auto opt = s1.last();
		ASSERT_EQ(opt.unwrap(), 1);
		ASSERT(!s1);
		s1 = make_seq<cr_t>(rb, re,ms);
		const auto optpos = s1.position([](const int& i)constexpr noexcept { return i == 2; });
		ASSERT_EQ(optpos.unwrap(), 4); //position start by 0
		ASSERT_EQ(*s1, 2);
	}
	{ //reverse
		auto sr = make_seq<cc_t>(b, e,ms).reverse();
		ASSERT_EQ(*sr++, 6);
		ASSERT_EQ(*sr++, 5);
		ASSERT_EQ(*sr++, 4);
		ASSERT_EQ(*sr++, 3);
		ASSERT_EQ(*sr++, 2);
		ASSERT_EQ(*sr++, 1);
		ASSERT(!sr);
	}
	{ //transform
		auto s1=make_seq<c_t>(b,e,ms);
		const auto s2 = s1.clone()
						.transform([](int& i)constexpr noexcept->iron::result<bool, iron::error_handler> { i *= 2; return iron::ok(true); })
						.clone();
		ASSERT(!s2);
		ASSERT(s1);
		s1.for_each([](const int& i)constexpr noexcept { ASSERT(!(i % 2)); return true; });
		ASSERT(!s1);
	}

#if 1
	{ //unicode iter
		constexpr char8_t s1[] = {
										0x41 //A
										,0xE2,0x82,0xAC //euro
										,0xFE,0xFD //invalid sequence
										,0x42 //B
										//,0x00 
		};
		f_unicode_iter(s1);
		constexpr char16_t s2[] = {
			0x41 //A
			,0x20AC //euro
			,0xDC00 //invalid sequence
			,0x42 //B
		};
		f_unicode_iter(s2);
	}
#endif 
	return true;
}

int main() {
	f();
	[[maybe_unused]] constexpr bool b = f();
}
