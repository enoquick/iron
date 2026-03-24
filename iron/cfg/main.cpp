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
#include "iron/cfg.h"
#endif 


int main() {
	std::cout << "CHAR_BITS: " << iron::cfg::char_bits << std::endl
		<< "arch: " << iron::cfg::str(iron::cfg::arch()) << std::endl
		<< "feature: " << iron::cfg::str(iron::cfg::feature()) << std::endl
		<< "os: " << iron::cfg::str(iron::cfg::os()) << std::endl
		<< "endian: " << iron::cfg::str(iron::cfg::endian()) << std::endl
		<< "family: " << iron::cfg::str(iron::cfg::family()) << std::endl
		<< "env: " << iron::cfg::str(iron::cfg::env()) << std::endl
		<< "pointer_width: " << iron::cfg::pointer_width() << std::endl
		<< "cpp_std: " << iron::cfg::cpp_std() << std::endl
		<< "compiler version: " << iron::cfg::compiler_version() << std::endl
		;
	return 0;

}
