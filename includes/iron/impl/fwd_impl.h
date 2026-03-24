
namespace iron {

	namespace unsafe {
		IRON_MODULE_EXPORT class asserts;
		IRON_MODULE_EXPORT class security_checker;
		IRON_MODULE_EXPORT template <typename _T> class raw_wrapper;
		IRON_MODULE_EXPORT template <typename _T> class base_allocator;
		IRON_MODULE_EXPORT template <typename _T> class raw_storage;
		IRON_MODULE_EXPORT class stacktrace_entry;
		IRON_MODULE_EXPORT class stacktrace;
		IRON_MODULE_EXPORT class unicode;
		IRON_MODULE_EXPORT class unicode_conversions;
		IRON_MODULE_EXPORT template <typename _T, typename _A> class raw_vector;
		IRON_MODULE_EXPORT template <typename _T, bool _ISBD> class  list_node;
		IRON_MODULE_EXPORT template <typename _NODE, typename _A> class raw_list;
		IRON_MODULE_EXPORT template <typename _KEY, typename _MAPPED, typename _EQ, typename _HASHER, typename _A, bool _MULTI, bool _HAS_MAPPED>
		class raw_hash_table;
		IRON_MODULE_EXPORT template <typename _T> using allocator = base_allocator<_T>;

		namespace btree {
			IRON_MODULE_EXPORT template <typename _KEY, typename _MAPPED, typename _LT, std::size_t _ORDER, typename _A, bool _MULTI, bool _HAS_MAPPED>
			class raw_table;
		}
		namespace str {
			IRON_MODULE_EXPORT template <typename _T, typename _A, std::size_t _FIXED_SIZE, bool _SAFE>
			class raw_str;
			IRON_MODULE_EXPORT template <typename _T, bool _SAFE>
			class raw_strview;
		}
		namespace iterators {
			IRON_MODULE_EXPORT template <typename _T, typename _INDEX> class btree_iterator;
			IRON_MODULE_EXPORT template <typename _T> class contiguous_iterator;
			IRON_MODULE_EXPORT template<typename _ITER, typename _ENUMERATE, typename _REFERENCE> class enumerate_iterator;
			IRON_MODULE_EXPORT template<typename _ITER, typename _PREDICATE> class filter_iterator;
			IRON_MODULE_EXPORT template <typename _T, typename _RAW_LIST> class hash_iterator;
			IRON_MODULE_EXPORT template <typename _T, typename _RAW_LIST> class list_iterator;
			IRON_MODULE_EXPORT template<typename _ITER> class step_iterator;
			IRON_MODULE_EXPORT template<typename _ITER> class take_iterator;
			IRON_MODULE_EXPORT template <typename _T> class unicode_iterator;

			IRON_MODULE_EXPORT template <typename _T>
			[[nodiscard]] constexpr contiguous_iterator<_T> operator+(const contiguous_iterator<_T>& _v, typename contiguous_iterator<_T>::difference_type _n)noexcept;

			IRON_MODULE_EXPORT template <typename _T>
			[[nodiscard]] constexpr contiguous_iterator<_T> operator+(typename contiguous_iterator<_T>::difference_type _n, const contiguous_iterator<_T>& _v)noexcept;

			IRON_MODULE_EXPORT template <typename _T>
			[[nodiscard]] constexpr contiguous_iterator<_T> operator-(const contiguous_iterator<_T>& _v, typename contiguous_iterator<_T>::difference_type _n)noexcept;

		}
	}
	IRON_MODULE_EXPORT class cfg;

	/*IRON_MODULE_EXPORT template<typename E> class enum_class;*/

	IRON_MODULE_EXPORT class error_handler;

	namespace unsafe::errors {
		IRON_MODULE_EXPORT class base;
		IRON_MODULE_EXPORT class base_with_info;
		IRON_MODULE_EXPORT class os;
		IRON_MODULE_EXPORT class posix;
		IRON_MODULE_EXPORT class exception;
	}

	IRON_MODULE_EXPORT template <typename _T> class ref_wrapper;

	IRON_MODULE_EXPORT template <typename _MASTER, typename _T>
	[[nodiscard]] constexpr ref_wrapper<_T> make_ref(const _MASTER&, _T&)noexcept
		requires(!iron::concepts::ref_wrapper<_T>);

