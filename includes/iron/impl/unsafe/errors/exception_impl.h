
namespace iron::unsafe::errors {

	class exception final : public base_with_info {
	public:
		using exception_ptr_t = std::exception_ptr;
		exception()noexcept : base_with_info() {}
		_IRON_DLL_EXPORT explicit exception(exception_ptr_t&& _e, const char* _file, line_number_t _line, const char* _func)noexcept;
		~exception() {
			if (_m_set) IRON_LIKELY
			{
				_destroy(*this);
			}
		}
		exception(const exception& _v) = delete;
		exception& operator=(const exception&) = delete;

		exception(exception&& _v)noexcept
		: base_with_info()
		{
			_swap(_v);
		}
		exception& operator=(exception&& _v)noexcept {
			_swap(_v);
			return *this;
		}
		void swap(exception& _v)noexcept {
			_swap(_v);
		}
		[[nodiscard]] const exception_ptr_t& current_exception()const noexcept {
			PANIC_IF(*this, "error not set");
			return _current_exception(*this);
		}
		void rethrow_exception()const noexcept(false) {
			auto& e = current_exception();
			std::rethrow_exception(e);
		}
		[[nodiscard]] posix::code_t to_posix()const noexcept {
			return _to_posix(*this);
		}
		[[nodiscard]] os::code_t to_os()const noexcept {
			return _to_os(*this);
		}
	protected:
		[[nodiscard]] bool virtual_is()const noexcept override { return !_m_set; }
		[[nodiscard]] const char* virtual_code()const noexcept override { return ""; }
		[[nodiscard]] const char8_t* virtual_description()const noexcept override { return _description(*this); }
		[[nodiscard]] base* const virtual_clone()const noexcept override { 
			auto _p = iron::unsafe::allocator<exception>{}.allocate(1);
			return _clone(_p,*this); 
		}
	private:
		[[nodiscard]] _IRON_DLL_EXPORT static const exception_ptr_t& _current_exception(const exception&)noexcept;
		[[nodiscard]] _IRON_DLL_EXPORT static const char8_t* _description(const exception&)noexcept;
		[[nodiscard]] _IRON_DLL_EXPORT static base* const _clone(exception*,const exception&)noexcept;
		[[nodiscard]] _IRON_DLL_EXPORT static posix::code_t _to_posix(const exception&)noexcept;
		[[nodiscard]] _IRON_DLL_EXPORT static os::code_t _to_os(const exception&)noexcept;
		void _construct(exception_ptr_t&& _v)noexcept;
		_IRON_DLL_EXPORT static void _destroy(exception&)noexcept;
		void _swap(exception& _v)noexcept {
			base_with_info::swap(_v);
			_swap2(*this, _v);
		}
		_IRON_DLL_EXPORT  static void _swap2(exception&,exception&)noexcept;
		bool _m_set{};
		unsigned char _m_excp[sizeof(exception_ptr_t)]{};
		mutable char8_t _m_description[255]{};
	};


}
