

namespace iron {

	template <typename _T, typename _E>
	class result final {
	public:
		struct result_tag { result_tag() = delete; };
		using value_t=_T;
		using value_type = value_t;
		using err_t = _E;
		using err_type = err_t;
		using error_handler_t = iron::error_handler;
		using error_handler_type = error_handler_t;
		static_assert(!std::is_reference_v<value_t>, "result<T,E>: T must not be a reference");
		static_assert(!std::is_array_v<value_t>, "result<T,E>: T must not be an array type");
		static_assert(!std::is_const_v<value_t>, "result<T,E>: T must not be const");
		static_assert(std::is_nothrow_move_constructible_v<value_t>, "result<T,E>: T must be nothrow move constructible");
		static_assert(std::is_nothrow_destructible_v<value_t>, "result<T,E>: T must be nothrow destructible");
		static_assert(std::is_nothrow_swappable_v<value_t>, "result<T,E>: T must be nothrow swappable");
		static_assert(!std::is_reference_v<err_t>, "result<T,E>: E must not be a reference");
		static_assert(!std::is_array_v<err_t>, "result<T,E>: E must not be an array type");
		static_assert(!std::is_const_v<err_t>, "result<T,E>: E must not be const");
		static_assert(std::is_nothrow_move_constructible_v<err_t>, "result<T,E>: E must be nothrow move constructible");
		static_assert(std::is_nothrow_destructible_v<err_t>, "result<T,E>: E must be nothrow destructible");
		static_assert(std::is_nothrow_swappable_v<value_t>, "result<T,E>: E must be nothrow swappable");

	private:
		[[nodiscard]] static constexpr size_t _max()noexcept { return static_cast<std::size_t>(std::numeric_limits<std::ptrdiff_t>::max()); }
		template<typename _U, typename = void>
		struct _get_value_type {
			using _type = _U;
		};
		template<typename _U>
		struct _get_value_type<_U, std::void_t<typename _U::value_t>> {
			using _type = typename _U::value_t;
		};
		using _rval_t = std::conditional_t<
			iron::concepts::ref_wrapper<value_t>
			, typename _get_value_type<value_t>::_type
			, value_t
		>;
		using _rerr_t = std::conditional_t<
			iron::concepts::ref_wrapper<err_t>
			, typename _get_value_type<err_t>::_type
			, err_t
		>;