	IRON_MODULE_EXPORT template <typename _T>
	[[nodiscard]] constexpr auto make_ref(_T&)noexcept
		requires(iron::concepts::ref_wrapper<_T>);

	IRON_MODULE_EXPORT template <typename _MASTER, typename _T>
	[[nodiscard]] constexpr auto make_ref(const _MASTER&, _T&)noexcept
		requires(iron::concepts::ref_wrapper<_T>);

	IRON_MODULE_EXPORT template <typename _MASTER, typename _T>
		[[nodiscard]] constexpr ref_wrapper<const _T> make_cref(const _MASTER&, const _T&)noexcept
		requires(!iron::concepts::ref_wrapper<_T>);

	IRON_MODULE_EXPORT template <typename _T>
	[[nodiscard]] constexpr auto make_cref(_T&)noexcept
		requires(iron::concepts::ref_wrapper<_T>);

	IRON_MODULE_EXPORT template <typename _MASTER, typename _T>
	[[nodiscard]] constexpr auto make_cref(const _MASTER&, _T&)noexcept
		requires(iron::concepts::ref_wrapper<_T>);

	//
	IRON_MODULE_EXPORT class clone;

	IRON_MODULE_EXPORT template<typename _T> class option;

	IRON_MODULE_EXPORT template <typename _T = void*>
	[[nodiscard]] constexpr option<std::decay_t<_T>> none()noexcept;

	IRON_MODULE_EXPORT template <typename _T>
	[[nodiscard]] constexpr option<std::decay_t<_T>> some(_T&&)noexcept
	requires (iron::concepts::nothrow_constructible_at<std::decay_t<_T>, _T&&>);


	IRON_MODULE_EXPORT template <typename _T, typename... _ARGS>
	[[nodiscard]] inline constexpr option<_T>  some_emplace(_ARGS&&... _args)noexcept
	requires (iron::concepts::nothrow_constructible_at<_T, _ARGS&&...>);


	//result
	IRON_MODULE_EXPORT template<typename _T, typename _E> class result;

	IRON_MODULE_EXPORT template <typename _T, typename _E = void*>
	[[nodiscard]] constexpr auto ok(_T&&)noexcept;

	IRON_MODULE_EXPORT template <typename _E, typename _T = void*>
	[[nodiscard]] constexpr auto err(_E&&)noexcept;

	namespace memory {
		IRON_MODULE_EXPORT template <bool _RECURSIVE> class base_mutex;
		IRON_MODULE_EXPORT using mutex = base_mutex<false>;
		IRON_MODULE_EXPORT using recursive_mutex = base_mutex<true>;

		IRON_MODULE_EXPORT template <typename _MUTEX> class base_condition_variable;
		IRON_MODULE_EXPORT using condition_variable = base_condition_variable<mutex>;

		IRON_MODULE_EXPORT template <typename _T> class lock_guard;
		IRON_MODULE_EXPORT template <bool _RECURSIVE> class base_spin_lock;
		IRON_MODULE_EXPORT using spin_lock = base_spin_lock<false>;
		IRON_MODULE_EXPORT using recursive_spin_lock = base_spin_lock<true>;
		IRON_MODULE_EXPORT template <typename _T>
		[[nodiscard]] lock_guard<_T> make_lock_guard(_T& _lock)noexcept;
	}

	IRON_MODULE_EXPORT template <typename _T, typename _A> class box_deleter;
	IRON_MODULE_EXPORT template <typename _T, typename _D> class base_box;
	IRON_MODULE_EXPORT template <typename _T, typename _D = box_deleter<_T, iron::unsafe::allocator<_T>>>
	using box = base_box<_T, _D>;

	IRON_MODULE_EXPORT template <typename _T>
	[[nodiscard]] constexpr iron::box<_T> make_empty_box()noexcept;

	IRON_MODULE_EXPORT template <typename _T, typename... _ARGS>
	[[nodiscard]] constexpr iron::result<iron::box<_T>, iron::error_handler> try_make_box(_ARGS&&... _args)noexcept;

	IRON_MODULE_EXPORT template <typename _T, typename... _ARGS>
	[[nodiscard]] constexpr iron::box<_T> make_box(_ARGS&&... _args)noexcept;

