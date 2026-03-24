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
#include "iron/str.h"
#include "iron/collector.h"
#endif 

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable:26472)
#endif 

template <typename T>
inline constexpr bool is_ascii() { return iron::concepts::basic_charset<T>; }

template <typename T>
inline constexpr bool is_utf8() { return iron::concepts::utf8_charset<T>; }

template <typename T>
inline constexpr bool is_utf16() { return iron::concepts::utf16_charset<T>; }

template <typename T>
inline constexpr bool is_utf32() { return iron::concepts::utf32_charset<T>; }

template <typename T>
inline constexpr bool is_wchar32() { return sizeof(T) == 4 && std::is_same_v<T, wchar_t>; }

template <typename T>
inline constexpr bool is_encoding_varlen() { return iron::concepts::varying_charset<T>; }

template <typename T>
inline constexpr bool is_encoding_fixed() { return iron::concepts::fixed_charset<T>; }

template <typename VIEW,typename T>
static constexpr void compare_view(VIEW w,std::initializer_list<T> list) {
	const auto wcount = w.count();
	const std::vector<T> v(list);
	const auto vcount = v.size();
	ASSERT_EQ(wcount, vcount);
	for (std::size_t i = 0; i < wcount; ++i) {
		const auto wvalue = static_cast<char32_t>(w.nth(i).unwrap().value());
		const auto vvalue = static_cast<char32_t>(v.at(i));
		ASSERT_EQ(wvalue, vvalue);
	}
	ASSERT(!w.nth(wcount));
}

