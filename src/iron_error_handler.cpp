#ifdef IRON_NO_USE_MODULE
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <limits>
#include <stdexcept>
#include "iron/error_handler.h"
#include "iron/unsafe/unicode_conversions.h"
#include "iron/unsafe/raw_storage.h"
#include "iron/memory/spin_lock.h"
#endif


namespace iron {
	namespace unsafe::errors {
		class _pimp final {
		public:
			_pimp() = delete;
			using _base_t = iron::unsafe::errors::base;
			using _stacktrace_t = iron::memory::stacktrace;
			static inline const char _unknow_error[] = "unknow error";
			static const _stacktrace_t _sm_empty_stacktrace;
			[[nodiscard]] static bool _is_stacktrace_enable()noexcept {
				const char* _stacktrace_env = iron::error_handler::stacktrace_enable_env;
#ifdef _WIN32
				using _a_t = iron::unsafe::allocator<char>;
				const DWORD _sz_require = ::GetEnvironmentVariableA(_stacktrace_env, NULL, 0); //in WIN32 is thread safe
				if (_sz_require) IRON_LIKELY
				{
					if (auto _p = _a_t{}.allocate(_sz_require)) IRON_LIKELY  //with space for \0
					{
						*_p = {};
						[[maybe_unused]] const DWORD _new_sz = ::GetEnvironmentVariableA(
							_stacktrace_env
							, _p
							, _sz_require
						);
						const bool _enable = *_p && *_p != '0' ? true : false;
						_a_t{}.deallocate(_p, _sz_require);
						return _enable;
					}
					return {}; //error - no such memory
				}
				return {};
#elif defined(__unix__)
				const char* _p = ::getenv(_stacktrace_env); //in linux is thread safe
				return _p && *_p  && *_p != '0' ? true : false;
#else
#error unknow platform
#endif
			}
			static inline bool _stacktrace_set(bool _enable)noexcept {
				const char* _stacktrace_env = iron::error_handler::stacktrace_enable_env;
#ifdef _WIN32
				return ::SetEnvironmentVariableA(_stacktrace_env,_enable ? "1" : "0") ? true : false;
#elif defined(__unix__)
				return ::setenv(_stacktrace_env, _enable ? "1" : "0", 1) ? false : true;
#else
#error unknow platform
#endif
			}
			static void _path_copy(char* _d, const char* _s, std::size_t _sz)noexcept {
				DEBUG_ASSERT(_d);
				DEBUG_ASSERT(_s);
				auto _slen = ::strlen(_s);
				if (_slen < _sz) IRON_LIKELY
				{
					std::memcpy(_d, _s, _slen + 1);
				}
				else { //copy from right to left
					auto _dp = _d + (_sz - 1);
					DEBUG_ASSERT(_dp);
					auto _sp = _s + _slen;
					DEBUG_ASSERT(_sp);
					DEBUG_ASSERT(!*_sp);
					while (_sz--) IRON_LIKELY
					{
						*_dp-- = *_sp--;
					}
					_sz = 3;
					while (_sz--) IRON_LIKELY
					{
						*_d++ = '.';
					}
				}
			}
			template <typename _T1>
			static void _copy(_T1* _to, size_t _to_size, const char* _from, size_t _from_size)noexcept
			requires (iron::concepts::basic_charset<_T1> || iron::concepts::utf8_charset<_T1>);
		private:
		}; //_pimp

		template <typename _T1>
		void _pimp::_copy(_T1* _to, size_t _to_size, const char* _from, size_t _from_size)noexcept
		requires (iron::concepts::basic_charset<_T1> || iron::concepts::utf8_charset<_T1>)
		{
			if (_from_size <= _to_size) IRON_LIKELY
			{
				std::memcpy(_to, _from, _from_size);
			}
			else {
				constexpr const char _final[] = "...";
				const auto _sz = _to_size - sizeof(_final);
				std::memcpy(_to, _from, _sz);
				_to += _sz;
				std::memcpy(_to, _final, sizeof(_final));
			}
		}

		const _pimp::_stacktrace_t _pimp::_sm_empty_stacktrace(0);

		inline void base_with_info::_copy(char* _d, const char* _s, std::size_t _sz)noexcept {
			iron::unsafe::errors::_pimp::_path_copy(_d, _s, _sz);
		}

		inline void exception::_construct(exception_ptr_t&& _v)noexcept {
			DEBUG_ASSERT(!_m_set);
			auto _p = reinterpret_cast<exception_ptr_t*>(&_m_excp[0]);
			std::construct_at(_p, std::move(_v));
			_m_set = true;
		}

		void exception::_destroy(exception& _v)noexcept {
			auto _p = reinterpret_cast<exception_ptr_t*>(&_v._m_excp[0]);
			std::destroy_at(_p);
		}

		auto exception::_current_exception(const exception& _v)noexcept -> const exception_ptr_t& {
			auto _p = reinterpret_cast<const exception_ptr_t*>(&_v._m_excp[0]);
			return *_p;
		}

		void exception::_swap2(exception& _v1,exception& _v2)noexcept {
			std::swap(_v1._m_set, _v2._m_set);
			std::swap(_v1._m_excp, _v2._m_excp);
			std::swap(_v1._m_description, _v2._m_description);
		}

		base*const exception::_clone(exception* _p,const exception& _v)noexcept {
			if (_p) IRON_LIKELY
			{
				if (_v._m_set) IRON_LIKELY
				{
					try {
						auto _x = reinterpret_cast<const exception_ptr_t*>(&_v._m_excp[0]);
						std::rethrow_exception(*_x);
					}
					catch (...) {
						std::construct_at(_p, std::current_exception(), _v.file(), _v.line(),_v.func());
					}
				}
				else {
					std::construct_at(_p);
				}
			}
			return _p;
		}

		const char8_t* exception::_description(const exception& _v)noexcept {
			static_assert(sizeof(_pimp::_unknow_error) < sizeof(_v._m_description), "internal error");
			if (_v._m_set) IRON_LIKELY
			{
				bool _unknow_error = true;
				if (!_v._m_description[0]) IRON_LIKELY
				{
					try {
						auto _x = reinterpret_cast<const exception_ptr_t*>(&_v._m_excp[0]);
						std::rethrow_exception(*_x);
					}
					catch (const std::exception& e) {
						try {
							auto _cs = e.what();
							if (_cs) IRON_LIKELY
							{
								_pimp::_copy(_v._m_description, sizeof(_v._m_description), _cs, std::strlen(_cs) + 1);
								_unknow_error = {};
							}
						}
						catch (...) {
						}
					}
					catch (...) {
					}
				}
				if (_unknow_error) IRON_UNLIKELY
				{
					_pimp::_copy(_v._m_description, sizeof(_v._m_description), _pimp::_unknow_error, sizeof(_pimp::_unknow_error));
				}
			}
			else {
				_v._m_description[0] = {};
			}
			return &(_v._m_description[0]);
		}

#ifdef WIN32
		static inline const char8_t* _win32_message(char8_t* _m,size_t _msz, long _code)noexcept {
			DEBUG_ASSERT(_m && !*_m && _msz);
			constexpr size_t _sz{ 200 };
			wchar_t _t[_sz]{}; //for ms 128 chars are sufficient
			const DWORD _size = ::FormatMessageW(
				FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_MAX_WIDTH_MASK
				, NULL
				, _code
				, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT)
				, std::begin(_t)
				, static_cast<DWORD>(_sz)
				, NULL
			);
			if (_size > 0) IRON_LIKELY
			{
				auto _p = &(_t[0]);
				constexpr bool _safe = false;
				[[maybe_unused]] const bool _u = iron::unsafe::unicode_conversions::copy<_safe>(
					_m
					,_msz
					, _p
					, _p + (static_cast<size_t>(_size) + 1)
				);
			}
			else {
				*_m={};
			}
			return _m;
		}
#elif defined(__unix__)
		static inline const char8_t* _posix_unix_message(char8_t* _m,size_t _sz, long _code)noexcept {
			DEBUG_ASSERT(_m && !*_m && _sz);
			const char* _p = ::strerror(_code); //strerror is thread safe in linux
			if (_p && *_p) IRON_LIKELY
			{
				_pimp::_copy(_m,_sz, _p, std::strlen(_p) + 1);
			}
			else {
				*_m={};
			}
			return _m;
		}
#else
#error unknow platform
#endif

#if 0
		bool os::_is()const noexcept {
			return !_m_numeric_code;
		}
#endif 
		base* const os::_clone(os* _p,const os& _v)noexcept {
			if (_p) IRON_LIKELY
			{
				std::construct_at(_p, _v._m_numeric_code, _v.file(),_v.line(), _v.func());
			}
			return _p;
		}

