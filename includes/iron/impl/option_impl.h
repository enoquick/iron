
namespace iron {


	template <typename _T>
	class option final {
	public:
		struct option_tag { option_tag() = delete; };
		using value_t = _T;
		using value_type = value_t;
		using error_handler_t = iron::error_handler;
		using error_handler_type = error_handler_t;
		static_assert(!std::is_reference_v<value_t>, "option value_type must not be a reference");
		static_assert(!std::is_array_v<value_t>, "option value_type must not be an array type");
		static_assert(!std::is_const_v<value_t>, "option value_type must not be const");
		static_assert(std::is_nothrow_move_constructible_v<value_t>, "option value_type must be nothrow move constructible");
		static_assert(std::is_nothrow_destructible_v<value_t>, "option value_type must be nothrow destructible");
		static_assert(std::is_nothrow_swappable_v<value_t>, "option value_type must be nothrow swappable");
	private:
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
			,typename _get_value_type<value_t>::_type
			,value_t
		>;
	public:
		using ref_t = _rval_t&;
		using reference = ref_t;
		using cref_t = const _rval_t&;
		using const_reference = cref_t;
		using safe_ref_t = iron::ref_wrapper<_rval_t>;
		using safe_reference = safe_ref_t;
		using safe_cref_t = iron::ref_wrapper<const _rval_t>;
		using safe_const_reference = safe_cref_t;
		[[nodiscard]] static constexpr size_t char_max_size()noexcept { return iron::unsafe::raw_storage<char32_t>::max_size(); }
		constexpr option()noexcept {}
		constexpr option(option&& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			if (_v._m_constructed) IRON_LIKELY
			{
				_m_v.construct(_v._m_v.release_and_destroy());
				std::swap(_m_constructed, _v._m_constructed);
			}
			IRON_SECURITY_MARK_TEMPORARY(_v);
		}
		constexpr option(const option& _v)noexcept 
		requires std::is_nothrow_copy_constructible_v<value_t>  
		{
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			if (_v._m_constructed) IRON_LIKELY
			{
				auto _x = _v._m_v.value();
				_m_v.construct(std::move(_x));
				_m_constructed = true;
			}
		}

		template <typename _U=std::decay_t<value_t> //NOTE: using requires !std::is_same_v<_U, void*> compile failed 
			,bool _B= !std::is_same_v<_U, void*>
			,std::enable_if_t<_B, bool> = true
		>
		constexpr option(option<void*>&& _v)noexcept { //for none() 
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			DEBUG_ASSERT(!_v); 
		} 
		constexpr option& operator=(option&& _v)noexcept 
		{
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_swap(_v);
			IRON_SECURITY_MARK_TEMPORARY(_v);
			return *this;
		}

		option& operator=(const option& _v)noexcept
		requires (std::is_nothrow_copy_constructible_v<value_t>)
		{
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			option t(_v);
			_swap(t);
			return *this;
		}

		constexpr ~option() { 
			_reset();
		}

		constexpr option& swap(option& _v)noexcept 
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_swap(_v);
			return *this;
		}
		[[nodiscard]] constexpr iron::result<option, error_handler_t> try_clone()const noexcept
		requires (iron::concepts::copyable_or_cloneable<value_t>);

		[[nodiscard]] constexpr option clone()const noexcept
		requires(iron::concepts::copyable_or_cloneable<value_t>);

