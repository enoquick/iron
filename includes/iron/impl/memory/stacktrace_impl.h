namespace iron::memory {


	class stacktrace_entry final {
		public:
			using line_number_t = std::uint_least32_t;
			using str_t = iron::ustrb;
			stacktrace_entry()noexcept = default;
			stacktrace_entry(str_t&& _file, line_number_t _line, str_t&& _func)noexcept {
				_construct(std::move(_file),_line,std::move(_func));
			}
			stacktrace_entry(stacktrace_entry&& _v)noexcept {
				IRON_SECURITY_CHECK_TEMPORARY(_v);
				_swap(_v);
				IRON_SECURITY_MARK_TEMPORARY(_v);
			}
			stacktrace_entry& operator=(stacktrace_entry&& _v)noexcept {
				IRON_SECURITY_CHECK_TEMPORARY(_v);
				_swap(_v);
				IRON_SECURITY_MARK_TEMPORARY(_v);
				return *this;
			}
			stacktrace_entry(const stacktrace_entry&)=delete;
			stacktrace_entry& operator=(const stacktrace_entry&)=delete;
			stacktrace_entry clone()const noexcept { //return empty stacktrace if error 
				IRON_SECURITY_CHECK_TEMPORARY_THIS();
				return _clone();
			}
			~stacktrace_entry() { if (_m_entry) _destroy(); }
			[[nodiscard]] explicit operator bool()const noexcept {
				IRON_SECURITY_CHECK_TEMPORARY_THIS();
				return _m_entry ? true : false;
			}
			void swap(stacktrace_entry& _v)noexcept {
				IRON_SECURITY_CHECK_TEMPORARY_THIS();
				IRON_SECURITY_CHECK_TEMPORARY(_v);
				_swap(_v);
			}
			[[nodiscard]] const iron::ref_wrapper<const str_t> file()const noexcept {
				IRON_SECURITY_CHECK_TEMPORARY_THIS();
				return _file();
			}
			[[nodiscard]] line_number_t line()const noexcept {
				IRON_SECURITY_CHECK_TEMPORARY_THIS();
				return _line();
			}
			[[nodiscard]] const iron::ref_wrapper<const str_t> func()const noexcept {
				IRON_SECURITY_CHECK_TEMPORARY_THIS();
				return _func();
			}
			IRON_SECURITY_METHOD()
		private:
			void _swap(stacktrace_entry& _v)noexcept {
				std::swap(_m_entry, _v._m_entry);
			}
			struct _struct_entry;
			using _entry_t=_struct_entry;
			_IRON_DLL_EXPORT void _construct(str_t&&,line_number_t, str_t&&)noexcept;
			_IRON_DLL_EXPORT void _destroy()noexcept;
			_IRON_DLL_EXPORT stacktrace_entry _clone()const noexcept;
			_IRON_DLL_EXPORT iron::ref_wrapper<const str_t> _file()const noexcept;
			_IRON_DLL_EXPORT line_number_t _line()const noexcept;
			_IRON_DLL_EXPORT iron::ref_wrapper<const str_t> _func()const noexcept;
			_entry_t* _m_entry{};
			IRON_SECURITY_DCL();
	};

	class stacktrace final {
		public:
			using entry_t=stacktrace_entry;
			using entries_t=iron::vector<entry_t>;
			using size_t = std::size_t;
			stacktrace(const size_t _n=std::numeric_limits<size_t>::max())noexcept {
				if (_n) {
					_construct(_n);
				}
			}
			stacktrace(stacktrace&& _v)noexcept {
				IRON_SECURITY_CHECK_TEMPORARY(_v);
				_swap(_v);
				IRON_SECURITY_MARK_TEMPORARY(_v);
			}
			stacktrace& operator=(stacktrace&& _v)noexcept {
				IRON_SECURITY_CHECK_TEMPORARY(_v);
				_swap(_v);
				IRON_SECURITY_MARK_TEMPORARY(_v);
				return *this;
			}
			stacktrace(const stacktrace&)=delete;
			stacktrace& operator=(const stacktrace&)=delete;
			~stacktrace() { if (_m_entries) _destroy();}
			[[nodiscard]]explicit operator bool()const noexcept {
				IRON_SECURITY_CHECK_TEMPORARY_THIS();
				return _m_entries ? true : false;
			}
			[[nodiscard]] const iron::ref_wrapper<const entries_t> entries()const noexcept {
				IRON_SECURITY_CHECK_TEMPORARY_THIS();
				return _entries();
			}
			stacktrace clone()noexcept {
				IRON_SECURITY_CHECK_TEMPORARY_THIS();
				return _clone();
			}
			IRON_SECURITY_METHOD()
		private:
			void _swap(stacktrace& _v)noexcept {
				std::swap(_m_disable_env, _v._m_disable_env);
				std::swap(_m_entries, _v._m_entries);
				IRON_SECURITY_SWAP(_v);
			}
			_IRON_DLL_EXPORT void _construct(size_t)noexcept;
			_IRON_DLL_EXPORT void _destroy()noexcept;
			[[nodiscard]] _IRON_DLL_EXPORT iron::ref_wrapper<const entries_t> _entries()const noexcept;
			[[nodiscard]] _IRON_DLL_EXPORT stacktrace _clone()const noexcept;
			[[nodiscard]] bool _disable_env()noexcept;
			[[nodiscard]] bool _enable_env()noexcept;
			bool _m_disable_env{};
			entries_t* _m_entries{};
			IRON_SECURITY_DCL();

	};

}
