#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable:26472)
#endif


namespace iron {

	namespace integral_impl {
		using u32_t = std::uint32_t;

		template <typename _T>
		class pimp final {
		public:
			using value_t = _T;
			using pair_t = std::pair<value_t, bool>;
			pimp() = delete;
			using uvalue_t = std::make_unsigned_t<value_t>;
			using svalue_t = std::make_signed_t<value_t>;
			[[nodiscard]] static constexpr bool is_signed()noexcept { return std::is_same<value_t, svalue_t>::value; }
			[[nodiscard]] static constexpr bool is_unsigned()noexcept { return std::is_same<value_t, uvalue_t>::value; }
			static constexpr auto char_bits = static_cast<u32_t>(CHAR_BIT);
			static constexpr auto min = std::numeric_limits<value_t>::min();
			static constexpr auto max = std::numeric_limits<value_t>::max();
			[[nodiscard]] static constexpr value_t binary_complement(const value_t& _v) { return ~_v; }
			[[nodiscard]] static constexpr u32_t count_ones(const value_t& _v)noexcept {
				auto _u = make_unsigned(_v);
				u32_t _count{};
				while (_u) {
					if (test_lbit(_u)) {
						++_count;
					}
					shr(_u);
				}
				return _count;
			}
			static constexpr auto bits = static_cast<u32_t>(sizeof(value_t) * char_bits);
			[[nodiscard]] static constexpr uvalue_t make_unsigned(const value_t& _v)noexcept { return static_cast<uvalue_t>(_v); }
			[[nodiscard]] static constexpr svalue_t make_signed(const value_t& _v)noexcept { return static_cast<svalue_t>(_v); }
			[[nodiscard]] static constexpr value_t make_type(const uvalue_t& _v)noexcept { return static_cast<value_t>(_v); }
			static constexpr void set_bit(uvalue_t& _v, const u32_t& _n)noexcept {
				DEBUG_ASSERT(_n < bits);
				_v |= (uvalue_t{ 1 } << _n);
			}
			static constexpr void reset_bit(uvalue_t& _v, const u32_t& _n)noexcept {
				DEBUG_ASSERT(_n < bits);
				_v &= ~(uvalue_t{ 1 } << _n);
			}
			static constexpr void flip_bit(uvalue_t& _v, const u32_t& _n)noexcept {
				DEBUG_ASSERT(_n < bits);
				_v ^= (uvalue_t{ 1 } << _n);
			}
			[[nodiscard]] static constexpr bool test_bit(const uvalue_t& _v, const u32_t& _n)noexcept {
				DEBUG_ASSERT(_n < bits);
				return ((_v >> _n) & uvalue_t { 1 }) ? true : false;
			}
			[[nodiscard]] static constexpr bool test_hbit(const uvalue_t& _v)noexcept { return test_bit(_v, bits - 1); }
			[[nodiscard]] static constexpr bool test_lbit(const uvalue_t& _v)noexcept { return test_bit(_v, 0); }
			static constexpr void set_hbit(uvalue_t& _v)noexcept { set_bit(_v, bits - 1); }
			static constexpr void reset_hbit(uvalue_t& _v)noexcept { reset_bit(_v, bits - 1); }
			static constexpr void set_lbit(uvalue_t& _v)noexcept { set_bit(_v, 0); }
			static constexpr void reset_lbit(uvalue_t& _v)noexcept { reset_bit(_v, 0); }
			static constexpr void shl(uvalue_t& _v)noexcept { _v <<= 1; }
			static constexpr void shr(uvalue_t& _v)noexcept { _v >>= 1; }
			[[nodiscard]] static constexpr u32_t leading_zeros(const value_t& _v)noexcept {
				u32_t _count{};
				if (!test_hbit(_v)) {
					auto _u = make_unsigned(_v);
					_count = bits;
					while (_u) {
						--_count;
						shr(_u);
					}
				}
				return _count;
			}
			[[nodiscard]] static constexpr u32_t trailing_zeros(const value_t& _v)noexcept {
				u32_t _count = _v ? 0 : bits;
				if (!_count) {
					auto _u = make_unsigned(_v);
					while (!test_lbit(_u)) {
						++_count;
						shr(_u);
					}
				}
				return _count;
			}
			[[nodiscard]] static constexpr u32_t leading_ones(const value_t& _v)noexcept {
				u32_t _count{};
				if (_v) {
					auto _u = make_unsigned(_v);
					while (_u) {
						if (!test_hbit(_u)) {
							break;
						}
						++_count;
						shl(_u);
					}
				}
				return _count;
			}
			[[nodiscard]] static constexpr u32_t trailing_ones(const value_t& _v)noexcept {
				u32_t _count = {};
				auto _u = make_unsigned(_v);
				while (_u) {
					if (!test_lbit(_u)) {
						break;
					}
					++_count;
					shr(_u);
				}
				return _count;
			}
			[[nodiscard]] static constexpr value_t rotate_left(const value_t& _v, const u32_t& _n)noexcept {
				if (_v) {
					auto _m = _n % bits;
					if (_m) {
						auto _u = make_unsigned(_v);
						while (_m--) {
							const auto _b = test_hbit(_u);
							shl(_u);
							if (_b) {
								set_lbit(_u);
							}
							else {
								reset_lbit(_u);
							}
						}
						return make_type(_u);
					}
				}
				return _v;
			}
			[[nodiscard]] static constexpr value_t rotate_right(const value_t& _v, const u32_t& _n)noexcept {
				if (_v) {
					auto _m = _n % bits;
					if (_m) {
						auto _u = make_unsigned(_v);
						while (_m--) {
							const auto _b = test_lbit(_u);
							shr(_u);
							if (_b) {
								set_hbit(_u);
							}
							else {
								reset_hbit(_u);
							}
						}
						return make_type(_u);
					}
				}
				return _v;
			}
			[[nodiscard]] static constexpr value_t swap_bytes(const value_t& _v)noexcept {
#ifdef __cpp_lib_byteswap
				return std::byteswap(_v);
#else
				if constexpr (sizeof(value_t) == 1) {
					return _v;
				}
				else if constexpr (sizeof(value_t) == 2) {
					const auto _u = static_cast<uvalue_t>(_v);
					return static_cast<value_t>((_u << 8) | (_u >> 8));
				}
				else if constexpr (sizeof(value_t) == 4) {
					const auto _u = static_cast<uvalue_t>(_v);
					return static_cast<value_t>((_u << 24) | ((_u << 8) & 0x00FF'0000) | ((_u >> 8) & 0x0000'FF00) | (_u >> 24));
				}
				else if constexpr (sizeof(value_t) == 8) {
					const auto _u = static_cast<uvalue_t>(_v);
					return static_cast<value_t>((_u << 56)
						| ((_u << 40) & 0x00FF'0000'0000'0000) | ((_u << 24) & 0x0000'FF00'0000'0000)
						| ((_u << 8) & 0x0000'00FF'0000'0000) | ((_u >> 8) & 0x0000'0000'FF00'0000)
						| ((_u >> 24) & 0x0000'0000'00FF'0000) | ((_u >> 40) & 0x0000'0000'0000'FF00) | (_u >> 56)
						);
				}
				else {
					static_assert(macros_support::panic::always_false<value_t>, "unexpected integer size");
				}
#endif
			}
			[[nodiscard]] static constexpr value_t reverse_bits(const value_t& _v)noexcept {
				auto _u = make_unsigned(_v);
				uvalue_t _r{};
				auto _i = bits;
				while (_i--) {
					_r |= (_u & 1) << _i;
					shr(_u);
				}
				return static_cast<value_t>(_r);
			}
			[[nodiscard]] static constexpr value_t from_be(const value_t& _v)noexcept {
				DEBUG_ASSERT(cfg::endian() != cfg::endian_t::unknow);
				if constexpr (cfg::endian() == cfg::endian_t::little) {
					return swap_bytes(_v);
				}
				else {
					return _v;
				}
			}
			[[nodiscard]] static constexpr value_t from_le(const value_t& _v)noexcept {
				DEBUG_ASSERT(cfg::endian() != cfg::endian_t::unknow);
				if constexpr (cfg::endian() == cfg::endian_t::big) {
					return swap_bytes(_v);
				}
				else {
					return _v;
				}
			}
			[[nodiscard]] static constexpr value_t to_be(const value_t& _v)noexcept {
				DEBUG_ASSERT(cfg::endian() != cfg::endian_t::unknow);
				if constexpr (cfg::endian() == cfg::endian_t::little) {
					return swap_bytes(_v);
				}
				else {
					return _v;
				}
			}
			[[nodiscard]] static constexpr value_t to_le(const value_t& _v)noexcept {
				DEBUG_ASSERT(cfg::endian() != cfg::endian_t::unknow);
				if constexpr (cfg::endian() == cfg::endian_t::big) {
					return swap_bytes(_v);
				}
				else {
					return _v;
				}
			}
			[[nodiscard]] static constexpr value_t uadd(const value_t& _v1, const value_t& _v2)noexcept { return _v1 + _v2; }
			[[nodiscard]] static constexpr value_t usub(const value_t& _v1, const value_t& _v2)noexcept { return _v1 - _v2; }
			[[nodiscard]] static constexpr value_t umul(const value_t& _v1, const value_t& _v2)noexcept { return _v1 * _v2; }
			[[nodiscard]] static constexpr value_t udiv(const value_t& _v1, const value_t& _v2)noexcept { return _v1 / _v2; }
			[[nodiscard]] static constexpr value_t urem(const value_t& _v1, const value_t& _v2)noexcept { return _v1 % _v2; }
			[[nodiscard]] static constexpr value_t uabs(const value_t& _v)noexcept { return _v >= 0 ? _v : umul(_v, -1); }
			[[nodiscard]] static constexpr value_t ushl(const value_t& _v, const u32_t& _n)noexcept { return _v << _n; }
			[[nodiscard]] static constexpr value_t ushr(const value_t& _v, const u32_t& _n)noexcept { return _v >> _n; }
			[[nodiscard]] static constexpr value_t upow(const value_t& _v, const u32_t& _n)noexcept { return _n ? umul(_v, upow(_v, _n - 1)) : 1; }
			[[nodiscard]] static constexpr pair_t cadd(const value_t& _v1, const value_t& _v2)noexcept { return is_add(_v1, _v2) ? _p_make_checked(uadd(_v1, _v2)) : _p_make_checked(); }
			[[nodiscard]] static constexpr pair_t csub(const value_t& _v1, const value_t& _v2)noexcept { return is_sub(_v1, _v2) ? _p_make_checked(usub(_v1, _v2)) : _p_make_checked(); }
			[[nodiscard]] static constexpr pair_t cmul(const value_t& _v1, const value_t& _v2)noexcept { return is_mul(_v1, _v2) ? _p_make_checked(umul(_v1, _v2)) : _p_make_checked(); }
			[[nodiscard]] static constexpr pair_t cdiv(const value_t& _v1, const value_t& _v2)noexcept { return is_div(_v1, _v2) ? _p_make_checked(udiv(_v1, _v2)) : _p_make_checked(); }
			[[nodiscard]] static constexpr pair_t crem(const value_t& _v1, const value_t& _v2)noexcept { return is_rem(_v1, _v2) ? _p_make_checked(urem(_v1, _v2)) : _p_make_checked(); }
			[[nodiscard]] static constexpr pair_t cneg(const value_t& _v)noexcept { return is_neg(_v) ? _p_make_checked(umul(_v, -1)) : _p_make_checked(); }
			[[nodiscard]] static constexpr pair_t cshl(const value_t& _v, const u32_t& _n)noexcept { return is_shl(_n) ? _p_make_checked(ushl(_v, _n)) : _p_make_checked(); }
			[[nodiscard]] static constexpr pair_t cshr(const value_t& _v, const u32_t& _n)noexcept { return is_shr(_n) ? _p_make_checked(ushr(_v, _n)) : _p_make_checked(); }
			[[nodiscard]] static constexpr pair_t cabs(const value_t& _v)noexcept { return is_abs(_v) ? _p_make_checked(uabs(_v)) : _p_make_checked(); }
			[[nodiscard]] static constexpr pair_t cpow(const value_t& _v, const u32_t& _exp)noexcept { return !_exp ? _p_make_checked(value_t{ 1 }) : _p_checked_mul(_v, cpow(_v, _exp - 1));}
			[[nodiscard]] static constexpr value_t sadd(const value_t& _v1, const value_t& _v2)noexcept {
				const auto _p = cadd(_v1, _v2);
				return _p.second
					? _p.first
					: _v1 < 0
					? min
					: max;
			}
			[[nodiscard]] static constexpr value_t ssub(const value_t& _v1, const value_t& _v2)noexcept {
				const auto _p = csub(_v1, _v2);
				return _p.second
					? _p.first
					: _v1 < 0
					? min
					: max;
			}
			[[nodiscard]] static constexpr value_t sneg(const value_t& _v)noexcept {
				const auto _p = cneg(_v);
				return _p.second
					? _p.first
					: max;
			}
			[[nodiscard]] static constexpr value_t sabs(const value_t& _v)noexcept {
				const auto _p = cabs(_v);
				return _p.second
					? _p.first
					: max;
			}
			[[nodiscard]] static constexpr value_t smul(const value_t& _v1, const value_t& _v2)noexcept {
				const auto _p = cmul(_v1, _v2);
				return _p.second
					? _p.first
					: (_v1 < 0 && _v2 < 0) || (_v1 >= 0 && _v2 >= 0)
					? max
					: min
					;
			}
			[[nodiscard]] static constexpr value_t sdiv(const value_t& _v1, const value_t& _v2)noexcept {
				if (!_v2) IRON_UNLIKELY
				{
					if (_v1 >= 0) IRON_LIKELY
					{
						return max;
					}
					return min;

				}
				const auto _p = cdiv(_v1, _v2);
				return _p.second
					? _p.first
					: (_v1 < 0 && _v2 < 0) || (_v1 >= 0 && _v2 >= 0)
					? max
					: min
					;
			}
			[[nodiscard]] static constexpr value_t spow(const value_t& _v, const u32_t& _n)noexcept {
				const auto _p = cpow(_v, _n);
				return _p.second
					? _p.first
					: _v >= 0 || is_pair(_n)
					? max
					: min
					;
			}
			[[nodiscard]] static constexpr value_t ilog2(const value_t& _v)noexcept {
				static_assert(is_unsigned());
				value_t _i{};
				for (value_t _k = sizeof(value_t) * char_bits; 0 < (_k /= 2);) {
					if (_v >= static_cast<value_t>(1) << _k) { _i += _k; _v >>= _k; }
				}
				return _i;
			}
			//[[nodiscard]] static constexpr uvalue_t abs_diff(const value_t&, const value_t&)noexcept;
			[[nodiscard]] static constexpr bool is_add(const value_t&, const value_t&)noexcept;
			[[nodiscard]] static constexpr bool is_sub(const value_t&, const value_t&)noexcept;
			[[nodiscard]] static constexpr bool is_mul(const value_t&, const value_t&)noexcept;
			[[nodiscard]] static constexpr bool is_div(const value_t& _v1, const value_t& _v2)noexcept { //suggest from https://searchfox.org/mozilla-central/source/mfbt/CheckedInt.h#203
				if constexpr (is_signed()) {
					if (_v1 == min && _v2 == value_t{ -1 }) {
						return {};
					}
				}
				return _v2;
			}
			[[nodiscard]] static constexpr bool is_rem(const value_t&, const value_t&)noexcept;
			[[nodiscard]] static constexpr bool is_neg(const value_t& _v)noexcept { return _v != min; }
			[[nodiscard]] static constexpr bool is_shl(const u32_t& _n)noexcept { return _n < bits; }
			[[nodiscard]] static constexpr bool is_shr(const u32_t& _n)noexcept { return is_shl(_n); }
			[[nodiscard]] static constexpr bool is_abs(const value_t& _v)noexcept { return _v != min; }
			[[nodiscard]] static constexpr bool is_odd(const value_t& _v)noexcept { return (_v % 2) ? true : false; }
			[[nodiscard]] static constexpr bool is_pair(const value_t& _v)noexcept { return !is_odd(_v); }
		private:
			[[nodiscard]] static constexpr auto _p_checked_mul(const value_t& _v1, const pair_t& _v2)noexcept -> pair_t {
				return _v2.second && is_mul(_v1, _v2.first) ? _p_make_checked(_v1 * _v2.first) : _p_make_checked();
			}
			[[nodiscard]] static constexpr pair_t _p_make_checked(const value_t& _v)noexcept { return std::make_pair(_v, true); }
			[[nodiscard]] static constexpr pair_t _p_make_checked()noexcept { return std::make_pair(value_t{}, false); }
		};


#undef _IRON_BUILTIN_OVERFLOW
#ifdef __has_builtin
#if __has_builtin(__builtin_add_overflow)
#define _IRON_BUILTIN_OVERFLOW
#endif
#endif