	IRON_MODULE_EXPORT template <typename _CONTAINER> class adapter_box;

	namespace _shared_impl { //flag for shared boxes
		static constexpr int _flag_thread_safe = 1;
		static constexpr int _flag_weak = 4;
	}

	template <typename _T, typename _D, int _FLAGS> //not exported
	class base_shared_box;

	IRON_MODULE_EXPORT template <typename _T, typename _D>
	using base_rc = base_shared_box<_T, _D, 0>;

	IRON_MODULE_EXPORT template <typename _T, typename _D = box_deleter<_T,iron::unsafe::allocator<_T>>>
	using rc = base_rc<_T, _D>;

	IRON_MODULE_EXPORT template <typename _T, typename _D = box_deleter<_T,iron::unsafe::allocator<_T>>>
	using rcw = typename base_rc<_T,_D>::weak_t;

	IRON_MODULE_EXPORT template <typename _T, typename _D>
	using base_arc = base_shared_box<_T, _D, _shared_impl::_flag_thread_safe>;

	IRON_MODULE_EXPORT template <typename _T, typename _D = box_deleter<_T,iron::unsafe::allocator<_T>>>
	using arc = base_arc<_T, _D>;

	IRON_MODULE_EXPORT template <typename _T, typename _D = box_deleter<_T,iron::unsafe::allocator<_T>>>
	using arcw = typename base_arc<_T,_D>::weak_t;


	IRON_MODULE_EXPORT template <typename _T, typename... _ARGS>
	[[nodiscard]] constexpr iron::result<iron::rc<_T>, iron::error_handler> try_make_rc(_ARGS&&... _args)noexcept;

	IRON_MODULE_EXPORT template <typename _T, typename... _ARGS>
	[[nodiscard]] constexpr iron::rc<_T> make_rc(_ARGS&&... _args)noexcept;

	IRON_MODULE_EXPORT template <typename _T, typename... _ARGS>
	[[nodiscard]] iron::result<iron::arc<_T>, iron::error_handler> try_make_arc(_ARGS&&... _args)noexcept;

	IRON_MODULE_EXPORT template <typename _T, typename... _ARGS>
	[[nodiscard]] iron::arc<_T> make_arc(_ARGS&&... _args)noexcept;

	IRON_MODULE_EXPORT template <std::size_t> class bit_set;

	IRON_MODULE_EXPORT template <std::size_t _SZ, typename... _ARGS>
	[[nodiscard]] constexpr iron::bit_set<_SZ> make_bit_set(const _ARGS&... _v)noexcept
	requires (_SZ > 0 && (iron::concepts::nothrow_convertible_to_integral<_ARGS>&& ...));

	IRON_MODULE_EXPORT template <typename _T> class enum_set;

	IRON_MODULE_EXPORT template <typename _T>
	[[nodiscard]] constexpr iron::enum_set<_T> make_enum_set()noexcept
	requires(std::is_enum_v<_T>);

	IRON_MODULE_EXPORT template <typename _T, typename... _ARGS>
	[[nodiscard]] constexpr iron::enum_set<_T> make_enum_set(_T _v,_ARGS... _args)noexcept
	requires(std::is_enum_v<_T> && (std::is_same_v<_T, _ARGS>&& ...));

	IRON_MODULE_EXPORT template <typename _T, std::size_t _N> class array;

	IRON_MODULE_EXPORT class collector;

	IRON_MODULE_EXPORT template <typename _ITER>
	class sequence;

	IRON_MODULE_EXPORT template <typename _F, typename _S>
	class proxy_pair;

	IRON_MODULE_EXPORT template <typename _T>
	class hasher;

	namespace comparables {
		IRON_MODULE_EXPORT template <typename _T> class equal_to;
		IRON_MODULE_EXPORT template <typename _T> class less_to;
	}

	template <typename _T, typename _A,bool _ISBD> class base_list;

	namespace _impl {
		struct _empty {};
	}

	template <typename _KEY, typename _MAPPED, typename _EQ, typename _HASHER, typename _A, bool _MULTI, bool _HAS_MAPPED> //not exported
	class hash_table;

	//hasher !multi
	IRON_MODULE_EXPORT template <
		typename _KEY
		,typename _MAPPED
		,typename _EQ
		,typename _HASHER
		, typename _A
	>
	using base_hash_map = hash_table<_KEY, _MAPPED, _EQ, _HASHER,_A,false, true>;

