
namespace iron::unsafe::errors {

	class os  final : public base_with_info {
	public:
		using code_t = std::int32_t;
		static constexpr code_t code_ok = {};
		os()noexcept= default;
		explicit os(code_t _code, const char* _file, line_number_t _line, const char* _func)noexcept
		: base_with_info(_file, _line, _func)
		, _m_numeric_code(_code)
		{
			DEBUG_ASSERT(_code);
		}
		~os() = default;
		os(os&& _v)noexcept
		: base_with_info()
		{
			_swap(_v);
		}
		os(const os& _v)=delete;
		os& operator=(const os&)=delete;
		os& operator=(os&& _v)noexcept {
			_swap(_v);
			return *this;
		}
		void swap(os& _v)noexcept {
			_swap(_v);
		}
		[[nodiscard]] code_t code()const noexcept {
			return _m_numeric_code;
		}
	protected:
		[[nodiscard]] bool virtual_is()const noexcept override { return !_m_numeric_code;}
		[[nodiscard]] const char* virtual_code()const noexcept override { return _code(*this); }
		[[nodiscard]] const char8_t* virtual_description()const noexcept override { return _description(*this); }
		[[nodiscard]]base* const virtual_clone()const noexcept override { 
			auto _p = iron::unsafe::allocator<os>{}.allocate(1);
			return _clone(_p,*this); 
		}
	private:
//		[[nodiscard]] _IRON_DLL_EXPORT static  bool _is(const os&)noexcept;
		[[nodiscard]] _IRON_DLL_EXPORT static const char* _code(const os&)noexcept;
		[[nodiscard]] _IRON_DLL_EXPORT static const char8_t* _description(const os&)noexcept;
		[[nodiscard]] _IRON_DLL_EXPORT static base* const _clone(os*,const os&)noexcept;
		void _swap(os& _v)noexcept {
			base_with_info::swap(_v);
			_swap2(*this, _v);
		}
		_IRON_DLL_EXPORT static void _swap2(os&, os&)noexcept;
#if 0
		{
			base_with_info::swap(_v);
			std::swap(_m_numeric_code, _v._m_numeric_code);
			std::swap(_m_arr_code, _v._m_arr_code);
			std::swap(_m_description, _v._m_description);
		}
#endif
		code_t _m_numeric_code{};
		mutable char   _m_arr_code[max_code_sz]{};
		mutable char8_t _m_description[max_path_sz]{};
	};


}