template <typename T, bool SAFE>
static constexpr bool f_str() {
	using v_t = T;
	using s_t = std::conditional_t<
		SAFE
		, iron::str<v_t, iron::unsafe::allocator<v_t>, iron::str_default_fixedsize>
		, iron::ustr<v_t, iron::unsafe::allocator<v_t>, iron::str_default_fixedsize>
	>;
	using namespace iron::literals;
	[[maybe_unused]] constexpr auto nes = char32_t{ 0xF1 };  //LATIN SMALL LETTER N WITH TILDE  
	[[maybe_unused]] constexpr auto cnes = char32_t{ 0xD1 }; //LATIN CAPITAL LETTER N WITH TILDE;
	[[maybe_unused]] constexpr auto ced = char32_t{ 0xC7 }; //LATIN CAPITAL LETTER C WITH CEDILLA

	[[maybe_unused]] constexpr auto sigma_upper = char32_t{ 0x03A3 };
	[[maybe_unused]] constexpr auto sigma_lower = char32_t{ 0x03C3 };
	[[maybe_unused]] constexpr auto sigma_final = char32_t{ 0x03C2 };
	constexpr auto icecube = char32_t{ 0x0001F9CA };
	constexpr auto is_utf8 = ::is_utf8<v_t>();
	constexpr auto is_utf16 = ::is_utf16<v_t>();
	[[maybe_unused]] constexpr auto is_utf32 = ::is_utf32<v_t>();
	constexpr auto is_ascii = ::is_ascii<v_t>();
//	[[maybe_unused]] constexpr auto ascii_push_back = !is_ascii || !SAFE;
//	[[maybe_unused]] constexpr auto is_fixed = is_encoding_fixed<v_t>();
//	[[maybe_unused]] constexpr auto is_wchar32 = ::is_wchar32<v_t>();
	if constexpr (is_ascii) {
		auto s4 = s_t::from("ab!");
		auto v4 = s4.view();
		{
			ASSERT(v4.is_chr_boundary(0));
			ASSERT(v4.is_chr_boundary(1));
			ASSERT(v4.is_chr_boundary(2));
			ASSERT(v4.is_chr_boundary(3));
			ASSERT(!v4.is_chr_boundary(4));
		}
		{
			v4.make_base_uppercase();
			compare_view(v4, { 'A','B','!' });
		}
		{
			v4.make_base_lowercase();
			compare_view(v4, { 'a','b','!' });
		}
		auto v5 = v4.slice(1, 1);
		compare_view(v5, { 'b' });
		{
			v5 = v4.ltrim_if([](char v)noexcept->bool { return v == 'a'; });
			compare_view(v5, { 'b','!' });
		}
		{
			auto ss = s_t::from("\t ab!");
			v4 = ss.view();
			v5 = v4.ltrim();
			compare_view(v5, { 'a','b','!' });
		}
		{
			auto ss = s_t::from("ab!");
			v4 = ss.view();
			v5 = v4.rtrim_if([](char v)noexcept->bool { return v == '!'; });
			compare_view(v5, { 'a','b'});
		}
		{
			auto ss = s_t::from("ab\t!\t ");
			v4 = ss.view();
			v5 = v4.rtrim();
			compare_view(v5, { 'a','b','\t','!'});
		}
		{
			auto s0 = s_t::from("ab!b");
			auto v0 = s0.view();
			auto v1 = v0.find('a');			
			v1.make_base_uppercase();
			compare_view(v1, { 'A','B','!','B'});
			ASSERT_EQ(v0.in(v1).unwrap(),0);
			ASSERT(v1.starts_with('A'));
			ASSERT(!v1.starts_with('B'));
			ASSERT(v1.contains('A'));
			ASSERT(v1.contains('!'));
			ASSERT(!v1.contains('a'));
			ASSERT(v1.ends_with('B'));
			ASSERT(!v1.ends_with('b'));
		}
		{
			auto s0 = s_t::from("abcdef");
			auto v0 = s0.view();
			auto vc = v0.slice(2,2); //cd
			auto v1 = v0.find(vc);
			compare_view(v1, { 'c','d','e','f'});
			ASSERT_EQ(v0.in(v1).unwrap(), 2);
			ASSERT(v1.starts_with(vc));
			ASSERT(v1.contains(vc));
			ASSERT(!v1.ends_with(vc));
			vc.reset();
			v1 = v0.find(vc);
			ASSERT_EQ(v1,v0);
			ASSERT_EQ(v0.in(v1).unwrap(),0);
			ASSERT(v1.starts_with(vc));
			ASSERT(v1.contains(vc));
			ASSERT(v1.ends_with(vc));
			auto s1 = s_t::from("abcdefg");
			vc = s1.view();
			v1 = v0.find(vc);
			ASSERT(!v1);
			ASSERT(!(v0.in(v1)));
			s1 = s_t::from("abcdEf");
			vc = s1.view();
			v1 = v0.find(vc);
			ASSERT(!v1);
			v0.reset();
			vc.reset();
			v1 = v0.find(vc);
			ASSERT(!v1);
			ASSERT(!(v0.in(v1)));
		}
		{
			auto s0 = s_t::from("abcdefabcdef");
			auto s1 = s_t::from("ef");
			auto v0 = s0.view();
			auto v1 = s1.view();
			ASSERT(!v0.starts_with(v1));
			ASSERT(v0.contains(v1));
			ASSERT(v0.ends_with(v1));
		}

	}
	else {
		auto s4 = iron::collector::make_str<s_t>(U'a', icecube, U"b!");
		auto v4 = s4.view();
		ASSERT(v4.is_chr_boundary(0));
		if constexpr (is_utf8) {
			ASSERT(v4.is_chr_boundary(1));
			ASSERT(!v4.is_chr_boundary(2));
			ASSERT(!v4.is_chr_boundary(3));
			ASSERT(!v4.is_chr_boundary(4));
			ASSERT(v4.is_chr_boundary(5));
			ASSERT(v4.is_chr_boundary(6));
			ASSERT(v4.is_chr_boundary(7));
			ASSERT(!v4.is_chr_boundary(8));
			ASSERT(!v4.is_chr_boundary(9));
		}
		else if constexpr (is_utf16) {
			ASSERT(v4.is_chr_boundary(1));
			ASSERT(!v4.is_chr_boundary(2));
			ASSERT(v4.is_chr_boundary(3));
			ASSERT(v4.is_chr_boundary(4));
			ASSERT(v4.is_chr_boundary(5));
			ASSERT(!v4.is_chr_boundary(6));
			ASSERT(!v4.is_chr_boundary(7));
		}
		else if constexpr (is_utf32) {
			ASSERT(v4.is_chr_boundary(1));
			ASSERT(v4.is_chr_boundary(2));
			ASSERT(v4.is_chr_boundary(3));
			ASSERT(v4.is_chr_boundary(4));
			ASSERT(!v4.is_chr_boundary(5));
		}
		{
			ASSERT_EQ(v4.front().unwrap(), U'a');
			compare_view(v4, { U'a',icecube,U'b',U'!' });
			ASSERT_EQ(v4.back().unwrap(), U'!');
		}
		{
			v4.make_base_uppercase();
			compare_view(v4, { U'A',icecube,U'B',U'!'});
		}
		{
			v4.make_base_lowercase();
			compare_view(v4, { U'a',icecube,U'b',U'!' });
		}
		auto v5 = v4.slice(1, 2);
		compare_view(v5, { icecube,U'b'});

		{
			v5 = v4.ltrim_if([](typename s_t::char_t v)noexcept->bool { return static_cast<char32_t>(v) == U'a'; });
			compare_view(v5, { icecube,U'b',U'!' });
		}
		if (!std::is_constant_evaluated()) //cannot evaluate ltrim in constexpr mode 
		{
			auto ss = s_t::from(U"\t ab!");
			v4 = ss.view();
			v5 = v4.ltrim();
			compare_view(v5, { U'a',U'b',U'!'});
			v4 = v5 = {};
			ss = s_t::from(U"\t ");
			v4 = ss.view();
			v5 = v4.ltrim();
			ASSERT(!v5.count());
		}
		{
			s4 = iron::collector::make_str<s_t>(U'a', icecube, U"!b!!");
			v4 = s4.view();
			v5 = v4.rtrim_if([](typename s_t::char_t v)noexcept->bool { return static_cast<char32_t>(v) == U'!'; });
			compare_view(v5, { U'a',icecube,U'!',U'b' });
		}
		if (!std::is_constant_evaluated()) //cannot evaluate rtrim in constexpr mode 
		{
			auto ss = s_t::from(U"\t ab!\t !\t ");
			v4 = ss.view();
			v5 = v4.rtrim();
			compare_view(v5, { U'\t',U' ',U'a',U'b',U'!',U'\t',U' ',U'!'});
			v4 = v5 = {};
			ss = s_t::from(U"\t ");
			v4 = ss.view();
			v5 = v4.rtrim();
			ASSERT(!v5);
		}
		{
			s4 = iron::collector::make_str<s_t>(U'a', icecube, U"b!");
			v4 = s4.view();
			v5 = v4.find(icecube);
			compare_view(v5, { icecube,U'b',U'!' });
		}
		{
			auto s0 = iron::collector::make_str<s_t>(U'a', icecube, U"bcdef");
			auto v0 = s0.view();
			auto vc = v0.slice(2, 2); //bc
			auto v1 = v0.find(vc);
			compare_view(v1, { U'b',U'c',U'd',U'e',U'f' });
			ASSERT_EQ(v0.in(v1).unwrap(), 2);
			vc.reset();
			v1 = v0.find(vc);
			ASSERT_EQ(v1, v0);
			ASSERT_EQ(v0.in(v1).unwrap(), 0);
			auto s1 = s_t::from("abcdefg");
			vc = s1.view();
			v1 = v0.find(vc);
			ASSERT(!v1);
			ASSERT(!(v0.in(v1)));
			s1 = s_t::from("abcdEf");
			vc = s1.view();
			v1 = v0.find(vc);
			ASSERT(!v1);
			v0.reset();
			vc.reset();
			v1 = v0.find(vc);
			ASSERT(!v1);
			ASSERT(!(v0.in(v1)));
		}
		{
			auto s0 = iron::collector::make_str<s_t>(icecube, U"abc",icecube);
			auto v0 = s0.view();
			ASSERT(v0.starts_with(icecube));
			ASSERT(v0.contains(icecube));
			ASSERT(v0.ends_with(icecube));
			s0 = iron::collector::make_str<s_t>(U"abc", icecube);
			v0 = s0.view();
			ASSERT(!v0.starts_with(icecube));
			ASSERT(v0.contains(icecube));
			ASSERT(v0.ends_with(icecube));
		}
		{
			auto s0 = iron::collector::make_str<s_t>(icecube, U"abc", icecube);
			auto v0 = s0.view();
			ASSERT(v0.starts_with(v0));
			ASSERT(v0.contains(v0));
			ASSERT(v0.ends_with(v0));
		}
		{
			auto s0 = iron::collector::make_str<s_t>(icecube, U"abc", icecube);
			auto v0 = s0.view();
			ASSERT(v0.starts_with(v0));
			ASSERT(v0.contains(v0));
			ASSERT(v0.ends_with(v0));
			ASSERT_EQ(v0.raw_index(0).unwrap(), 0); //icecube
			if constexpr (iron::concepts::fixed_charset<v_t>) {
				ASSERT_EQ(v0.raw_index(1).unwrap(), 1); //a
				ASSERT_EQ(v0.raw_index(2).unwrap(), 2);//b
				ASSERT_EQ(v0.raw_index(3).unwrap(), 3);//c
				ASSERT_EQ(v0.raw_index(4).unwrap(), 4);//icecube
				ASSERT(!v0.raw_index(5));
			}
			else if constexpr (is_utf8) {
				ASSERT_EQ(v0.raw_index(1).unwrap(), 4);//a
				ASSERT_EQ(v0.raw_index(2).unwrap(), 5);//b
				ASSERT_EQ(v0.raw_index(3).unwrap(), 6);//c
				ASSERT_EQ(v0.raw_index(4).unwrap(), 7);//icecube
				ASSERT(!v0.raw_index(5));
			}
			else if constexpr (is_utf16) {
				ASSERT_EQ(v0.raw_index(1).unwrap(), 2);
				ASSERT_EQ(v0.raw_index(2).unwrap(), 3);
				ASSERT_EQ(v0.raw_index(3).unwrap(), 4);
				ASSERT_EQ(v0.raw_index(4).unwrap(), 5);
				ASSERT(!v0.raw_index(5));
			}
			auto s1 = iron::collector::make_str<s_t>(icecube, U"abc");
			auto v1 = s1.view();
			ASSERT(v0.starts_with(v1));
			ASSERT(v0.contains(v1));
			ASSERT(!v0.ends_with(v1));
			s1 = iron::collector::make_str<s_t>(U"b",icecube, U"a");
			v1 = s1.view();
			ASSERT(!v0.starts_with(v1));
			ASSERT(!v0.contains(v1));
			ASSERT(!v0.ends_with(v1));
			s1 = iron::collector::make_str<s_t>(U"c", icecube);
			v1 = s1.view();
			ASSERT(!v0.starts_with(v1));
			ASSERT(v0.contains(v1));
			ASSERT(v0.ends_with(v1));
		}
	}
	return true;
}