	IRON_MODULE_EXPORT template <
		typename _KEY
		, typename _MAPPED
		, typename _EQ = iron::comparables::equal_to<_KEY>
		, typename _HASHER = iron::hasher<_KEY>
		, typename _A =iron::unsafe::allocator<std::pair<_KEY, _MAPPED>>
	>
	using hash_map = base_hash_map<_KEY, _MAPPED, _EQ, _HASHER, _A>;

	IRON_MODULE_EXPORT template <
		typename _KEY
		, typename _EQ
		, typename _HASHER
		, typename _A
	>
	using base_hash_set = hash_table<_KEY, _impl::_empty, _EQ, _HASHER,_A,false,false>;

	IRON_MODULE_EXPORT template <
		typename _KEY
		, typename _EQ = iron::comparables::equal_to<_KEY>
		, typename _HASHER = iron::hasher<_KEY>
		, typename _A =iron::unsafe::allocator<_KEY>
	>
	using hash_set = base_hash_set<_KEY,_EQ, _HASHER, _A>;

	//hasher multi

	IRON_MODULE_EXPORT template <
		typename _KEY
		, typename _MAPPED
		, typename _EQ
		, typename _HASHER
		, typename _A
	>
		using base_multi_hash_map = hash_table<_KEY, _MAPPED, _EQ, _HASHER, _A,true, true>;

	IRON_MODULE_EXPORT template <
		typename _KEY
		, typename _MAPPED
		, typename _EQ = iron::comparables::equal_to<_KEY>
		, typename _HASHER = iron::hasher<_KEY>
		, typename _A =iron::unsafe::allocator<std::pair<_KEY, _MAPPED>>
	>
		using multi_hash_map = base_multi_hash_map<_KEY, _MAPPED, _EQ, _HASHER, _A>;


	IRON_MODULE_EXPORT template <
		typename _KEY
		, typename _EQ
		, typename _HASHER
		, typename _A
	>
		using base_multi_hash_set = hash_table<_KEY, _impl::_empty, _EQ, _HASHER, _A, true, false>;

	IRON_MODULE_EXPORT template <
		typename _KEY
		, typename _EQ = iron::comparables::equal_to<_KEY>
		, typename _HASHER = iron::hasher<_KEY>
		, typename _A =iron::unsafe::allocator<_KEY>
	>
		using multi_hash_set = base_multi_hash_set<_KEY, _EQ, _HASHER, _A>;


	//btree
	template <typename _KEY, typename _MAPPED, typename _LT, std::size_t _ORDER, typename _A, bool _MULTI, bool _HAS_MAPPED>
	class btree_table;

	IRON_MODULE_EXPORT constexpr std::size_t btree_default_order = 3;

	IRON_MODULE_EXPORT template <
		typename _KEY
		, typename _MAPPED
		, typename _LT= iron::comparables::less_to<_KEY>
		, typename _A=iron::unsafe::allocator<std::pair<_KEY, _MAPPED>>
		, std::size_t _ORDER = btree_default_order
	>
	using btree_map = btree_table<_KEY, _MAPPED, _LT,_ORDER, _A, false, true>;

	IRON_MODULE_EXPORT template <
		typename _KEY
		, typename _LT = iron::comparables::less_to<_KEY>
		, typename _A =iron::unsafe::allocator<_KEY>
		, std::size_t _ORDER = btree_default_order
	>
	using btree_set = btree_table<_KEY, _impl::_empty, _LT, _ORDER, _A, false,false>;

	IRON_MODULE_EXPORT template <
		typename _KEY
		, typename _MAPPED
		, typename _LT = iron::comparables::less_to<_KEY>
		, typename _A =iron::unsafe::allocator<std::pair<_KEY, _MAPPED>>
		, std::size_t _ORDER = btree_default_order
	>
	using multi_btree_map = btree_table<_KEY, _MAPPED, _LT, _ORDER, _A,true, true>;

	IRON_MODULE_EXPORT template <
		typename _KEY
		, typename _LT = iron::comparables::less_to<_KEY>
		, typename _A =iron::unsafe::allocator<_KEY>
		, std::size_t _ORDER = btree_default_order
	>
	using multi_btree_set = btree_table<_KEY, _impl::_empty, _LT, _ORDER, _A, true, false>;


