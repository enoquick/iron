
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:26492)
#pragma warning(disable:26465)
#pragma warning(disable:26478)
#endif

namespace iron::unsafe {
	template <typename _T>
	class raw_wrapper final {
	public:
		using value_t = _T;
		static_assert(!std::is_reference_v<value_t>, "raw_wrapper template parameter must not be a reference type");
		static_assert(!std::is_array_v<value_t>, "raw_wrapper template parameter must not be an array type");
		static_assert(!std::is_const_v<value_t>, "raw_wrapper template parameter must not be const");
		static_assert(std::is_nothrow_destructible_v<value_t>,"raw_wrapper template parameter must be nothrow destructible");
#ifndef NDEBUG
		[[nodiscard]] constexpr bool constructed()const noexcept { return _m_constructed; }
#endif
		constexpr raw_wrapper()noexcept {}
		raw_wrapper(const raw_wrapper&) = delete;
		raw_wrapper& operator=(const raw_wrapper&) = delete;
		constexpr raw_wrapper(raw_wrapper&& _v)noexcept
		requires(std::is_nothrow_move_constructible_v<value_t>)
		{
			DEBUG_ASSERT(_v.constructed());
			_construct(_v.release());
		}
		constexpr raw_wrapper& operator=(raw_wrapper&& _v)noexcept
		requires(std::is_nothrow_move_constructible_v<value_t>)
		{
			DEBUG_ASSERT(_v.constructed());
			DEBUG_ASSERT(constructed());
			value() = _v.release();
			return *this;
		}
		constexpr ~raw_wrapper() {
			DEBUG_ASSERT(!constructed()); //if failed use destroy() before
		}
		template <typename... _ARGS >
		constexpr void construct(_ARGS&&... _args)noexcept
		requires (iron::concepts::nothrow_constructible_at<value_t, _ARGS&&...>)
		{
			DEBUG_ASSERT(!constructed());
			_construct(std::forward<_ARGS>(_args)...);
		}

		constexpr void destroy()noexcept {
			DEBUG_ASSERT(constructed());
			_destroy();
			DEBUG_ASSERT(!constructed());
		}
		[[nodiscard]] constexpr value_t release()noexcept
		requires(std::is_nothrow_move_constructible_v<value_t>)
		{
			DEBUG_ASSERT(constructed());
			return _m_u._release();
		}
		[[nodiscard]] constexpr value_t release_and_destroy()noexcept
		requires(std::is_nothrow_move_constructible_v<value_t>)
		{
			auto _v = release();
			_destroy();
			return _v;
		}
		[[nodiscard]] constexpr const value_t& value()const noexcept {
			DEBUG_ASSERT(constructed());
			return _m_u._get();
		}
		[[nodiscard]] constexpr const value_t& cvalue()const noexcept {
			return value();
		}
		[[nodiscard]] constexpr value_t& value()noexcept {
			DEBUG_ASSERT(constructed());
			return _m_u._get();
		}

	private:
		constexpr void _set_contructed(bool _v)noexcept {
#ifndef NDEBUG
			_m_constructed = _v;
#endif
		}
		template <typename... _ARGS >
		constexpr void _construct(_ARGS&&... _args)noexcept
		{
			_m_u._construct(std::forward<_ARGS>(_args)...);
			_set_contructed(true);
		}
#if 0
		template <typename _F,typename _S>
		constexpr void _construct_from_pair(_F&& _first, _S&& _second)noexcept {
			_m_u._construct_from_pair(std::forward<_F>(_first),std::forward<_S>(_second));
			_set_contructed(true);
		}
#endif
		constexpr void _destroy()noexcept {
			_m_u._destroy();
			_set_contructed(false);
		}
		typedef union _u {
			constexpr _u()noexcept {
				_clear();
			}
			_u(_u&& _u) = delete;
			_u(const _u&) = delete;
			_u& operator=(const _u&) = delete;
			_u& operator=(_u&&) = delete;
			constexpr ~_u() {}
			template <typename... _ARGS >
			constexpr void _construct(_ARGS&&... _args)noexcept {
				std::construct_at(&_m_v, std::forward<_ARGS>(_args)...);
			}
#if 0
			template <typename _F, typename _S>
			constexpr void _construct_from_pair(_F&& _first, _S&& _second)noexcept {
				construct_at(
					&_m_v
					, std::piecewise_construct
					, std::forward_as_tuple(std::forward<_F>(_first))
					, std::forward_as_tuple(std::forward<_S>(_second))
				);
			}
#endif
			constexpr void _destroy()noexcept {
				std::destroy_at(&_m_v);
				_clear();
			}
			[[nodiscard]] constexpr value_t* _ptr()noexcept {
				return &_m_v;
			}
			[[nodiscard]] constexpr const value_t* _ptr()const noexcept {
				return &_m_v;
			}
			[[nodiscard]] constexpr value_t _release()noexcept {
				return std::move(_m_v); //msvc failed ? - warning on constant value but value_t and _release() are not const
			}
#if 0
			[[nodiscard]] constexpr auto _release_pair()noexcept
			requires(iron::concepts::std_pair<value_t>)
			{
				using f_t = std::decay_t<typename value_t::first_type>;
				using s_t = std::decay_t<typename value_t::second_type>;
				static_assert(std::is_nothrow_move_constructible_v<f_t>, "first_type must be nothrow move constructible");
				static_assert(std::is_nothrow_move_constructible_v<s_t>, "second_type must be nothrow move constructible");
				auto& _f = const_cast<f_t&>(_m_v.first);
				auto& _s = const_cast<s_t&>(_m_v.second);
				return std::make_pair(
					std::move(_f)
					, std::move(_s)
				);
			}
#endif 
			[[nodiscard]] constexpr value_t& _get()noexcept {
				auto _p = _ptr();
				DEBUG_ASSERT(_p);
				return *_p;
			}
			[[nodiscard]] constexpr const value_t& _get()const noexcept {
				auto _p = _ptr();
				DEBUG_ASSERT(_p);
				return *_p;
			}
		private:
			constexpr void _clear()noexcept {
				if (!std::is_constant_evaluated()) {
					std::memset(&_m_u[0], 0x00, sizeof(value_t));
				}
			}
			unsigned char _m_u[sizeof(value_t)];
			value_t _m_v;
		} _u_t;
#ifndef NDEBUG
		bool _m_constructed{};
#endif
		_u_t _m_u{};
	};
}

#if defined(_MSC_VER)
#pragma warning(pop)
#endif
