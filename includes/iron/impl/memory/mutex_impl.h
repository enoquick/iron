
namespace iron::memory {

	template <bool _RECURSIVE>
	class base_mutex final {
	public:
		struct mutex_tag { mutex_tag() = delete; };
		using error_handler_t=iron::error_handler;
		static constexpr bool recursive = _RECURSIVE;
		base_mutex()noexcept { _init(& _m_area);}
		base_mutex(const base_mutex&)=delete;
		base_mutex& operator=(const base_mutex&)=delete;
		base_mutex(base_mutex&&) = delete;
		base_mutex& operator=(base_mutex&&) = delete;
		~base_mutex() { _destroy(&_m_area); }
		[[nodiscard]] const void* native_handle()const noexcept {
			return _native_handle(&_m_area);
		}
		[[nodiscard]] error_handler_t try_acquire()noexcept {
			return _acquire(&_m_area);
		}
		[[nodiscard]] result<bool, error_handler_t> try_can_acquired()noexcept {
			return _can_acquired(&_m_area);
		}
		[[nodiscard]] error_handler_t try_release()noexcept {
			return _release(&_m_area);
		}
		base_mutex& acquire()noexcept {
			auto _h = try_acquire();
			if (_h) return *this;
			_h.do_panic();
		}
		[[nodiscard]] bool can_acquired()noexcept {
			auto _r = try_can_acquired(); RP(_r);
		}
		base_mutex& release()noexcept {
			auto _h = try_release(); RVP(_h, *this);
		}
		void lock()noexcept { //for std::lock_guard
			acquire();
		}
		void unlock()noexcept { //for std::lock_guard
			release();
		}
	private:
		using _stack_area_t = iron::_internal::_stack_area<50>;
		_IRON_DLL_EXPORT static void _init(void*)noexcept;
		[[nodiscard]] _IRON_DLL_EXPORT static  error_handler_t _acquire(void*)noexcept;
		[[nodiscard]] _IRON_DLL_EXPORT static  result<bool,error_handler_t> _can_acquired(void*)noexcept;
		[[nodiscard]] _IRON_DLL_EXPORT static  error_handler_t _release(void*)noexcept;
		_IRON_DLL_EXPORT static void _destroy(void*)noexcept;
		[[nodiscard]] _IRON_DLL_EXPORT static const void* _native_handle(const void*)noexcept;
		_stack_area_t _m_area;
	};

	template <>
	void base_mutex<false>::_init(void*)noexcept;

	template <>
	void base_mutex<true>::_init(void*)noexcept;

	template <>
	auto base_mutex<false>::_acquire(void*)noexcept -> error_handler_t;

	template <>
	auto base_mutex<true>::_acquire(void*)noexcept -> error_handler_t;

	template <>
	auto base_mutex<false>::_can_acquired(void*)noexcept -> result<bool, error_handler_t>;

	template <>
	auto base_mutex<true>::_can_acquired(void*)noexcept -> result<bool, error_handler_t>;

	template <>
	auto base_mutex<false>::_release(void*)noexcept -> error_handler_t;

	template <>
	auto base_mutex<true>::_release(void*)noexcept -> error_handler_t;

	template <>
	void base_mutex<false>::_destroy(void*)noexcept;

	template <>
	void base_mutex<true>::_destroy(void*)noexcept;

	template <>
	const void* base_mutex<false>::_native_handle(const void*)noexcept;

	template <>
	const void* base_mutex<true>::_native_handle(const void*)noexcept;


} //ns

	static_assert(iron::concepts::base_mutex<iron::memory::mutex>, "internal error");
	static_assert(iron::concepts::mutex<iron::memory::mutex> && !iron::concepts::recursive_mutex<iron::memory::mutex>, "internal error");

	static_assert(iron::concepts::base_mutex<iron::memory::recursive_mutex>, "internal error");
	static_assert(!iron::concepts::mutex<iron::memory::recursive_mutex> && iron::concepts::recursive_mutex<iron::memory::recursive_mutex>, "internal error");




