#ifdef IRON_NO_USE_MODULE
#if IRON_SECURITY_CHECKER
#include "iron/unsafe/security_checker.h"
#include "iron/memory/mutex.h"
#include "iron/unsafe/allocator.h"
#endif 
#else
#include "iron/macros.h"
#endif

namespace iron::unsafe {

#if IRON_SECURITY_CHECKER
	//in dll 
	static iron::memory::mutex _sm_sck_mutex;
#define _LOCK() const auto _lk = iron::memory::make_lock_guard(_sm_sck_mutex)
//#define _LOCK()

	void security_checker::_make_ref(_shared_data_t*& _sh,bool _master)noexcept {
		_LOCK();
		if (!_sh) {
			using _shared_allocator_t = iron::unsafe::allocator<_shared_data_t>;
			auto _p = _shared_allocator_t{}.allocate(1);
			_sh = _p;
			if (_sh) {
				_sh->_counter = 1;
				if (_master) {
					_sh->_master_live = true;
				}
			}
		}
		if (_sh) {
			auto& _counter = _sh->_counter;
			++_counter;
		}
	}

	void security_checker::_reset(_shared_data_t*& _sh, bool _master)noexcept {
		if (_sh) {
			_LOCK();
			if (_master) {
				_sh->_master_live = false;
			}
			auto& _counter = _sh->_counter;
			DEBUG_ASSERT(_counter);
			if (!--_counter) {
				using _shared_allocator_t = iron::unsafe::allocator<_shared_data_t>;
				_shared_allocator_t{}.deallocate(_sh, 1);
			}
			_sh = {};
		}
	}

	auto  security_checker::_ref_count(_shared_data_t*const & _sh)noexcept -> size_t {
		size_t _counter{};
		{
			_LOCK();
			if (_sh) {
				_counter = _sh->_counter;
				if (_sh->_master_live) {
					DEBUG_ASSERT(_counter);
					--_counter;
				}
			}
		}
		return _counter;
	}

	bool security_checker::_master_is_live(_shared_data_t*const & _sh)noexcept {
		_LOCK();
		return !_sh || _sh->_master_live;
	}

	bool security_checker::_same_data(_shared_data_t*const & _shl,_shared_data_t*const& _shr)noexcept {
		_LOCK();
		return !_shl || !_shr || _shl == _shr;
	}

#undef _LOCK
#endif 

}

