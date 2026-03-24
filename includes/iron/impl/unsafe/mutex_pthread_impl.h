#ifdef __unix__

#include <pthread.h>

namespace iron::unsafe {
	[[nodiscard]] static inline iron::error_handler _last_error(int _e,const char* _file, int _line, const char* _func)noexcept {
		DEBUG_ASSERT(_e);
		return iron::error_handler::make(_e, _file, _line, _func);
	}

	template <bool _R>
	class _os_mx final {
	public:
		using _mutex_t = iron::memory::base_mutex<_R>;
		using _error_handler_t = _mutex_t::error_handler_t;
		using _native_handle_t = ::pthread_mutex_t;
		explicit _os_mx()noexcept : _m_mutex(_init_value()) {}
		_os_mx(const _os_mx&) = delete;
		_os_mx& operator=(const _os_mx&) = delete;
		_os_mx(_os_mx&&) = delete;
		_os_mx& operator=(_os_mx&&) = delete;
		~_os_mx() {
			::pthread_mutex_destroy(&_m_mutex);
		}
		[[nodiscard]] static auto _last_error(int _e,int _line, const char* _func)noexcept {
			return iron::unsafe::_last_error(_e, __FILE__, _line, _func);
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
			if (const auto _e=::pthread_mutex_lock(&_m_mutex)) {
				return _last_error(_e,__LINE__, __func__);
			}
			return {};
		}
		[[nodiscard]] result<bool, _error_handler_t> _can_acquired()noexcept {
			if (const auto _e = ::pthread_mutex_trylock(&_m_mutex)) {
				switch (_e) {
					case EBUSY: return ok(false);
					default:;
				}
				return err(_last_error(_e, __LINE__, __func__));
			}
			return ok(true);
		}
		[[nodiscard]] _error_handler_t _release()noexcept {
			if (const auto _e=::pthread_mutex_unlock(&_m_mutex)) {
				return _last_error(_e, __LINE__, __func__);
			}
			return {};
		}
		[[nodiscard]] const _native_handle_t* _native_handle()const noexcept {
			return &_m_mutex;
		}
	private:
		static ::pthread_mutex_t _init_value()noexcept {
			if constexpr (_R) {
				return PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
			}
			else {
				return PTHREAD_MUTEX_INITIALIZER;
			}
		}
		_native_handle_t _m_mutex;
	};

}

#endif