	public:
		using ref_value_t = _rval_t&;
		using value_reference = ref_value_t;
		using cref_value_t = const _rval_t&;
		using const_value_reference = cref_value_t;
		using safe_ref_value_t = iron::ref_wrapper<_rval_t>;
		using safe_value_reference = safe_ref_value_t;
		using safe_cref_value_t = iron::ref_wrapper<const _rval_t>;
		using safe_const_value_reference = safe_cref_value_t;
		using ref_err_t = _rerr_t&;
		using err_reference = ref_err_t;
		using cref_err_t = const _rerr_t&;
		using const_err_reference = cref_err_t;
		using safe_ref_err_t = iron::ref_wrapper<_rerr_t>;
		using safe_err_reference = safe_ref_err_t;
		using safe_cref_err_t = iron::ref_wrapper<const _rerr_t>;
		using safe_const_err_reference = safe_cref_err_t;
		[[nodiscard]] static constexpr size_t char_max_size()noexcept { return _max() / sizeof(char32_t); }
		[[nodiscard]] static constexpr result ok(value_t&& _v)noexcept { return result(std::forward<value_t>(_v)); } 
		[[nodiscard]] static constexpr result ok(const value_t& _v)noexcept requires std::is_nothrow_copy_constructible_v<value_t> { return result(_v);} 
		[[nodiscard]] static constexpr result err(err_t&& _v)noexcept { return result(std::forward<err_t>(_v),true); } 
		[[nodiscard]] static constexpr result err(const err_t& _v)noexcept requires std::is_nothrow_copy_constructible_v<err_t>  { return result(_v,true);}
		constexpr result()noexcept 
		requires std::is_nothrow_default_constructible_v<value_t>
		: _m_c(true)
		{
			_m_u._construct_v(value_t{});
		}
		constexpr result(result&& _v)noexcept
			: _m_c(_v._m_c)
		{
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			if (_m_c) IRON_LIKELY
			{
				_m_u._construct_v(std::move(_v._m_u._rel_v()));
			}
			else {
				_m_u._construct_e(std::move(_v._m_u._rel_e()));
			}
			IRON_SECURITY_MARK_TEMPORARY(_v);
		}
		constexpr result& operator=(result&& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_swap(_v);
			IRON_SECURITY_MARK_TEMPORARY(_v);
			return *this;
		}
		result& operator=(const result&) = delete;
		constexpr result(const result&) = delete;
		constexpr ~result() { _destroy(); }
		template <typename _U = value_t, std::enable_if_t<!std::is_same_v<std::decay_t<_U>, void*>, bool> = true>
		constexpr result(result<value_t, void*>&& _v)noexcept
			: _m_c(_v._m_c)
		{
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			PANIC_UNLESS(_m_c,"illegal construction");
			_m_u._construct_v(std::move(_v._m_u._rel_v()));
			IRON_SECURITY_MARK_TEMPORARY(_v);
		}
		template <typename _U = err_t, std::enable_if_t<!std::is_same_v<std::decay_t<_U>, void*>, bool> = true>
		constexpr result(result<void*,err_t>&& _v)noexcept
			: _m_c(_v._m_c)
		{
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			PANIC_IF(_m_c,"illegal construction");
			_m_u._construct_e(std::move(_v._m_u._rel_e()));
			IRON_SECURITY_MARK_TEMPORARY(_v);
		}
		[[nodiscard]] constexpr bool is_ok()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();;
			return _m_c;
		}
		[[nodiscard]] constexpr bool is_err()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();;
			return !_m_c;
		}
		[[nodiscard]] constexpr explicit operator bool()const noexcept { return is_ok(); }
		[[nodiscard]] constexpr const cref_value_t unchecked_unwrap()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();;
			DEBUG_ASSERT(is_ok());
			auto& _v= _m_u._get_v();
			if constexpr (iron::concepts::ref_wrapper<decltype(_v)>) {
				return _v.value();
			}
			else {
				return _v;
			}
		}
		[[nodiscard]] constexpr const cref_value_t unchecked_cunwrap()const noexcept {
			return unchecked_unwrap();
		}
		[[nodiscard]] constexpr safe_cref_value_t unwrap()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();;
			return _expect("result<T,E>::unwrap() failed");
		}
		[[nodiscard]] constexpr safe_cref_value_t cunwrap()const noexcept {
			return unwrap();
		}
		[[nodiscard]] constexpr ref_value_t unchecked_unwrap()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();;
			DEBUG_ASSERT(is_ok());
			auto& _v= _m_u._get_v();
			if constexpr (iron::concepts::ref_wrapper<decltype(_v)>) {
				return _v.value();
			}
			else {
				return _v;
			}
		}

		[[nodiscard]] constexpr safe_ref_value_t unwrap()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();;
			return _expect("result<T,E>::unwrap() failed");
		}

		[[nodiscard]] constexpr value_t unwrap_or_default()const noexcept 
		requires std::is_nothrow_default_constructible_v<value_t> && std::is_nothrow_copy_constructible_v<value_t>
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();;
			return _m_c ? _m_u._get_v() : value_t{};
		}
		[[nodiscard]] constexpr cref_err_t unchecked_unwrap_err()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();;
			DEBUG_ASSERT(is_err());
			auto& _v= _m_u._get_e();
			if constexpr (iron::concepts::ref_wrapper<decltype(_v)>) {
				return _v.value();
			}
			else {
				return _v;
			}
		}
		[[nodiscard]] constexpr cref_err_t unchecked_cunwrap_err()const noexcept {
			return unchecked_unwrap_err();
		}
		[[nodiscard]] constexpr safe_cref_err_t unwrap_err()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();;
			return _expect_err("result<T,E>::unwrap_err() failed");
		}
		[[nodiscard]] constexpr safe_cref_err_t cunwrap_err()const noexcept {
			return unwrap_err();
		}
		[[nodiscard]] constexpr ref_err_t unchecked_unwrap_err()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();;
			DEBUG_ASSERT(is_err());
			auto& _v= _m_u._get_e();
			if constexpr (iron::concepts::ref_wrapper<decltype(_v)>) {
				return _v.value();
			}
			else {
				return _v;
			}
		}
		[[nodiscard]] constexpr safe_ref_err_t unwrap_err()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();;
			return _expect_err("result<T,E>::unwrap_err() failed");
		}

		[[nodiscard]] constexpr err_t unwrap_err_or_default()const noexcept
		requires (std::is_nothrow_default_constructible_v<err_t> && std::is_nothrow_copy_constructible_v<err_t>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();;
			return _m_c ? err_t{} : _m_u._get_e();
		}

		template <typename _M,std::size_t _N>
		[[nodiscard]] constexpr safe_cref_value_t expect(const _M(&_m)[_N])const noexcept 
		requires ((std::is_same_v<_M, char> || std::is_same_v<_M, char32_t>) && _N < char_max_size())
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _expect(_m);
		}
		template <typename _M, std::size_t _N>
		[[nodiscard]] constexpr safe_cref_value_t cexpect(const _M(&_m)[_N])const noexcept
			requires ((std::is_same_v<_M, char> || std::is_same_v<_M, char32_t>) && _N < char_max_size())
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _expect(_m);
		}
		template <typename _M, std::size_t _N>
		[[nodiscard]] constexpr safe_ref_value_t expect(const _M(&_m)[_N])noexcept
		requires ((std::is_same_v<_M, char> || std::is_same_v<_M, char32_t>) && _N < char_max_size())
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _expect(_m);
		}

		template <typename _M, std::size_t _N>
		[[nodiscard]] constexpr safe_cref_err_t expect_err(const _M(&_m)[_N])const noexcept
		requires ((std::is_same_v<_M, char> || std::is_same_v<_M, char32_t>) && _N < char_max_size())
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();;
			return _expect_err(_m);
		}
		template <typename _M, std::size_t _N>
		[[nodiscard]] constexpr safe_cref_err_t cexpect_err(const _M(&_m)[_N])const noexcept
			requires ((std::is_same_v<_M, char> || std::is_same_v<_M, char32_t>) && _N < char_max_size())
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();;
			return _expect_err(_m);
		}
		template <typename _M, std::size_t _N>
		[[nodiscard]] constexpr safe_ref_err_t expect_err(const _M(&_m)[_N])noexcept
		requires ((std::is_same_v<_M, char> || std::is_same_v<_M, char32_t>) && _N < char_max_size())
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();;
			return _expect_err(_m);
		}
		constexpr result& swap(result& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_swap(_v);
			return *this;
		}
		IRON_SECURITY_METHOD()
	private:
		constexpr explicit result(value_t&& _v)noexcept
		: _m_c(true)
		{
			_m_u._construct_v(std::move(_v));
		}
		constexpr explicit result(const value_t& _v)noexcept
		: _m_c(true)
		{
			_m_u._construct_v(_v);
		}
		constexpr explicit result(err_t&& _v,bool)noexcept //bool is a dummy arg for distinguisc the case T is same type of E
		: _m_c()
		{
			_m_u._construct_e(std::move(_v));
		}
		constexpr explicit result(const err_t& _v,bool)noexcept //bool is a dummy arg for distinguisc the case T is same type of E
		: _m_c()
		{
			_m_u._construct_e(_v);
		}

		static constexpr void _swap_eq(result& _v0, result& _v1)noexcept {
			DEBUG_ASSERT(_v0._m_c == _v1._m_c);
			if (_v0._m_c) IRON_LIKELY
			{
				std::swap(_v0._m_u._get_v(), _v1._m_u._get_v());
			}
			else {
				std::swap(_v0._m_u._get_e(), _v1._m_u._get_e());
			}
		}
		static constexpr void _swap_ne(result& _v0, result& _v1)noexcept {
			DEBUG_ASSERT(_v0._m_c != _v1._m_c);
			if (_v0._m_c) IRON_LIKELY
			{
				auto _v = _v0._m_u._rel_v();
				auto _e = _v1._m_u._rel_e();
				_v0._m_u._destroy_v();
				_v1._m_u._destroy_e();
				_v0._m_u._construct_e(std::move(_e));
				_v1._m_u._construct_v(std::move(_v));
				std::swap(_v0._m_c, _v1._m_c);
			}
			else {
				_swap_ne(_v1, _v0);
			}
		}
		constexpr void _swap(result& _v)noexcept {
			if (_m_c == _v._m_c) IRON_LIKELY
			{
				_swap_eq(*this, _v);
			}
			else {
				_swap_ne(*this, _v);
			}
			IRON_SECURITY_SWAP(_v);
		}
		constexpr void _destroy()noexcept {
			if (_m_c) IRON_LIKELY
			{
				_m_u._destroy_v();
			}
			else {
				_m_u._destroy_e();
			}
		}
		template <typename _U>
		[[nodiscard]] constexpr auto _make_ref(_U& _v)noexcept 
		{
			return iron::make_ref(*this, _v);
		}
		template <typename _U>
		[[nodiscard]] constexpr auto _make_cref(const _U& _v)const noexcept {
			return iron::make_cref(*this, _v);
		}
		template <typename _M>
		[[nodiscard]] constexpr safe_cref_value_t _expect(const _M* _m)const noexcept {
			if (is_ok()) IRON_LIKELY
			{
				return _make_cref(_m_u._get_v());
			}
			PANIC(_m);
		}
		template <typename _M>
		[[nodiscard]] constexpr safe_ref_value_t _expect(const _M* _m)noexcept {
			if (is_ok()) IRON_LIKELY
			{
				return _make_ref(_m_u._get_v());
			}
			PANIC(_m);
		}
		template <typename _M>
		[[nodiscard]] constexpr safe_cref_err_t _expect_err(const _M* _m)const noexcept {
			if (is_err()) IRON_LIKELY
			{
				return _make_cref(_m_u._get_e());
			}
			PANIC(_m);
		}
		template <typename _M>
		[[nodiscard]] constexpr safe_ref_err_t _expect_err(const _M* _m)noexcept {
			if (is_err()) IRON_LIKELY
			{
				return _make_ref(_m_u._get_e());
			}
			PANIC(_m);
		}
		typedef union _u {
		public:
			_u(_u&& _u) = delete;
			_u(const _u&) = delete;
			_u& operator=(const _u&) = delete;
			_u& operator=(_u&&) = delete;
			constexpr ~_u() {}
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:26495)
#endif
			constexpr _u()noexcept {}