		const char* os::_code(const os& _v)noexcept {
			if (!_v._m_arr_code[0]) IRON_LIKELY
			{
				static_assert(sizeof(long) >= sizeof(code_t),"internal error");
				[[maybe_unused]] const auto _sz=std::snprintf(_v._m_arr_code, sizeof(_v._m_arr_code), "%ld", static_cast<long>(_v._m_numeric_code));
				_v._m_arr_code[sizeof(_m_arr_code) - 1] = '\0';
			}
			return &(_v._m_arr_code[0]);
		}
		void os::_swap2(os& _v1, os& _v2)noexcept {
			std::swap(_v1._m_numeric_code, _v2._m_numeric_code);
			std::swap(_v1._m_arr_code, _v2._m_arr_code);
			std::swap(_v1._m_description, _v2._m_description);
		}

#ifdef _WIN32
#define _FUNC _win32_message
#elif defined(__unix)
#define _FUNC _posix_unix_message
#else
#error unknow platform
#endif

		const char8_t* os::_description(const os& _v)noexcept {
			if (!_v._m_description[0]) IRON_UNLIKELY
			{
				_FUNC(_v._m_description,sizeof(_v._m_description),_v._m_numeric_code);

				if (!_v._m_description[0]) IRON_UNLIKELY
				{
					_pimp::_copy(_v._m_description,sizeof(_v._m_description), _pimp::_unknow_error, sizeof(_pimp::_unknow_error));
				}
			}
			return &(_v._m_description[0]);
		}

#undef _FUNC
		
		base* const posix::_clone(posix* _p,const posix& _v)noexcept {
			if (_p) IRON_LIKELY
			{
				std::construct_at(_p, _v._m_err_code, _v.file(), _v.line(), _v.func());
			}
			return _p;
		}

		const char* posix::_code(const posix& _v)noexcept {
			if (!_v._m_arr_code[0]) IRON_UNLIKELY
			{
				const auto  _n = numeric_code(_v._m_err_code);
				using _t_t = decltype(_n);
				static_assert(sizeof(long) >= sizeof(_t_t), "internal error");
				[[maybe_unused]] const auto _sz=std::snprintf(_v._m_arr_code, sizeof(_v._m_arr_code), "%ld",static_cast<long>(_n));
				_v._m_arr_code[sizeof(_m_arr_code) - 1] = '\0';
			}
			return &(_v._m_arr_code[0]);
		}

#ifdef _WIN32
		static inline const char8_t* _posix_win32_message(char8_t* _m,std::size_t _msz,long _code)noexcept {
			DEBUG_ASSERT(_m && !*_m && _msz);
			constexpr size_t _sz{ 100 }; //for ms 96 chars are sufficients
			char _t[_sz]{};
			errno = 0;
			::strerror_s(std::begin(_t),_sz,_code);
			if (errno) IRON_LIKELY
			{
				*_m={};
			}
			else {
				_pimp::_copy(_m,_msz, _t, _sz);
			}
			return _m;
		}
#endif
#ifdef _WIN32
		#define _FUNC _posix_win32_message
#elif defined(__unix)
		#define _FUNC _posix_unix_message
#else
#error unknow platform
#endif

		const char8_t* posix::_description(const posix& _v)noexcept {
			if (!_v._m_description[0]) IRON_UNLIKELY
			{
				const auto _n = numeric_code(_v._m_err_code);
				_FUNC(_v._m_description,sizeof(_v._m_description),_n);
				if (!_v._m_description[0]) IRON_UNLIKELY
				{
					_pimp::_copy(_v._m_description,sizeof(_v._m_description), _pimp::_unknow_error, sizeof(_pimp::_unknow_error));
				}
			}
			return &(_v._m_description[0]);
		}

		void posix::_swap2(posix& _v1, posix& _v2)noexcept {
			std::swap(_v1._m_err_code, _v2._m_err_code);
			std::swap(_v1._m_arr_code, _v2._m_arr_code);
			std::swap(_v1._m_description, _v2._m_description);

		}
#undef _FUNC