	IRON_MODULE_EXPORT class chr;
	template <typename _T,bool _SAFE> class base_strview;
	template <typename _T, typename _A, std::size_t _FIXED_SIZE,bool _SAFE> class base_str;

	///
	IRON_MODULE_EXPORT template <typename _T, typename _A, std::size_t _FIXED_SIZE>
	using  str = base_str<_T, _A, _FIXED_SIZE,true>;

	IRON_MODULE_EXPORT template <typename _T, typename _A, std::size_t _FIXED_SIZE>
	using  ustr = base_str<_T, _A, _FIXED_SIZE,false>;

	IRON_MODULE_EXPORT constexpr const std::size_t str_default_fixedsize = 30;

	//ustrb
	IRON_MODULE_EXPORT template <typename _A =iron::unsafe::allocator<char>, std::size_t _FIXED_SIZE = str_default_fixedsize>
	using base_ustrb = ustr<char, _A, _FIXED_SIZE>;
	IRON_MODULE_EXPORT using ustrb = base_ustrb<>;
	IRON_MODULE_EXPORT using uvstrb = base_strview<char, false>;

	//strb
	IRON_MODULE_EXPORT template <typename _A =iron::unsafe::allocator<char>, std::size_t _FIXED_SIZE = str_default_fixedsize>
	using base_strb = str<char, _A, _FIXED_SIZE>;
	IRON_MODULE_EXPORT using strb = base_strb<>;
	IRON_MODULE_EXPORT using vstrb = base_strview<char,true>;

	//ustr8
	IRON_MODULE_EXPORT template <typename _A=iron::unsafe::allocator<char8_t>,std::size_t _FIXED_SIZE= str_default_fixedsize>
	using base_ustr8 = ustr<char8_t, _A, _FIXED_SIZE>;
	IRON_MODULE_EXPORT using ustr8 = base_ustr8<>;
	IRON_MODULE_EXPORT using uvstr8 = base_strview<char8_t, false>;


	//str8
	IRON_MODULE_EXPORT template <typename _A =iron::unsafe::allocator<char8_t>, std::size_t _FIXED_SIZE = str_default_fixedsize>
	using base_str8 = str<char8_t, _A, _FIXED_SIZE>;
	IRON_MODULE_EXPORT using str8 = base_str8<>;
	IRON_MODULE_EXPORT using vstr8 = base_strview<char8_t,true>;

	//ustrw
	IRON_MODULE_EXPORT template <typename _A =iron::unsafe::allocator<wchar_t>, std::size_t _FIXED_SIZE = str_default_fixedsize>
	using base_ustrw = ustr<wchar_t, _A, _FIXED_SIZE>;
	IRON_MODULE_EXPORT using ustrw = base_ustrw<>;
	IRON_MODULE_EXPORT using uvstrw = base_strview<wchar_t,false>;

	//strw
	IRON_MODULE_EXPORT template <typename _A =iron::unsafe::allocator<wchar_t>, std::size_t _FIXED_SIZE = str_default_fixedsize>
	using base_strw = str<wchar_t, _A, _FIXED_SIZE>;
	IRON_MODULE_EXPORT using strw = base_strw<>;
	IRON_MODULE_EXPORT using vstrw = base_strview<wchar_t,true>;

	//ustr16
	IRON_MODULE_EXPORT template <typename _A =iron::unsafe::allocator<char16_t>, std::size_t _FIXED_SIZE = str_default_fixedsize>
	using base_ustr16 = ustr<char16_t, _A, _FIXED_SIZE>;
	IRON_MODULE_EXPORT using ustr16 = base_ustr16<>;
	IRON_MODULE_EXPORT using uvstr16 = base_strview<wchar_t,false>;

	//str16
	IRON_MODULE_EXPORT template <typename _A =iron::unsafe::allocator<char16_t>, std::size_t _FIXED_SIZE = str_default_fixedsize>
	using base_str16 = str<char16_t, _A, _FIXED_SIZE>;
	IRON_MODULE_EXPORT using str16 = base_str16<>;
	IRON_MODULE_EXPORT using vstr16 = base_strview<char16_t,true>;

