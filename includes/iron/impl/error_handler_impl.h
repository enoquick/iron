#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable:26409)
#pragma warning (disable:26429)
#pragma warning (disable:26446)
#pragma warning (disable:26482)
#endif

namespace iron {

	class error_handler final {
		public:
			using line_number_t = iron::unsafe::errors::base_with_info::line_number_t;
			using stacktrace_t = iron::memory::stacktrace;
			using size_t = std::size_t;
			constexpr error_handler()noexcept=default;
			using base_t = iron::unsafe::errors::base;
			static inline const char* stacktrace_enable_env = "IRON_STACKTRACE";
			static constexpr auto max_count = std::numeric_limits<size_t>::max();
			struct error_handler_tag { error_handler_tag() = delete; };
			typedef enum class enum_error_type {
				no_error
				,posix
				, os
				, exception
				, user
			} error_type_t;
			template <typename _T>
			error_handler(_T&& _v)noexcept
			requires (std::is_base_of_v<iron::unsafe::errors::base, _T>)
			{
				if (auto _p_err = _data::_allocator_t<_T>{}.allocate(1)) IRON_LIKELY
				{
					std::construct_at(_p_err, std::move(_v));
					_m_data=_data::_make(_p_err,sizeof(_T));
				}
				else {
					_m_data = _data::_make(nullptr,0);
				}
			}
			[[nodiscard]] static error_handler make(iron::unsafe::errors::posix::code_t _code, const char* _file,line_number_t _line, const char* _func)noexcept {
				if (_code == iron::unsafe::errors::posix::code_ok) IRON_UNLIKELY
				{
					return {};
				}
				return error_handler(iron::unsafe::errors::posix(_code, _file, _line, _func));
			}
			[[nodiscard]] static error_handler make(iron::unsafe::errors::os::code_t _code, const char* _file,line_number_t _line, const char* _func)noexcept {
				if (!_code) IRON_UNLIKELY
				{
					return {};
				}
#ifdef __unix__
				using _posix_t = iron::unsafe::errors::posix;
				_posix_t::code_t _posix_code;
				if (_posix_t::make_code(_posix_code, _code)) IRON_LIKELY 
				{
					return error_handler(_posix_t(_posix_code, _file, _line, _func));
				}
#endif
				return error_handler(iron::unsafe::errors::os(_code, _file, _line, _func));
			}
			[[nodiscard]] static error_handler make(iron::unsafe::errors::exception::exception_ptr_t&& _code, const char* _file,line_number_t _line, const char* _func)noexcept {
				iron::unsafe::errors::exception _e(std::move(_code), _file, _line, _func);
				const auto _posix_code = _e.to_posix();
				if (_posix_code != iron::unsafe::errors::posix::code_ok)
				{
					return error_handler(iron::unsafe::errors::posix(_posix_code, _file, _line, _func));
				}
				const auto _os_code = _e.to_os();
				if (_os_code) IRON_UNLIKELY
				{
					return make(_os_code,_file,_line,_func);
				}
				return error_handler(std::move(_e));
			}
			constexpr error_handler(error_handler&& _v)noexcept {
				IRON_SECURITY_CHECK_TEMPORARY(_v);
				_swap(_v);
				IRON_SECURITY_MARK_TEMPORARY(_v);
			}
			constexpr error_handler& operator=(error_handler&& _v)noexcept {
				IRON_SECURITY_CHECK_TEMPORARY(_v);
				_swap(_v);
				IRON_SECURITY_MARK_TEMPORARY(_v);
				return *this;
			}
			constexpr error_handler(const error_handler& _v)noexcept 
			: _m_data(_v._m_data)
			{
				IRON_SECURITY_CHECK_TEMPORARY(_v);
				if (_m_data) IRON_UNLIKELY
				{
					_m_data->_inc_shared_counter();
				}
			}
			error_handler& operator=(const error_handler& _v)noexcept {
				IRON_SECURITY_CHECK_TEMPORARY(_v);
				error_handler _t(_v);
				_swap(_t);
				return *this;
			}
			constexpr ~error_handler() {
				if (_m_data) IRON_UNLIKELY
				{
					_data::_destroy(_m_data);
				}
			}
			[[nodiscard]] constexpr explicit operator bool()const noexcept {
				IRON_SECURITY_CHECK_TEMPORARY_THIS();
				return !_m_data;
			}
			constexpr error_handler& swap(error_handler& _v)noexcept {
				IRON_SECURITY_CHECK_TEMPORARY_THIS();
				IRON_SECURITY_CHECK_TEMPORARY(_v);
				_swap(_v);
				return *this;
			}
			[[nodiscard]] const base_t* unsafe_error()const noexcept {
				IRON_SECURITY_CHECK_TEMPORARY_THIS();
				if (_m_data) IRON_LIKELY
				{
					auto& _data = _get_data();
					return _data._error();
				}
				return {};
			}

