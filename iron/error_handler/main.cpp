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
#include "iron/cfg.h"
#include "iron/error_handler.h"
#include <iostream>
#endif



#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable:26432)
#pragma warning (disable:26409)
#pragma warning (disable:26429)
#pragma warning (disable:26481)
#pragma warning (disable:26490)
#endif

class print_u8 final {
public:
	explicit print_u8(const char8_t* p)noexcept : _m_ptr(p) {}
	const char8_t* ptr()const noexcept { return _m_ptr ? _m_ptr : u8""; }
private:
	const char8_t* _m_ptr;

};

inline std::ostream& operator<<(std::ostream& os, const print_u8& wrapper) {
	return os << reinterpret_cast<const char*>(wrapper.ptr());
}
 
void test_posix() {
	std::cout << "start test_posix" << std::endl;
	using posix_t = iron::unsafe::errors::posix;
	constexpr auto code_p = posix_t::code_t::not_connected;
	const auto h = iron::error_handler::make(code_p, __FILE__, __LINE__, __func__);
	ASSERT(!h);
#ifdef _MSC_VER
#if _MSC_VER < 1944 && defined(USE_MODULES)  
	std::cout << " the dynamic cast in this version is not correctly " << std::endl;
#else 
	ASSERT_EQ(h.error_type(), iron::error_handler::error_type_t::posix);
#endif 
#else 
	ASSERT_EQ(h.error_type(), iron::error_handler::error_type_t::posix);
#endif 
	const auto h1 = h;
	ASSERT(!h1);
	std::cout << "message: " << print_u8(h1.unsafe_description()) << std::endl;
}

void f_test_trace() {
	using error_handler_t = iron::error_handler;
	const auto h = error_handler_t::make(100, __FILE__, __LINE__, __func__);
	std::cout << h.unsafe_code()  << ' ' << print_u8(h.unsafe_description()) << std::endl;
}

void test_os() {
	std::cout << "start test_os " << std::endl;
	constexpr auto code_p = 10000;
	const auto h = ERROR_HANDLER(code_p);
	ASSERT(!h);
#ifdef _MSC_VER
#if _MSC_VER < 1944 && defined(USE_MODULES)  
	std::cout << " the dynamic cast in this version is not correctly " << std::endl;
#else 
	ASSERT_EQ(h.error_type(), iron::error_handler::error_type_t::os);
#endif 
#else 
	ASSERT_EQ(h.error_type(), iron::error_handler::error_type_t::os);
#endif 

	std::cout << "code: " << h.unsafe_code() << " message: " << print_u8(h.unsafe_description()) << std::endl;
	const auto h1 = h;
	ASSERT(!h1);
	std::cout << "code: " << h1.unsafe_code() << " message: " << print_u8(h1.unsafe_description()) << std::endl;
}

void f_test_excp() {
	iron::error_handler h;
	ASSERT(h);
	{
		const auto ne = h.numeric_error();
		ASSERT_EQ(ne, 0);
		const auto code = h.unsafe_code();
		ASSERT_EQ(code[0], '0');
		ASSERT(!code[1]);
		const auto descr = h.unsafe_description();
		ASSERT(*descr);
	}
	try {
		throw std::bad_typeid();
	}
	catch (...) {
		h = iron::error_handler::make(std::current_exception(), __FILE__, __LINE__,__func__);
		ASSERT_EQ(h.error_type(), iron::error_handler::error_type_t::exception);
		try {
			auto ex = dynamic_cast<const iron::unsafe::errors::exception*>(h.unsafe_error());
			ASSERT(ex);
			ex->rethrow_exception();
		}
		catch (const std::bad_typeid&) {
		}
		catch (...) {
			PANIC("internal error");
		}
	}
	ASSERT(!h);
	auto e = dynamic_cast<const iron::unsafe::errors::exception*>(h.unsafe_error());
	ASSERT(e);
	const auto dw = h.unsafe_description();
	std::cout << "exception message: " << print_u8(dw) << std::endl;
	auto& expc = e->current_exception();
	ASSERT(expc);
	try {
		e->rethrow_exception();
	}
	catch (const std::exception& ex) {
		const char* m = ex.what();
		ASSERT(m);
	}	
	const auto dc = h;
	std::cout << "exception message not cloned: " << dc.unsafe_code() << ' ' << print_u8(dc.unsafe_description()) << std::endl;
	try {
		throw std::bad_alloc{};
	}
	catch (...) {
		h = iron::error_handler::make(std::current_exception(), __FILE__, __LINE__, __func__);
		auto ex = dynamic_cast<const iron::unsafe::errors::posix*>(h.unsafe_error());
		ASSERT(ex);
		ASSERT_EQ(ex->code(), iron::unsafe::errors::posix::code_t::not_enough_memory);
	}
}

int main() {	
	std::cout << "compiler: " << iron::cfg::str(iron::cfg::env()) << std::endl 
		<< " version: " << iron::cfg::compiler_version() << std::endl
	;
	test_posix();
	test_os();
	f_test_trace();
	f_test_excp();	
	return 0;
}
