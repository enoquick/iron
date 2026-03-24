
namespace iron::unsafe::errors {

	 class base {
		public:
			using line_number_t = std::uint_least32_t;
			using size_t = std::size_t;
			static constexpr size_t max_code_sz = 20;
			static constexpr size_t max_path_sz = 256;
			virtual ~base() = default;
			base(const base&) = delete;
			base& operator=(const base&) = delete;
			[[nodiscard]] const char* code()const noexcept {
				return virtual_code();
			}
			[[nodiscard]] const char8_t* description()const noexcept {
				return virtual_description();
			}
			[[nodiscard]] base* const clone()const noexcept {
				return virtual_clone();
			}
			[[nodiscard]] explicit operator bool()const noexcept {
				return virtual_is();
			}
			[[nodiscard]] const char* file()const noexcept {
				return virtual_file();
			}
			[[nodiscard]] line_number_t line()const noexcept {
				return virtual_line();
			}
			[[nodiscard]] const char* func()const noexcept {
				return virtual_func();
			}
		protected:
			base()noexcept = default;
			base(base&& _v)noexcept
			{
				_swap(_v);
			}
			base& operator=(base&& _v)noexcept {
				_swap(_v);
				return *this;
			}
			void swap(base& _v)noexcept {
				_swap(_v);
			}
			[[nodiscard]] virtual const char* virtual_code()const noexcept { return ""; }
			[[nodiscard]] virtual const char8_t* virtual_description()const noexcept { return u8""; }
			[[nodiscard]] virtual base* const virtual_clone()const noexcept { return {}; }
			[[nodiscard]] virtual bool virtual_is()const noexcept=0;
			[[nodiscard]] virtual const char* virtual_file()const noexcept { return ""; }
			[[nodiscard]] virtual line_number_t virtual_line()const noexcept { return {}; }
			[[nodiscard]] virtual const char* virtual_func()const noexcept { return ""; }
		private:
			void _swap(base& _v)noexcept {
			}
	};

}
