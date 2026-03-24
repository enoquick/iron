

namespace iron  {

	class clone final {
	public:
		using error_handler_t=iron::error_handler;
		clone()=delete;

		template <typename _T>
		[[nodiscard]] static constexpr result<_T, error_handler_t> try_clone_from(const _T&)noexcept
		;

		template <typename _T>
		[[nodiscard]] static constexpr _T clone_from(const _T&)noexcept;

		template <typename _T,std::size_t _N>
		static void try_clone_from(const _T(&)[_N]) = delete;

		template <typename _T, std::size_t _N>
		static void clone_from(const _T(&)[_N]) = delete;

		template <typename _U, typename _T,std::size_t _N>
		static _U try_clone_from_to(const _T(&)[_N])=delete;

		template <typename _U, typename _T, std::size_t _N>
		static _U clone_from_to(const _T(&)[_N]) = delete;

		template <typename _U, typename _T>
		[[nodiscard]] static constexpr result<_U, error_handler_t> try_clone_from_to(const _T&)noexcept;

		template <typename _U,typename _T>
		[[nodiscard]] static constexpr _U clone_from_to(const _T& _v)noexcept;


	private:
		template <typename _U,typename _T>
		[[nodiscard]] static constexpr result<_U, error_handler_t> _clone(const _T& _v)noexcept;

		template<typename _T, std::enable_if_t<std::tuple_size_v<std::decay_t<_T>> >= 0, int> = 0>
		[[nodiscard]] static constexpr result<_T, error_handler_t> _clone_tuple(const _T& _v)noexcept;

		template<typename _T>
		[[nodiscard]] static constexpr _T _clone_or_copy_throw(const _T& _v)noexcept(false);

	};
}

#ifdef IRON_NO_USE_MODULE
#include "iron/error_handler.h"
#include "iron/result.h"
#endif 

namespace iron {
	
	template <typename _T>
	inline constexpr auto clone::try_clone_from(const _T& _v)noexcept -> result<_T,error_handler_t> 
	{ 
		return _clone<_T,_T>(_v);
	}
	
	template <typename _T>
	inline constexpr _T clone::clone_from(const _T& _v)noexcept 
	{
		auto _r = try_clone_from(_v); RP(_r);
	}


	template <typename _U,typename _T>
	inline constexpr auto clone::try_clone_from_to(const _T& _v)noexcept -> result<_U,error_handler_t> 
	{ 
		return _clone<_U,_T>(_v);
	}

	template <typename _U,typename _T>
	inline constexpr _U clone::clone_from_to(const _T& _v)noexcept 
	{
		auto _r = try_clone_from_to<_U>(_v); RP(_r);
	}

	template<typename _T>
	inline constexpr  _T clone::_clone_or_copy_throw(const _T& _v)noexcept(false) {
		auto _r = try_clone_from(_v);
		if (_r) IRON_LIKELY 
		{
			return std::move(_r.unchecked_unwrap());
		}
		auto _h = std::move(_r.unchecked_unwrap_err());
		throw _h;
	}

	template <typename _T, std::enable_if_t<std::tuple_size_v<std::decay_t<_T>> >= 0, int>>
	inline constexpr auto clone::_clone_tuple(const _T& _v)noexcept -> result<_T, error_handler_t> {
		return std::apply([](const auto&... _elements)noexcept -> result<_T, error_handler_t>  {
				try {
					return iron::ok(_T(_clone_or_copy_throw(_elements)...));
				}
				catch (const iron::error_handler& _h) {
					DEBUG_ASSERT(!_h);
					return iron::err(_h);
				}
				catch (...) {
					return iron::err(ERROR_HANDLER(std::current_exception()));
				}
			}, _v
		);
	}

	template <typename _U,typename _T>
	inline constexpr auto  clone::_clone(const _T& _v)noexcept -> result<_U, error_handler_t> {
		using _dest_t = std::decay_t<_U>;
		using _src_t = std::decay_t<_T>;
		if constexpr (iron::concepts::std_tuple<_src_t>) {
			if constexpr (std::is_same_v<_src_t, _dest_t>) {
				return _clone_tuple(_v);
			}
			else {
				static_assert(iron::unsafe::asserts::always_false<_dest_t>, "for clone tuple the destination must be the same tuple type");
			}
		}
		else if constexpr (std::is_same_v<_dest_t, _src_t>) {
			if constexpr (iron::concepts::cloneable<_src_t>) {
				if constexpr (std::is_nothrow_move_constructible_v<_src_t>) {
					return _v.try_clone();
				}
				else {
					static_assert(iron::unsafe::asserts::always_false<_src_t>, "value_t is cloneable but is not nothow move constructible");
				}
			}
			else if constexpr (std::is_nothrow_copy_constructible_v<_src_t>) {
				return iron::ok(_v);
			}
			else if constexpr (std::is_copy_constructible_v<_src_t>) {
				if constexpr (std::is_nothrow_move_constructible_v<_src_t>) {
					try {
						return iron::ok(_src_t{ _v });
					}
					catch (...) {
						return iron::err(ERROR_HANDLER(std::current_exception()));
					}
				}
				else {
					static_assert(iron::unsafe::asserts::always_false<_src_t>, "value_t is throw copy contructible but is not nothow move constructible");
				}
			}
			else {
				static_assert(iron::unsafe::asserts::always_false<_src_t>, "value_t is not copy contructible or cloneable");
			}
		}
		else if constexpr (std::is_nothrow_convertible_v<_src_t,_dest_t>) {
			if constexpr (std::is_nothrow_move_constructible_v<_dest_t>) {
				_dest_t _u{ _v };
				return iron::ok(std::move(_u));
			}
			else {
				static_assert(iron::unsafe::asserts::always_false<_dest_t>, "value_t is nothrow convertible to DEST but DEST is not nothrow move constructible");
			}
		}
		else if constexpr (std::is_convertible_v<_src_t,_dest_t>) {
			if constexpr (std::is_nothrow_move_constructible_v<_dest_t>) {
				try {
					_dest_t _u{ _v };
					return iron::ok(std::move(_u));
				}
				catch (...) {
					return iron::err(ERROR_HANDLER(std::current_exception()));
				}
			}
			else {
				static_assert(iron::unsafe::asserts::always_false<_dest_t>, "value_t is throw convertible to DEST but DEST is not nothrow move constructible");
			}
		}
		else {
			static_assert(iron::unsafe::asserts::always_false<_dest_t>, "value_t is not convertible into DEST");
		}
	} // _clone
	namespace concepts {
		IRON_MODULE_EXPORT template <typename _T>
			concept copyable_or_cloneable = std::is_copy_constructible_v<_T> || cloneable<_T> || requires(_T _v) {
				{ iron::clone::clone_from(_v)} noexcept -> std::same_as<_T>;

		};

		IRON_MODULE_EXPORT template <typename _T>
		concept nothrow_copyable_or_cloneable = std::is_nothrow_copy_constructible_v<_T> || copyable_or_cloneable<_T>;

		IRON_MODULE_EXPORT template<typename _T>
		concept std_tuple_copyable_or_cloneable =
				std_tuple<_T> && (std::tuple_size_v<_T> == 0 || []<std::size_t... _I>(std::index_sequence<_I...>) {
				return (copyable_or_cloneable<std::tuple_element_t<_I, _T>> && ...);
			}(std::make_index_sequence<std::tuple_size_v<_T>>{})
		);

	}
} //iron
