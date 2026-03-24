
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
#include "iron/integrals.h"
#endif 

#ifdef IRON_NO_USE_MODULE
template class iron::base_integral<int>;
template class iron::base_integral<unsigned int>;

template class iron::base_integral_bounded<int>;
template class iron::base_integral_bounded<unsigned int>;
#endif 

constexpr bool f()noexcept {
	using namespace iron;

	{
		i8 m8;
		m8 = 127;
		constexpr i16 m16{ 24444 };
		[[maybe_unused]] constexpr auto m16_1 = m16;
		ASSERT_EQ(i8::min(), -128);
		ASSERT_EQ(i8::max(), 127);
		constexpr auto c = i8::max().count_ones();
		ASSERT_EQ(c, 7U);
		ASSERT_EQ(u8::max().count_ones(), 8U);
		constexpr auto c1 = i8::max().count_zeros();
		ASSERT_EQ(c1, 1U);
		ASSERT_EQ(u8::max().count_zeros(), 0U);
	}
	//
	{
		constexpr auto c = i8(0).leading_zeros();
		ASSERT_EQ(c, 8U);
		ASSERT_EQ(i8(1).leading_zeros(), 7U);
		ASSERT_EQ(i8(2).leading_zeros(), 6U);
		ASSERT_EQ(i8(3).leading_zeros(), 6U);
		ASSERT_EQ(i8(4).leading_zeros(), 5U);
		ASSERT_EQ(i8(-1).leading_zeros(), 0U);
	}
	//
	{
		constexpr auto c = i8(0).trailing_zeros();
		ASSERT_EQ(c, 8U);
		ASSERT_EQ(i8(1).trailing_zeros(), 0U);
		ASSERT_EQ(i8(2).trailing_zeros(), 1U);
		ASSERT_EQ(i8(3).trailing_zeros(), 0U);
		ASSERT_EQ(i8(4).trailing_zeros(), 2U);
		ASSERT_EQ(i8(-1).trailing_zeros(), 0U);
	}
	//
	{
		constexpr auto c = i8(-3).leading_ones();
		ASSERT_EQ(c, 6U);
		ASSERT_EQ(i8(-2).leading_ones(), 7U);
		ASSERT_EQ(i8(-1).leading_ones(), 8U);
		ASSERT_EQ(i8(0).leading_ones(), 0U);
		ASSERT_EQ(i8(1).leading_ones(), 0U);
	}
	//
	{
		constexpr auto c = i8(-3).trailing_ones();
		ASSERT_EQ(c, 1U);
		ASSERT_EQ(i8(-2).trailing_ones(), 0U);
		ASSERT_EQ(i8(-1).trailing_ones(), 8U);
		ASSERT_EQ(i8(0).trailing_ones(), 0U);
		ASSERT_EQ(i8(1).trailing_ones(), 1U);
		ASSERT_EQ(i8(2).trailing_ones(), 0U);
		ASSERT_EQ(i8(3).trailing_ones(), 2U);
		ASSERT_EQ(i8(4).trailing_ones(), 0U);
	}

	{
		constexpr auto c = i8(-3).rotate_left(1);
		ASSERT_EQ(c, -5);
		ASSERT_EQ(i8(-2).rotate_left(1), -3);
		ASSERT_EQ(i8(-1).rotate_left(1), -1);
		ASSERT_EQ(i8(0).rotate_left(1), 0);
		ASSERT_EQ(i8(1).rotate_left(1), 2);
		ASSERT_EQ(i8(2).rotate_left(1), 4);
		ASSERT_EQ(i8(3).rotate_left(1), 6);
		ASSERT_EQ(i8(4).rotate_left(1), 8);
	}
	{
		constexpr auto c = i8(-3).rotate_right(1);
		ASSERT_EQ(c, -2);
		ASSERT_EQ(i8(-2).rotate_right(1), 127);
		ASSERT_EQ(i8(-1).rotate_right(1), -1);
		ASSERT_EQ(i8(0).rotate_right(1), 0);
		ASSERT_EQ(i8(1).rotate_right(1), -128);
		ASSERT_EQ(i8(2).rotate_right(1), 1);
		ASSERT_EQ(i8(3).rotate_right(1), -127);
		ASSERT_EQ(i8(4).rotate_right(1), 2);
	}
	{
		constexpr auto c = i8(1).swap_bytes();
		ASSERT_EQ(c, i8(1));
		ASSERT_EQ(i16(0x1234).swap_bytes(), 0x3412);
		ASSERT_EQ(i32(0x12345678).swap_bytes(), i32(0x78563412));
		ASSERT_EQ(i64(0x1234567890123456).swap_bytes(), i64(0x5634129078563412));
	}

	{

		constexpr auto c = i8(0x12).reverse_bits();
		ASSERT_EQ(c, i8(0x48));
		ASSERT_EQ(i16(0x1234).reverse_bits(), i16(0x2c48));
		ASSERT_EQ(i32(0x12345678).reverse_bits(), i32(0x1e6a2c48));
		ASSERT_EQ(i64(0x1234567890123456).reverse_bits(), i64(0x6a2c48091e6a2c48));
	}

	{
		constexpr auto n = u32(0x1A);
		if constexpr (cfg::endian() == cfg::endian_t::little) {
			constexpr auto c1 = u32::from_be(n);
			constexpr auto c2 = n.swap_bytes();
			ASSERT_EQ(c1, c2);
		}
		else {
			constexpr auto c = u32::from_be(n);
			ASSERT_EQ(c, n);
		}
	}
	{
		constexpr auto n = u32(0x1A);
		if constexpr (cfg::endian() == cfg::endian_t::big) {
			constexpr auto c1 = u32::from_le(n);
			constexpr auto c2 = n.swap_bytes();
			ASSERT_EQ(c1, c2);
		}
		else {
			constexpr auto c = u32::from_le(n);
			ASSERT_EQ(c, n);
		}
	}
	{

		constexpr auto n = u32(0x1A);
		if constexpr (cfg::endian() == cfg::endian_t::big) {
			constexpr auto c = n.to_be();
			ASSERT_EQ(c, n);
		}
		else {
			constexpr auto c1 = n.to_be();
			constexpr auto c2 = n.swap_bytes();
			ASSERT_EQ(c1, c2);
		}
	}
	{

		constexpr auto n = u32(0x1A);
		if constexpr (cfg::endian() == cfg::endian_t::big) {
			ASSERT_EQ(n.to_le(), n.swap_bytes());
		}
		else {
			ASSERT_EQ(n.to_le(), n);
		}
	}

	{
		constexpr auto r = i16::max().try_add(i16::max()).is_none();
		ASSERT(r);
	}
	{
		constexpr auto r = i16::max().try_add(i16(1)).is_none();
		ASSERT(r);
	}
	{
		auto c1 = i16::max().try_add(i16(0));
		constexpr auto c2 = i16::max();
		ASSERT_EQ(c1.unwrap(), c2);
	}
	{
		ASSERT(!(i16::min().try_add(i16(-1))));
	}
	{
		constexpr auto c1 = i16::min().try_add(i16(1));
		constexpr auto c2 = i16(i16::min().value() + 1);
		ASSERT_EQ(c1.unwrap(), c2);
		ASSERT(!(u16::max().try_add(u16(1))));
	}
	{
		constexpr auto c1 = u32(1).try_sub(u32(1));
		ASSERT(!(c1.unwrap().value()));
		ASSERT(!(u32().try_sub(u32(1))));
		ASSERT_EQ(i16().try_sub(i16(1)).unwrap(), i16(-1));
		ASSERT(!(i16::min().try_sub(i16(1))));
		ASSERT_EQ(i16::min().try_sub(i16(-1)).unwrap(), i16(i16::min().value() + 1));
	}
	{
		constexpr auto c = u32(5).try_mul(1);
		ASSERT_EQ(c.unwrap(), u32(5));
	}
	{
		constexpr auto r = u32::max().try_mul(2).is_none();
		ASSERT(r);
		ASSERT_EQ(i32::max().try_mul(1).unwrap(), i32::max());
		ASSERT(!(i32::max().try_mul(2)));
		ASSERT_EQ(i32(-2).try_mul(-2).unwrap(), i32(4));
		ASSERT_EQ(i32(-2).try_mul(2).unwrap(), i32(-4));
		ASSERT(!(i32::min().try_mul(2)));
		ASSERT_EQ(i32::min().try_mul(0).unwrap(), i32(0));

	}
	{
		constexpr auto r = i32::min().try_div(-1).is_none();
		ASSERT(r);
	}
	{
		constexpr auto r = i32(1).try_div(0).is_none();
		ASSERT(r);
	}
	{
		constexpr auto c = i32(5).try_rem(2);
		ASSERT_EQ(c.unwrap(), i32(1));
	}
	{
		constexpr auto r = i32(5).try_rem(0).is_none();
		ASSERT(r);
		ASSERT(!(i32::min().try_rem(-1)));
	}

	{
		constexpr auto c = i32(5).try_neg();
		ASSERT_EQ(c.unwrap(), i32(-5));
	}
	{
		constexpr auto r = i32::min().try_neg().is_none();
		ASSERT(r);
	}
	{
		[[maybe_unused]] constexpr auto c1 = i32(5).try_neg();
		//constexpr auto c = u32(5).try_neg();
	}

	{
		constexpr auto c = i32(0x1).try_shl(4);
		ASSERT_EQ(c.unwrap(), i32(0x10));
	}
	{
		constexpr auto r = i32(0x1).try_shl(32).is_none();
		ASSERT(r);
	}
	{
		constexpr auto c = i32(0x10).try_shr(4);
		ASSERT_EQ(c.unwrap(), i32(0x1));
	}
	{
		constexpr auto r = i32(0x10).try_shr(32).is_none();
		ASSERT(r);
	}
	{
		constexpr auto c = i32::max().try_abs();
		ASSERT_EQ(c.unwrap(), i32::max());
		ASSERT_EQ(i32(-5).try_abs().unwrap(), i32(5));
		constexpr auto r = i32::min().try_abs().is_none();
		ASSERT(r);
	}
	{
		constexpr auto c = i32(2).try_pow(3);
		ASSERT_EQ(c.unwrap(), i32(8));
	}
	{
		constexpr auto c = i32(-2).try_pow(3);
		ASSERT_EQ(c.unwrap(), i32(-8));
		ASSERT_EQ(i16::max().try_pow(0).unwrap(), i16(1));
		ASSERT_EQ(i16::min().try_pow(0).unwrap(), i16(1));
		ASSERT_EQ(i16::max().try_pow(1).unwrap(), i16::max());
		ASSERT(!(i16::max().try_pow(2)));
		ASSERT(!(i16::min().try_pow(2)));
	}
	{
		constexpr auto c = i32(100).saturating_add(1);
		ASSERT_EQ(c, i32(101));
	}
	{
		constexpr auto c = i32::max().saturating_add(100);
		ASSERT_EQ(c, i32::max());
	}
	{
		constexpr auto c = i32::min().saturating_add(-1);
		ASSERT_EQ(c, i32::min());
	}
	{
		constexpr auto c = i32(100).saturating_sub(127);
		ASSERT_EQ(c, i32(-27));
		ASSERT_EQ(i32::min().saturating_sub(100), i32::min());
		ASSERT_EQ(i32::max().saturating_sub(-1), i32::max());
	}
	{
		constexpr auto c = i32(100).saturating_neg();
		ASSERT_EQ(c, i32(-100));
		ASSERT_EQ(i32(-100).saturating_neg(), i32(100));
		ASSERT_EQ(i32::min().saturating_neg(), i32::max());
		ASSERT_EQ(i32::max().saturating_neg(), i32(i32::min().value() + 1));
	}
	{
		constexpr auto c = i32(100).saturating_abs();
		ASSERT_EQ(c, i32(100));
		ASSERT_EQ(i32(-100).saturating_abs(), i32(100));
		ASSERT_EQ(i32::min().saturating_abs(), i32::max());
		ASSERT_EQ(i32(i32::min().value() + 1).saturating_abs(), i32::max());
	}
	{
		constexpr auto c = i32(10).saturating_mul(12);
		ASSERT_EQ(c, i32(120));
		ASSERT_EQ(i32::max().saturating_mul(10), i32::max());
		ASSERT_EQ(i32::max().saturating_mul(-10), i32::min());
		ASSERT_EQ(i32::min().saturating_mul(10), i32::min());
		ASSERT_EQ(i32::min().saturating_mul(-10), i32::max());
	}
	{
		//constexpr auto c = i32(1).saturating_div(0); //PANIC - not constexpr expr
		constexpr auto c = i32(5).saturating_div(2);
		ASSERT_EQ(c, 2);
		ASSERT_EQ(i32::max().saturating_div(-1), i32(i32::min().value() + 1));
		ASSERT_EQ(i32::min().saturating_div(-1), i32::max());
	}
	{
		constexpr auto c = i32(-4).saturating_pow(3);
		ASSERT_EQ(c, i32(-64));
		ASSERT_EQ(i32(4).saturating_pow(3), i32(64));
		ASSERT_EQ(i32::max().saturating_pow(2), i32::max());
		ASSERT_EQ(i32::max().saturating_pow(3), i32::max());
		ASSERT_EQ(i32::min().saturating_pow(2), i32::max());
		ASSERT_EQ(i32::min().saturating_pow(3), i32::min());
	}
	{
		constexpr auto c = i32(10).signum();
		ASSERT_EQ(c, 1);
		ASSERT_EQ(i32().signum(), 0);
		ASSERT_EQ(i32(-10).signum(), -1);
		ASSERT_EQ(u32().signum(), 0U);
		ASSERT_EQ(u32(10).signum(), 1U);
	}
	{
		constexpr auto c = i32(5) + i32(6);
		ASSERT_EQ(c, i32(11));
	}
	{
		auto c = i32(5) + i32(6);
		c += 1;
		ASSERT_EQ(c, i32(12));
		c -= 1;
		ASSERT_EQ(c, i32(11));
		c *= 2;
		ASSERT_EQ(c, i32(22));
		c /= 2;
		ASSERT_EQ(c, i32(11));
		c %= 2;
		ASSERT_EQ(c, i32(1));
	}

	{
		constexpr auto c = i32(5) - i32(6);
		ASSERT_EQ(c, i32(-1));
	}

	{
		constexpr auto c = i32(5) * i32(6);
		ASSERT_EQ(c, i32(30));
	}
	{
		constexpr auto c = i32(5) / i32(5);
		ASSERT_EQ(c, i32(1));
	}
	{
		constexpr auto c = i32(5) % i32(2);
		ASSERT_EQ(c, i32(1));
	}
	{
		constexpr auto c = i32(2) & i32(1);
		ASSERT_EQ(c, i32());
	}
	{
		constexpr auto c = i32(2) | i32(1);
		ASSERT_EQ(c, i32(3));
	}
	{
		constexpr auto c = ~i32();
		ASSERT_EQ(c, i32(-1));
	}
	{
		constexpr auto c = i32(1) ^ i32(1);
		ASSERT_EQ(c, i32());
	}
	{

		[[maybe_unused]] const auto x = i32(12) <=> i64(13LL);
	}
	return true;
}

int main() {
	f();
	[[maybe_unused]] constexpr bool b = f();
	return 0;

}
