

namespace iron::unsafe {

	class asserts final {
	public:
		asserts(const asserts&) = delete;
		asserts& operator=(const asserts&) = delete;
		asserts(asserts&&) = delete;
		asserts& operator=(asserts&&) = delete;
		~asserts() = default;
		using line_number_t = std::uint_least32_t;
		[[noreturn]] static void do_panic(const char* _func, const char* _file,line_number_t _line, const char32_t* _msg)noexcept { _do_panic(_func, _file, _line, _msg); }
		[[noreturn]] static void do_panic(const char* _func, const char* _file, line_number_t _line, const char* _msg)noexcept { _do_panic(_func, _file, _line, _msg); }
		[[noreturn]] static void do_panic(const char* _func, const char* _file, line_number_t _line, const error_handler& _h)noexcept { _do_panic(_func, _file, _line, _h); }
		static constexpr void void0()noexcept { }

		typedef enum class enum_type {
			force_panic
			, bool_assert
			, assert_eq
			, assert_ne
			, first = force_panic
			, last = assert_ne
		} type_t;

		[[noreturn ]] static void do_assert(type_t _t, const char* _func, const char* _file, line_number_t _line, const char* _expr1, const char* _expr2)noexcept {
			_do_assert(_t,_func, _file, _line, _expr1, _expr2);
		}

		template <class> // false value attached to a dependent name (for static_assert)
		static constexpr bool always_false = false;

		//inlines for macro RE
		template <typename _T,typename _E>
		[[nodiscard]] static constexpr _E re(iron::result<_T,_E>& _v)noexcept { return std::move(_v.unchecked_unwrap_err());}

		template <typename _E> //_E is error_handler
		[[nodiscard]] static constexpr _E re(_E& _v)noexcept { return std::move(_v);}

#if 0 //streams is not implemented now
		template <typename _STREAM, typename _ENCODER>
		[[nodiscard]] static constexpr auto re(iron::io::chr_writer<_STREAM, _ENCODER>& _v)noexcept { return _re(_v);}

		template <typename _STREAM, typename _ENCODER>
		[[nodiscard]] static constexpr auto re(iron::io::chr_reader<_STREAM, _ENCODER>& _v)noexcept { return _re(_v);}

		template <typename _STREAM, typename _ENCODER>
		[[nodiscard]] static constexpr auto re(iron::io::chr_rw<_STREAM, _ENCODER>& _v)noexcept { return _re(_v);}

		template <typename _STREAM>
		[[nodiscard]] static constexpr auto re(iron::io::raw_reader<_STREAM>& _v)noexcept { return _re(_v);}

		template <typename _STREAM>
		[[nodiscard]] static constexpr auto re(iron::io::raw_writer<_STREAM>& _v)noexcept { return _re(_v);}

		template <typename _STREAM>
		[[nodiscard]] static constexpr auto re(iron::io::raw_rw<_STREAM>& _v)noexcept { return _re(_v); }
#endif 

		//inlines for macro RR

		template <typename _E> //_E is error_handler
		[[nodiscard]] static constexpr auto rr(_E& _v)noexcept { return iron::err(std::move(_v));}

		template <typename _T,typename _E>
		[[nodiscard]] static constexpr auto rr(iron::result<_T,_E>& _v)noexcept { return iron::err(std::move(_v.unchecked_unwrap_err()));}

#if 0  //streams is not implemented now
		template <typename _STREAM, typename _ENCODER>
		[[nodiscard]] static constexpr auto rr(iron::io::chr_writer<_STREAM, _ENCODER>& _v)noexcept { return _rr(_v);}

		template <typename _STREAM, typename _ENCODER>
		[[nodiscard]] static constexpr auto rr(iron::io::chr_reader<_STREAM, _ENCODER>& _v)noexcept { return _rr(_v);}

		template <typename _STREAM, typename _ENCODER>
		[[nodiscard]] static constexpr auto rr(iron::io::chr_rw<_STREAM, _ENCODER>& _v)noexcept { return _rr(_v);}

		template <typename _STREAM>
		[[nodiscard]] static constexpr auto rr(iron::io::raw_reader<_STREAM>& _v)noexcept { return _rr(_v);}

		template <typename _STREAM>
		[[nodiscard]] static constexpr auto rr(iron::io::raw_writer<_STREAM>& _v)noexcept { return _rr(_v);}

		template <typename _STREAM>
		[[nodiscard]] static constexpr auto rr(iron::io::raw_rw<_STREAM>& _v)noexcept { return _rr(_v);}

#endif 
		template <typename _T>
		[[noreturn]] static void r1(const iron::result<_T, iron::error_handler>& _v)noexcept { _do_panic(nullptr, nullptr, 0, _v.unchecked_unwrap_err()); }

		[[noreturn]] static void r1(const iron::error_handler& _v)noexcept { _do_panic(nullptr,nullptr,0,_v); }
		
#if defined(__clang__)
#define _IRON_FMT_ATTR(_fmt_idx, _va_idx) __attribute__((__format__ (printf, _fmt_idx, _va_idx)))
#else
#define _IRON_FMT_ATTR(_fmt_idx, _va_idx)
#endif

		_IRON_DLL_EXPORT static void print(const char* fmt, ...)noexcept _IRON_FMT_ATTR(1,2);
		
#undef _IRON_FMT_ATTR
	private:
		asserts()noexcept {}
		[[noreturn]] _IRON_DLL_EXPORT static void _do_panic(const char* _func, const char* _file, line_number_t _line, const char32_t*)noexcept;
		[[noreturn]] _IRON_DLL_EXPORT static void _do_panic(const char* _func, const char* _file, line_number_t _line, const char*)noexcept;
		[[noreturn]] _IRON_DLL_EXPORT static void _do_panic(const char* _func, const char* _file, line_number_t _line, const iron::error_handler&)noexcept;
		[[noreturn]] _IRON_DLL_EXPORT static void _do_assert(type_t, const char* func, const char* file, line_number_t line, const char* expr, const char* expr2)noexcept;

#if 0  //streams is not implemented now
		template <typename _STREAM>
		[[nodiscard]] static constexpr auto _re(_STREAM& _v)noexcept { return _v.release_status(); }

		template <typename _STREAM>
		[[nodiscard]] static constexpr auto _rr(_STREAM& _v)noexcept { return iron::err(_v.release_status());}
#endif

		class _pimp;
	};

}