		template <typename _T>
		inline constexpr bool pimp<_T>::is_add(const value_t& _v1, const value_t& _v2)noexcept { //suggest from https://searchfox.org/mozilla-central/source/mfbt/CheckedInt.h#203
			if (!std::is_constant_evaluated()) {
#ifdef _IRON_BUILTIN_OVERFLOW
				value_t _t;
				return !__builtin_add_overflow(_v1, _v2, &_t);
#endif
			}
			if constexpr (is_signed()) {
				const auto _ux = make_unsigned(_v1);
				const auto _uy = make_unsigned(_v2);
				const uvalue_t _r = _ux + _uy;
				return test_hbit(binary_complement(value_t((_r ^ _v1) & (_r ^ _v2))));
			}
			else {
				return binary_complement(_v1) >= _v2;
			}
		}

#undef _IRON_BUILTIN_OVERFLOW
#ifdef __has_builtin
#if __has_builtin(__builtin_sub_overflow)
#define _IRON_BUILTIN_OVERFLOW
#endif
#endif

		template <typename _T>
		inline constexpr bool pimp<_T>::is_sub(const value_t& _v1, const value_t& _v2)noexcept { //suggest from https://searchfox.org/mozilla-central/source/mfbt/CheckedInt.h#203
			if (!std::is_constant_evaluated()) {
#ifdef _IRON_BUILTIN_OVERFLOW
				value_t _t;
				return !__builtin_sub_overflow(_v1, _v2, &_t);
#endif
			}
			const auto _ux = make_unsigned(_v1);
			const auto _uy = make_unsigned(_v2);
			const uvalue_t _r = _ux - _uy;
			if constexpr (is_signed()) {
				return test_hbit(binary_complement(value_t((_r ^ _v1) & (_v1 ^ _v2))));
			}
			else {
				return  _v1 >= _v2;
			}
		}

#undef _IRON_BUILTIN_OVERFLOW
#ifdef __has_builtin
#if __has_builtin(__builtin_mul_overflow)
#define _IRON_BUILTIN_OVERFLOW
#endif
#endif

