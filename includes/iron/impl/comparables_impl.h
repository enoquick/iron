
namespace iron::comparables {


	template <typename _T>
	class equal_to final {
	public:
		using value_t = _T;
		using value_type = value_t;
		constexpr equal_to()noexcept = default;
		constexpr equal_to(const equal_to& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY(_v);
		}
		constexpr equal_to(equal_to&& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			IRON_SECURITY_MARK_TEMPORARY(_v);
		}
		constexpr equal_to& operator=(const equal_to& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			IRON_SECURITY_ASSIGN(_v);
			return *this;
		}
		constexpr equal_to& operator=(equal_to& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			IRON_SECURITY_ASSIGN(_v);
			IRON_SECURITY_MARK_TEMPORARY(_v);
			return *this;
		}
		constexpr ~equal_to()=default;
		[[nodiscard]] constexpr bool operator()(const value_t& _v1, const value_t& _v2)const noexcept 
		requires(iron::concepts::nothrow_equality_comparable<value_t>)
		{
			IRON_SECURITY_CHECK_TEMPORARY(_v1);
			IRON_SECURITY_CHECK_TEMPORARY(_v2);
			if constexpr (iron::concepts::comparable<std::equal_to<value_t>,value_t>) {
				return std::equal_to<value_t>{}(_v1, _v2);
			}
			else {
				return _v1 == _v2;
			}
		}
		IRON_SECURITY_METHOD()
	private:
		IRON_SECURITY_DCL();
	};

	template <typename _T>
	class less_to final {
	public:
		using value_t = _T;
		using value_type = value_t;
		constexpr less_to()noexcept = default;
		constexpr less_to(const less_to& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY(_v);
		}
		constexpr less_to(less_to&& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			IRON_SECURITY_MARK_TEMPORARY(_v);
		}
		constexpr less_to& operator=(const less_to& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			IRON_SECURITY_ASSIGN(_v);
			return *this;
		}
		constexpr less_to& operator=(less_to& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			IRON_SECURITY_ASSIGN(_v);
			IRON_SECURITY_MARK_TEMPORARY(_v);
			return *this;
		}
		constexpr ~less_to()=default;
		[[nodiscard]] constexpr bool operator()(const value_t& _v1, const value_t& _v2)const noexcept 
		requires(iron::concepts::nothrow_less_comparable<value_t>)
		{
			IRON_SECURITY_CHECK_TEMPORARY(_v1);
			IRON_SECURITY_CHECK_TEMPORARY(_v2);
			return _v1 < _v2;
		}
		IRON_SECURITY_METHOD()
	private:
		IRON_SECURITY_DCL();
	};

} //ns