template <typename S1,typename S2>
constexpr void f_compare_eq(const S1& s1, const S2& s2)noexcept {
	using c_t = std::strong_ordering;
	ASSERT_EQ(s1, s2);
	ASSERT_EQ(s2, s1);
	ASSERT_EQ(s1 <=> s2, c_t::equal);
}

template <typename S1, typename S2>
constexpr void f_compare_lt(const S1& s1, const S2& s2)noexcept {
	using c_t = std::strong_ordering;
	ASSERT(s1 < s2);
	ASSERT_EQ(s1 <=> s2, c_t::less);
	ASSERT(!(s2 < s1));
	ASSERT_EQ(s1 <=> s2, c_t::less);
	const auto r = s2 <=> s1;
	ASSERT(r == c_t::equal || r == c_t::greater);

}

bool constexpr f_compare()noexcept {
	constexpr auto icecube = char32_t{ 0x0001F9CA };
	constexpr auto bubble_tea = char32_t{ 0x0001F9CB };
	{ // eq base 
		const auto s0 = iron::collector::make_strb("abc");
		const auto sb = s0.clone();
		const auto s18 = iron::str8::clone_from(s0);
		const auto s116 = iron::str16::clone_from(s0);
		const auto s1w = iron::strw::clone_from(s0);
		const auto s132 = iron::str32::clone_from(s0);
		f_compare_eq(s0, sb);
		f_compare_eq(s0, s18);
		f_compare_eq(s0, s116);
		f_compare_eq(s0, s1w);
		f_compare_eq(s0, s132);
	}
	{ // less base 
		const auto s0 = iron::collector::make_strb("ABC");
		const auto s1 = iron::collector::make_strb("ABCD");
		const auto s18 = iron::str8::clone_from(s1);
		const auto s116 = iron::str16::clone_from(s1);
		const auto s1w = iron::strw::clone_from(s1);
		const auto s132 = iron::str32::clone_from(s1);
		f_compare_lt(s0,"ABc"); //in ascii
		f_compare_lt(s0, s1);
		f_compare_lt(s0, s18);
		f_compare_lt(s0, s1w);
		f_compare_lt(s0, s132);
	}

	{ //eq not base
		const auto s8 = iron::collector::make_str8(icecube, U"abc", icecube);
		const auto s16 = iron::str16::clone_from(s8);
		const auto sw = iron::strw::clone_from(s8);
		const auto s32 = iron::str32::clone_from(s8);
		f_compare_eq(s8, s8);
		f_compare_eq(s8, s16);
		f_compare_eq(s8, sw);
		f_compare_eq(s8, s32);
		f_compare_eq(s16, sw);
		f_compare_eq(s16, s32);
		f_compare_eq(sw, s32);
		f_compare_eq(s32, s32);
	}
	{ //less not base
		const auto s08 = iron::collector::make_str8(icecube, U"abc", icecube);
		const auto s016 = iron::str16::clone_from(s08);
		const auto s0w = iron::strw::clone_from(s08);
		const auto s032 = iron::str32::clone_from(s08);
		const auto s18 = iron::collector::make_str8(icecube, U"abc", bubble_tea);
		const auto s116 = iron::str16::clone_from(s18);
		const auto s1w = iron::strw::clone_from(s18);
		const auto s132 = iron::str32::clone_from(s18);
		ASSERT(!(s08 < s08));
		ASSERT(!(s016 < s016));
		ASSERT(!(s0w < s0w));
		ASSERT(!(s032 < s032));
		f_compare_lt(s08, s18);
		f_compare_lt(s08, s116);
		f_compare_lt(s08, s1w);
		f_compare_lt(s08, s116);
		f_compare_lt(s016, s1w);
		f_compare_lt(s016, s132);
		f_compare_lt(s0w, s132);
		f_compare_lt(s032, s132);
	}
	return true;
}

