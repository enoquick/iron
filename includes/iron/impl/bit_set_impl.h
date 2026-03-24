
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

	template <std::size_t _N>
	class bit_set final {
	private:
		using _rs_t = iron::unsafe::raw_storage<unsigned char>;
	public:
		using size_t=std::size_t;
		static_assert(_N > 0,"cannot set bitset size to 0");
		static constexpr size_t size()noexcept { return _N;}
		static constexpr size_t capacity()noexcept { return _sm_internal_sz; }
		struct bit_set_tag { bit_set_tag() = delete; };
		constexpr bit_set()noexcept 
		{
			[[maybe_unused]] const auto _h=_rs_t::uninitialized_fill(&_m_area[0], _sm_internal_sz, 0x00);
			DEBUG_ASSERT(_h);
			DEBUG_ASSERT(none());
		}
		constexpr bit_set(bit_set&& _v)noexcept
		{
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			IRON_SECURITY_SWAP(_v);
			_construct_n(&_m_area[0], _sm_internal_sz,&_v._m_area[0]);
			IRON_SECURITY_MARK_TEMPORARY(_v);
		}
		constexpr bit_set(const bit_set& _v)noexcept
		{
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_construct_n(&_m_area[0], _sm_internal_sz, &_v._m_area[0]);
		}
		constexpr bit_set& operator=(bit_set&& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_swap(_v);
			IRON_SECURITY_MARK_TEMPORARY(_v);
			return *this;
		}

		constexpr bit_set& operator=(const bit_set& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			[[maybe_unused]] const auto _h=_rs_t::assign_n(&_m_area[0], _sm_internal_sz,&_v._m_area[0]);
			DEBUG_ASSERT(_h);
			return *this;
		}
		constexpr ~bit_set() = default;
		[[nodiscard]] constexpr explicit operator bool()const noexcept { return !none();}
		constexpr bit_set& set(size_t _n)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			PANIC_UNLESS(_n < size(),"bitset number overflow");
			auto& _v = _m_area[_n / CHAR_BIT];
			unsigned char _u = 1 << (_n % CHAR_BIT);
			_v |= _u;
			DEBUG_ASSERT(test(_n));
			return *this;
		}
		constexpr bit_set& reset(size_t _n)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			PANIC_UNLESS(_n < size(),"bitset number overflow");
			auto& _v = _m_area[_n / CHAR_BIT];
			unsigned char _u = ~(1 << (_n % CHAR_BIT));
			_v &= _u;
			DEBUG_ASSERT(!test(_n));
			return *this;
		}
		constexpr bit_set& set_all()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			if constexpr (!(size() % CHAR_BIT)) {
				_rs_t::assign_fill(&_m_area[0], _sm_internal_sz,0xFF);
			}
			else {
				for (size_t i = 0; i < size(); ++i) {
					set(i);
				}
			}
			DEBUG_ASSERT(all());
			return *this;
		}
		constexpr bit_set& reset_all()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			[[maybe_unused]] const auto _h=_rs_t::assign_fill(&_m_area[0], _sm_internal_sz, 0x00);
			DEBUG_ASSERT(_h);
			DEBUG_ASSERT(none());
			return *this;
		}
		[[nodiscard]] constexpr bool test(size_t _n)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			PANIC_UNLESS(_n < size(),"bitset number overflow");
			auto& _v = _m_area[_n / CHAR_BIT];
			const unsigned char _u = 1 << (_n % CHAR_BIT);
			return _v & _u ? true : false;
		}
		
		[[nodiscard]] constexpr bool test_or(std::initializer_list<size_t> _v)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			for (auto& _x : _v) {
				if (test(_x)) {
					return true;
				}
			}
			return {};
		}
		[[nodiscard]] constexpr bool test_and(std::initializer_list<size_t> _v)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			for (auto& _x : _v) {
				if (!test(_x)) {
					return {};
				}
			}
			return true;
		}

		[[nodiscard]] constexpr bool none()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			auto _sz = _sm_internal_sz;
			auto _p = &(_m_area[0]);
			DEBUG_ASSERT(_p); //ms analyzer error
			while (_sz--) {
				if (*_p) {
					return {};
				}
				++_p;
			}
			return true;
		}
		[[nodiscard]] constexpr bool all()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			for (size_t i = 0; i < size();++i) {
				if (!test(i)) {
					return {};
				}
			}
			return true;
		}
		constexpr bit_set& swap(bit_set& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_swap(_v);
			return *this;
		}
		IRON_SECURITY_METHOD()
	private:
		[[nodiscard]] static constexpr size_t _area_size()noexcept {
			auto _sz=size() / CHAR_BIT;
			if ((size() % CHAR_BIT) != 0) {
				++_sz;
			}
			return _sz;
		}
		static constexpr auto _sm_internal_sz = _area_size();
		constexpr void _swap(bit_set& _v)noexcept {
			std::swap(_m_area,_v._m_area);
			IRON_SECURITY_SWAP(_v);
		}
		static constexpr void _construct_n(unsigned char* _d, size_t _sz,const unsigned char* _s)noexcept {
			[[maybe_unused]] const auto _h = _rs_t::uninitialized_copy_n(_d, _sz,_s);
			DEBUG_ASSERT(_h);
		}
		unsigned char _m_area[_sm_internal_sz];
		IRON_SECURITY_DCL();
	};


	namespace _impl {
		template <std::size_t _SZ>
		inline constexpr void _bit_set(bit_set<_SZ>&)noexcept {}

		template <std::size_t _SZ, typename... _ARGS>
		inline constexpr void _bit_set(bit_set<_SZ>& _s, std::size_t _v, const _ARGS&... _args)noexcept {
			_s.set(_v);
			_bit_set(_s, _args...);
		}

	}
	template <std::size_t _SZ, typename... _ARGS>
	[[nodiscard]] inline constexpr iron::bit_set<_SZ> make_bit_set(const _ARGS&... _v)noexcept
		requires (_SZ > 0 && (iron::concepts::nothrow_convertible_to_integral<_ARGS>&& ...))
	{
		bit_set<_SZ> _e;
		if constexpr (sizeof...(_ARGS) != 0) {
			_impl::_bit_set(_e, _v...);
		}
		return _e;
	}


} //ns

static_assert(iron::concepts::bit_set<iron::bit_set<1>>, "internal error");

#if defined(_MSC_VER)
#pragma warning(pop)
#endif 