		template <typename _T>
		inline constexpr bool pimp<_T>::is_mul(const value_t& _v1, const value_t& _v2)noexcept { //suggest from https://searchfox.org/mozilla-central/source/mfbt/CheckedInt.h#203
			if (!std::is_constant_evaluated()) {
#ifdef _IRON_BUILTIN_OVERFLOW
				value_t _t;
				return !__builtin_mul_overflow(_v1, _v2, &_t);
#endif
			}
			if (!_v1 || !_v2) {
				return true;
			}
			if (_v1 > 0) {
				return _v2 > 0 ? _v1 <= max / _v2 : _v2 >= min / _v1;
			}
			return _v2 > 0 ? _v1 >= min / _v2 : _v2 >= max / _v1;
		}

#undef _IRON_BUILTIN_OVERFLOW

		/*
			NOTE: if v1 < 0 || v2 < 0 the sign of result is implementation defined
			for portability is necessary to return false if v1 < 0 || v2 < 0
		*/

		template <typename _T>
		inline constexpr bool pimp<_T>::is_rem(const value_t& _v1, const value_t& _v2)noexcept { //suggest from https://searchfox.org/mozilla-central/source/mfbt/CheckedInt.h#203
			if constexpr (is_signed()) {
				if (_v1 < 0) {
					return {};
				}
			}
			return _v2 > 0;
		}

