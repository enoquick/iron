
#ifndef __cplusplus
#error this compiler its not c++
#else
#if defined(_MSC_VER)
#if _MSVC_LANG >= 202002L
#else
#error c++20 or superior is required
#endif
#else
#if  __cplusplus >= 202002L
#else
#error c++20 or superior is required
#endif
#endif
#endif

#undef IRON_LIKELY
#define IRON_LIKELY [[likely]]
#undef IRON_UNLIKELY
#define IRON_UNLIKELY [[unlikely]]

#undef IRON_NO_UNIQUE_ADDRESS
#ifdef _MSC_VER
#define IRON_NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
#elif defined(__GNUC__) || defined(__clang__)
#define IRON_NO_UNIQUE_ADDRESS [[no_unique_address]]
#else
#define IRON_NO_UNIQUE_ADDRESS
#endif

#undef IRON_EBCDIC
#ifdef _EBCDIC_
#define IRON_EBCDIC
#endif

#undef STRINGIFY
#define STRINGIFY(_s) #_s

#undef TODO
#ifdef NDEBUG
#define TODO() iron::unsafe::asserts::void0()
#else
#define TODO() \
	do { \
		if (!std::is_constant_evaluated()) { \
			iron::unsafe::asserts::print("TODO: (%s,%d)\n",__FILE__,__LINE__); \
		} \
	} while(0)
#endif

#undef PANIC
#define PANIC(_m) iron::unsafe::asserts::do_panic(__func__,__FILE__,__LINE__,_m)

#define PANIC_IF(_expr,_m) \
	do { \
		if (_expr) IRON_UNLIKELY { \
			PANIC(_m); \
		} \
	} while(0)

#undef PANIC_UNLESS
#define PANIC_UNLESS(_expr,_m) \
	do { \
		if (!(_expr)) IRON_UNLIKELY { \
			PANIC(_m); \
		} \
	} while(0)


