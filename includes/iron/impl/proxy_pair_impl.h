
namespace iron {
	template <typename _F,typename _S>
	class proxy_pair final {
	public:
		struct proxy_pair_tag { proxy_pair_tag() = delete; };
		using first_type=_F;
		using second_type = _S;
		[[nodiscard]] static constexpr proxy_pair unchecked_from(first_type& _f, second_type& _s)noexcept {
			return proxy_pair(_f, _s);
		}
		template <typename _MASTER>
		[[nodiscard]] static constexpr proxy_pair unchecked_from(const _MASTER& _master, first_type& _f, second_type& _s)noexcept
#if IRON_SECURITY_CHECKER
		requires(iron::concepts::has_security_checker<_MASTER>)
#endif 
		{
			IRON_SECURITY_CHECK_TEMPORARY(_master);
			IRON_SECURITY_CHECK_TEMPORARY(_f);
			IRON_SECURITY_CHECK_TEMPORARY(_s);
			return proxy_pair(_master, _f, _s);
		}
		constexpr proxy_pair(const proxy_pair& _v)noexcept
			: first(_v.first)
			, second(_v.second)
		{
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			IRON_SECURITY_ASSIGN(_v);
		}

		constexpr proxy_pair(proxy_pair&& _v)noexcept
			: first(_v.first)
			, second(_v.second)
		{
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			IRON_SECURITY_SWAP(_v);
			IRON_SECURITY_MARK_TEMPORARY(_v);
		}
		constexpr proxy_pair& operator=(proxy_pair&& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_swap(_v);
			IRON_SECURITY_MARK_TEMPORARY(_v);
			return *this;
		}
		proxy_pair& operator=(const proxy_pair& _v)=delete;
		constexpr ~proxy_pair() = default;
		[[nodiscard]] constexpr proxy_pair* operator->()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return this;
		}
		IRON_SECURITY_METHOD();
		first_type& first;
		second_type& second;
	private:
		constexpr proxy_pair(first_type& _f, second_type& _s)noexcept
			: first(_f)
			, second(_s)
		{
		}
		template <typename _MASTER>
		constexpr proxy_pair(const _MASTER& _master, first_type& _f, second_type& _s)noexcept
		: first(_f)
		, second(_s)
		{
			IRON_SECURITY_MAKE_REF(_master);
		}
		constexpr void _swap(proxy_pair& _v)noexcept {
			std::swap(first, _v.first);
			std::swap(second, _v.second);
			IRON_SECURITY_SWAP(_v);
		}
		IRON_SECURITY_DCL();
	};
}