int main() {
	f_str<char, false>();
	f_str<char, true>();
	f_str<char8_t, false>();
	f_str<char8_t, true>();
	f_str<char16_t, false>();
	f_str<char16_t, true>();
	f_str<wchar_t, false>();
	f_str<wchar_t, true>();
	f_str<char32_t, false>();
	f_str<char32_t, true>();
	f_compare();

#if 1
	[[maybe_unused]] constexpr auto b0 = f_str<char, false>();
	[[maybe_unused]] constexpr auto b1=f_str<char, true>();
	[[maybe_unused]] constexpr auto b2=f_str<char8_t, false>();
	[[maybe_unused]] constexpr auto b3=f_str<char8_t, true>();
	[[maybe_unused]] constexpr auto b4=f_str<char16_t, false>();
	[[maybe_unused]] constexpr auto b5=f_str<char16_t, true>();
	[[maybe_unused]] constexpr auto b6=f_str<wchar_t, false>();
	[[maybe_unused]] constexpr auto b7=f_str<wchar_t, true>();
	[[maybe_unused]] constexpr auto b8=f_str<char32_t, false>();
	[[maybe_unused]] constexpr auto b9=f_str<char32_t, true>();
	[[maybe_unused]] constexpr auto b10=f_compare();

#endif 
}

#if defined(_MSC_VER)
#pragma warning(pop)
#endif 
