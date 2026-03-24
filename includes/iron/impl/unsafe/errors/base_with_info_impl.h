
namespace iron::unsafe::errors {

	class base_with_info : public base {
	public:
		~base_with_info() = default;
		base_with_info(const base_with_info&) = delete;
		base_with_info& operator=(const base_with_info&) = delete;
	protected:
		base_with_info()noexcept = default;
		_IRON_DLL_EXPORT base_with_info(const char* _file, line_number_t _line, const char* _func)noexcept;
		base_with_info(base_with_info&& _v)noexcept : base()
		{
			_swap(_v);
		}
		base_with_info& operator=(base_with_info&& _v)noexcept {
			_swap(_v);
			return *this;
		}
		void swap(base_with_info& _v)noexcept {
			_swap(_v);
		}
		[[nodiscard]] const char* virtual_file()const noexcept override { return &_m_file[0]; }
		[[nodiscard]] line_number_t virtual_line()const noexcept override { return _m_line; }
		[[nodiscard]] const char* virtual_func()const noexcept override { return &_m_func[0]; }
	private:
		friend class iron::error_handler;
		void _swap(base_with_info& _v)noexcept {
			base::swap(_v);
			std::swap(_m_file, _v._m_file);
			std::swap(_m_line, _v._m_line);
			std::swap(_m_func, _v._m_func);
		}
		static void _copy(char*,const char*,std::size_t)noexcept;
		char _m_file[max_path_sz]{};
		line_number_t _m_line{};
		char _m_func[max_path_sz]{};
	};


}