		typedef enum class enum_status {
			ok
			, overflow //overflow or underflow
			, illegal_operation //one or two operands they have illegal value for the operation
			, division_by_zero //this is an illegal_operation but for easiness...
			,first=ok
			,last= division_by_zero
		} status_t;


	} //ns

	template <typename _T>
	class base_integral final {
	private:
		using _p_t = integral_impl::pimp<_T>;
		template <typename _U>
		using _pp_t = integral_impl::pimp<_U>;
	public:
		using value_t = _T;
		using value_type = value_t;
		using uvalue_t = typename _p_t::uvalue_t;
		using unsigned_value_type = uvalue_t;
		using svalue_t = typename _p_t::svalue_t;
		using signed_value_type = svalue_t;
		using base32_t = base_integral<integral_impl::u32_t>;
		using base32_type = base32_t;
		static_assert(iron::concepts::numeric_integral<value_t>, "the value_type must be a numeric integral");
		struct base_integral_tag { base_integral_tag() = delete; };
		static constexpr auto char_bits = _p_t::char_bits;
		[[nodiscard]] static constexpr bool is_signed()noexcept { return _p_t::is_signed(); }
		[[nodiscard]] static constexpr bool is_unsigned()noexcept { return _p_t::is_unsigned(); }
		constexpr base_integral()noexcept {}
		constexpr base_integral(value_t v)noexcept : _m_v(v) {}
		constexpr base_integral(const base_integral& _v)noexcept 
		: _m_v(_v._m_v)
		{
			IRON_SECURITY_CHECK_TEMPORARY(_v);	
		}
		constexpr base_integral& operator=(const base_integral& _v)noexcept {
#if IRON_SECURITY_CHECKER
			auto _t = _v;
			_swap(_t);
#else
			_m_v = _v._m_v;
#endif
			return *this;
		}
		constexpr base_integral(base_integral&& _v)noexcept
		{
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_swap(_v);
			IRON_SECURITY_MARK_TEMPORARY(_v);
		}
		constexpr base_integral& operator=(base_integral&& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_swap(_v);
			IRON_SECURITY_MARK_TEMPORARY(_v);
			return *this;
		}
		constexpr ~base_integral() = default;
		constexpr base_integral& swap(base_integral& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_swap(_v);
			return *this;
		}
		[[nodiscard]] static constexpr base_integral min()noexcept { return _p_t::min; }
		[[nodiscard]] static constexpr base_integral max()noexcept { return _p_t::max; }
		[[nodiscard]] static constexpr base32_t bits()noexcept { return _p_t::bits; }
		[[nodiscard]] constexpr value_t value()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _m_v; 
		}
		[[nodiscard]] constexpr explicit operator bool()const noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY_THIS(); 
			return _m_v ? true : false; 
		}
		[[nodiscard]] constexpr base32_t count_ones()const noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY_THIS(); 
			return _p_t::count_ones(_m_v); 
		}
		[[nodiscard]] constexpr base32_t count_zeros()const noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY_THIS(); 
			return _p_t::bits - _p_t::count_ones(_m_v); 
		}
		[[nodiscard]] constexpr base32_t leading_zeros()const noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY_THIS(); 
			return _p_t::leading_zeros(_m_v); 
		}
		[[nodiscard]] constexpr base32_t trailing_zeros()const noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY_THIS(); 
			return _p_t::trailing_zeros(_m_v); 
		}
		[[nodiscard]] constexpr base32_t leading_ones()const noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY_THIS(); 
			return _p_t::leading_ones(_m_v); 
		}
		[[nodiscard]] constexpr base32_t trailing_ones()const noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY_THIS(); 
			return _p_t::trailing_ones(_m_v); 
		}
		[[nodiscard]] constexpr base_integral rotate_left(const base32_t& _n)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_n);
			return _p_t::rotate_left(_m_v, _n.value());
		}
		[[nodiscard]] constexpr base_integral rotate_right(const base32_t& _n)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_n);
			return _p_t::rotate_right(_m_v, _n.value());
		}
		[[nodiscard]] constexpr base_integral swap_bytes()const noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY_THIS(); 
			return _p_t::swap_bytes(_m_v); 
		}
		[[nodiscard]] constexpr base_integral reverse_bits()const noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY_THIS(); 
			return _p_t::reverse_bits(_m_v); 
		}
		[[nodiscard]] static constexpr base_integral from_be(const base_integral& _v)noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY(_v); 
			return _p_t::from_be(_v._m_v); 
		}
		[[nodiscard]] static constexpr base_integral from_le(const base_integral& _v)noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY(_v); 
			return _p_t::from_le(_v._m_v); 
		}
		[[nodiscard]] constexpr base_integral to_be()const noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY_THIS(); 
			return _p_t::to_be(_m_v); 
		}
		[[nodiscard]] constexpr base_integral to_le()const noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY_THIS(); 
			return _p_t::to_le(_m_v); 
		}
		[[nodiscard]] constexpr iron::option<base_integral> try_add(const base_integral& _v)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			return _make(_p_t::cadd(_m_v, _v.value()));
		}
		[[nodiscard]] constexpr iron::option<base_integral> try_sub(const base_integral& _v)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			return _make(_p_t::csub(_m_v, _v.value()));
		}
		[[nodiscard]] constexpr iron::option<base_integral> try_mul(const base_integral& _v)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			return _make(_p_t::cmul(_m_v, _v.value()));
		}
		[[nodiscard]] constexpr iron::option<base_integral> try_div(const base_integral& _v)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			return _make(_p_t::cdiv(_m_v, _v.value()));
		}
		[[nodiscard]] constexpr iron::option<base_integral> try_rem(const base_integral& _v)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			return _make(_p_t::crem(_m_v, _v.value()));
		}
		[[nodiscard]] constexpr iron::option<base_integral> try_neg()const noexcept 
		requires(is_signed())
		{ 
			IRON_SECURITY_CHECK_TEMPORARY_THIS(); 
			return _make(_p_t::cneg(_m_v));
		}
		[[nodiscard]] constexpr option<base_integral> try_shl(const base32_t& _n)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_n);
			return _make(_p_t::cshl(_m_v, _n.value()));
		}
		[[nodiscard]] constexpr iron::option<base_integral> try_shr(const base32_t& _n)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_n);
			return _make(_p_t::cshr(_m_v, _n.value()));
		}
		[[nodiscard]] constexpr iron::option<base_integral> try_abs()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			if constexpr (_p_t::is_unsigned()) {
				return iron::some(base_integral(_m_v));
			}
			else {
				return _make(_p_t::cabs(_m_v));
			}
		}
		[[nodiscard]] constexpr iron::option<base_integral> try_pow(const base32_t& _n)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_n);
			return _make(_p_t::cpow(_m_v, _n.value()));
		}
		[[nodiscard]] constexpr base_integral saturating_add(const base_integral& _v)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			return _p_t::sadd(_m_v, _v.value());
		}
		[[nodiscard]] constexpr base_integral saturating_sub(const base_integral& _v)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			return _p_t::ssub(_m_v, _v.value());
		}
		[[nodiscard]] constexpr base_integral saturating_neg()const noexcept 
		requires(is_signed())
		{ 
			IRON_SECURITY_CHECK_TEMPORARY_THIS(); 
			return _p_t::sneg(_m_v);
		}
		[[nodiscard]] constexpr base_integral saturating_abs()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			if constexpr (_p_t::is_unsigned()) {
				return *this;
			}
			else {
				return _p_t::sabs(_m_v);
			}
		}
		[[nodiscard]] constexpr base_integral saturating_mul(const base_integral& _v)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			return _p_t::smul(_m_v, _v.value());
		}
		[[nodiscard]] constexpr base_integral saturating_div(const base_integral& _v)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			return _p_t::sdiv(_m_v, _v.value());
		}
		[[nodiscard]] constexpr base_integral saturating_pow(const base32_t& _n)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_n);
			return _p_t::spow(_m_v, _n.value());
		}
		[[nodiscard]] constexpr base_integral abs()const noexcept {
			return try_abs().expect(U"abs() panic");
		}
		[[nodiscard]] constexpr int signum()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			if constexpr (_p_t::is_signed()) {
				if (_m_v < 0) IRON_UNLIKELY 
				{
					return -1;
				}
			}
			if (_m_v) IRON_LIKELY
			{
				return 1;
			}
			return 0;
		}
		[[nodiscard]] constexpr bool positive()const noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY_THIS(); 
			return _m_v > 0; 
		}
		[[nodiscard]] constexpr bool negative()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			if constexpr (_p_t::is_unsigned()) {
				return {};
			}
			else {
				return _m_v < 0;
			}
		}
		[[nodiscard]] constexpr bool zero()const noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY_THIS(); 
			return !_m_v; 
		}
		[[nodiscard]] constexpr base_integral operator+(const base_integral& _v)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			return try_add(_v).expect(U"add overflow/underflow");
		}
		[[nodiscard]] constexpr base_integral operator-(const base_integral& _v)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			return try_sub(_v).expect(U"sub overflow/underflow");
		}
		[[nodiscard]] constexpr base_integral operator*(const base_integral& _v)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			return try_mul(_v).expect(U"mul overflow/underflow");
		}
		[[nodiscard]] constexpr base_integral operator/(const base_integral& _v)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			return try_div(_v).expect(U"div overflow/underflow  or division by zero");
		}
		[[nodiscard]] constexpr base_integral operator%(const base_integral& _v)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			return try_rem(_v).expect(U"rem overflow/underflow or division by zero");
		}
		constexpr base_integral& operator+=(const base_integral& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_m_v = try_add(_v).expect(U"add overflow/underflow")->value();
			return *this;
		}
		constexpr base_integral& operator-=(const base_integral& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_m_v = try_sub(_v).expect(U"sub overflow/underflow")->value();
			return *this;
		}
		constexpr base_integral& operator*=(const base_integral& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_m_v = try_mul(_v).expect(U"mul overflow/underflow")->value();
			return *this;
		}
		constexpr base_integral& operator/=(const base_integral& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_m_v = try_div(_v).expect(U"div overflow/underflow")->value();
			return *this;
		}
		constexpr base_integral& operator%=(const base_integral& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_m_v = try_rem(_v).expect(U"rem overflow/underflow")->value();
			return *this;
		}

		[[nodiscard]] constexpr base_integral pow(const base32_t& _v)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			return try_pow(_v).expect(U"pow overflow/underflow");
		}
		[[nodiscard]] constexpr bool operator&&(const base_integral& _v)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			return _m_v && _v.value();
		}
		[[nodiscard]] constexpr bool operator||(const base_integral& _v)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			return _m_v || _v.value();
		}
		[[nodiscard]] constexpr bool operator!()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return !_m_v;
		}
		[[nodiscard]] constexpr base_integral operator&(const base_integral& _v)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			return _m_v & _v.value();
		}
		[[nodiscard]] constexpr base_integral operator|(const base_integral& _v)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			return _m_v | _v.value();
		}
		constexpr base_integral& operator&=(const base_integral& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_m_v &= _v.value();
			return *this;
		}
		constexpr base_integral& operator|=(const base_integral& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_m_v |= _v.value();
			return *this;
		}
		[[nodiscard]] constexpr base_integral operator^(const base_integral& _v)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			return _m_v ^ _v.value();
		}
		constexpr base_integral& operator^=(const base_integral& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_m_v ^= _v.value();
			return *this;
		}
		[[nodiscard]] constexpr base_integral operator~()const noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY_THIS(); 
			return ~_m_v; 
		}
		[[nodiscard]] constexpr base_integral operator<<(const base32_t& _n)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_n);
			return try_shl(_n).expect(U"left shift with n > bits");
		}
		constexpr base_integral& operator<<=(const base32_t& _n)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_n);
			_m_v = try_shl(_n).expect(U"left shift with n > bits")->value();
			return *this;
		}

		[[nodiscard]] constexpr base_integral operator>>(const base32_t& _n)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_n);
			return try_shr(_n).expect(U"right shift with n > bits");
		}
		constexpr base_integral& operator>>=(const base32_t& _n)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_n);
			_m_v = try_shr(_n).expect(U"right shift with n > bits")->value();
			return *this;
		}

		template <typename _U>
		[[nodiscard]] constexpr bool operator==(const base_integral<_U>& _v)const noexcept 
		requires (is_signed() == base_integral<_U>::is_signed())
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			return _m_v == _v.value();
		}
		template <typename _U>
		[[nodiscard]] constexpr bool operator==(_U _v)const noexcept 
		requires (iron::concepts::numeric_integral<_U> && is_signed() == base_integral<_U>::is_signed())
		{ 
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _m_v == _v;
		}
		template <typename _U>
		[[nodiscard]] constexpr bool operator==(const iron::ref_wrapper<_U>& _v)const noexcept
		requires (iron::concepts::base_integral<_U> && is_signed() == _U::is_signed())
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			return _m_v == _v->value();
		} 
		template <typename _U>
		[[nodiscard]] constexpr bool operator==(const iron::ref_wrapper<_U>& _v)const noexcept
			requires (iron::concepts::numeric_integral<_U>&& is_signed() == base_integral<_U>::is_signed())
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			return _m_v == *_v;
		}

		template <typename _U>
		[[nodiscard]] constexpr auto operator<=>(const base_integral<_U>& _v)const noexcept
			requires (is_signed() == base_integral<_U>::is_signed())
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			return _m_v <=> _v.value();
		}

		template <typename _U>
		[[nodiscard]] constexpr auto operator<=>(_U _v)const noexcept
			requires (iron::concepts::numeric_integral<_U>&& is_signed() == base_integral<_U>::is_signed())
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _m_v <=> _v;
		}
		template <typename _U>
		[[nodiscard]] constexpr auto operator<=>(const iron::ref_wrapper<_U>& _v)const noexcept
			requires (iron::concepts::base_integral<_U>&& is_signed() == _U::is_signed())
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			return _m_v <=> _v->value();
		}
		template <typename _U>
		[[nodiscard]] constexpr auto operator<=>(const iron::ref_wrapper<_U>& _v)const noexcept
			requires (iron::concepts::numeric_integral<_U>&& is_signed() == base_integral<_U>::is_signed())
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			return _m_v <=> *_v;
		}

		IRON_SECURITY_METHOD();
	private:
		[[nodiscard]] static constexpr iron::option<base_integral> _make(const std::pair<value_t, bool>& _p)noexcept {
			if (_p.second) IRON_LIKELY
			{
				return iron::some(base_integral(_p.first));
			}
			return {};
		}
		constexpr void _swap(base_integral& _v)noexcept {
			std::swap(_m_v, _v._m_v);
			IRON_SECURITY_SWAP(_v);
		}
		IRON_SECURITY_DCL();
		value_t _m_v{};
	};

	template <typename _T>
	class base_integral_bounded final {
	private:
		using _p_t = integral_impl::pimp<_T>;
	public:
		using value_t = _T;
		using value_type = value_t;
		using uvalue_t = typename _p_t::uvalue_t;
		using uvalue_type = uvalue_t;
		using svalue_t = typename _p_t::svalue_t;
		using svalue_type = svalue_t;
		using enum_status = integral_impl::status_t;
		using status_t = enum_status;
		using status_type = status_t;
		using base32_t = base_integral_bounded<integral_impl::u32_t>;
		using base32_type = base32_t;
		static_assert(iron::concepts::numeric_integral<value_t>, "the value_type must be a numeric integral");
		struct base_integral_bounded_tag { base_integral_bounded_tag() = delete; };
		[[nodiscard]] static constexpr const char* unsafe_str(status_t _v)noexcept {
			switch (_v) {
				case status_t::ok: return "ok";
				case status_t::division_by_zero: return "division by zero";
				case status_t::overflow: return "overflow";
				case status_t::illegal_operation: return "illegal operation";
			}
			DEBUG_ASSERT(false);
			return "";
		}
		constexpr base_integral_bounded(value_t _v={})noexcept : _m_v(_v) {}
		constexpr base_integral_bounded(value_t _v, status_t _s)noexcept : _m_v(_v), _m_status(_s) {}
		constexpr base_integral_bounded(const base_integral_bounded& _v)noexcept
		: _m_v(_v._m_v)
		, _m_status(_v._m_status)
		{
			IRON_SECURITY_CHECK_TEMPORARY(_v);
		}
		constexpr base_integral_bounded& operator=(const base_integral_bounded& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			auto _t = _v;
			_swap(_t);
			return *this;
		}
		constexpr base_integral_bounded(base_integral_bounded&& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_swap(_v);
			IRON_SECURITY_MARK_TEMPORARY(_v);
		}
		constexpr base_integral_bounded& operator=(base_integral_bounded&& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_swap(_v);
			IRON_SECURITY_MARK_TEMPORARY(_v);
			return *this;
		}
		constexpr ~base_integral_bounded() = default;
		constexpr base_integral_bounded& swap(base_integral_bounded& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_swap(_v);
			return *this;
		}
		[[nodiscard]] static constexpr base_integral_bounded min()noexcept { return _p_t::min; }
		[[nodiscard]] static constexpr base_integral_bounded max()noexcept { return _p_t::max; }
		[[nodiscard]] static constexpr base32_t bits()noexcept { return _p_t::bits; }
		[[nodiscard]] constexpr iron::option<value_t> value()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			if (*this) IRON_LIKELY
			{
				return iron::some(_m_v);
			}
			return {};
		}
		[[nodiscard]] constexpr value_t operator*()const noexcept {
			if (*this) IRON_LIKELY
			{
				return _m_v;
			}
			PANIC("cannot read value - status is not OK");
		}
		[[nodiscard]] constexpr value_t unchecked_value()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			DEBUG_ASSERT(*this);
			return _m_v;
		}
		[[nodiscard]] constexpr explicit operator bool()const noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY_THIS(); 
			return _m_status == status_t::ok; 
		}
		[[nodiscard]] constexpr status_t status()const noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY_THIS(); 
			return _m_status; 
		}
		[[nodiscard]] constexpr base32_t count_ones()const noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY_THIS(); 
			return base32_t(_p_t::count_ones(_m_v),_m_status); 
		}
		[[nodiscard]] constexpr base32_t count_zeros()const noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY_THIS(); 
			return base32_t(_p_t::bits - _p_t::count_ones(_m_v),_m_status); 
		}
		[[nodiscard]] constexpr base32_t leading_zeros()const noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY_THIS(); 
			return base32_t(_p_t::leading_zeros(_m_v),_m_status); 
		}
		[[nodiscard]] constexpr base32_t trailing_zeros()const noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY_THIS(); 
			return base32_t(_p_t::trailing_zeros(_m_v),_m_status); 
		}
		[[nodiscard]] constexpr base32_t leading_ones()const noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY_THIS(); 
			return base32_t(_p_t::leading_ones(_m_v),_m_status); 
		}
		[[nodiscard]] constexpr base32_t trailing_ones()const noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY_THIS(); 
			return base32_t(_p_t::trailing_ones(_m_v), _m_status); 
		}
		[[nodiscard]] constexpr base_integral_bounded rotate_left(const base32_t& _n)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_n);
			const auto _x = _p_t::rotate_left(_m_v, _n ? _n.unchecked_value() : 1);
			return base_integral_bounded(_x, !*this ? _m_status : !_n ? _n.status() : status_t::ok);
		}
		[[nodiscard]] constexpr base_integral_bounded rotate_right(const base32_t& _n)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_n);
			const auto _x = _p_t::rotate_right(_m_v, _n ? _n.unchecked_value() : 1);
			return base_integral_bounded(_x, !*this ? _m_status : !_n ? _n.status() : status_t::ok);
		}
		[[nodiscard]] constexpr base_integral_bounded swap_bytes()const noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY_THIS(); 
			return base_integral_bounded(_p_t::swap_bytes(_m_v),_m_status); 
		}
		[[nodiscard]] constexpr base_integral_bounded reverse_bits()const noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY_THIS(); 
			return base_integral_bounded(_p_t::reverse_bits(_m_v),_m_status); 
		}
		[[nodiscard]] constexpr base_integral_bounded from_be()const noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY_THIS(); 
			return base_integral_bounded(_p_t::from_be(_m_v),_m_status); 
		}
		[[nodiscard]] constexpr base_integral_bounded from_le()const noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY_THIS(); 
			return base_integral_bounded(_p_t::from_le(_m_v),_m_status); 
		}
		[[nodiscard]] constexpr base_integral_bounded to_be()const noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY_THIS(); 
			return base_integral_bounded(_p_t::to_be(_m_v),_m_status); 
		}
		[[nodiscard]] constexpr base_integral_bounded to_le()const noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY_THIS(); 
			return base_integral_bounded(_p_t::to_le(_m_v),_m_status); 
		}
		[[nodiscard]] constexpr base_integral_bounded abs()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			if constexpr (_p_t::is_unsigned()) {
				return *this;
			}
			else {
				auto _r = *this;
				_r._m_v = _p_t::uabs(_m_v);
				if (_r && !_p_t::is_abs(_m_v)) IRON_UNLIKELY
				{
					_r._m_status = status_t::illegal_operation;
				}
				return _r;
			}
		}
		[[nodiscard]] constexpr base_integral_bounded<int> signum()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			if constexpr (_p_t::is_signed()) {
				if (_m_v < 0) return base_integral_bounded<int>(-1, _m_status);
			}
			return base_integral_bounded<int>(_m_v ? 1 : 0, _m_status);
		}
		[[nodiscard]] constexpr iron::option<bool> positive()const noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY_THIS(); 
			if (*this) IRON_LIKELY
			{
				return iron::some(unchecked_positive());
			}
			return {};
		}
		[[nodiscard]] constexpr bool unchecked_positive()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			DEBUG_ASSERT(*this);
			return _m_v > 0;
		}
		[[nodiscard]] constexpr iron::option<bool> negative()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			if (*this) IRON_LIKELY
			{
				return iron::some(unchecked_negative());
			}
			return {};
		}
		[[nodiscard]] constexpr bool unchecked_negative()const noexcept {
			DEBUG_ASSERT(*this);
			if constexpr (_p_t::is_unsigned()) {
				return {};
			}
			else {
				return _m_v < 0;
			}
		}
		[[nodiscard]] constexpr iron::option<bool> zero()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			if (*this) IRON_LIKELY
			{
				return iron::some(unchecked_zero());
			}
			return {};
		}
		[[nodiscard]] constexpr bool unchecked_zero()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			DEBUG_ASSERT(*this);
			return !_m_v;
		}
		[[nodiscard]] constexpr base_integral_bounded pow(const base32_t& _n)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_n);
			auto _r = *this;
			if (_r) IRON_LIKELY
			{
				_r._m_status = _n.status();
			}
			const auto _p = _p_t::cpow(_m_v, _r ? _n.unchecked_value() : 0);
			if (_r && !_p.second) IRON_UNLIKELY
			{
				_r._m_status = status_t::overflow;
			}
			_r._m_v = _p.first;
			return _r;
		}
		constexpr base_integral_bounded& operator+=(const base_integral_bounded& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_set_status(_v, _p_t::is_add(_m_v, _v._m_v), status_t::overflow);
			_m_v += _v._m_v;
			return *this;
		}
		constexpr base_integral_bounded& operator-=(const base_integral_bounded& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_set_status(_v, _p_t::is_sub(_m_v, _v._m_v), status_t::overflow);
			_m_v -= _v._m_v;
			return *this;
		}
		constexpr base_integral_bounded& operator*=(const base_integral_bounded& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_set_status(_v, _p_t::is_mul(_m_v, _v._m_v), status_t::overflow);
			_m_v *= _v._m_v;
			return *this;
		}
		constexpr base_integral_bounded& operator/=(const base_integral_bounded& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_set_status(_v);
			if (*this && !_v._m_v) IRON_UNLIKELY
			{
				_m_status = status_t::division_by_zero;
			}
			if (*this && !_p_t::is_div(_m_v, _v._m_v)) IRON_UNLIKELY
			{
				_m_status = status_t::overflow;
			}
			if (*this) IRON_LIKELY
			{
				_m_v /= _v._m_v;
			}
			return *this;
		}
		constexpr base_integral_bounded& operator%=(const base_integral_bounded& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_set_status(_v);
			if (*this && !_v._m_v) IRON_UNLIKELY
			{
				_m_status = status_t::division_by_zero;
			}
			if (*this && !_p_t::is_rem(_m_v, _v._m_v)) IRON_UNLIKELY
			{
				_m_status = status_t::illegal_operation;
			}
			if (*this) IRON_LIKELY
			{
				_m_v %= _v._m_v;
			}
			return *this;
		}
		constexpr base_integral_bounded& operator&=(const base_integral_bounded& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_set_status(_v);
			_m_v &= _v._m_v;
			return *this;
		}
		constexpr base_integral_bounded& operator|=(const base_integral_bounded& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_set_status(_v);
			_m_v |= _v._m_v;
			return *this;
		}
		constexpr base_integral_bounded& operator^=(const base_integral_bounded& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_set_status(_v);
			_m_v ^= _v._m_v;
			return *this;
		}
		constexpr base_integral_bounded& operator<<=(const base32_t& _n)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_n);
			_set_status(_n, _p_t::is_shl(_n.unchecked_value()),status_t::illegal_operation);
			if (*this) IRON_LIKELY
			{
				_m_v <<= _n.unchecked_value();
			}
			return *this;
		}
		constexpr base_integral_bounded& operator>>=(const base32_t& _n)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_n);
			_set_status(_n, _p_t::is_shr(_n.unchecked_value()), status_t::illegal_operation);
			if (*this) IRON_LIKELY
			{
				_m_v >>= _n.unchecked_value();
			}
			return *this;
		}
		[[nodiscard]] constexpr base_integral_bounded operator+(const base_integral_bounded& _v)const noexcept { auto _t = *this; _t += _v; return _t; }
		[[nodiscard]] constexpr base_integral_bounded operator-(const base_integral_bounded& _v)const noexcept { auto _t = *this; _t -= _v; return _t; }
		[[nodiscard]] constexpr base_integral_bounded operator*(const base_integral_bounded& _v)const noexcept { auto _t = *this; _t *= _v; return _t; }
		[[nodiscard]] constexpr base_integral_bounded operator/(const base_integral_bounded& _v)const noexcept { auto _t = *this; _t /= _v; return _t; }
		[[nodiscard]] constexpr base_integral_bounded operator%(const base_integral_bounded& _v)const noexcept { auto _t = *this; _t %= _v; return _t; }
		[[nodiscard]] constexpr base_integral_bounded operator&(const base_integral_bounded& _v)const noexcept { auto _t = *this; _t &= _v; return _t; }
		[[nodiscard]] constexpr base_integral_bounded operator|(const base_integral_bounded& _v)const noexcept { auto _t = *this; _t |= _v; return _t; }
		[[nodiscard]] constexpr base_integral_bounded operator^(const base_integral_bounded& _v)const noexcept { auto _t = *this; _t ^= _v; return _t; }
		[[nodiscard]] constexpr base_integral_bounded operator~()const noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY_THIS(); 
			return base_integral_bounded(~_m_v, _m_status); 
		}
		[[nodiscard]] constexpr base_integral_bounded operator<<(const base32_t& _n)const noexcept { auto _t = *this; _t <<= _n; return _t; }
		[[nodiscard]] constexpr base_integral_bounded operator>>(const base32_t& _n)const noexcept { auto _t = *this; _t >>= _n; return _t; }
		IRON_SECURITY_METHOD()
	private:
		template <typename _U>
		friend class base_integral_bounded;
		template <typename _U>
		constexpr void _set_status(const _U& _v)noexcept 
		requires(iron::concepts::base_integral_bounded<_U>)
		{
			if (*this) IRON_LIKELY
			{
				_m_status = _v._m_status;
			}
		}
		template <typename _U>
		constexpr void _set_status(const _U& _v,bool _ok,status_t _st)noexcept {
			_set_status(_v);
			if (_m_status == status_t::ok) IRON_LIKELY
			{
				if (!_ok) IRON_UNLIKELY
				{
					_m_status = _st;
				}
			}
		}
		constexpr void _swap(base_integral_bounded& _v)noexcept {
			std::swap(_m_v, _v._m_v);
			std::swap(_m_status, _v._m_status);
			IRON_SECURITY_SWAP(_v);
		}
		value_t _m_v{};
		status_t _m_status{};
		IRON_SECURITY_DCL();
	};


} //ns

static_assert(iron::concepts::base_integral<iron::i32>, "internal error");
static_assert(iron::concepts::base_integral_bounded<iron::i32b>, "internal error");

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

static_assert(iron::concepts::base_integral<iron::u8>, "internal error");
static_assert(iron::concepts::base_integral_bounded<iron::u8b>, "internal error");

