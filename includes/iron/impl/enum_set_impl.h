
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable:26481)
#pragma warning (disable:26446)
#pragma warning (disable:26482)
#ifdef NDEBUG
#pragma warning (disable:26429)
#endif 
#endif 

namespace iron {

	template <typename _T>
	class enum_set final {
	public:
		using value_t = _T;
		static_assert(std::is_enum_v<value_t>, "the param T of enum_set must be an enum");
		using underlying_type_t = std::underlying_type_t<_T>;
		static_assert(std::is_unsigned_v<underlying_type_t>, "the T underlying type of enum_set must be unsigned");
		[[nodiscard]] static constexpr underlying_type_t numeric_value(value_t _v)noexcept { return static_cast<underlying_type_t>(_v); }
		[[nodiscard]] static constexpr bool is_bit(value_t _v)noexcept { return numeric_value(_v) < _sm_bitset_sz; }
		constexpr enum_set()noexcept = default;
		constexpr enum_set(enum_set&& _v)noexcept = default;
		constexpr enum_set(const enum_set& _v)noexcept = default;
		constexpr enum_set& operator=(enum_set&& _v)noexcept = default;
		constexpr enum_set& operator=(const enum_set& _v)noexcept = default;
		constexpr ~enum_set() = default;
		[[nodiscard]] constexpr explicit operator bool()const noexcept {
			return _m_bs ? true : false;
		}
		constexpr enum_set& set(value_t _v)noexcept {
			DEBUG_ASSERT(is_bit(_v));
			_m_bs.set(numeric_value(_v));
			return *this;
		}
		constexpr enum_set& reset(value_t _v)noexcept {
			DEBUG_ASSERT(is_bit(_v));
			_m_bs.reset(numeric_value(_v));
			return *this;
		}
		[[nodiscard]] constexpr bool test(value_t _v)const noexcept {
			DEBUG_ASSERT(is_bit(_v));
			return _m_bs.test(numeric_value(_v));
		}		
		[[nodiscard]] constexpr bool test_or(const std::initializer_list<value_t>& _v)const noexcept {
			for (auto& _x : _v) {
				if (test(_x)) {
					return true;
				}
			}
			return {};
		}
		[[nodiscard]] constexpr bool test_and(const std::initializer_list<value_t>& _v)const noexcept {
			for (auto& _x : _v) {
				if (!test(_x)) {
					return {};
				}
			}
			return true;
		}
		constexpr enum_set& swap(enum_set& _v)noexcept {
			_m_bs.swap(_v._m_bs);
			return *this;
		}
#if IRON_SECURITY_CHECKER
		[[nodiscard]] constexpr const iron::unsafe::security_checker& checker()const noexcept { return _m_bs.checker(); }
#endif
	private:
		static constexpr auto _sm_bitset_sz = sizeof(underlying_type_t) * CHAR_BIT;
		using _bit_set_t = bit_set<_sm_bitset_sz>;
		_bit_set_t _m_bs{};
	};


	namespace _impl {
		template <typename _T>
		inline constexpr void _enum_set(iron::enum_set<_T>&)noexcept {}

		template <typename _T, typename... _ARGS>
		inline constexpr void _enum_set(iron::enum_set<_T>& _s, _T _v, _ARGS... _args)noexcept {
			_s.set(_v);
			_enum_set(_s, _args...);
		}
	}

	template <typename _T>
	[[nodiscard]] inline constexpr iron::enum_set<_T> make_enum_set()noexcept
	requires(std::is_enum_v<_T>)
	{
		return 	{};
	}

	template <typename _T,typename... _ARGS>
	[[nodiscard]] inline constexpr iron::enum_set<_T> make_enum_set(_T _v,_ARGS... _args)noexcept 
	requires(std::is_enum_v<_T> && (std::is_same_v<_T,_ARGS>&& ...))
	{
		iron::enum_set<_T> _e;
		_impl::_enum_set(_e, _v, _args...);
		return _e;
	}

} //ns

#if defined(_MSC_VER)
#pragma warning(pop)
#endif 



