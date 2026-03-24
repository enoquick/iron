
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable:6326)
#pragma warning (disable:26497)
#endif

namespace iron {

	class cfg final {
	public:
		struct cfg_tag { cfg_tag() = delete; };
		typedef struct version {
			using number_t = std::uint32_t;
			number_t major, minor, patch;
		} version_t;
		using size_t=std::size_t;
		cfg() = delete;
		typedef enum class enum_target_arch {
			unknow
			, x86
			, x86_64
			, powerpc
			, powerpc64
			, arm
			, arm64
			, arm64ec
			, aarch64
			,first=unknow
			,last= aarch64
		} arch_t;
		typedef enum class enum_feature {
			unknow
			, avx
			, avx2
			, crt_static
			, rdrand
			, sse
			, sse2
			, sse4_1
			, first = unknow
			, last = sse4_1
		} feature_t;
		typedef enum class enum_target_os {
			unknow
			,windows
			,macos
			,ios
			,linux
			,android
			,freebsd
			,dragonfly
			,openbsd
			,netbsd
			, first = unknow
			, last = netbsd
		} os_t;
		typedef enum class enum_endian {
			unknow
			,big
			,little
			, first = unknow
			, last = little
		} endian_t;
		typedef enum class enum_family {
			unknow
			,unix
			,windows
			, first = unknow
			, last= windows
		} family_t;
		typedef enum class enum_target_env {
			unknow
			,gnu
			,msvc
			,clang
			,first = unknow
			,last = clang
		} env_t;
		static constexpr auto char_bits = static_cast<std::uint32_t>(CHAR_BIT);
		[[nodiscard]] static consteval arch_t arch()noexcept {
			using t_t = arch_t;
#ifdef _MSC_VER  //https://learn.microsoft.com/en-us/cpp/preprocessor/predefined-macros?view=msvc-170
#ifdef _M_ARM
			return t_t::arm;
#endif
#ifdef _M_ARM64
			return t_t::arm64;
#endif
#ifdef _M_ARM64EC
			return t_t::arm64ec;
#endif
#if defined(_M_AMD64) && !defined(_M_ARM64EC)
			return t_t::x86_64;
#endif
#if defined(_M_IX86) && !defined(_M_ARM)
			return t_t::x86;
#endif
#endif
			return t_t::unknow;
		}
		[[nodiscard]] static consteval feature_t feature()noexcept { return feature_t::unknow; }
		[[nodiscard]] static consteval os_t os()noexcept { return os_t::unknow; }
		[[nodiscard]] static consteval endian_t endian()noexcept {
			using t_t = endian_t;
			if constexpr (std::endian::native == std::endian::big) {
				return t_t::big;
			}
			else if constexpr (std::endian::native == std::endian::little) {
				return t_t::little;
			}
			else {
				return t_t::unknow;
			}
		}
		[[nodiscard]] static consteval family_t family()noexcept { return family_t::unknow; }
		[[nodiscard]] static consteval env_t env()noexcept {
			using t_t = env_t;
#if defined(_MSC_VER)
			return t_t::msvc;
#elif defined(__clang__)
			return t_t::clang;
#elif defined(__GNUC__)
			return t_t::gnu;
#else
			return t_t::unknow;
#endif
		}
		[[nodiscard]] static consteval std::uint32_t pointer_width()noexcept {
			constexpr auto sz = sizeof(size_t) * char_bits;
			if constexpr (sz == 64) {
				return 64;
			}
			else if constexpr (sz == 32) {
				return 32;
			}
			else if constexpr (sz == 16) {
				return 16;
			}
			else {
				return 0;
			}
		}
		[[nodiscard]] static consteval std::uint32_t cpp_std()noexcept {
#ifdef __cplusplus //it's a C++ compiler

#ifdef _MSC_VER //it's a MS VISUAL C++ compiler
#ifndef _MSVC_LANG
			return 0;
#elif _MSVC_LANG == 201402L
			return 14;
#elif _MSVC_LANG == 201703L
			return 17;
#elif _MSVC_LANG == 202002L || _MSVC_LANG == 202004L
			return 20;
#else
			return 0;
#endif
#endif //_MSC_VER

#ifdef __GNUC__ //it's g++ compiler
#if __cplusplus == 199711L //c++98
			return 0;
#elif __cplusplus ==  201103L
			return 11;
#elif __cplusplus ==  201402L
			return 14;
#elif __cplusplus ==  201703L
			return 17;
#elif __cplusplus ==  202002L
			return 20;
#else
			return 0;
#endif //__cplusplus
#endif //__GNUC__
			return 0;
#else //not a c++ compiler
			return 0;
#endif
		}
		[[nodiscard]] static const char* vendor()noexcept { return _vendor(); }
		[[nodiscard]] static const char* str(arch_t _v)noexcept { return _str(_v); }
		[[nodiscard]] static const char* str(feature_t _v)noexcept { return _str(_v); }
		[[nodiscard]] static const char* str(os_t _v)noexcept { return _str(_v); }
		[[nodiscard]] static const char* str(endian_t _v)noexcept { return _str(_v); }
		[[nodiscard]] static const char* str(family_t _v)noexcept { return _str(_v); }
		[[nodiscard]] static const char* str(env_t _v)noexcept { return _str(_v); }
		[[nodiscard]] static consteval std::uint32_t compiler_version()noexcept {
#ifdef _MSC_VER
			return _MSC_VER;
#elif defined(__GNUC__)
			return __GNUC__ * 1000 + __GNUC_MINOR__;
#elif defined(__clang_major__)
			return __clang_major__ * 1000 + __clang_minor__;
#else
			return 0;
#endif
		}
		[[nodiscard]] static consteval size_t hash_prime()noexcept {
			constexpr auto _x = pointer_width();
			if constexpr (_x == 16) return 403UL;
			else if constexpr (_x == 32) return 16777619UL;
			else if constexpr (_x == 64) return 1099511628211ULL;
			else return 0;
		}

		[[nodiscard]] static consteval size_t hash_prime_start()noexcept {
			constexpr auto _x = pointer_width();
			if constexpr (_x == 16) return 1056197UL;
			else if constexpr (_x == 32) return 2166136261UL;
			else if constexpr (_x == 64) return 14695981039346656037ULL;
			else return 0;
		}
		[[nodiscard]] static constexpr version_t iron_version()noexcept { return version_t{ IRON_VERSION }; }
	private:
		_IRON_DLL_EXPORT static const char* _vendor()noexcept;
		_IRON_DLL_EXPORT static const char* _str(arch_t)noexcept; 
		_IRON_DLL_EXPORT static const char* _str(feature_t)noexcept;
		_IRON_DLL_EXPORT static const char* _str(os_t)noexcept; 
		_IRON_DLL_EXPORT static const char* _str(endian_t)noexcept;
		_IRON_DLL_EXPORT static const char* _str(family_t)noexcept;
		_IRON_DLL_EXPORT static const char* _str(env_t)noexcept;
		class pimp;
	};

	static_assert(iron::concepts::cfg<iron::cfg>, "internal error");

} //ns

#if defined(_MSC_VER)
#pragma warning(pop)
#endif