			[[nodiscard]] error_type_t error_type()const noexcept {
				if (auto base = unsafe_error()) IRON_LIKELY
				{
					using _t = error_type_t;
					if (dynamic_cast<const iron::unsafe::errors::posix*>(base)) IRON_LIKELY
					{
						return _t::posix;
					}
					if (dynamic_cast<const iron::unsafe::errors::os*>(base)) IRON_LIKELY
					{
						return _t::os;
					}
					if (dynamic_cast<const iron::unsafe::errors::exception*>(base)) IRON_LIKELY
					{
						return _t::exception;
					}
					return _t::user;
				}
				return {};
			}
			[[nodiscard]] const char* unsafe_code()const noexcept {
				IRON_SECURITY_CHECK_TEMPORARY_THIS();
				auto _e = _get_data()._error();
				DEBUG_ASSERT(_e);
				return _e->code();
			}
			[[nodiscard]] const char8_t* unsafe_description()const noexcept {
				IRON_SECURITY_CHECK_TEMPORARY_THIS();
				auto _e = _get_data()._error();
				DEBUG_ASSERT(_e);
				return _e->description();
			}
			[[nodiscard]] const char* unsafe_file()const noexcept {
				IRON_SECURITY_CHECK_TEMPORARY_THIS();
				auto _e = _get_data()._error();
				DEBUG_ASSERT(_e);
				return _e->file();
			}
			[[nodiscard]] line_number_t line()const noexcept {
				IRON_SECURITY_CHECK_TEMPORARY_THIS();
				auto _e = _get_data()._error();
				DEBUG_ASSERT(_e);
				return _e->line();
			}
			[[nodiscard]] const char* unsafe_func()const noexcept {
				IRON_SECURITY_CHECK_TEMPORARY_THIS();
				auto _e = _get_data()._error();
				DEBUG_ASSERT(_e);
				return _e->func();
			}
			[[nodiscard]] constexpr bool has_stacktrace()const noexcept {
				IRON_SECURITY_CHECK_TEMPORARY_THIS();
				return _m_data && _m_data->_has_stacktrace();
			}
			[[nodiscard]] const stacktrace_t& unsafe_stacktrace()const noexcept {
				IRON_SECURITY_CHECK_TEMPORARY_THIS();
				auto& _data = _get_data();
				return _data._stackstrace();
			}
			[[noreturn]] void do_panic()const noexcept {
				IRON_SECURITY_CHECK_TEMPORARY_THIS();
				iron::unsafe::asserts::do_panic(nullptr,nullptr,0,*this);
			}
			[[nodiscard]] iron::unsafe::errors::os::code_t numeric_error()const noexcept {
				IRON_SECURITY_CHECK_TEMPORARY_THIS();
				return _get_data()._numeric_error();
			}
			IRON_SECURITY_METHOD();
		private:
			constexpr void _swap(error_handler& _v)noexcept {
				std::swap(_m_data, _v._m_data);
				IRON_SECURITY_SWAP(_v);
			}
			class _data final  {
			public:
				template <typename _U>
				using _allocator_t = iron::unsafe::allocator<_U>;
				_data()noexcept = default;
				[[nodiscard]] _IRON_DLL_EXPORT static _data* _make(base_t*,size_t)noexcept;
				_IRON_DLL_EXPORT static void _destroy(_data*)noexcept;
				[[nodiscard]] _IRON_DLL_EXPORT static const _data& _get_data(const error_handler&)noexcept;
				_data(const _data&) = delete;
				_data(_data&&) = delete;
				_data& operator=(const _data&) = delete;
				_data& operator=(_data&&)=delete;
				~_data();
				[[nodiscard]] bool _has_stacktrace()const noexcept { return !_m_stacktrace_static; }
				[[nodiscard]] const stacktrace_t& _stackstrace()const noexcept {
					DEBUG_ASSERT(_m_stacktrace);
					return *_m_stacktrace;
				}
				[[nodiscard]] const base_t* _error()const noexcept {
					return _m_base_error;
				}
				[[nodiscard]] iron::unsafe::errors::os::code_t _numeric_error()const noexcept {
					return _numeric_error_s(*this);
				}
				void _inc_shared_counter()noexcept {
					_inc_shared_counter_s(*this);
				}
			private:
				static void _make_stacktrace(_data&)noexcept;
				[[nodiscard]] static _data* _make_no_such_memory()noexcept;
				[[nodiscard]] static const _data& _make_no_err()noexcept;
				[[nodiscard]] _IRON_DLL_EXPORT static iron::unsafe::errors::os::code_t _numeric_error_s(const _data&)noexcept;
				_IRON_DLL_EXPORT static void _inc_shared_counter_s(_data&)noexcept;
				static void _inline_lock()noexcept;
				static void _inline_unlock()noexcept;
				base_t* _m_base_error{};
				size_t _m_size{};
				stacktrace_t* _m_stacktrace{};
				size_t _m_shared_counter{};
				bool _m_error_static{};
				bool _m_stacktrace_static{};
				bool _m_data_static{};
			};
			[[nodiscard]] const _data& _get_data()const noexcept {
				return _data::_get_data(*this);
			}
			template <typename _U>
			[[nodiscard]] iron::ref_wrapper<const _U> _make_ref(const _U& _v)const noexcept {
				return iron::make_cref(*this,_v);
			}
			_data* _m_data{};
			IRON_SECURITY_DCL();
	};


}

static_assert(iron::concepts::error_handler<iron::error_handler>, "internal error");

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