		constexpr void reset()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			_reset();
		}

		[[nodiscard]] value_t release()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			if (_m_constructed) IRON_LIKELY
			{
				auto _v = _m_v.reset();
				_m_constructed = {};
				return _v;
			}
			PANIC("release() failed");
		}

		[[nodiscard]] constexpr value_t release_or()noexcept 
		requires (std::is_nothrow_default_constructible_v<value_t>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			if (_m_constructed) IRON_LIKELY 
			{
				auto _v = _m_v.reset();
				_m_constructed = {};
				return _v;
			}
			return {};
		}

		[[nodiscard]] static constexpr option none()noexcept { return {}; }

		template <typename... _ARGS>
		[[nodiscard]] static constexpr option some(_ARGS&&... _args)noexcept 
		requires (iron::concepts::nothrow_constructible_at<value_t, _ARGS&&...>)
		{
			option _opt;
			_opt._m_v.construct(std::forward<_ARGS>(_args)...);
			_opt._m_constructed = true;
			return _opt;
		}
		[[nodiscard]] constexpr safe_cref_t unwrap()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _expect("unwrap() failed");
		}
		[[nodiscard]] constexpr auto cunwrap()const noexcept {
			return unwrap();
		}
		[[nodiscard]] constexpr safe_cref_t operator*()const noexcept {
			return unwrap();
		}
		[[nodiscard]] constexpr cref_t unchecked_unwrap()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			DEBUG_ASSERT(*this);
			if constexpr (iron::concepts::ref_wrapper<value_t>) {
				return _get().value();
			}
			else {
				return _get();
			}
		}
		[[nodiscard]] constexpr cref_t unchecked_cunwrap()const noexcept {
			return unchecked_unwrap();
		}
		[[nodiscard]] constexpr safe_ref_t unwrap()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _expect("unwrap() failed");
		}
		[[nodiscard]] constexpr safe_ref_t operator*()noexcept {
			return unwrap();
		}
		[[nodiscard]] constexpr ref_t unchecked_unwrap()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			DEBUG_ASSERT(_m_constructed);
			if constexpr (iron::concepts::ref_wrapper<value_t>) {
				return _get().value();
			}
			else {
				return _get();
			}
		}
		template <typename _M,std::size_t _N>
		[[nodiscard]] constexpr safe_cref_t expect(const _M(&_p)[_N])const noexcept 
		requires ((std::is_same_v<_M, char> || std::is_same_v<_M, char32_t>) && _N < char_max_size())
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _expect(_p);
		}
		template <typename _M, std::size_t _N>
		[[nodiscard]] constexpr safe_cref_t cexpect(const _M(&_p)[_N])const noexcept
		requires ((std::is_same_v<_M, char> || std::is_same_v<_M, char32_t>) && _N < char_max_size())
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _expect(_p);
		}
		template <typename _M,std::size_t _N>
		[[nodiscard]] constexpr safe_ref_t expect(const _M(&_p)[_N])noexcept
		requires ((std::is_same_v<_M, char> || std::is_same_v<_M, char32_t>) && _N < char_max_size())
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _expect(_p);
		}

		[[nodiscard]] constexpr bool is_some()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _m_constructed;
		}
		[[nodiscard]] constexpr explicit operator bool()const noexcept { return is_some(); }
		[[nodiscard]] constexpr bool is_none()const noexcept { return !is_some(); }

		template <typename _F>
		[[nodiscard]] constexpr bool is_some_and(_F _f)const noexcept 
		requires (iron::concepts::bool_predicate<_F,value_t>) 
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _m_constructed && _f(_get()); 
		}
		[[nodiscard]] constexpr value_t unwrap_or(value_t _v)const noexcept 
			requires(std::is_nothrow_copy_constructible_v<value_t>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			if (_m_constructed) IRON_LIKELY
			{
				return _m_v.value();
			}
			return _v;
		}

		[[nodiscard]] constexpr value_t& unchecked_unwrap_or(value_t& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			if (_m_constructed) IRON_LIKELY
			{
				return _get();
			}
			return _v;
		}

		[[nodiscard]] constexpr const value_t& unchecked_unwrap_or(const value_t& _v)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			if (_m_constructed) IRON_LIKELY
			{
				return _get();
			}
			return _v;
		}
		[[nodiscard]] constexpr const value_t& unchecked_cunwrap_or(const value_t& _v)const noexcept {
			return unchecked_unwrap_or(_v);
		}
		[[nodiscard]] constexpr value_t unwrap_or_default()const noexcept 
		requires (std::is_nothrow_default_constructible_v<value_t> && std::is_nothrow_copy_constructible_v<value_t>)
		{ 
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			if (_m_constructed) IRON_LIKELY
			{
				return _m_v.value();
			}
			return {}; 
		}
		IRON_SECURITY_METHOD()
	private:
		using _rw_t = iron::unsafe::raw_wrapper<value_t>;
		constexpr void _swap(option& _v)noexcept {
			if (_m_constructed == _v._m_constructed) IRON_LIKELY 
			{
				if (_m_constructed) IRON_LIKELY
				{
					auto& _v0 = _get();
					auto& _v1 = _v._get();
					std::swap(_v0, _v1);
				}
			}
			else {
				if (_m_constructed) IRON_LIKELY 
				{
					_v._m_v.construct(_m_v.release_and_destroy());
				}
				else {
					_m_v.construct(_v._m_v.release_and_destroy());
				}
				std::swap(_m_constructed, _v._m_constructed);
			}
			IRON_SECURITY_SWAP(_v);
		}
		constexpr void _reset()noexcept {
			if (_m_constructed) IRON_LIKELY
			{
				_m_v.destroy();
				_m_constructed = {};
			}
		}
		[[nodiscard]] constexpr value_t& _get()noexcept {
			DEBUG_ASSERT(_m_constructed);
			return _m_v.value();
		}
		[[nodiscard]] constexpr const value_t& _get()const noexcept {
			DEBUG_ASSERT(_m_constructed);
			return _m_v.value();
		}
		[[nodiscard]] constexpr auto _get_ref()const noexcept {
			return iron::make_cref(*this, _get());
		}
		[[nodiscard]] constexpr auto _get_ref(const value_t& _v)const noexcept {
			return iron::make_cref(*this, _v);
		}
		[[nodiscard]] constexpr auto _get_ref()noexcept {
			DEBUG_ASSERT(_m_constructed);
			return iron::make_ref(*this, _get());
		}
		[[nodiscard]] constexpr auto _get_ref(value_t& _v)noexcept {
			return iron::make_ref(*this, _v);
		}

		template <typename _M>
		[[nodiscard]] constexpr safe_cref_t _expect(const _M* _p)const noexcept {
			if (_m_constructed) IRON_LIKELY
			{
				return _get_ref();
			}
			PANIC(_p);
		}
		template <typename _M>
		[[nodiscard]] constexpr safe_ref_t _expect(const _M* _p)noexcept {
			if (_m_constructed) IRON_LIKELY
			{
				return _get_ref();
			}
			PANIC(_p);
		}
		bool _m_constructed{};
		_rw_t _m_v{};
		IRON_SECURITY_DCL();
	};


	template <typename _T>
	[[nodiscard]] inline constexpr option<std::decay_t<_T>> none()noexcept { return {}; }

	template <typename _T>
	[[nodiscard]] inline constexpr option<std::decay_t<_T>>  some(_T&& _v)noexcept
	requires (iron::concepts::nothrow_constructible_at<std::decay_t<_T>,_T&&>)
	{
		return option<std::decay_t<_T>>::some(std::forward<_T>(_v));
	}

	template <typename _T, typename... _ARGS>
	[[nodiscard]] inline constexpr option<_T>  some_emplace(_ARGS&&... _args)noexcept 
	requires (iron::concepts::nothrow_constructible_at<_T,_ARGS&&...>)
	{
		return option<_T>::some(std::forward<_ARGS>(_args)...);
	}

}

#ifdef IRON_NO_USE_MODULE
#include "iron/result.h"
#endif 

namespace iron {
	template <typename _T>
	inline constexpr auto option<_T>::try_clone()const noexcept -> iron::result<option, error_handler_t> 
	requires (iron::concepts::copyable_or_cloneable<value_t>)
	{
		IRON_SECURITY_CHECK_TEMPORARY_THIS();
		option _opt;
		if (_m_constructed) {
			auto& _v = _m_v.value();
			auto _r = iron::clone::try_clone_from(_v); RR(_r);
			_opt._m_v.construct(std::move(_r.unchecked_unwrap()));
			_opt._m_constructed = true;
		}
		return iron::ok(std::move(_opt));
	}
	template <typename _T>
	inline constexpr auto  option<_T>::clone()const noexcept -> option 
	requires (iron::concepts::copyable_or_cloneable<value_t>)
	{
		auto _r = try_clone(); RP(_r);
	}


} //ns

static_assert(iron::concepts::option<iron::option<int>>, "internal error");


