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
#include "iron/chr.h"
#endif 

constexpr bool f() {
	using namespace iron;
	if (!std::is_constant_evaluated())
	{
		const auto c = chr::from(U'a').unwrap().value();
		ASSERT(c.is_alphabetic());
		ASSERT(c.is_alphanumeric());
		ASSERT(!c.is_numeric());
		ASSERT(c.is_ascii());
	}

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable:4566)
#endif
	if (!std::is_constant_evaluated())
	{
		ASSERT(chr::unchecked_from(U'٣').is_numeric());
		ASSERT(chr::unchecked_from(U'7').is_numeric());
		ASSERT(chr::unchecked_from(U'৬').is_numeric());
		ASSERT(chr::unchecked_from(U'¾').is_numeric());
		ASSERT(chr::unchecked_from(U'①').is_numeric());
		ASSERT(!chr::unchecked_from(U'K').is_numeric());
		ASSERT(!chr::unchecked_from(U'و').is_numeric());
		ASSERT(!chr::unchecked_from(U'藏').is_numeric());
		ASSERT(!chr::unchecked_from(U'三').is_numeric());
	}
#if defined(_MSC_VER)
#pragma warning(pop)
#endif

	{
		constexpr auto c = chr::unchecked_from(U'0');
		const auto d = c.to_digit(10).unwrap().value();
		ASSERT(!d);
	}
	{
		const auto c = iron::chr::from(U'a').unwrap().value();
		ASSERT(c.to_digit(10).is_none());
		ASSERT_EQ(c.to_digit(16).unwrap(),10);
	}
	{
		const auto c = iron::chr::from(U'A').unwrap().value();
		ASSERT(c.to_digit(10).is_none());
		ASSERT_EQ(c.to_digit(16).unwrap(),10);
	}
	{
		const auto c = iron::chr::from(U' ').unwrap().value();
		ASSERT(c.to_digit(10).is_none());
		ASSERT(c.to_digit(16).is_none());
	}
	{
		const auto c = iron::chr::from(U'F').unwrap().value();
		ASSERT(c.to_digit(10).is_none());
		ASSERT_EQ(c.to_digit(16).unwrap(),15);
		ASSERT(c.to_digit(15).is_none());
	}
	{
		const auto c = iron::chr::from_u32(0x100).unwrap().value();
		ASSERT(c.to_digit(10).is_none());
		ASSERT(c.to_digit(16).is_none());
		ASSERT(c.to_digit(15).is_none());
	}

	{
		char8_t buff[5]{};
		const auto c = iron::chr::from_u32(0x100).unwrap().value();
		const auto sz = c.encode_utf8(buff);
		ASSERT(sz == 2);
		ASSERT(c.size_utf8() == 2);
	}
	{
		char16_t buff[3]{};
		const auto c = iron::chr::from_u32(0x100).unwrap().value();
		const auto sz = c.encode_utf16(buff);
		ASSERT(sz == 1);
		ASSERT(c.size_utf16() == 1);
	}

	{
		constexpr auto opt = chr::from_digit(10);
		ASSERT(opt);
		ASSERT_EQ(opt.unwrap()->to_utf32(), U'A');
		const auto c1 = chr::from_digit(4, 10).unwrap().value();
		ASSERT_EQ(chr::from(U'4').unwrap(), c1);
		const auto c2 = chr::from_digit(11, 16).unwrap().value();
		ASSERT_EQ(chr::from(U'b').unwrap(), c2);
		const auto c3 = chr::from_digit(20, 10);
		ASSERT(!c3);
	}

	{
		constexpr auto c1 = chr::unchecked_from(U'^');
		constexpr auto c2 = c1.to_ascii_uppercase();
		ASSERT_EQ(c1, c2);
		constexpr auto c3 = c1.to_ascii_lowercase();
		ASSERT_EQ(c1, c3);
	}
	{
		constexpr auto c1 = chr::unchecked_from(U'b');
		constexpr auto c2 = c1.to_ascii_uppercase();
		ASSERT_EQ(chr::unchecked_from(U'B'), c2);
		ASSERT(c1.eq_ignore_ascii_case(c2));
	}
	{
		constexpr auto c1 = chr::unchecked_from(U'B');
		constexpr auto c2 = c1.to_ascii_lowercase();
		ASSERT_EQ(chr::unchecked_from(U'b'), c2);
		ASSERT(c1.eq_ignore_ascii_case(c2));
	}
	{
		constexpr auto sz = chr::unchecked_from(U'B').size_utf8();
		ASSERT_EQ(sz, 1);
		constexpr auto sz1 = chr::unchecked_from(U'ñ').size_utf8();
		ASSERT_EQ(sz1, 2);

	}

	if (!std::is_constant_evaluated())
	{
		constexpr auto c = chr::unchecked_from(U'B');
		auto iter = c.to_lowercase();
		ASSERT_EQ(iter.next().unwrap(),chr::unchecked_from(U'b'));
		ASSERT(!iter.next());
	}

	if (!std::is_constant_evaluated())
	{
		constexpr auto c1 = chr::unchecked_from(U'b');
		auto iter = c1.to_uppercase();
		ASSERT_EQ(iter.next().unwrap(), chr::unchecked_from(U'B'));
		ASSERT(iter.next().is_none());
	}
	if (!std::is_constant_evaluated())
	{
		constexpr auto c1 = chr::unchecked_from(U'b');
		auto iter = c1.escape_unicode();
		ASSERT_EQ(iter.next().unwrap(), chr::unchecked_from(U'0'));
		ASSERT_EQ(iter.next().unwrap(), chr::unchecked_from(U'X'));
		ASSERT_EQ(iter.next().unwrap(), chr::unchecked_from(U'6'));
		ASSERT_EQ(iter.next().unwrap(), chr::unchecked_from(U'2'));
		ASSERT(iter.next().is_none());
	}
	if (!std::is_constant_evaluated())
	{
		constexpr auto c1 = chr::unchecked_from(U'\t');
		auto iter = c1.escape_debug();
		ASSERT_EQ(iter.next().unwrap(), chr::unchecked_from(U'\\'));
		ASSERT_EQ(iter.next().unwrap(), chr::unchecked_from(U't'));
		ASSERT(iter.next().is_none());
		constexpr auto c2 = chr::unchecked_from_u32(0x347);
		iter = c2.escape_debug();
		ASSERT_EQ(iter.next().unwrap(), chr::unchecked_from(U'0'));
		ASSERT_EQ(iter.next().unwrap(), chr::unchecked_from(U'X'));
		ASSERT_EQ(iter.next().unwrap(), chr::unchecked_from(U'3'));
		ASSERT_EQ(iter.next().unwrap(), chr::unchecked_from(U'4'));
		ASSERT_EQ(iter.next().unwrap(), chr::unchecked_from(U'7'));
		ASSERT(!iter.next());

	}
	return true;
 }

 int main() {
	 f();
 }
