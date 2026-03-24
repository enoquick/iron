namespace iron::memory {

	template <bool _RECURSIVE>
	class base_spin_lock {
	public:
		struct spin_lock_tag { spin_lock_tag() = delete; };
		static constexpr bool recursive=_RECURSIVE;
		base_spin_lock()noexcept=default;
		base_spin_lock(const base_spin_lock&)=delete;
		base_spin_lock& operator=(const base_spin_lock&)=delete;
		base_spin_lock(base_spin_lock&&)=delete;
		base_spin_lock& operator=(base_spin_lock&&)=delete;
		~base_spin_lock() = default;
		static_assert(!recursive || noexcept(std::this_thread::get_id()), "this_thread::get_id() not nothrow");
		void lock()noexcept 
		{
			if constexpr (recursive) {
				const auto _current_id = std::this_thread::get_id();
				static_assert(noexcept(_m_recursive._owner_thread_id.load(std::memory_order_acquire)), "thread_id::load() not nothrow");
				if (_m_recursive._owner_thread_id.load(std::memory_order_acquire) == _current_id) {
					static_assert(noexcept(_m_recursive._lock_count.fetch_add(1, std::memory_order_relaxed)), "atomic::fetch_add() not nothrow");
					_m_recursive._lock_count.fetch_add(1, std::memory_order_relaxed);
					return;
				}
				auto _expected_id = std::thread::id(); // Represents no owner

				static_assert(
					noexcept(
						_m_recursive._owner_thread_id.compare_exchange_weak(
							_expected_id
							, _current_id
							, std::memory_order_acquire
							, std::memory_order_relaxed
						)
						)
					, "thread::compare_exchange_weak() not nothrow"
					);

				// Try to acquire the lock
				while (!_m_recursive._owner_thread_id.compare_exchange_weak(
						_expected_id
						,_current_id
						,std::memory_order_acquire
						,std::memory_order_relaxed
					)
				) {
					_expected_id = std::thread::id(); // Reset expected_id for the next attempt
					std::this_thread::yield();
				}

				// Lock acquired, set count to 1
				static_assert(noexcept(_m_recursive._lock_count.store(1, std::memory_order_release)), "atomic::store() not nothrow");
				_m_recursive._lock_count.store(1, std::memory_order_release);
			}
			else {
				constexpr auto _order = std::memory_order_acquire;
				static_assert(noexcept(_m_recursive._lock.test_and_set(_order)), "internal error - atomic_flag::test_and_set() not nothrow");
				while (_m_recursive._lock.test_and_set(_order)) {
					std::this_thread::yield();
				}
			}
		}
		base_spin_lock& acquire()noexcept
		{
			lock();
			return *this;
		}
		void unlock()noexcept 
		{
			if constexpr (recursive) {

				const auto _current_id = std::this_thread::get_id();

				// Ensure the current thread owns the lock before unlocking
				if (_m_recursive._owner_thread_id.load(std::memory_order_acquire) != _current_id) {
					// This indicates an error: unlocking a lock not owned by the current thread
					return;
				}

				// If the count is greater than 1, just decrement it
				if (_m_recursive._lock_count.load(std::memory_order_acquire) > 1) {
					_m_recursive._lock_count.fetch_sub(1, std::memory_order_release);
				}
				else {
					// If count is 1, release the lock and clear the owner
					_m_recursive._lock_count.store(0, std::memory_order_release);
					_m_recursive._owner_thread_id.store(std::thread::id(), std::memory_order_release);
				}
			}
			else {
				constexpr auto _order = std::memory_order_release;
				static_assert(noexcept(_m_recursive._lock.clear(_order)), "internal error - lock clear() it's not noexcept");
				_m_recursive._lock.clear(_order);
			}
		}
		base_spin_lock& release()noexcept
		{
			unlock();
			return *this;
		}
	private:
		typedef struct _recursive_true final {
			_recursive_true()noexcept = default;
			std::atomic<std::thread::id> _owner_thread_id = std::thread::id();
			std::atomic<int> _lock_count{};
		} _recursive_true_t;
		typedef struct _recursive_false final {
			_recursive_false()noexcept = default;
			std::atomic_flag _lock = ATOMIC_FLAG_INIT;
		} _recursive_false_t;
		using _recursive_t = std::conditional_t<_RECURSIVE,_recursive_true_t,_recursive_false_t>;
		_recursive_t _m_recursive;
	};

}

static_assert(iron::concepts::base_spin_lock<iron::memory::spin_lock>, "internal error");
static_assert(iron::concepts::spin_lock<iron::memory::spin_lock> && !iron::concepts::recursive_spin_lock<iron::memory::spin_lock>, "internal error");

static_assert(iron::concepts::base_spin_lock<iron::memory::recursive_spin_lock>, "internal error");
static_assert(!iron::concepts::recursive_spin_lock<iron::memory::spin_lock>&& iron::concepts::recursive_spin_lock<iron::memory::recursive_spin_lock>, "internal error");