		static 	iron::memory::recursive_spin_lock _g_lock;
		static iron::error_handler::stacktrace_t _g_empty_stacktrace(0);


		base_with_info::base_with_info(const char* _file, line_number_t _line, const char* _func)noexcept
		: base()
		, _m_line(_line)
		{
			if (_file && *_file) IRON_LIKELY 
			{
				_copy(&_m_file[0], _file, sizeof(_m_file));
			}
			if (_func && *_func) IRON_LIKELY
			{
				_copy(&_m_func[0], _func, sizeof(_m_func));
			}
		}


		exception::exception(exception_ptr_t&& _e, const char* _file, line_number_t _line, const char* _func)noexcept
			: base_with_info(_file, _line, _func)
		{
			_construct(std::move(_e));
		}
		
		auto exception::_to_posix(const exception& _e)noexcept -> posix::code_t {
			using _c_t = posix::code_t;
			try {
				_e.rethrow_exception();
			}
			catch (const std::bad_alloc&) {
				return _c_t::not_enough_memory;
			}
			catch (const std::domain_error&) {
				return _c_t::argument_out_of_domain;
			}
			catch (const std::invalid_argument&) {
				return _c_t::invalid_argument;
			}
			catch (const std::length_error&) {
				return _c_t::value_too_large;
			}
			catch (const std::out_of_range&) {
				return _c_t::result_out_of_range;
			}
			catch (const std::overflow_error&) {
				return _c_t::result_out_of_range;
			}
			catch (const std::underflow_error&) {
				return _c_t::result_out_of_range;
			}
			catch (const std::range_error&) {
				return _c_t::result_out_of_range;
			}
			catch (...) {}
			return posix::code_ok;
		}
		auto exception::_to_os(const exception& _e)noexcept -> os::code_t {
			return {};
		}

	} //namespace errors

	inline void error_handler::_data::_inline_lock()noexcept {
		iron::unsafe::errors::_g_lock.lock();
	}

	inline void error_handler::_data::_inline_unlock()noexcept {
		iron::unsafe::errors::_g_lock.unlock();
	}

	error_handler::_data::~_data() {
		DEBUG_ASSERT(_m_base_error);
		if (!_m_error_static) IRON_LIKELY
		{
			std::destroy_at(_m_base_error);
			DEBUG_ASSERT(_m_size);
			_allocator_t<unsigned char>{}.deallocate(reinterpret_cast<unsigned char*>(_m_base_error),_m_size);
		}
		DEBUG_ASSERT(_m_stacktrace);
		if (!_m_stacktrace_static) IRON_UNLIKELY
		{
			std::destroy_at(_m_stacktrace);
			_allocator_t<stacktrace_t>{}.deallocate(_m_stacktrace, 1);
		}
	}

	void error_handler::_data::_destroy(_data* _p)noexcept {
		DEBUG_ASSERT(_p);
		if (!_p->_m_data_static) IRON_LIKELY
		{
			auto& _counter = _p->_m_shared_counter;
			DEBUG_ASSERT(_counter);
			_inline_lock();
			if (!--_counter) IRON_LIKELY
			{
				std::destroy_at(_p);
				constexpr bool _reset{};
				_allocator_t<_data>{}. template deallocate<_reset>(_p, 1);
			}
			_inline_unlock();
		}
	}

	inline void  error_handler::_data::_make_stacktrace(_data& _d)noexcept {
		DEBUG_ASSERT(!_d._m_stacktrace_static);
		DEBUG_ASSERT(!_d._m_stacktrace);
		if (iron::unsafe::errors::_pimp::_is_stacktrace_enable()) {
			if (auto _p = _allocator_t<stacktrace_t>{}.allocate(1)) IRON_LIKELY
			{
				std::construct_at(_p);
				_d._m_stacktrace = _p;
				return;
			}
		}
		_d._m_stacktrace = &iron::unsafe::errors::_g_empty_stacktrace;
		_d._m_stacktrace_static = true;
	}

	inline auto error_handler::_data::_make_no_such_memory()noexcept -> _data* {
		static iron::unsafe::errors::posix _sm_err(
			iron::unsafe::errors::posix::code_t::not_enough_memory
			,__FILE__
			,__LINE__
			,__func__
		);
		static _data _sm_data{};
		_sm_data._m_base_error = &_sm_err;
		_sm_data._m_error_static = true;
		_sm_data._m_shared_counter = 1;
		_sm_data._m_stacktrace = &iron::unsafe::errors::_g_empty_stacktrace;
		_sm_data._m_stacktrace_static = true;
		_sm_data._m_data_static = true;
		return &_sm_data;
	}

	inline auto error_handler::_data::_make_no_err()noexcept -> const _data& {
		static _data _sm_data{};
		static iron::unsafe::errors::posix _sm_noerr;
		_sm_data._m_base_error = &_sm_noerr;
		_sm_data._m_error_static = true;
		_sm_data._m_shared_counter = 1;
		_sm_data._m_stacktrace = &iron::unsafe::errors::_g_empty_stacktrace;
		_sm_data._m_stacktrace_static = true;
		_sm_data._m_data_static = true;
		return _sm_data;
	}

	auto error_handler::_data::_make(base_t* _p,size_t _sz)noexcept -> _data* {
		if (_p) IRON_LIKELY
		{
			if (auto _p_data = _allocator_t<_data>{}.allocate(1)) IRON_LIKELY
			{
				std::construct_at(_p_data);
				_p_data->_m_base_error = _p;
				_p_data->_m_size = _sz;
				_make_stacktrace(*_p_data);
				_p_data->_m_shared_counter = 1;
				return _p_data;
			}
			//no such memory
			std::destroy_at(_p);
			_allocator_t<unsigned char>{}.deallocate(reinterpret_cast<unsigned char*>(_p), _sz);
		}
		return _make_no_such_memory();
	}

	iron::unsafe::errors::os::code_t error_handler::_data::_numeric_error_s(const _data& _p)noexcept {
		DEBUG_ASSERT(_p._m_base_error);
		if (auto _posix = dynamic_cast<const iron::unsafe::errors::posix*>(_p._m_base_error)) IRON_LIKELY
		{
			return _posix->numeric_code();
		}
		if (auto _os = dynamic_cast<const iron::unsafe::errors::os*>(_p._m_base_error)) IRON_LIKELY
		{
			return _os->code();
		}
#ifdef _WIN32
		return ERROR_UNIDENTIFIED_ERROR;
#elif defined(__unix__)
		return ENOSYS;
#else 
#error unknow platform
#endif 
	}

	auto error_handler::_data::_get_data(const error_handler& _h)noexcept -> const _data& {
		if (_h._m_data) IRON_LIKELY
		{
			return *(_h._m_data);
		}
		return _make_no_err();
	}

	void error_handler::_data::_inc_shared_counter_s(_data& _v)noexcept {
		_inline_lock();
		if (_v._m_shared_counter == max_count) IRON_UNLIKELY
		{
			_inline_unlock();
			PANIC("error_handler max shared count");
		}
		++_v._m_shared_counter;
		_inline_unlock();
	}