	//ustr32
	IRON_MODULE_EXPORT template <typename _A =iron::unsafe::allocator<char32_t>, std::size_t _FIXED_SIZE = str_default_fixedsize>
	using base_ustr32 = ustr<char32_t, _A, _FIXED_SIZE>;
	IRON_MODULE_EXPORT using ustr32 = base_ustr32<>;
	IRON_MODULE_EXPORT using uvstr32 = base_strview<char32_t,false>;

	//str32
	IRON_MODULE_EXPORT template <typename _A =iron::unsafe::allocator<char32_t>, std::size_t _FIXED_SIZE = str_default_fixedsize>
	using base_str32 = str<char32_t, _A, _FIXED_SIZE>;
	IRON_MODULE_EXPORT using str32 = base_str32<>;
	IRON_MODULE_EXPORT using vstr32 = base_strview<char32_t,true>;

	////

	IRON_MODULE_EXPORT template <typename _T> class base_integral;
	IRON_MODULE_EXPORT using i8 = base_integral<std::int8_t>;
	IRON_MODULE_EXPORT using i16 = base_integral<std::int16_t>;
	IRON_MODULE_EXPORT using i32 = base_integral<std::int32_t>;
	IRON_MODULE_EXPORT using i64 = base_integral<std::int64_t>;
	IRON_MODULE_EXPORT using isize = base_integral<std::ptrdiff_t>;

	IRON_MODULE_EXPORT using u8 = base_integral<std::uint8_t>;
	IRON_MODULE_EXPORT using u16 = base_integral<std::uint16_t>;
	IRON_MODULE_EXPORT using u32 = base_integral<std::uint32_t>;
	IRON_MODULE_EXPORT using u64 = base_integral<std::uint64_t>;
	IRON_MODULE_EXPORT using usize = base_integral<std::size_t>;

	IRON_MODULE_EXPORT template <typename _T> class base_integral_bounded;

	IRON_MODULE_EXPORT using i8b = base_integral_bounded<std::int8_t>;
	IRON_MODULE_EXPORT using i16b = base_integral_bounded<std::int16_t>;
	IRON_MODULE_EXPORT using i32b = base_integral_bounded<std::int32_t>;
	IRON_MODULE_EXPORT using i64b = base_integral_bounded<std::int64_t>;
	IRON_MODULE_EXPORT using isizeb = base_integral_bounded<std::ptrdiff_t>;

	IRON_MODULE_EXPORT using u8b = base_integral_bounded<std::uint8_t>;
	IRON_MODULE_EXPORT using u16b = base_integral_bounded<std::uint16_t>;
	IRON_MODULE_EXPORT using u32b = base_integral_bounded<std::uint32_t>;
	IRON_MODULE_EXPORT using u64b = base_integral_bounded<std::uint64_t>;
	IRON_MODULE_EXPORT using usizeb = base_integral_bounded<std::size_t>;

	namespace literals {
		IRON_MODULE_EXPORT constexpr ustrb operator""_ub(const char*, std::size_t)noexcept;
		IRON_MODULE_EXPORT constexpr strb operator""_sb(const char*, std::size_t)noexcept;

		IRON_MODULE_EXPORT constexpr ustr8 operator""_u8(const char*, std::size_t)noexcept;
		IRON_MODULE_EXPORT constexpr ustr8 operator""_u8(const char8_t*, std::size_t)noexcept;
		IRON_MODULE_EXPORT constexpr ustr8 operator""_u8(const wchar_t*, std::size_t)noexcept;
		IRON_MODULE_EXPORT constexpr ustr8 operator""_u8(const char16_t*, std::size_t)noexcept;
		IRON_MODULE_EXPORT constexpr ustr8 operator""_u8(const char32_t*, std::size_t)noexcept;

		IRON_MODULE_EXPORT constexpr str8 operator""_s8(const char*, std::size_t)noexcept;
		IRON_MODULE_EXPORT constexpr str8 operator""_s8(const char8_t*, std::size_t)noexcept;
		IRON_MODULE_EXPORT constexpr str8 operator""_s8(const wchar_t*, std::size_t)noexcept;
		IRON_MODULE_EXPORT constexpr str8 operator""_s8(const char16_t*, std::size_t)noexcept;
		IRON_MODULE_EXPORT constexpr str8 operator""_s8(const char32_t*, std::size_t)noexcept;

