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
#include <string>
#include "iron/ref_wrapper.h"
#endif 

class not_copiable {
public:
	constexpr not_copiable(int x)noexcept : m_i(x) { }
	constexpr not_copiable(const not_copiable&) = delete;
	constexpr not_copiable(not_copiable&& _v)noexcept
		:m_i()
	{
		IRON_SECURITY_CHECK_TEMPORARY(_v);
		_swap(_v);
		IRON_SECURITY_MARK_TEMPORARY(_v);
	}
	constexpr not_copiable& operator=(const not_copiable&) = delete;
	constexpr not_copiable& operator=(not_copiable&& _v)noexcept {
		IRON_SECURITY_CHECK_TEMPORARY(_v);
		_swap(_v);
		IRON_SECURITY_MARK_TEMPORARY(_v);
		return *this;
	}
	constexpr ~not_copiable() = default;
	constexpr const int& get()const noexcept {
		IRON_SECURITY_CHECK_TEMPORARY_THIS();
		return m_i;
	}
	constexpr int& get()noexcept {
		IRON_SECURITY_CHECK_TEMPORARY_THIS();
		return m_i;
	}
	constexpr bool operator==(const not_copiable& _v)const noexcept {
		IRON_SECURITY_CHECK_TEMPORARY_THIS();
		IRON_SECURITY_CHECK_TEMPORARY(_v);
		return m_i == _v.m_i;
	}
#if IRON_SECURITY_CHECKER
	[[nodiscard]] constexpr const iron::unsafe::security_checker& checker()const noexcept { return _m_ck; }
#endif
private:
	constexpr void _swap(not_copiable& v)noexcept {
		std::swap(m_i, v.m_i);
		IRON_SECURITY_SWAP(v);
	}
	int m_i;
	IRON_SECURITY_DCL();
};


int main() {
	not_copiable nc{ 3 };
	const auto r1=iron::ref_wrapper<int>::from(nc, nc.get());
	const auto r2 = r1;
	ASSERT_EQ(r1, r2);
	ASSERT_EQ(r1, 3);
	const auto x = r1 + r2;
	ASSERT_EQ(x, 6);
	const auto rc1 = iron::make_ref(nc, nc);
	const auto rc2 = iron::make_ref(nc, nc);
	ASSERT_EQ(rc1, *rc2); //not accept rc1 == rc2
}