#if 0
	auto  error_handler::error_type_dll()const noexcept -> error_type_t  {
		if (auto base = unsafe_error()) IRON_LIKELY
		{
			[[maybe_unused]] auto _p = dynamic_cast<const iron::unsafe::errors::posix*>(base);
			[[maybe_unused]] auto _pos = dynamic_cast<const iron::unsafe::errors::os*>(base);
			[[maybe_unused]] auto _ex = dynamic_cast<const iron::unsafe::errors::exception*>(base);

			std::cerr << "posix " << _p << std::endl;
			std::cerr << "os " << _pos << std::endl;
			std::cerr << "exception " << _ex << std::endl;

			using _t = error_type_t;
			if (dynamic_cast<const iron::unsafe::errors::posix*>(base)) IRON_LIKELY
			{
				return _t::posix;
			}
			if (dynamic_cast<const iron::unsafe::errors::os*>(base)) IRON_LIKELY
			{
				return _t::os;
			}
			if (dynamic_cast<const iron::unsafe::errors::exception*>(base)) IRON_LIKELY
			{
				return _t::exception;
			}
			return _t::user;
		}
		return {};
	}
#endif 

#if 0
#ifdef _WIN32
	void* error_handler::_alloc(size_t _sz)noexcept {
		return std::malloc(_sz);
	 }
#endif 
#endif 

}

