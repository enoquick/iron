
#ifdef IRON_NO_USE_MODULE
#include "iron/cfg.h"
#else
#endif
#include "iron/macros.h"

namespace iron {

	class cfg::pimp final {
	public:
		pimp() = delete;
		static const char* sm_values[];
	};

	const char* cfg::pimp::sm_values[]{
		"unknow"  //0
		,"x86"
		,"x86_64"
		,"powerpc"
		,"powerpc64"
		,"arm"    
		,"arm64"
		,"arm64ec"
		,"aarch64" 
		,"avx" //9
		,"avx2"
		,"crt_static"
		,"rdrand"
		,"sse"
		,"sse2"
		,"sse4.1" //15
		,"windows"
		,"macos"
		,"ios"
		,"linux"
		,"android"
		,"freebsd"
		,"dragonfly"
		,"openbsd"
		,"netbsd" //24
		,"big" 
		,"little"
		,"unix" //27
		,"gnu"
		,"msvc"
		,"clang"
		,"sgx"
		,"p16" //32
		,"p32"
		,"p64"
		,"pre11" //35 
		,"11"
		,"14"
		,"17"
		,"20"
		,"22"

	};

#undef V
#define V(i) pimp::sm_values[i]

	const char* cfg::_vendor()noexcept { return V(0); }

	const char* cfg::_str(arch_t v)noexcept {
		switch(v) {
			using t_t = arch_t;
			case t_t::unknow: return V(0);
			case t_t::x86: return V(1);
			case t_t::x86_64: return V(2);
			case t_t::powerpc: return V(3);
			case t_t::powerpc64: return V(4);
			case t_t::arm: return V(5);
			case t_t::arm64: return V(6);
			case t_t::arm64ec: return V(7);
			case t_t::aarch64: return V(8);
		}
		return V(0);
	}

	const char* cfg::_str(feature_t v)noexcept {
		switch (v) {
			using t_t = feature_t;
			case t_t::unknow: return V(0);
			case t_t::avx: return V(9);
			case t_t::avx2: return V(10);
			case t_t::crt_static: return V(11);
			case t_t::rdrand: return V(12);
			case t_t::sse: return V(13);
			case t_t::sse2: return V(14);
			case t_t::sse4_1: return V(15);
		}
		return V(0);
	}

	const char* cfg::_str(os_t v)noexcept {
		switch (v) {
			using t_t = os_t;
			case t_t::unknow: return V(0);
			case t_t::windows: return V(16);
			case t_t::macos: return V(17);
			case t_t::ios: return V(18);
			case t_t::linux: return V(19);
			case t_t::android: return V(20);
			case t_t::freebsd: return V(21);
			case t_t::dragonfly: return V(22);
			case t_t::openbsd: return V(23);
			case t_t::netbsd: return V(24);
		}
		return V(0);
	}

	const char* cfg::_str(endian_t v)noexcept {
		switch (v) {
			using t_t = endian_t;
			case t_t::unknow: return V(0);
			case t_t::big: return V(25);
			case t_t::little: return V(26);
		}
		return V(0);
	}

	const char* cfg::_str(family_t v)noexcept {
		switch (v) {
			using t_t = family_t;
			case t_t::unknow: return V(0);
			case t_t::unix: return V(27);
			case t_t::windows: return V(16);
		}
		return V(0);
	}

	const char* cfg::_str(env_t v)noexcept {
		switch (v) {
			using t_t = env_t;
			case t_t::unknow: return V(0);
			case t_t::gnu: return V(28);
			case t_t::msvc: return V(29);
			case t_t::clang: return V(30);
		}
		return V(0);
	}


#undef V
} //ns 
