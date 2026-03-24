
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable:26481)
#pragma warning (disable:26490)
#pragma warning (disable:26473)
#pragma warning (disable:26429)
#endif

namespace iron {

	template <typename _T>
	class hasher final {
	public:
		using value_t = std::decay_t<_T>;
		using value_type = value_t;
		using size_t = std::size_t;
		using size_type = size_t;
		constexpr hasher()noexcept = default;
		constexpr  hasher(const  hasher& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY(_v);
		}
		constexpr  hasher(hasher&& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			IRON_SECURITY_MARK_TEMPORARY(_v);
		}
		constexpr  hasher& operator=(const  hasher& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			IRON_SECURITY_ASSIGN(_v);
			return *this;
		}
		constexpr  hasher& operator=( hasher& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			IRON_SECURITY_ASSIGN(_v);
			IRON_SECURITY_MARK_TEMPORARY(_v);
			return *this;
		}
		constexpr ~hasher()=default;

		[[nodiscard]] constexpr size_t operator()(const value_t& _v)const noexcept { return _hash(_v);}

		IRON_SECURITY_METHOD()

	private:
		template <typename _U>
		friend class hasher;
		static constexpr size_t _p_append_bytes(size_t _prime,size_t _start, const unsigned char* _p, size_t _c)noexcept {
			DEBUG_ASSERT(_prime);
			DEBUG_ASSERT(_start);
			DEBUG_ASSERT(_p);
			DEBUG_ASSERT(_c);
			while (_c--) {
				_start ^= static_cast<size_t>(*_p++);
				_start *= _prime;
			}
			return _start;
		}

		template <typename _U>
		static constexpr size_t _p_hash(const _U& _v)noexcept {
			using _value_t = _U;
			if constexpr (std::is_enum_v<_value_t> || std::is_integral_v<_value_t> || std::is_floating_point_v<_value_t>) {
				return static_cast<size_t>(_v);
			}
			else if constexpr (std::is_same_v<std::decay_t<_value_t>, std::nullptr_t>) {
				return {};
			}
			else if constexpr (std::is_pointer_v<_value_t>) {
				return static_cast<std::intptr_t>(_v);
			}
			else if constexpr (std::is_nothrow_convertible_v<_U, const void*>) {
				const void* _p = _v;
				return _p_hash(_p);
			}
			else if constexpr (iron::concepts::nothrow_convertible_to_integral<_U>) {
				const auto _p = static_cast<std::uint64_t>(_v);
				return _p_hash(_p);
			}
			else {
				return _v.hash();
			}
		}
		static constexpr size_t _p_hash(const value_t& _v,bool _use_start)noexcept {
			constexpr auto _start = cfg::hash_prime_start();
			static_assert(_start, "internal error - iron::cfg::hash_prime_start() return 0");
			constexpr auto _prime = cfg::hash_prime();
			static_assert(_prime, "internal error - iron::cfg::hash_prime() return 0");
			if constexpr (iron::concepts::raw_sequence_maker<value_t>) { //for str|str_view
				if (std::is_constant_evaluated()) {
					size_t _r{ _use_start ? _start : size_t{} };
					for (auto& _x : _v.raw_seq()) {
						_r ^= hasher<decltype(_x)>::_p_hash(_x,false);
						_r *= _prime;
					}
					return _r;
				}
				else {
					const auto _sz = _v.size() * sizeof(typename value_t::value_t);
					auto _u = reinterpret_cast<const unsigned char*>(_v.unsafe_data());
					const auto _x = _p_append_bytes(_prime, _start, _u, _sz);
					return _x;
				}
			}
			else if constexpr (iron::concepts::sequence<value_t>) {
				size_t _r{ _use_start ? _start : size_t{} };
				for (auto& _x : _v) {
					_r ^= hasher<decltype(_x)>::_p_hash(_x,false);
				}
				return _r;
			}
			else if constexpr (iron::concepts::fwd_sequence_maker<value_t>) {
				return hasher()(_v.seq());
			}
			else {
				if (std::is_constant_evaluated()) {
					return (_start + _p_hash(_v)) * _prime;
				}
				else {
					const auto _s = _p_hash(_v);
					auto& _u = reinterpret_cast<const unsigned char&>(_s);
					const auto _x = _p_append_bytes(_prime, _start, &_u, sizeof(size_t));
					return _x;
				}
			}
		} 
		[[nodiscard]] constexpr size_t _hash(const _T& _v)const noexcept {
			if (std::is_constant_evaluated()) {
				return _p_hash(_v,true);
			}
			else {
				if constexpr (iron::concepts::hashable<std::hash<_T>, _T>) {
					return std::hash<_T>{}(_v);
				}
				else {
					return _p_hash(_v, true);
				}
			}
		}
		IRON_SECURITY_DCL();
	};
}

#if defined(_MSC_VER)
#pragma warning(pop)
#endif