#if defined(_MSC_VER)
#pragma warning(pop)
#endif

			constexpr void _construct_v(value_t&& _v)noexcept {
				std::construct_at(&_m_v);
				_m_v.construct(std::forward<value_t>(_v));
			}
			constexpr void _construct_v(const value_t& _v)noexcept {
				std::construct_at(&_m_v);
				_m_v.construct(_v);
			}
			constexpr void _construct_e(err_t&& _v)noexcept {
				std::construct_at(&_m_e);
				_m_e.construct(std::move(_v));
			}
			constexpr void _construct_e(const err_t& _v)noexcept {
				std::construct_at(&_m_e);
				_m_e.construct(_v);
			}
			[[nodiscard]] constexpr value_t& _get_v()noexcept { return _m_v.value(); }
			[[nodiscard]] constexpr const value_t& _get_v()const noexcept { return _m_v.value(); }
			[[nodiscard]] constexpr err_t& _get_e()noexcept { return _m_e.value(); }
			[[nodiscard]] constexpr const err_t& _get_e()const noexcept { return _m_e.value(); }
			[[nodiscard]] constexpr value_t _rel_v()noexcept { return _m_v.release(); }
			[[nodiscard]] constexpr err_t _rel_e()noexcept { return _m_e.release();}
			constexpr void _destroy_v()noexcept { _m_v.destroy(); }
			constexpr void _destroy_e()noexcept { _m_e.destroy(); }
		private:
			using _rwv_t = iron::unsafe::raw_wrapper<value_t>;
			using _rwe_t = iron::unsafe::raw_wrapper<err_t>;
			_rwv_t _m_v;
			_rwe_t _m_e;
		} _u_t;
		template <typename _T0,typename _E0>
		friend class result;
		bool _m_c;
		_u_t _m_u{};
		IRON_SECURITY_DCL();
	};

	template <typename _T, typename _E>
	inline constexpr auto ok(_T&& _v)noexcept {
		return result<std::decay_t<_T>,_E>::ok(std::forward<_T>(_v));
	}

	template <typename _E, typename _T>
	inline constexpr auto err(_E&& _v)noexcept {
		return result<_T, std::decay_t<_E>>::err(std::forward<_E>(_v));
	}
}

static_assert(iron::concepts::result<iron::result<int, int>>, "internal error");


