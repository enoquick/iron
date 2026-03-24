#ifdef IRON_NO_USE_MODULE
#include "iron/memory/condition_variable.h"
#else
#endif

namespace iron::unsafe {
	template <typename _MUTEX>
	class _os_cv;
}


#include "iron/impl/unsafe/cv_win32_impl.h"
#include "iron/impl/unsafe/cv_pthread_impl.h"


namespace iron::memory {


	template <>
	void base_condition_variable<iron::memory::mutex>::_init(void* _p)noexcept {
		using _cv_t = iron::unsafe::_os_cv<iron::memory::mutex>;
		static_assert(sizeof(_cv_t) <= _stack_area_t::_area_size);
		std::construct_at(_cv_t::_get(_p));
	}

	template <>
	void  base_condition_variable<iron::memory::mutex>::_destroy(void* _p)noexcept {
		using _cv_t = iron::unsafe::_os_cv<iron::memory::mutex>;
		std::destroy_at(_cv_t::_get(_p));
	}

	template <>
	void  base_condition_variable<iron::memory::mutex>::_notify_one(void* _p)noexcept {
		using _cv_t = iron::unsafe::_os_cv<iron::memory::mutex>;
		_cv_t::_get(_p)->_notify_one();
	}
	template <>
	void  base_condition_variable<iron::memory::mutex>::_notify_all(void* _p)noexcept {
		using _cv_t = iron::unsafe::_os_cv<iron::memory::mutex>;
		_cv_t::_get(_p)->_notify_all();
	}
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning (disable:26460)
#endif

	template <>
	auto  base_condition_variable<iron::memory::mutex>::_wait(void* _p,lock_t& _lock)noexcept -> error_handler_t {
		using _cv_t = iron::unsafe::_os_cv<iron::memory::mutex>;
		return _cv_t::_get(_p)->_wait(_lock);
	}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

	template <>
	const void* base_condition_variable<iron::memory::mutex>::_native_handle(const void* _p)noexcept {
		using _cv_t = iron::unsafe::_os_cv<iron::memory::mutex>;
		return _cv_t::_get(_p)->_native_handle();
	}

}



