#ifdef IRON_NO_USE_MODULE
#include <cstdio>
#include <exception>
#include <cstdarg>
#include "iron/str.h"
#include "iron/memory/stacktrace.h"
#include "iron/unsafe/unicode_conversions.h"
#include "iron/error_handler.h"
#include "iron/vector.h"
#include "iron/unsafe/asserts.h"
#endif

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable:26481)
#pragma warning (disable:26485)
#pragma warning (disable:26472)
#pragma warning (disable:26429)
#pragma warning (disable:26826)
#endif

namespace iron::unsafe  {

	class asserts::_pimp final {
	public:
		_pimp() = delete;
		using _uc_t = iron::unsafe::unicode_conversions;
		static constexpr const char* str(type_t)noexcept;
		static constexpr const char* compare(type_t)noexcept;
		static void put(char) = delete;
		static void put(char8_t) = delete;
		static void put(std::byte) = delete;
		static void put(unsigned char) = delete;
		static void put(signed char) = delete;
		static void put(wchar_t) = delete;
		static void put(char16_t) = delete;
		static void put(char32_t) = delete;
		static void put(const char*)noexcept;
		static void put(const char32_t*)noexcept;
		static void put(const unsigned char*)noexcept;
		static void put(const char8_t* _s)noexcept { return put(reinterpret_cast<const unsigned char*>(_s)); }
		static void put(short _v)noexcept { put(static_cast<int>(_v)); }
		static void put(int)noexcept;
		static void put(long)noexcept;
		static void put(unsigned short) = delete;
		static void put(std::uint_least32_t)noexcept;
		static std::size_t copy(char*, const char32_t*, std::size_t)noexcept;
		static void put_stacktrace(const error_handler&)noexcept;
		[[noreturn]] static void terminate()noexcept { 
#ifndef NDEBUG
			std::terminate(); 
#else 
			exit(101);
#endif 
		}
	};


	inline constexpr const char* asserts::_pimp::str(type_t t)noexcept {
		switch (t) {
			case type_t::force_panic: return "PANIC: ";
			case type_t::bool_assert: return "ASSERT failed: ";
			case type_t::assert_eq: return "ASSERT_EQ failed: ";
			case type_t::assert_ne: return "ASSERT_NE failed: ";
		}
		return "";
	}

	inline constexpr const char* asserts::_pimp::compare(type_t t)noexcept {
		switch (t) {
			case type_t::assert_eq: return "==";
			case type_t::assert_ne: return "!=";
			default: {}
		}
		return "";
	}

#ifndef stderr
#if defined(_WIN32)
#define stderr __acrt_iob_func(2)
#else 
#error macro stderr must be defined
#endif 
#endif 

	inline void asserts::_pimp::put(const char* _s)noexcept {
		DEBUG_ASSERT(_s);
		std::fprintf(stderr, "%s", _s);
	}

	inline void asserts::_pimp::put(const unsigned char* _s)noexcept {
		DEBUG_ASSERT(_s);
		std::fprintf(stderr, "%s", reinterpret_cast<const char*>(_s));
	}
	void asserts::_pimp::put(const char32_t* _s)noexcept {
		DEBUG_ASSERT(_s);
		char8_t _a[5]{};
		while (*_s) IRON_LIKELY
		{
			auto _sz = _uc_t::utf32_to_utf8(_a, *_s++);
			if (!_sz) IRON_UNLIKELY
			{
				_sz = _uc_t::utf32_to_utf8(_a, _uc_t::default_replacement_chr);
				DEBUG_ASSERT(_sz);
			}
			_a[_sz] = '\0';
			put(&_a[0]);
		}
	}


	inline void asserts::_pimp::put(int _n)noexcept {
		std::fprintf(stderr, "%d", _n);
	}
	inline void asserts::_pimp::put(long _n)noexcept {
		std::fprintf(stderr, "%ld", _n);
	}

	inline void asserts::_pimp::put(std::uint_least32_t _n)noexcept {
		std::fprintf(stderr, "%u", _n);
	}

