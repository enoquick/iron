
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
#include "iron/unsafe/security_checker.h"
#endif 

#if IRON_SECURITY_CHECKER
using sc_t = iron::unsafe::security_checker;

class reference {
public:
	constexpr reference(const sc_t& sck)noexcept;
	constexpr const auto& security_checker()const noexcept { return _m_master_sc; }
	constexpr auto& security_checker()noexcept { return _m_master_sc; }
private:
	sc_t _m_master_sc;

};

inline constexpr reference::reference(const sc_t& sck)noexcept 
: _m_master_sc(sck.make_ref()) 
{
	DEBUG_ASSERT(_m_master_sc.status() == sc_t::status_t::ref);
}

class master {
public:
	constexpr master()noexcept = default;
	constexpr master(const master&)noexcept = default;
	constexpr master(master&&)noexcept = default;
	constexpr master& operator=(const master&)noexcept = default;
	constexpr master& operator=(master&&)noexcept = default;
	constexpr ~master() { }
	constexpr void swap(master&)noexcept;
	constexpr const auto& security_checker()const noexcept { return _m_sc; }
	constexpr auto& security_checker()noexcept { return _m_sc; }
	constexpr int get()const noexcept;
	constexpr reference create_servant()noexcept { return reference(_m_sc); }
	constexpr reference create_servant1()noexcept { reference t(_m_sc); return t; }
private:
	int _m_data{};
	sc_t _m_sc{};
};

inline constexpr void master::swap(master& _v)noexcept {
	std::swap(_m_data, _v._m_data);
	_m_sc.swap(_v._m_sc);
}


inline constexpr int master::get()const noexcept {
	_m_sc.check_for_temporary();
	return _m_data;
}

inline constexpr int f() {
#ifndef NDEBUG 
	using s_t = sc_t::status_t;
#endif 
	master m1;
	DEBUG_ASSERT(m1.security_checker().status() == s_t::master);
	master m2(m1);
	DEBUG_ASSERT(m1.security_checker().status() == s_t::master);
	DEBUG_ASSERT(m2.security_checker().status() == s_t::master);
	m2 = std::move(m1);
	DEBUG_ASSERT(m2.security_checker().status() == s_t::master);

#ifndef USE_MODULES //msvc bug in constexpr eval and use modules 
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable:26800)
#endif
	DEBUG_ASSERT(m1.security_checker().status() == s_t::master_temporary);
#if defined(_MSC_VER)
#pragma warning(pop)
#endif
#endif 

	m1 = m2;
	DEBUG_ASSERT(m1.security_checker().status() == s_t::master);
	DEBUG_ASSERT(m2.security_checker().status() == s_t::master);
	master m3(std::move(m1));
	DEBUG_ASSERT(m3.security_checker().status() == s_t::master);
	DEBUG_ASSERT(m1.security_checker().status() == s_t::master_temporary);

	DEBUG_ASSERT_EQ(m3.security_checker().ref_count(), 0);
	auto reference = m3.create_servant();

	if (!std::is_constant_evaluated()) {
		DEBUG_ASSERT_EQ(m3.security_checker().ref_count(), 1);
		DEBUG_ASSERT_EQ(reference.security_checker().ref_count(), 1);
	}
	DEBUG_ASSERT(reference.security_checker().status() == s_t::ref);
	auto servant1 = m3.create_servant1();
	if (!std::is_constant_evaluated()) {
		DEBUG_ASSERT_EQ(m3.security_checker().ref_count(), 2);
		DEBUG_ASSERT_EQ(reference.security_checker().ref_count(),2);
		DEBUG_ASSERT_EQ(servant1.security_checker().ref_count(), 2);
	}
	DEBUG_ASSERT(servant1.security_checker().status() == s_t::ref);
	auto servant2 = servant1;
	if (!std::is_constant_evaluated()) {
		DEBUG_ASSERT_EQ(m3.security_checker().ref_count(), 3);
		DEBUG_ASSERT_EQ(reference.security_checker().ref_count(), 3);
		DEBUG_ASSERT_EQ(servant1.security_checker().ref_count(), 3);
		DEBUG_ASSERT_EQ(servant2.security_checker().ref_count(), 3);
	}
	DEBUG_ASSERT(servant2.security_checker().status() == s_t::ref);
	servant2.security_checker().reset();
	if (!std::is_constant_evaluated()) {
		DEBUG_ASSERT_EQ(m3.security_checker().ref_count(), 2);
		DEBUG_ASSERT_EQ(reference.security_checker().ref_count(), 2);
		DEBUG_ASSERT_EQ(servant1.security_checker().ref_count(), 2);
		DEBUG_ASSERT_EQ(servant2.security_checker().ref_count(), 0);
	}
	return 0;
}

#endif 

int main() {

#if IRON_SECURITY_CHECKER
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable:26814)
#pragma warning (disable:26498)
#endif

	[[maybe_unused]] constexpr auto x = f();
	[[maybe_unused]] const auto y = f();

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

	{
		const auto reference = []()noexcept {
			master m;
			return m.create_servant();
		}();
		DEBUG_ASSERT(!reference.security_checker().master_is_live());
	}
#endif 
}