		IRON_MODULE_EXPORT constexpr ustr16 operator""_u16(const char*, std::size_t)noexcept;
		IRON_MODULE_EXPORT constexpr ustr16 operator""_u16(const char8_t*, std::size_t)noexcept;
		IRON_MODULE_EXPORT constexpr ustr16 operator""_u16(const wchar_t*, std::size_t)noexcept;
		IRON_MODULE_EXPORT constexpr ustr16 operator""_u16(const char16_t*, std::size_t)noexcept;
		IRON_MODULE_EXPORT constexpr ustr16 operator""_u16(const char32_t*, std::size_t)noexcept;

		IRON_MODULE_EXPORT constexpr str16 operator""_s16(const char*, std::size_t)noexcept;
		IRON_MODULE_EXPORT constexpr str16 operator""_s16(const char8_t*, std::size_t)noexcept;
		IRON_MODULE_EXPORT constexpr str16 operator""_s16(const wchar_t*, std::size_t)noexcept;
		IRON_MODULE_EXPORT constexpr str16 operator""_s16(const char16_t*, std::size_t)noexcept;
		IRON_MODULE_EXPORT constexpr str16 operator""_s16(const char32_t*, std::size_t)noexcept;

		IRON_MODULE_EXPORT constexpr ustrw operator""_uw(const char*, std::size_t)noexcept;
		IRON_MODULE_EXPORT constexpr ustrw operator""_uw(const char8_t*, std::size_t)noexcept;
		IRON_MODULE_EXPORT constexpr ustrw operator""_uw(const wchar_t*, std::size_t)noexcept;
		IRON_MODULE_EXPORT constexpr ustrw operator""_uw(const char16_t*, std::size_t)noexcept;
		IRON_MODULE_EXPORT constexpr ustrw operator""_uw(const char32_t*, std::size_t)noexcept;

		IRON_MODULE_EXPORT constexpr strw operator""_sw(const char*, std::size_t)noexcept;
		IRON_MODULE_EXPORT constexpr strw operator""_sw(const char8_t*, std::size_t)noexcept;
		IRON_MODULE_EXPORT constexpr strw operator""_sw(const wchar_t*, std::size_t)noexcept;
		IRON_MODULE_EXPORT constexpr strw operator""_sw(const char16_t*, std::size_t)noexcept;
		IRON_MODULE_EXPORT constexpr strw operator""_sw(const char32_t*, std::size_t)noexcept;

		IRON_MODULE_EXPORT constexpr ustr32 operator""_u32(const char*, std::size_t)noexcept;
		IRON_MODULE_EXPORT constexpr ustr32 operator""_u32(const char8_t*, std::size_t)noexcept;
		IRON_MODULE_EXPORT constexpr ustr32 operator""_u32(const wchar_t*, std::size_t)noexcept;
		IRON_MODULE_EXPORT constexpr ustr32 operator""_u32(const char16_t*, std::size_t)noexcept;
		IRON_MODULE_EXPORT constexpr ustr32 operator""_u32(const char32_t*, std::size_t)noexcept;

		IRON_MODULE_EXPORT constexpr str32 operator""_s32(const char*, std::size_t)noexcept;
		IRON_MODULE_EXPORT constexpr str32 operator""_s32(const char8_t*, std::size_t)noexcept;
		IRON_MODULE_EXPORT constexpr str32 operator""_s32(const wchar_t*, std::size_t)noexcept;
		IRON_MODULE_EXPORT constexpr str32 operator""_s32(const char16_t*, std::size_t)noexcept;
		IRON_MODULE_EXPORT constexpr str32 operator""_s32(const char32_t*, std::size_t)noexcept;
	}


	template <typename _T, typename _A> class base_vector;

	IRON_MODULE_EXPORT template <typename _T, typename _A =iron::unsafe::allocator<_T>>
	using vector = base_vector<_T, _A>;

	IRON_MODULE_EXPORT template <typename _T, typename _A=iron::unsafe::allocator<_T>>
	using list = base_list<_T, _A,true>;

	IRON_MODULE_EXPORT template <typename _T, typename _A=iron::unsafe::allocator<_T>>
	using fwd_list = base_list<_T, _A,false>;


} //ns