#undef ASSERT
#define ASSERT(_expr) \
	do { \
		if (!(_expr)) IRON_UNLIKELY { \
			iron::unsafe::asserts::do_assert(iron::unsafe::asserts::type_t::bool_assert,__func__,__FILE__,__LINE__,#_expr,nullptr); \
		} \
	} while(0)


#undef ASSERT_EQ
#define ASSERT_EQ(_expr1,_expr2) \
	do { \
		if (!((_expr1)  == (_expr2))) IRON_UNLIKELY { \
			iron::unsafe::asserts::do_assert(iron::unsafe::asserts::type_t::assert_eq,__func__,__FILE__,__LINE__,#_expr1,#_expr2); \
		} \
	} while(0)



#undef ASSERT_NE
#define ASSERT_NE(_expr1,_expr2) \
	do { \
		if ((_expr1)  == (_expr2)) IRON_UNLIKELY { \
			iron::unsafe::asserts::do_assert(iron::unsafe::asserts::type_t::assert_ne,__func__,__FILE__,__LINE__,#_expr1,#_expr2); \
		} \
	} while(0)

#undef DEBUG_ASSERT
#undef DEBUG_ASSERT_EQ
#undef DEBUG_ASSERT_NE

#ifdef NDEBUG
#define DEBUG_ASSERT(_expr) iron::unsafe::asserts::void0()
#define DEBUG_ASSERT_EQ(_expr1,_expr2) iron::unsafe::asserts::void0()
#define DEBUG_ASSERT_NE(_expr1,_expr2) iron::unsafe::asserts::void0()
#else
#define DEBUG_ASSERT(expr) ASSERT(expr)
#define DEBUG_ASSERT_EQ(_expr1,_expr2) ASSERT_EQ(_expr1,_expr2)
#define DEBUG_ASSERT_NE(_expr1,_expr2) ASSERT_NE(_expr1,_expr2)
#endif

#undef ERROR_HANDLER
#define ERROR_HANDLER(_x) iron::error_handler::make(_x,__FILE__,__LINE__,__func__)

#undef IRON_SECURITY_CHECKER
#ifndef NDEBUG
#define IRON_SECURITY_CHECKER 1
#else
#define IRON_SECURITY_CHECKER 0
#endif

#undef IRON_SECURITY_CHECK_TEMPORARY_THIS
#undef IRON_SECURITY_CHECK_TEMPORARY
#undef IRON_SECURITY_MARK_TEMPORARY
#undef IRON_SECURITY_CHECK_REF_ACTIVES_THIS
#undef IRON_SECURITY_CHECK_REF_ACTIVES
#undef IRON_SECURITY_CHECK_MASTER_LIVE
#undef IRON_SECURITY_CHECK_MASTER_LIVE_THIS
#undef IRON_SECURITY_CHECK_TEMPORARY_ARGS
#undef IRON_SECURITY_DCL
#undef IRON_SECURITY_SWAP
#undef IRON_SECURITY_METHOD
#undef IRON_SECURITY_ASSIGN
#undef IRON_SECURITY_MAKE_REF
#undef IRON_SECURITY_MAKE_REF_TO
#undef IRON_SECURITY_REQ
#undef IRON_SECURITY_RESET_THIS
#undef IRON_SECURITY_RESET

#if IRON_SECURITY_CHECKER
#define IRON_SECURITY_CHECK_TEMPORARY_THIS() \
	this->checker().check_for_temporary()

#define IRON_SECURITY_CHECK_TEMPORARY(_v) \
	do { \
		if constexpr (iron::concepts::has_security_checker<decltype(_v)>) { \
			(_v).checker().check_for_temporary(); \
		} \
	} while(0)

#define IRON_SECURITY_MARK_TEMPORARY(_v) \
	(_v)._m_ck.mark_temporary()

#define IRON_SECURITY_CHECK_REF_ACTIVES_THIS() \
	this->checker().check_ref_actives()

#define IRON_SECURITY_CHECK_REF_ACTIVES(_v) \
	do { \
		if constexpr (iron::concepts::has_security_checker<decltype(_v)>) { \
			(_v).checker().check_ref_actives(); \
		} \
	} while(0)


#define IRON_SECURITY_CHECK_TEMPORARY_ARGS(...) \
	iron::unsafe::security_checker::check_args<iron::unsafe::security_checker::check_type_t::temporary>(__VA_ARGS__)

#define IRON_SECURITY_CHECK_MASTER_LIVE_THIS() \
	this->checker().check_master_for_live()

#define IRON_SECURITY_CHECK_MASTER_LIVE(_v) \
	do { \
		if constexpr (iron::concepts::has_security_checker<decltype(_v)>) { \
			(_v).checker().check_master_for_live(); \
		} \
	} while(0)


#define IRON_SECURITY_DCL() \
	iron::unsafe::security_checker _m_ck{}

#define IRON_SECURITY_SWAP(_v) \
	this->_m_ck.swap((_v)._m_ck)

#define IRON_SECURITY_METHOD() \
	[[nodiscard]] constexpr const iron::unsafe::security_checker& checker()const noexcept { return this->_m_ck; }

#define IRON_SECURITY_ASSIGN(_v) \
	this->_m_ck = (_v)._m_ck

#define IRON_SECURITY_MAKE_REF(_v) \
	this->_m_ck = (_v).checker().make_ref()

#define IRON_SECURITY_MAKE_REF_TO(_to,_v) \
	(_to)._m_ck = (_v).checker().make_ref()

#define IRON_SECURITY_REQ() \
	requires(iron::concepts::has_security_checker<_MASTER>)

#define IRON_SECURITY_RESET_THIS() \
	this->_m_ck.reset()

#define IRON_SECURITY_RESET(_v) \
	(_v)._m_ck.reset()

#else
#define IRON_SECURITY_CHECK_TEMPORARY_THIS()
#define IRON_SECURITY_CHECK_TEMPORARY(_v)
#define IRON_SECURITY_MARK_TEMPORARY(_v)
#define IRON_SECURITY_CHECK_REF_ACTIVES_THIS()
#define IRON_SECURITY_CHECK_REF_ACTIVES(_v)
#define IRON_SECURITY_CHECK_TEMPORARY_ARGS(...)
#define IRON_SECURITY_CHECK_MASTER_LIVE_THIS()
#define IRON_SECURITY_CHECK_MASTER_LIVE(_v)
#define IRON_SECURITY_DCL()
#define IRON_SECURITY_SWAP(_v)
#define IRON_SECURITY_METHOD()
#define IRON_SECURITY_ASSIGN(_v)
#define IRON_SECURITY_MAKE_REF(_v)
#define IRON_SECURITY_MAKE_REF_TO(_to,_v)
#define IRON_SECURITY_REQ()
#define IRON_SECURITY_RESET_THIS()
#define IRON_SECURITY_RESET(_v)
#endif


/*** support macros RE|RR|... ****/


#undef RE
#define RE(_param) \
	do { \
		if (!_param) IRON_UNLIKELY { \
			return  iron::unsafe::asserts::re(_param); \
		} \
	} while(false)

#undef RR
#define RR(_param) \
	do { \
		if (!_param) IRON_UNLIKELY { \
			return iron::unsafe::asserts::rr(_param); \
		} \
	} while(false)

#undef RP
#define RP(_param) \
	do { \
		if (_param)  IRON_LIKELY { \
			return std::move(_param.unchecked_unwrap()); \
		} \
		_param.unchecked_unwrap_err().do_panic(); \
	} while(false)

#undef RVP
#define RVP(_param,_ret) \
	do { \
		if (_param) IRON_LIKELY { \
			return _ret; \
		} \
		iron::unsafe::asserts::r1(_param); \
	} while(false)



#ifdef _MSC_VER
#if _MSVC_LANG >= 202002L  // from c++20
#ifdef USE_MODULES
#undef IRON_NO_USE_MODULES
#else
#define IRON_NO_USE_MODULE
#endif
#endif
#else //other compilers
#ifdef __cpp_modules
#undef IRON_NO_USE_MODULE
#else
#define IRON_NO_USE_MODULE
#endif
#endif

/* macros for exports */

#ifdef IRON_NO_USE_MODULE
#define IRON_MODULE_EXPORT
#else
#define IRON_MODULE_EXPORT export
#endif


#if defined(_MSC_VER) //visual c++
#ifdef IRON_EXPORTS
#define _IRON_DLL_EXPORT   __declspec( dllexport )
#else //IRON_EXPORTS
#define _IRON_DLL_EXPORT   __declspec( dllimport )
#ifdef IRON_NO_USE_MODULE
#pragma comment( lib, "iron")
#else
#pragma comment( lib, "iron_modules")
#endif //!IRON_NO_USE_MODULE
#endif  //!IRON_EXPORTS
#elif defined(__clang__) || defined(__GNUC__) //clang++ || g++
#ifdef IRON_EXPORTS
#define _IRON_DLL_EXPORT  __attribute__((__visibility__("default")))
#else
#define _IRON_DLL_EXPORT
#endif
#else
#error unknow compiler
#endif

#define IRON_DLL_EXPORT _IRON_DLL_EXPORT

#ifdef IRON_NO_USE_MODULE
#define _IRON_MODULES_DLL_EXPORT
#else
#define _IRON_MODULES_DLL_EXPORT  _IRON_DLL_EXPORT
#endif
