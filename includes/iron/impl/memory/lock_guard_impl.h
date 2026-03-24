namespace iron::memory {

	template <typename _T>
	class lock_guard {
	public:
		struct lock_guard_tag { lock_guard_tag() = delete; };
		using value_t = _T;
		using value_type = value_t;
		static_assert(iron::concepts::lock<value_t>, "template parameter value is not a lock");
		explicit lock_guard(value_t& _lock)noexcept
		: _m_lock(_lock)
		{
			_m_lock.lock();
		}
		lock_guard(const lock_guard&)=delete;
		lock_guard& operator=(const lock_guard&)=delete;
		lock_guard(lock_guard&&)=delete;
		lock_guard& operator=(lock_guard&&)=delete;
		~lock_guard()
		{
			_m_lock.unlock();
		}
	private:
		value_t& _m_lock;
	};

	template <typename _T>
	[[nodiscard]] inline lock_guard<_T> make_lock_guard(_T& _lock)noexcept {
		return lock_guard<_T>(_lock);
	}
} //ns