	void asserts::_do_assert(type_t _t,const char* _func, const char* _file, line_number_t _line, const char* _expr1,const char* _expr2)noexcept {
		_pimp::put(_pimp::str(_t));
		if (_func) IRON_LIKELY
		{
			_pimp::put(" ");
			_pimp::put(_func);
		}
		if (_file) IRON_LIKELY
		{
			_pimp::put(" (");
			_pimp::put(_file);
			_pimp::put(",");
			_pimp::put(_line);
			_pimp::put(") ");
		}
		if (_expr1) IRON_LIKELY
		{
			if (!_expr2) {
				_pimp::put(_expr1);
			}
			else {
				_pimp::put("(");
				_pimp::put(_expr1);
				_pimp::put(") ");
				_pimp::put(_pimp::compare(_t));
				_pimp::put(" (");
				_pimp::put(_expr2);
				_pimp::put(")");
			}
		}
		_pimp::put("\n");
		_pimp::terminate();
	}

	inline void asserts::_pimp::put_stacktrace(const error_handler& _h)noexcept {
		auto _seq = _h.unsafe_stacktrace().entries()->seq(); 
		for (auto& _e : _seq) IRON_LIKELY
		{
			auto _file = _e.file();
			const auto _line = _e.line();
			auto _func = _e.func();
			put(" (");
			put(_file->unsafe_data());
			put(",");
			put(_line);
			put(") ");
			put(_func->unsafe_data());
			put("\n");
		}
	}

	void asserts::_do_panic(const char* _func, const char* _file, line_number_t _line, const char* m)noexcept {
		_pimp::put(_pimp::str(type_t::force_panic));
		if (_func) IRON_LIKELY
		{
			_pimp::put(" ");
			_pimp::put(_func);
		}
		if (_file) IRON_LIKELY
		{
			_pimp::put(" (");
			_pimp::put(_file);
			_pimp::put(",");
			_pimp::put(_line);
			_pimp::put(") ");
		}
		if (m) IRON_LIKELY
		{
			_pimp::put(m);
		}
		_pimp::put("\n");
		_pimp::terminate();
	}

	void asserts::_do_panic(const char* _func, const char* _file, line_number_t _line, const error_handler& _h)noexcept {
		_pimp::put(_pimp::str(type_t::force_panic));
		if (auto _e = dynamic_cast<const iron::unsafe::errors::base_with_info*>(_h.unsafe_error())) IRON_LIKELY
		{
			if (_e->func()) IRON_LIKELY
			{
				_pimp::put(" ");
				_pimp::put(_e->func());
			}
			if (_e->file()) IRON_LIKELY
			{
				_pimp::put(" (");
				_pimp::put(_e->file());
				_pimp::put(",");
				_pimp::put(_e->line());
				_pimp::put(") ");
			}
		}
		const auto _c = _h.unsafe_code();
		const auto _d = _h.unsafe_description();
		_pimp::put(_c);
		_pimp::put(" ");
		_pimp::put(_d);
		_pimp::put("\n");
		if (_h.has_stacktrace()) IRON_UNLIKELY
		{
			_pimp::put_stacktrace(_h);
		}
		_pimp::terminate();
	}


	std::size_t asserts::_pimp::copy(char* _s, const char32_t* _u, std::size_t _n)noexcept {
		std::size_t _count{};
		if (_s && _n && _u) {
			--_n; //for \0 final
			char8_t _buff[5]{};
			while (_n && *_u) {
				auto _sz = _uc_t::utf32_to_utf8(_buff, *_u++);
				if (!_sz) {
					_sz = _uc_t::utf32_to_utf8(_buff, _uc_t::default_replacement_chr);
				}
				auto _p = &_buff[0];
				while(_sz--) {
					if (_n) {
						*_s++ = *_p++;
						--_n;
						++_count;
					}
				}
				if (!_n) {
					break;
				}
			}
			*_s = '\0';
		}
		return _count;
	}

	void asserts::_do_panic(const char* _func, const char* _file, line_number_t _line, const char32_t* _m)noexcept {
		char _buff[512];
		_pimp::copy(_buff, _m, sizeof(_buff));
		_do_panic(_func, _file, _line, _buff);
	}

	void asserts::print(const char* _fmt, ...)noexcept
	{
		va_list _args;
		va_start(_args, _fmt);
		std::vfprintf(stderr,_fmt, _args); 
		va_end(_args);
	}


} //ns

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

