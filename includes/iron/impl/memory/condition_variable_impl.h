

namespace iron::memory {

	template <typename _MUTEX>
	class base_condition_variable final {
	public:
		using mutex_t=_MUTEX;
		using error_handler_t=iron::error_handler;
		using lock_t = std::unique_lock<mutex_t>;
		struct condition_variable_tag { condition_variable_tag() = delete; };
		template<typename _F>
		[[nodiscard]] static consteval bool is_bool_predicate()noexcept { return std::is_nothrow_invocable_r_v<bool, _F&>; }
	private:
		static constexpr bool _is_std_mutex=std::is_same_v<mutex_t, std::mutex>;
	public:
		base_condition_variable()noexcept
		requires (_is_std_mutex)
		: _m_p() {}
		base_condition_variable()noexcept 
		requires(!_is_std_mutex) { _init(&_m_p); }
		base_condition_variable(const base_condition_variable&)=delete;
		base_condition_variable& operator=(const base_condition_variable&)=delete;
		base_condition_variable(base_condition_variable&&)=delete;
		base_condition_variable& operator=(base_condition_variable&&)=delete;
		~base_condition_variable() {
			if constexpr (!_is_std_mutex) {
				_destroy(&_m_p);
			}
		}
		void notify_one()noexcept {
			if constexpr (_is_std_mutex) {
				_m_p.notify_one();
			}
			else {
				_notify_one(&_m_p);
			}
		}
		void notify_all()noexcept {
			if constexpr (_is_std_mutex) {
				_m_p.notify_all();
			}
			else {
				_notify_all(&_m_p);
			}
		}
		[[nodiscard]] error_handler_t try_wait(lock_t& _lock)noexcept {
			if constexpr (_is_std_mutex) {
				try {
					_m_p.wait(_lock);
				}
				catch (...) {
					return ERROR_HANDLER(std::current_exception());
				}
				return {};
			}
			else {
				return _wait(&_m_p, _lock);
			}
		}
		base_condition_variable& wait(lock_t& _lock)noexcept {
			const auto _h = try_wait(_lock); RVP(_h, *this);
		}
		template<typename _PREDICATE>
		[[nodiscard]] error_handler_t try_wait(lock_t& _lock, _PREDICATE _pred)noexcept 
		requires(is_bool_predicate<_PREDICATE>())
		{
			error_handler_t _h;
			while (!_pred()) {
				_h=try_wait(_lock);
				if (!_h) IRON_UNLIKELY 
				{
					break;
				}
			}
			return _h;
		}
		template<typename _PREDICATE>
		base_condition_variable& wait(lock_t& lock,_PREDICATE pred )noexcept 
		requires(is_bool_predicate<_PREDICATE>())
		{
			const auto _h=try_wait(lock,pred); RVP(_h, *this);
		}
		[[nodiscard]] const auto native_handle()const noexcept 
		requires (_is_std_mutex)
		{ 
			return _m_p.native_handle(); 
		}
		[[nodiscard]] const void* native_handle()const noexcept 
		requires (!_is_std_mutex)
		{ 
			return _native_handle(&_m_p); 
		}

	private:
		using _stack_area_t = iron::_internal::_stack_area<50>;
		using _internal_t=std::conditional_t<_is_std_mutex, std::condition_variable,_stack_area_t>;
		_IRON_MODULES_DLL_EXPORT static void _init(void*)noexcept;
		_IRON_MODULES_DLL_EXPORT static void _destroy(void*)noexcept;
		_IRON_MODULES_DLL_EXPORT static void _notify_one(void*)noexcept;
		_IRON_MODULES_DLL_EXPORT static void _notify_all(void*)noexcept;
		_IRON_MODULES_DLL_EXPORT  [[nodiscard]] static error_handler_t _wait(void*,lock_t&)noexcept;
		_IRON_MODULES_DLL_EXPORT  [[nodiscard]] static const void* _native_handle(const void*)noexcept;
		_internal_t _m_p;
	};

	template <>
	_IRON_DLL_EXPORT void base_condition_variable<iron::memory::mutex>::_init(void*)noexcept;

	template <>
	_IRON_DLL_EXPORT void base_condition_variable<iron::memory::mutex>::_destroy(void*)noexcept;

	
	template <>
	_IRON_DLL_EXPORT void base_condition_variable<iron::memory::mutex>::_notify_one(void*)noexcept;
	
	template <>
	_IRON_DLL_EXPORT void base_condition_variable<iron::memory::mutex>::_notify_all(void*)noexcept;
	
	template <>
	_IRON_DLL_EXPORT auto base_condition_variable<iron::memory::mutex>::_wait(void*,lock_t&)noexcept->error_handler_t;
	
	template <>
	_IRON_DLL_EXPORT const void* base_condition_variable<iron::memory::mutex>::_native_handle(const void*)noexcept;


} //ns



