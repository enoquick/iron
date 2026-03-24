
namespace iron {

	template <typename _T>
	class ref_wrapper final {
	public:
		using value_t = _T;
		using value_type = value_t;
		struct ref_wrapper_tag { ref_wrapper_tag() = delete; };
		static_assert(std::is_object_v<value_t>, "ref_wrapper requires value_type to be an object type");
		static_assert(!iron::concepts::ref_wrapper<value_t>, "ref_wrapper value_type not to be a ref_wrapper");
		template <typename _MASTER>
		[[nodiscard]] static constexpr ref_wrapper from(const _MASTER& _master, value_t& _v)noexcept 
		IRON_SECURITY_REQ()
		{
			IRON_SECURITY_CHECK_TEMPORARY(_master);
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			return ref_wrapper(_master, _v);
		}
		constexpr ref_wrapper(const ref_wrapper& _v)noexcept
			: _m_p(_v._m_p)
		{
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			IRON_SECURITY_CHECK_MASTER_LIVE(_v);
			IRON_SECURITY_ASSIGN(_v);
		}
		constexpr ref_wrapper(ref_wrapper&& _v)noexcept
			: _m_p(_v._m_p)
		{
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			IRON_SECURITY_CHECK_MASTER_LIVE(_v);
			IRON_SECURITY_SWAP(_v);
			IRON_SECURITY_MARK_TEMPORARY(_v);
		}
		constexpr ref_wrapper& operator=(const ref_wrapper& _v)noexcept
		{
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			IRON_SECURITY_CHECK_MASTER_LIVE(_v);
#if IRON_SECURITY_CHECKER
			ref_wrapper _t(_v);
			_swap(_t);
#else 
			_m_p = _v._m_p;
#endif 
			return *this;
		}
		constexpr ref_wrapper& operator=(ref_wrapper&& _v)noexcept
		{
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			IRON_SECURITY_CHECK_MASTER_LIVE(_v);
#if IRON_SECURITY_CHECKER
			ref_wrapper _t(std::move(_v));
			_swap(_t);
#else 
			_m_p = _v._m_p;
#endif 
			return *this;
		}
		constexpr ~ref_wrapper() = default;
		[[nodiscard]] constexpr value_t* operator->()const noexcept 
		requires(std::is_class_v<value_t>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			return _m_p;
		}
		[[nodiscard]] constexpr value_t& operator*()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			return *_m_p;
		}
		[[nodiscard]] constexpr value_t& value()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			return *_m_p;
		}
		[[nodiscard]] constexpr operator value_t& ()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			return *_m_p;
		}
		constexpr ref_wrapper& swap(ref_wrapper& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			IRON_SECURITY_CHECK_MASTER_LIVE(_v);
			_swap(_v);
			return *this;
		}
		IRON_SECURITY_METHOD()
	private:
		template <typename _MASTER>
		constexpr ref_wrapper(const _MASTER& _master, value_t& _v)noexcept
		: _m_p(std::addressof(_v))
		{
			IRON_SECURITY_MAKE_REF(_master);
		}
		constexpr void _swap(ref_wrapper& _v)noexcept {
			std::swap(_m_p, _v._m_p);
			IRON_SECURITY_SWAP(_v);
		}
		value_t* _m_p;
		IRON_SECURITY_DCL();
	};

	template <typename _MASTER,typename _T>
	[[nodiscard]] inline constexpr ref_wrapper<_T> make_ref(const _MASTER& _master,_T& _v)noexcept
	requires(!iron::concepts::ref_wrapper<_T>)
	{
		return ref_wrapper<_T>::from(_master, _v);
	}

	template <typename _T>
	[[nodiscard]] inline constexpr auto make_ref(_T& _v)noexcept
	requires(iron::concepts::ref_wrapper<_T>)
	{
		auto& _t = *_v;
		using _t_t = std::remove_reference_t<decltype(_t)>;
		if constexpr (iron::concepts::ref_wrapper<_t_t>) {
			return make_ref(_t);
		}
		else {
			return ref_wrapper<_t_t>::from(_v, _t);
		}
	}
	template <typename _MASTER,typename _T>
	[[nodiscard]] inline constexpr auto make_ref(const _MASTER&,_T& _v)noexcept
	requires(iron::concepts::ref_wrapper<_T>)
	{
		return make_ref(_v);
	}

	template <typename _MASTER, typename _T>
	[[nodiscard]] inline constexpr ref_wrapper<const _T> make_cref(const _MASTER& _master, const _T& _v)noexcept
	requires(!iron::concepts::ref_wrapper<_T>)
	{
		return ref_wrapper<const _T>::from(_master, _v);
	}

	template <typename _T>
	[[nodiscard]] inline constexpr auto make_cref(const _T& _v)noexcept
	requires(iron::concepts::ref_wrapper<_T>)
	{
		auto& _t = *_v;
		using _t_t = std::remove_reference_t<decltype(_t)>;
		if constexpr (iron::concepts::ref_wrapper<_t_t>) {
			return make_cref(_t);
		}
		else {
			return ref_wrapper<const _t_t>::from(_v, _t);
		}
	}

	template <typename _MASTER,typename _T>
	[[nodiscard]] inline constexpr auto make_cref(const _MASTER&,const _T& _v)noexcept
	requires(iron::concepts::ref_wrapper<_T>)
	{
		return make_cref(_v);
	}

} //ns


