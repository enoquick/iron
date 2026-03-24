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
#include "iron/str.h"
#include "iron/collector.h"
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

template <typename T>
constexpr auto  to_adapter(const std::vector<T>& v) {
	iron::unsafe::raw_vector<T,iron::unsafe::allocator<T>> rv;
	auto h = rv.reserve(v.size());
	ASSERT(h);
	auto b = v.begin();
	constexpr bool transactional = false;
	auto r=rv.template range_push_back<transactional>(b, v.end());
	ASSERT(r);
	return iron::adapter_box<decltype(rv)>::unchecked_from(std::move(rv));

}

template <typename T, bool SAFE>
constexpr bool f_str() {
	using v_t = T;
	using s_t = std::conditional_t<
		SAFE
		,iron::str<v_t, iron::unsafe::allocator<v_t>,iron::str_default_fixedsize>
		,iron::ustr<v_t, iron::unsafe::allocator<v_t>,iron::str_default_fixedsize>
	>;
	using namespace iron::literals;
	constexpr auto nes = char32_t{ 0xF1 };  //LATIN SMALL LETTER N WITH TILDE
	[[maybe_unused]] constexpr auto cnes = char32_t{ 0xD1 }; //LATIN CAPITAL LETTER N WITH TILDE;
	constexpr auto ced = char32_t{ 0xC7 }; //LATIN CAPITAL LETTER C WITH CEDILLA
	[[maybe_unused]] constexpr auto is_utf8 = ::is_utf8<v_t>();
	[[maybe_unused]] constexpr auto is_utf16 = ::is_utf16<v_t>();
	[[maybe_unused]] constexpr auto is_utf32 = ::is_utf32<v_t>();
	constexpr auto is_ascii = ::is_ascii<v_t>();
	constexpr auto ascii_push_back = !is_ascii || !SAFE;
	[[maybe_unused]] constexpr auto is_fixed = is_encoding_fixed<v_t>();
	constexpr auto is_wchar32 = ::is_wchar32<v_t>();
	{
		const s_t s_empty;
		ASSERT(s_empty.empty());
		ASSERT(!s_empty.view().count());
		const v_t u[] = { 0x31,0x32,0x33 };
		auto _b = std::begin(u);
		auto s = s_t::unchecked_from(_b, std::end(u));
		ASSERT(s.size() == 3);
		const auto len = s.view().count();
		ASSERT(len == s.size());
		const auto c = s.capacity();
		s.reserve_exact(1);
		ASSERT(s.capacity() == c); //not realloc
		_b = std::begin(u);
		const auto s1 = s_t::unchecked_from(_b, std::end(u), 30);
		ASSERT(s1.size() == 30 * 3);
		ASSERT(s1.view().count() == s1.size());
		auto s2 = s1.clone(2);
		ASSERT(s2.size() == s1.size() * 2);
		ASSERT(s2.view().count() == s2.size());
		s2 = s_t::from(U'a', 10);
		ASSERT(s2.size() == 10);
		ASSERT(s2.view().count() == s2.size());
		using v_t = typename s_t::value_t;
		const std::vector<v_t> sv{0x31,0x32,0x33,0x00};
		auto adapter = to_adapter(sv);
		const auto s3 = s_t::from(std::move(adapter));
		ASSERT(s3.size() == 4);
	}
	if constexpr (!s_t::is_safe()) {
		auto s4 = s_t::from(U"abc");
		ASSERT(s4.size() == 3);
		if constexpr (ascii_push_back) {
			s4.push_back(nes); //not valid ascii char - truncate in char
		}
		s4 = s4.clone(2);
		if constexpr (is_ascii) {
			ASSERT_EQ(s4.view().count(),8);
		}
		else {
			ASSERT_EQ(s4.view().count(),8);
		}
		const auto sz = s4.size();
		{
			auto seq = s4.raw_seq();
			seq.peek().unwrap().value() = v_t{ 'c' };
			ASSERT_EQ(seq.peek().unwrap(),'c');
			const auto wus = seq.next().unwrap();
			ASSERT_EQ(wus,'c');
		}
		auto box = s4.release();
		ASSERT(box.size() == sz);
	}
	if constexpr (!is_ascii)
	{
		constexpr std::size_t n = 100;
		auto s6 = s_t::from(ced, n);
		ASSERT_EQ(s6.view().count(), n);
		const auto cp = s6.capacity();
		s6.clear();
		ASSERT(s6.empty());
		ASSERT(!s6.view().count());
		ASSERT(s6.capacity() == cp);
	}

	if constexpr (ascii_push_back) {
		constexpr std::size_t n = 100;
		auto s6 = s_t::from(ced, n);
		const auto cp = s6.capacity();
		const auto sz = s6.size();
		auto box = s6.release();
		ASSERT(box.capacity() == cp || box.capacity() == cp + 1);
		ASSERT(box.size() == sz);
	}
	if constexpr (ascii_push_back) {
		auto s6 = s_t::from(ced);
		if (is_ascii) {
			ASSERT_EQ(s6.view().count(), 1);
		}
		else {
			ASSERT_EQ(s6.view().count(), 1);
		}
		const v_t u[] = { 0x31,0x32,0x33 };
		auto b = std::begin(u);
		s6.unchecked_push_back(b, std::end(u), 2);
		if (is_ascii) {
			ASSERT_EQ(s6.view().count(), 7);
		}
		else {
			ASSERT_EQ(s6.view().count(), 7);
		}
	}

	if constexpr (!is_ascii) {
		auto s6 = s_t::from(ced);
		ASSERT_EQ(s6.view().count(), 1);
		s6.push_back(nes, 50);
		ASSERT(s6.view().count() == 51);
		constexpr char32_t _c = U'x';
		s6.push_back(_c);
	}
	if constexpr (is_utf16 || is_utf32) {
		s_t s7;
		constexpr char32_t c = 0x1F9CA;
		s7.push_back(c);
		auto seq = s7.seq();
		{
			const auto c1 = *seq++;
			ASSERT_EQ(c, c1);
			ASSERT(!seq);
		}
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable:26481)
#endif
		{
			s7.clear();
			char8_t buff[5]{};
			size_t sz = sizeof(buff);
			constexpr char32_t ch32 = 0x1F9CA;
			auto p32 = &ch32;
			const auto ret=iron::unsafe::unicode_conversions::copy<s_t::is_safe()>(&buff[0], sz, p32, p32 + 1);
			ASSERT(ret);
			auto p = &buff[0];
			s7.unchecked_push_back(p, p + sz);
			seq = s7.seq();
			ASSERT_EQ(*seq++, ch32);
			ASSERT(!seq);
		}
#if defined(_MSC_VER)
#pragma warning(pop)
#endif
	}
	if constexpr (ascii_push_back) {
		const char32_t v32[]{
			U'a' //utf8 1 byte
			,nes //utf8 2 bytes
			,char32_t{ 0xFBD3 } //ARABIC LETTER NG ISOLATED FORM - utf8 3 bytes
			,char32_t{ 0x1F9CA } //ice cube - utf8 4 bytes
			,char32_t{ 0x104D8 } //OSAGE SMALL LETTER - utf8 4 bytes
		};
		s_t s7;
		for (auto& c32 : v32) {
			s7.push_back(c32);
		}
		if constexpr (is_ascii) {
			ASSERT_EQ(s7.view().count(),5);
			ASSERT_EQ(s7.front().unwrap(), 'a');
		}
		else {
			ASSERT_EQ(s7.view().count(),5);
			ASSERT_EQ(s7.front().unwrap(), 'a');
			ASSERT_EQ(s7.nth(0).unwrap(), 'a');
			ASSERT_EQ(s7.nth(1).unwrap(),nes);
			ASSERT_EQ(s7.nth(2).unwrap(), char32_t{ 0xFBD3 });
			ASSERT_EQ(s7.nth(3).unwrap(), char32_t{ 0x1F9CA });
			ASSERT_EQ(s7.nth(4).unwrap(), char32_t{ 0x104D8 });
			ASSERT_EQ(s7.back().unwrap(), char32_t{0x104D8});
			ASSERT(!s7.nth(5));
		}
		auto seq = s7.seq();
		auto b = seq.begin();
		const auto e = seq.end();
		ASSERT(b != e);
		auto c32 = *b;
		ASSERT_EQ(c32, U'a');
		c32 = *b;
		ASSERT_EQ(c32, U'a');
		++b; //step 1 byte
		if constexpr (!is_ascii) {
			ASSERT(b != e);
			c32 = *b;
			ASSERT_EQ(c32, nes);
			c32 = *b++; //step 2 bytes
			ASSERT_EQ(c32, nes);
			ASSERT(b != e);
			c32 = *b++; //step 3bytes
			ASSERT_EQ(c32, char32_t{ 0xFBD3 });
			ASSERT(b != e);
			c32 = *b++; //step 4bytes
			ASSERT_EQ(c32, char32_t{ 0x1F9CA });
			c32 = *b++; // step 4bytes
			ASSERT_EQ(c32, char32_t{ 0x104D8 });
			ASSERT_EQ(b,e);
		}
		seq = s7.seq();
		c32 = seq.next().unwrap();
		ASSERT_EQ(c32, U'a');
		if constexpr (!is_ascii) {
			c32 = seq.next().unwrap();
			ASSERT_EQ(c32, nes);
			c32 = seq.next().unwrap();
			ASSERT_EQ(c32, char32_t{ 0xFBD3 });
			c32 = seq.next().unwrap();
			ASSERT_EQ(c32, char32_t{ 0x1F9CA });
			c32 = seq.next().unwrap();
			ASSERT_EQ(c32, char32_t{ 0x104D8 });
		}
	}

	if constexpr (ascii_push_back) {
		constexpr char32_t t1[] = { U'a',U'b',nes,U'c' };
		auto _b = std::begin(t1);
		auto s1 = s_t::unchecked_from(_b, std::end(t1));
		if constexpr (is_ascii) {
			ASSERT_EQ(s1.view().count(), 4);
		}
		else {
			ASSERT_EQ(s1.view().count(), 4);
		}
		s1.resize(3);
		ASSERT_EQ(s1.view().count(), 3);
		constexpr char32_t t2[] = { nes,U'b' };
		_b = std::begin(t2);
		s1 = s_t::unchecked_from(_b, std::end(t2));
		if constexpr (is_ascii) {
			ASSERT_EQ(s1.view().count(), 2);
		}
		else {
			ASSERT_EQ(s1.view().count(), 2);
		}
		s1.resize(1);
		ASSERT_EQ(s1.view().count(), 1);
		s1.resize(0);
		ASSERT(s1.empty());
		if constexpr (is_ascii) {
			s1.resize(10, 'Z');
		}
		else if constexpr (is_wchar32) {
			s1.resize(10, static_cast<wchar_t>(U'Z'));
		}
		else {
			s1.resize(10, U'Z');
		}
		ASSERT_EQ(s1.size(), 10);
		ASSERT_EQ(s1.view().count(), 10);
		ASSERT_EQ(s1.cseq().next().unwrap(), U'Z');
		ASSERT_EQ(s1.cseq().skip(9).next().unwrap(), U'Z');

	}
	if constexpr(!is_ascii) {
		constexpr char32_t t1[] = { U'a',U'b',nes,U'c' };
		auto b = std::begin(t1);
		auto s = s_t::unchecked_from(b, std::end(t1));
		const auto v = s.unchecked_raw_nth(0);
		ASSERT_EQ(v, U'a');
		auto opt = s.raw_nth(0);
		ASSERT_EQ(opt.unwrap(), U'a');
		opt = s.raw_front();
		ASSERT_EQ(opt.unwrap(), U'a');
		opt = s.raw_back();
		ASSERT_EQ(opt.unwrap(), U'c');
		opt = s.raw_nth(10);
		ASSERT(!opt);
		auto opt_char = s.front();
		ASSERT_EQ(opt_char.unwrap(), U'a');
		opt_char = s.nth(2);
		ASSERT_EQ(opt_char.unwrap(), nes);
		opt_char = s.nth(3);
		ASSERT_EQ(opt_char.unwrap(), U'c');
		opt_char = s.nth(4);
		ASSERT(!opt_char);
	}
	if constexpr (is_ascii)
	{ //collector
		constexpr char t1[] = { 'a','b','c' };
		auto b = std::begin(t1);
		auto s = s_t::unchecked_from(b, std::end(t1));
		auto box = s.clone().release();
		s = iron::collector::make_str<s_t>(std::move(box), s.seq(),'x',"yz");
		ASSERT_EQ(s, "abcabcxyz");
	}
	else { //collector
		constexpr char32_t t1[] = { U'a',U'b',nes,U'c' };
		auto b = std::begin(t1);
		auto s = s_t::unchecked_from(b, std::end(t1));
		auto box = s.clone().release();
		auto box1 = box.clone();
		s = iron::collector::make_str<s_t>(std::move(box),std::move(box1),s.seq(),'x',"yz");
		ASSERT_EQ(s, iron::collector::make_str8(U"ab", nes, U"cab", nes, U"cab", nes, U"cxyz"));
		ASSERT_EQ(s, iron::collector::make_str16(U"ab", nes, U"cab", nes, U"cab", nes, U"cxyz"));
		ASSERT_EQ(s, iron::collector::make_strw(U"ab", nes, U"cab", nes, U"cab", nes, U"cxyz"));
		ASSERT_EQ(s, iron::collector::make_str32(U"ab", nes, U"cab", nes, U"cab", nes, U"cxyz"));
		ASSERT_EQ(s, iron::collector::make_ustr8(U"ab", nes, U"cab", nes, U"cab", nes, U"cxyz"));
		ASSERT_EQ(s, iron::collector::make_ustr16(U"ab", nes, U"cab", nes, U"cab", nes, U"cxyz"));
		ASSERT_EQ(s, iron::collector::make_ustrw(U"ab", nes, U"cab", nes, U"cab", nes, U"cxyz"));
		ASSERT_EQ(s, iron::collector::make_ustr32(U"ab", nes, U"cab", nes, U"cab", nes, U"cxyz"));
		s.reserve_at_least(100);
		s.shrink_to_fit();
	}
	if (!std::is_constant_evaluated()) {
		const auto r = s_t::try_from(char32_t{ 0xFFFFFF }); //invalid
		if constexpr (s_t::is_safe()) {
			ASSERT(!r);
		}
		else {
			ASSERT(r);
			if constexpr (!is_ascii) {
				auto& s = r.unchecked_unwrap();
				const auto r1 = s.to_upper();
			}
		}
	}
	if constexpr (is_ascii) {
		auto s0 = "abc1"_sb.to_upper();
		ASSERT_EQ(s0, "ABC1");
		s0=s0.to_lower();
		ASSERT_EQ(s0, "abc1");
		s0 = s0.clone(100).to_upper();
		ASSERT_EQ(s0, "ABC1"_sb.clone(100));
		s0 = s0.to_lower();
		ASSERT_EQ(s0, "abc1"_sb.clone(100));
	}
	else {
		if (!std::is_constant_evaluated()) {
//			auto s = s_t::from(U"añb1");
			auto s = s_t::from(U"a\u00f1b1");
			s = s.to_upper();
			ASSERT_EQ(s, U"A\u00d1B1");

//			ASSERT_EQ(s, U"AÑB1");
			s = s.to_lower();
			s = s_t::from(U"a\u00f1b1");

//			ASSERT_EQ(s, U"añb1");
		}
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


#if 1
	[[maybe_unused]] constexpr auto _b0=f_str<char, false>();
	[[maybe_unused]] constexpr auto _b1=f_str<char, true>();
	[[maybe_unused]] constexpr auto _b2=f_str<char8_t, false>();
	[[maybe_unused]] constexpr auto _b3=f_str<char8_t, true>();
	[[maybe_unused]] constexpr auto _b4=f_str<char16_t, false>();
	[[maybe_unused]] constexpr auto _b5=f_str<char16_t, true>();
	[[maybe_unused]] constexpr auto _b6=f_str<wchar_t, false>();
	[[maybe_unused]] constexpr auto _b7=f_str<wchar_t, true>();
	[[maybe_unused]] constexpr auto _b8=f_str<char32_t, false>();
	[[maybe_unused]] constexpr auto _b9=f_str<char32_t, true>();
#endif
}
