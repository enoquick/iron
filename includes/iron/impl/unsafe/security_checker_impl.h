
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable:26465)
#endif

namespace iron::unsafe {

	class security_checker {
	public:
		struct security_checker_tag { security_checker_tag() = delete; };
#if IRON_SECURITY_CHECKER
		using size_t = std::size_t;
		typedef enum class status {
			master
			, master_temporary
			, ref
			, ref_temporary
		} status_t;
		typedef enum class enum_check_type {
			temporary
			, servant_active
		} check_type_t;
		constexpr security_checker()noexcept
			: security_checker(status_t::master)
		{}
		constexpr security_checker(const security_checker& _v)noexcept
			:security_checker(_v._m_status)
		{
			DEBUG_ASSERT(!_v.temporary());
			if (_v.ref()) {
				auto _t = _v.make_ref();
				std::swap(_m_sh, _t._m_sh);
			}
		}
		constexpr security_checker(security_checker&& _v)noexcept
			: security_checker(_v._m_status)
		{
			DEBUG_ASSERT(!_v.temporary());
			std::swap(_m_sh, _v._m_sh);
			_v.mark_temporary();
		}
		constexpr security_checker& operator=(const security_checker& _v)noexcept {
			DEBUG_ASSERT(!_v.temporary());
			security_checker _t(_v);
			swap(_t);
			return *this;
		}
		constexpr security_checker& operator=(security_checker&& _v)noexcept {
			DEBUG_ASSERT(!_v.temporary());
			security_checker _t(std::move(_v));
#ifndef USE_MODULES //for a bug in msvc constexpr eval and use modules
			DEBUG_ASSERT(_v.temporary());
#endif
			swap(_t);
			return *this;
		}
		constexpr void reset()noexcept {
			if (!std::is_constant_evaluated()) {
				_reset(_m_sh, master());
			}
			_m_status = temporary() ? status_t::master_temporary : status_t::master;
		}
		constexpr ~security_checker() {
			reset();
		}
		[[nodiscard]] constexpr security_checker make_ref()const {
			DEBUG_ASSERT(!temporary());
			if (std::is_constant_evaluated()) { //for a bug in msvc constexpr eval not use security_checker _t(status_t::reference);
				security_checker _t;
				_t._m_status = status_t::ref;
				return _t;
			}
			else {
				security_checker _t(status_t::ref);
				auto& _p = const_cast<_shared_data_t*&>(_m_sh);
				_make_ref(_p,master());
				PANIC_UNLESS(_m_sh, "no such memory");
				_t._m_sh = _m_sh;
				return _t;
			}
		}
		[[nodiscard]] constexpr status_t status()const noexcept { return _m_status; }
		[[nodiscard]] constexpr bool temporary()const noexcept {
			switch (_m_status) {
				case status_t::ref_temporary:
				case status_t::master_temporary: return true;
				default:;
			}
			return {};
		}
		[[nodiscard]] constexpr bool master()const noexcept {
			switch (_m_status) {
				case status_t::master:
				case status_t::master_temporary: return true;
				default:;
			}
			return {};
		}
		[[nodiscard]] constexpr bool master_is_live()const noexcept {
			if (!std::is_constant_evaluated()) {
				return _master_is_live(_m_sh);
			}
			return true;
		}
		[[nodiscard]] constexpr bool ref()const noexcept {
			switch (_m_status) {
				case status_t::ref:
				case status_t::ref_temporary: return true;
				default:;
			}
			return {};
		}
		[[nodiscard]] constexpr size_t ref_count()const noexcept {
			if (!std::is_constant_evaluated()) {
				return _ref_count(_m_sh);
			}
			return {};
		}
		[[nodiscard]] constexpr bool has_ref_active()const noexcept {
			return ref_count() ? true : false;
		}
		constexpr void check_ref_actives()const noexcept {
			if (!std::is_constant_evaluated()) {
				PANIC_IF(has_ref_active(), "attempt to modify a container with reference active");
			}
		}
		constexpr void mark_temporary() {
			if (master()) {
				_m_status = status_t::master_temporary;
			}
			else {
				_m_status = status_t::ref_temporary;
			}
		}
		constexpr void swap(security_checker& _v)noexcept {
			std::swap(_m_status, _v._m_status);
			std::swap(_m_sh, _v._m_sh);
		}
		constexpr void check_for_temporary()const noexcept {
			PANIC_IF(temporary(), "attempt to use a temporary object");
		}
		constexpr void check_master_for_live()const noexcept {
			if (!std::is_constant_evaluated()) {
				PANIC_UNLESS(master_is_live(), "master is not alive");
			}
		}
		[[nodiscard]] constexpr bool same_data(const security_checker& _v)const noexcept {
			if (!std::is_constant_evaluated()) {
				return _same_data(_m_sh, _v._m_sh);
			}
			return true;
		}
		template <check_type_t _check_type, typename... _ARGS>
		static constexpr void check_args(const _ARGS&... _args)noexcept {
			_check_args<_check_type>(_args...);
		}

#else
		security_checker() = delete;
#endif
	private:
#if IRON_SECURITY_CHECKER
		typedef struct _shared_data {
			size_t _counter;
			bool _master_live;
		} _shared_data_t;
		constexpr explicit security_checker(status_t _st)noexcept
			: _m_status(_st)
			, _m_sh()
		{
		}
		template <check_type_t _check_type>
		static constexpr void _check_args()noexcept {}
		template <check_type_t _check_type, typename _T, typename... _ARGS>
		static constexpr void _check_args(const _T& _v, const _ARGS&... _args)noexcept {
			if constexpr (_check_type == check_type_t::temporary) {
				if constexpr (iron::concepts::has_security_checker<std::decay_t<_T>>) {
					_v.checker().check_for_temporary();
				}
			}
			else if constexpr (_check_type == check_type_t::servant_active) {
				if constexpr (iron::concepts::has_security_checker<std::decay_t<_T>>) {
					_v.checker().check_servant_actives();
				}
			}
			else {
				static_assert(iron::unsafe::asserts::always_false<_T>, "internal error - unknow check_type value");
			}
			_check_args<_check_type>(_args...);
		}
		_IRON_DLL_EXPORT static size_t _ref_count(_shared_data_t*const&)noexcept;
		_IRON_DLL_EXPORT static void _make_ref(_shared_data_t*&,bool)noexcept;
		_IRON_DLL_EXPORT static void _reset(_shared_data_t*&, bool)noexcept;
		_IRON_DLL_EXPORT static bool _master_is_live(_shared_data_t*const&)noexcept;
		_IRON_DLL_EXPORT static bool _same_data(_shared_data_t*const&,_shared_data_t*const&)noexcept;
		status_t _m_status;
		_shared_data_t* _m_sh;
#endif
	};

} //ns

static_assert(iron::concepts::security_checker<iron::unsafe::security_checker>, "internal error");


#if defined(_MSC_VER)
#pragma warning(pop)
#endif

