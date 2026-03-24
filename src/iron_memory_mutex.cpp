#ifdef IRON_NO_USE_MODULE
#include "iron/memory/mutex.h"
#else
#endif

namespace iron::unsafe {
	template <bool _R>
	class _os_mx;

	//mapping user mutex with low level implementation
	template <typename _MUTEX>
	struct _mutex_mapping;

#if 1
	template <>
	struct _mutex_mapping<iron::memory::mutex> {
		using _mapping_t = _os_mx<true>; //recursive
	};
#endif

}


#include "iron/impl/unsafe/mutex_win32_impl.h"
#include "iron/impl/unsafe/mutex_pthread_impl.h"

namespace iron::memory {

	template <bool _R,typename _SA>
	static inline void _sinit(void* _p)noexcept {
		static_assert(sizeof(iron::unsafe::_os_mx<_R>) <= _SA::_area_size);
		std::construct_at(iron::unsafe::_os_mx<_R>::_get(_p));
	}

	template <>
	void  base_mutex<false>::_init(void* _p)noexcept {
		_sinit<false,base_mutex<false>::_stack_area_t>(_p);
	}

	template <>
	void  base_mutex<true>::_init(void* _p)noexcept {
		_sinit<true,base_mutex<true>::_stack_area_t>(_p);
	}

	template <>
	auto  base_mutex<false>::_acquire(void* _p)noexcept -> error_handler_t {
		return iron::unsafe::_os_mx<false>::_get(_p)->_acquire();
	}

	template <>
	auto  base_mutex<true>::_acquire(void* _p)noexcept -> error_handler_t {
		return iron::unsafe::_os_mx<true>::_get(_p)->_acquire();
	}

	template <>
	auto  base_mutex<false>::_can_acquired(void* _p)noexcept -> result<bool, error_handler_t> {
		return iron::unsafe::_os_mx<false>::_get(_p)->_can_acquired();
	}

	template <>
	auto  base_mutex<true>::_can_acquired(void* _p)noexcept -> result<bool, error_handler_t> {
		return iron::unsafe::_os_mx<true>::_get(_p)->_can_acquired();
	}

	template <>
	auto base_mutex<false>::_release(void* _p)noexcept -> error_handler_t {
		return iron::unsafe::_os_mx<false>::_get(_p)->_release();
	}

	template <>
	auto base_mutex<true>::_release(void* _p)noexcept -> error_handler_t {
		return iron::unsafe::_os_mx<true>::_get(_p)->_release();
	}

	template <>
	const void* base_mutex<false>::_native_handle(const void* _p)noexcept {
		return iron::unsafe::_os_mx<false>::_get(_p)->_native_handle();
	}

	template <>
	const void* base_mutex<true>::_native_handle(const void* _p)noexcept {
		return iron::unsafe::_os_mx<true>::_get(_p)->_native_handle();
	}

	template <>
	void base_mutex<false>::_destroy(void* _p)noexcept{
		std::destroy_at(iron::unsafe::_os_mx<false>::_get(_p));
	}

	template <>
	void base_mutex<true>::_destroy(void* _p)noexcept{
		std::destroy_at(iron::unsafe::_os_mx<true>::_get(_p));
	}

}



