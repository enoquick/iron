#ifdef _WIN32

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning (disable:26135)
#pragma warning (disable:26471)
#pragma warning (disable:26472)
#endif

#undef max

namespace iron::unsafe {

	[[nodiscard]] static inline iron::error_handler _last_error(const char* _file, int _line, const char* _func)noexcept {
		const auto _e = ::GetLastError();
		return iron::error_handler(iron::unsafe::errors::os(_e, _file, _line, _func));
	}

	template <bool _R>
	class _os_mx final {
	public:
		using _mutex_t = iron::memory::base_mutex<_R>;
		using _error_handler_t = _mutex_t::error_handler_t;
		using _native_handle_t = ::CRITICAL_SECTION;
		_os_mx()noexcept {
			::InitializeCriticalSection(&_m_cs);
		}
		_os_mx(const _os_mx&) = delete;
		_os_mx& operator=(const _os_mx&) = delete;
		_os_mx(_os_mx&&) = delete;
		_os_mx& operator=(_os_mx&&) = delete;
		~_os_mx() {
			::DeleteCriticalSection(&_m_cs);
		}
		[[nodiscard]] static _os_mx* _get(void* _p)noexcept {
			DEBUG_ASSERT(_p);
			return reinterpret_cast<_os_mx*>(_p);
		}
		[[nodiscard]] static const _os_mx* _get(const void* _p)noexcept {
			DEBUG_ASSERT(_p);
			return reinterpret_cast<const _os_mx*>(_p);
		}
		[[nodiscard]] _error_handler_t _acquire()noexcept {
			try {
				::EnterCriticalSection(&_m_cs);
			}
			catch (...) {
				return ERROR_HANDLER(iron::unsafe::errors::posix::code_t::resource_deadlock_would_occur);
			}
			return {};
		}
		[[nodiscard]] result<bool, _error_handler_t> _can_acquired()noexcept {
			auto _b = ::TryEnterCriticalSection(&_m_cs) ? true : false;
			return ok(std::move(_b));
		}
		[[nodiscard]] _error_handler_t _release()noexcept {
			::LeaveCriticalSection(&_m_cs);
			return {};
		}
		[[nodiscard]] const _native_handle_t* _native_handle()const noexcept {
			return &_m_cs;
		}
		[[nodiscard]] static _error_handler_t _last_error(int _line, const char* _func)noexcept {
			return iron::unsafe::_last_error(__FILE__, _line, _func);
		}
	private:
		_native_handle_t _m_cs;
	};


#ifdef _MSC_VER
#pragma warning(pop)
#endif


}
#endif
