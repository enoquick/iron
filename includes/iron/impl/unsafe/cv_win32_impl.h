#ifdef _WIN32

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning (disable:26135)
#pragma warning (disable:26471)
#pragma warning (disable:26472)
#pragma warning (disable:26492)
#endif

namespace iron::unsafe {

	template <typename _MUTEX>
	class _os_cv final {
	public:
		using _mutex_t = _MUTEX;
		using _cv_t = iron::memory::base_condition_variable<_mutex_t>;
		using _error_handler_t = typename _cv_t::error_handler_t;
		using _lock_t = typename _cv_t::lock_t;
		using _native_handle_t = ::CONDITION_VARIABLE;
		using _mutex_native_t = typename iron::unsafe::_mutex_mapping<_mutex_t>::_mapping_t;
		_os_cv()noexcept : _m_cv(CONDITION_VARIABLE_INIT) {}
		_os_cv(const _os_cv&) = delete;
		_os_cv& operator=(const _os_cv&) = delete;
		_os_cv(_os_cv&&) = delete;
		_os_cv& operator=(_os_cv&&) = delete;
		~_os_cv() {}
		void _notify_one()noexcept {
			::WakeConditionVariable(&_m_cv);
		}
		void _notify_all()noexcept {
			::WakeAllConditionVariable(&_m_cv);
		}
		_error_handler_t _wait(const _lock_t& _lock)noexcept {
			if (auto _mutex = _lock.mutex()) {
				auto _cs = reinterpret_cast<_mutex_native_t::_native_handle_t*>(const_cast<void*>(_mutex->native_handle()));
				if (!::SleepConditionVariableCS(&_m_cv, _cs, INFINITE)) {
					return _last_error(__LINE__, __func__);
				}
				return {};
			}
			return ERROR_HANDLER(iron::unsafe::errors::posix::code_t::invalid_argument);
		}
		static _error_handler_t _last_error(int _line, const char* _func)noexcept {
			return iron::unsafe::_last_error(__FILE__, _line, _func);
		}
		const _native_handle_t* _native_handle()const noexcept { return &_m_cv; }
		static _os_cv* _get(void* _p)noexcept { DEBUG_ASSERT(_p); return reinterpret_cast<_os_cv*>(_p); }
		static const _os_cv* _get(const void* _p)noexcept { DEBUG_ASSERT(_p); return reinterpret_cast<const _os_cv*>(_p); }
	private:
		_native_handle_t _m_cv;
	};



}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif
