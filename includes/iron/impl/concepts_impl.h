
namespace iron {
	namespace unsafe {
		IRON_MODULE_EXPORT class security_checker;
	}
	IRON_MODULE_EXPORT template<typename _T, typename _E> class result;
	IRON_MODULE_EXPORT class error_handler;
}

namespace iron::concepts {

	IRON_MODULE_EXPORT template <typename _T>
	concept binary_data = std::same_as<std::decay_t<_T>, unsigned char> /* added ? || std::same_as<std::decay_t<_T>, std::byte>*/;

	IRON_MODULE_EXPORT template <typename _T>
	concept basic_charset = std::same_as<std::decay_t<_T>, char>;

	IRON_MODULE_EXPORT template <typename _T>
	concept utf8_charset = std::same_as<std::decay_t<_T>, char8_t>;

	IRON_MODULE_EXPORT template <typename _T>
	concept utf16_charset = (std::same_as<std::decay_t<_T>, wchar_t> && sizeof(wchar_t) == 2) || std::same_as<std::decay_t<_T>, char16_t>;

	IRON_MODULE_EXPORT template <typename _T>
	concept utf32_charset = (std::same_as<std::decay_t<_T>, wchar_t> && sizeof(wchar_t) == 4) || std::same_as<std::decay_t<_T>, char32_t>;

	IRON_MODULE_EXPORT template <typename _T>
	concept varying_charset = utf8_charset<_T> || utf16_charset<_T>;

	IRON_MODULE_EXPORT template <typename _T>
	concept fixed_charset = basic_charset<_T> || utf32_charset<_T>;

	IRON_MODULE_EXPORT template <typename _T>
	concept charset = varying_charset<_T> || fixed_charset<_T>;

	IRON_MODULE_EXPORT template <typename _T>
	concept charset_forward_iterator = std::forward_iterator<_T> && charset<decltype(*std::declval<std::decay_t<_T>>())>;

	IRON_MODULE_EXPORT template <typename _T>
	concept numeric_integral = std::integral<_T>
		&& !charset<_T>
		&& !std::same_as<std::decay_t<_T>, bool>
		&& !std::is_pointer_v<_T>
		;

	IRON_MODULE_EXPORT template<typename  _T>
	concept std_pair = requires(_T _v) {
		typename std::decay_t<_T>::first_type;
		typename std::decay_t<_T>::second_type;
		_v.first;
		_v.second;
	};

	IRON_MODULE_EXPORT template<typename  _T>
	concept proxy_pair = requires(_T) {
		typename _T::proxy_pair_tag;
	};

	IRON_MODULE_EXPORT template <typename _T>
	concept cloneable = requires(_T _v)
	{
		{ _v.clone() }noexcept ->std::same_as<std::decay_t<_T>>;
		{ _v.try_clone() } noexcept -> std::same_as<iron::result<std::decay_t<_T>, iron::error_handler>>;
	};

	IRON_MODULE_EXPORT template <typename _T>
	concept std_tuple = requires (_T) {
		typename std::enable_if_t<!std::is_void_v<std::decay_t<_T>> && !std::is_function_v<std::decay_t<_T>>>;
		std::is_same_v<decltype(std::tuple_size<std::decay_t<_T>>::value), std::size_t>;
	};

	IRON_MODULE_EXPORT template <typename _F, typename _T>
	concept bool_predicate = requires(_F _f, const _T& _v) {
			{ std::invoke(_f, _v) } -> std::same_as<bool>;
	}&& std::is_nothrow_invocable_r_v<bool, _F, const _T&>;

	IRON_MODULE_EXPORT template <typename _T>
	concept bit_set = requires(_T) {
		typename std::decay_t<_T>::bit_set_tag;
	};

	IRON_MODULE_EXPORT template <typename _T>
	concept  base_integral = requires(_T) {
		typename std::decay_t<_T>::base_integral_tag;

	};

	IRON_MODULE_EXPORT template <typename _T>
	concept  base_integral_bounded = requires(_T) {
		typename std::decay_t<_T>::base_integral_bounded_tag;
	};


	IRON_MODULE_EXPORT template<typename  _T>
	concept has_security_checker = requires(std::decay_t<_T> _v) {
		{_v.checker()} noexcept ->std::same_as<const iron::unsafe::security_checker&>;
	};

	IRON_MODULE_EXPORT template <typename _T>
	concept sequence = requires(_T _v) {
		typename std::decay_t<_T>::sequence_tag;
	};

	IRON_MODULE_EXPORT template <typename _T>
	concept charset_sequence = (sequence<_T> && charset<typename _T::value_type>);

	IRON_MODULE_EXPORT template <typename _T>
	concept fwd_sequence_maker = requires(_T _v)
	{
		typename std::decay_t<_T>::iterator;
		typename std::decay_t<_T>::const_iterator;
		{ _v.seq() } noexcept;
		{ _v.cseq() } noexcept;
	};

	IRON_MODULE_EXPORT template <typename _T>
	concept bd_sequence_maker = fwd_sequence_maker<_T> && requires(_T _v)
	{
		typename _T::reverse_iterator;
		typename _T::const_reverse_iterator;
		{ _v.rseq() } noexcept;
		{ _v.crseq() } noexcept;
	};


	IRON_MODULE_EXPORT template <typename _T>
	concept raw_sequence_maker = requires(_T _v)
	{
		{ _v.raw_seq() } noexcept;
	};

	IRON_MODULE_EXPORT template <typename _T>
	concept nothrow_equality_comparable = 
		std::equality_comparable<_T> && noexcept(std::declval<const _T&>() == std::declval<const _T&>());

	IRON_MODULE_EXPORT template <typename _T1,typename _T2>
	concept nothrow_equality_comparable_with = 
		std::equality_comparable_with<_T1,_T2> && noexcept(std::declval<const _T1&>() == std::declval<const _T2&>());

	IRON_MODULE_EXPORT template<typename _T>
	concept less_comparable = requires(_T _a, _T _b) {
		{ _a < _b } -> std::convertible_to<bool>;
	};

	IRON_MODULE_EXPORT template <typename _T>
	concept nothrow_less_comparable = requires(_T _a,_T _b) {
		{ _a < _b }noexcept -> std::convertible_to<bool>;
	};

	IRON_MODULE_EXPORT template <typename _T1, typename _T2>
	concept less_comparable_with = requires(_T1 _a, _T2 _b) {
		{ _a < _b } -> std::convertible_to<bool>;
	};

	IRON_MODULE_EXPORT template <typename _T1,typename _T2>
	concept nothrow_less_comparable_with = requires(_T1 _a,_T2 _b) {
		{ _a < _b }noexcept -> std::convertible_to<bool>;
	};

	IRON_MODULE_EXPORT template <typename _T, typename _U = _T>
	concept nothrow_three_way_comparable =
		std::three_way_comparable_with<_T, _U> &&
		requires(const std::remove_reference_t<_T>& a,const std::remove_reference_t<_U>& b) {
			{ a <=> b } noexcept;
	};

	IRON_MODULE_EXPORT template <typename _T, typename... _ARGS>
	concept constructible_at = requires(_T* _p, _ARGS&&... _args) {
			{ std::construct_at(_p, std::forward<_ARGS>(_args)...) } -> std::same_as<_T*>;
	};

	IRON_MODULE_EXPORT template <typename _T, typename... _ARGS>
	concept nothrow_constructible_at = requires(_T* _p, _ARGS&&... _args) {
			{ std::construct_at(_p, std::forward<_ARGS>(_args)...) } noexcept -> std::same_as<_T*>;
	};

	IRON_MODULE_EXPORT template<typename _T>
	concept std_allocator = requires(_T & _alloc, std::size_t _n) {
			{ std::allocator_traits<_T>::allocate(_alloc, _n) }-> std::same_as<typename std::allocator_traits<_T>::pointer>;
			{ std::allocator_traits<_T>::deallocate(_alloc, typename std::allocator_traits<_T>::pointer{}, _n) }->std::same_as<void>;
			typename std::allocator_traits<_T>::value_type;
			typename std::allocator_traits<_T>::size_type;
			typename std::allocator_traits<_T>::difference_type;
			//			typename std::allocator_traits<_T>::template rebind_alloc<void>;
			//			typename std::allocator_traits<_T>:: template rebind_alloc<void>(_alloc);
			{typename std::allocator_traits<_T>:: template rebind_alloc<void>(_alloc) } noexcept;
	};


	IRON_MODULE_EXPORT template<typename _T>
	concept allocator = requires(_T& _alloc, std::size_t _n) {
			{ _alloc.allocate(_n)}noexcept -> std::same_as<typename std::allocator_traits<_T>::pointer>;
			{ _alloc.deallocate(typename std::allocator_traits<_T>::pointer{}, _n)}noexcept -> std::same_as<void>;
			typename std::allocator_traits<_T>::value_type;
			typename std::allocator_traits<_T>::size_type;
			typename std::allocator_traits<_T>::difference_type;
			{typename std::allocator_traits<_T>:: template rebind_alloc<void>(_alloc)} noexcept;
	};

#if 0
	IRON_MODULE_EXPORT template<typename _A1, typename _A2>
	concept same_allocator =
		std_allocator<_A1> &&
		std_allocator<_A2> &&
		requires(const _A1 & _a1, const _A2 & _a2) {
			{ typename std::allocator_traits<_A1>::template rebind_alloc<void>(_a1) ==
				typename std::allocator_traits<_A2>::template rebind_alloc<void>(_a2) } -> std::convertible_to<bool>;
	};
#endif 

	IRON_MODULE_EXPORT template <typename _T>
	concept iron_allocator = requires(_T _v) {
		typename std::decay_t<_T>::allocator_tag;
	};

	namespace _impl_ri {
		template <typename _T>
		struct is_reverse_iterator : std::false_type {};

		template <typename _T>
		struct is_reverse_iterator<std::reverse_iterator<_T>> : std::true_type {};
	}

	IRON_MODULE_EXPORT template <typename _T>
	concept reverse_iterator = _impl_ri::is_reverse_iterator<std::decay_t<_T>>::value;

	IRON_MODULE_EXPORT template <typename _T>
	concept box_deleter = requires(_T _v, typename _T::value_t * _p) {
		{_v.operator()(_p)} noexcept -> std::same_as<void>;
	};

#if 0 //TODO
	IRON_MODULE_EXPORT template<typename _FROM, typename _TO>
		concept safe_copy_convertible_to = std::is_convertible_v<_FROM, _TO>&& [] {
		if constexpr (std::is_arithmetic_v<_FROM> && std::is_arithmetic_v<_TO>) {
			if constexpr (std::is_floating_point_v<_FROM> && std::is_integral_v<_TO>) {
				return false;
			}
			else if constexpr (std::is_floating_point_v<_FROM> && std::is_floating_point_v<_TO>) {
				if (sizeof(_TO) < sizeof(_FROM)) {
					return false;
				}
			}
			else if constexpr (std::is_integral_v<_FROM> && std::is_integral_v<_TO>) {
				if (sizeof(_TO) < sizeof(_FROM)) {
					return false;
				}
				if (std::is_signed_v<_TO> != std::is_signed_v<_FROM>) {
					return false;
				}
			}
		}
		else if constexpr (std::is_same_v<_FROM, _TO>) {
			if constexpr (!copyable_or_cloneable<_FROM>) {
				return false;
			}
		}

		return true;
	}();
#endif 

	IRON_MODULE_EXPORT template <typename _T>
	concept lock = requires(_T _v) {
			{_v.lock()}noexcept ->std::same_as<void>;
			{_v.unlock()}noexcept->std::same_as<void>;
	};

	IRON_MODULE_EXPORT template <typename _T>
	concept nothrow_convertible_to_integral =
		 std::is_nothrow_convertible_v<_T, std::int8_t> ||
		 std::is_nothrow_convertible_v<_T, std::uint8_t> ||
		 std::is_nothrow_convertible_v<_T, std::int16_t> ||
		 std::is_nothrow_convertible_v<_T, std::uint16_t> ||
		 std::is_nothrow_convertible_v<_T, std::int32_t> ||
		 std::is_nothrow_convertible_v<_T, std::uint32_t> ||
		 std::is_nothrow_convertible_v<_T, std::int64_t> ||
		 std::is_nothrow_convertible_v<_T, std::uint64_t> ||
		 std::is_nothrow_convertible_v<_T, char> ||
		 std::is_nothrow_convertible_v<_T, wchar_t> ||
		 std::is_nothrow_convertible_v<_T, char16_t> ||
		 std::is_nothrow_convertible_v<_T, char32_t> ||
		 std::is_nothrow_convertible_v<_T, char8_t> ||
		 std::is_nothrow_convertible_v<_T, std::size_t> ||
		 std::is_nothrow_convertible_v<_T, std::ptrdiff_t>
	;


	IRON_MODULE_EXPORT template <typename _H, typename _T>
	concept hashable = std::is_nothrow_default_constructible_v<_H> && requires(_H _h, _T _v) {
		{ _h(_v)}noexcept -> std::same_as<std::size_t>;
	};

	IRON_MODULE_EXPORT template <typename _C, typename _T>
	concept comparable = std::is_nothrow_default_constructible_v<_C> && requires(_C _c, _T _v) {
			{ _c(_v,_v)}noexcept -> std::same_as<bool>;
	};


#if 0

	IRON_MODULE_EXPORT template <typename _T>
		concept unbuffered_stream = requires(_T _v, typename _T::value_t * _p, typename _T::size_t _sz) {
		typename _T::size_t;
		typename _T::value_t;
		typename _T::error_handler_t;
		typename _T::seek_t;
		{_v.unchecked_read(_p, _sz)}->std::convertible_to<typename _T::error_handler_t>;
		{_v.unchecked_write(_p, _sz)}->std::convertible_to<typename _T::error_handler_t>;
		{_v.seek(long{}, typename _T::seek_t{})}->std::convertible_to<typename _T::error_handler_t>;
		noexcept(_v.unchecked_read(_p, _sz));
		noexcept(_v.unchecked_write(_p, _sz));
		noexcept(_v.seek(long{}, typename _T::seek_t{}));
	};
#endif

	IRON_MODULE_EXPORT template <typename _T>
	concept condition_variable = requires(_T _v) {
		typename std::decay_t<_T>::condition_variable_tag;
	};

	IRON_MODULE_EXPORT template <typename _T>
	concept error_handler = requires(_T _v) {
		typename std::decay_t<_T>::error_handler_tag;
	};

	IRON_MODULE_EXPORT template <typename _T>
	concept vector = requires(_T _v) {
		typename std::decay_t<_T>::vector_tag;
	};

	IRON_MODULE_EXPORT template <typename _T>
	concept array = requires(_T _v) {
		typename std::decay_t<_T>::array_tag;
	};

	IRON_MODULE_EXPORT template <typename _T>
	concept vector_iterator = requires  {
		typename std::decay_t<_T>::contiguous_iterator_tag;
	};

	IRON_MODULE_EXPORT template <typename _T>
	concept array_iterator = requires  {
		typename std::decay_t<_T>::contiguous_iterator_tag;
	};

	IRON_MODULE_EXPORT template <typename _T>
	concept base_list = requires(_T _v) {
		typename std::decay_t<_T>::list_tag;
	};

	IRON_MODULE_EXPORT template <typename _T>
	concept fwd_list = base_list<_T> && [] { return !std::decay_t<_T>::is_bidirectional; }();
	
	IRON_MODULE_EXPORT template <typename _T>
	concept list = base_list<_T> && [] { return std::decay_t<_T>::is_bidirectional; }();

	IRON_MODULE_EXPORT template <typename _T>
	concept list_iterator = requires  {
		typename std::decay_t<_T>::list_iterator_tag;
	};


	IRON_MODULE_EXPORT template <typename _T>
	concept hash_table = requires(_T) {
		typename std::decay_t<_T>::hash_table_tag;
	};

	IRON_MODULE_EXPORT template <typename _T>
	concept base_hash_map = hash_table<_T> && [] { return std::decay_t<_T>::has_mapped; }();

	IRON_MODULE_EXPORT template <typename _T>
	concept hash_map = base_hash_map<_T> && [] { return !std::decay_t<_T>::is_multi; }();
	
	IRON_MODULE_EXPORT template <typename _T>
	concept multi_hash_map = base_hash_map<_T> && [] { return std::decay_t<_T>::is_multi; }();

	IRON_MODULE_EXPORT template <typename _T>
	concept base_hash_set = hash_table<_T> && [] { return !std::decay_t<_T>::has_mapped; }();

	IRON_MODULE_EXPORT template <typename _T>
	concept hash_set = base_hash_set<_T> && [] { return !std::decay_t<_T>::is_multi; }();
	
	IRON_MODULE_EXPORT template <typename _T>
	concept multi_hash_set = base_hash_set<_T> && [] { return std::decay_t<_T>::is_multi; }();

	IRON_MODULE_EXPORT template <typename _T>
	concept hash_table_iterator = requires  {
		typename std::decay_t<_T>::hash_table_iterator_tag;
	};


	IRON_MODULE_EXPORT template <typename _T>
	concept btree_table = requires(_T) {
		typename std::decay_t<_T>::btree_table_tag;
	};

	IRON_MODULE_EXPORT template <typename _T>
	concept base_btree_map = btree_table<_T> && [] { return std::decay_t<_T>::has_mapped; }();

	IRON_MODULE_EXPORT template <typename _T>
	concept btree_map = base_btree_map<_T> && [] { return !std::decay_t<_T>::is_multi; }();

	IRON_MODULE_EXPORT template <typename _T>
	concept multi_btree_map = base_btree_map<_T> && [] { return std::decay_t<_T>::is_multi; }();

	IRON_MODULE_EXPORT template <typename _T>
	concept base_btree_set = btree_table<_T> && [] { return !std::decay_t<_T>::has_mapped; }();

	IRON_MODULE_EXPORT template <typename _T>
	concept btree_set = base_btree_set<_T> && [] { return !std::decay_t<_T>::is_multi; }();

	IRON_MODULE_EXPORT template <typename _T>
	concept multi_btree_set = base_btree_set<_T> && [] { return std::decay_t<_T>::is_multi; }();

	IRON_MODULE_EXPORT template <typename _T>
	concept btree_table_iterator = requires  {
		typename std::decay_t<_T>::btree_table_iterator_tag;
	};

	IRON_MODULE_EXPORT template <typename _T>
	concept str = requires(_T _v) {
		typename std::decay_t<_T>::str_tag;
	};
	
	IRON_MODULE_EXPORT template <typename _T>
	concept safe_str = str<_T> && [] { return std::decay_t<_T>::is_safe(); }();

	IRON_MODULE_EXPORT template <typename _T>
	concept unsafe_str = str<_T> && [] { return !std::decay_t<_T>::is_safe(); }();

	IRON_MODULE_EXPORT template <typename _T>
	concept str_view = requires(_T _v) {
		typename std::decay_t<_T>::str_view_tag;
	};

	IRON_MODULE_EXPORT template <typename _T>
	concept safe_str_view = str_view<_T> && std::decay_t<_T>::is_safe();

	IRON_MODULE_EXPORT template <typename _T>
	concept unsafe_str_view = str_view<_T> && !std::decay_t<_T>::is_safe();

	IRON_MODULE_EXPORT template <typename _T>
	concept raw_str = requires(_T _v) {
		typename std::decay_t<_T>::rawstr_tag;
	};
	IRON_MODULE_EXPORT template <typename _T>
	concept raw_strview = requires(_T _v) {
		typename std::decay_t<_T>::raw_strview_tag;
	};

#if 0
	IRON_MODULE_EXPORT template <typename _T> 
	concept path = requires(_T _v) {
		typename std::decay_t<_T>::path_tag;
	};
#endif 

	IRON_MODULE_EXPORT template <typename _T>
	concept box = requires(_T) {
		typename std::decay_t<_T>::box_tag;
	};

	IRON_MODULE_EXPORT template <typename _T> 
	concept base_shared_box = requires(_T) {
		typename std::decay_t<_T>::shared_box_tag;
	};

	IRON_MODULE_EXPORT template <typename _T>
	concept shared_box = base_shared_box<_T> && !std::decay_t<_T>::weak();

	IRON_MODULE_EXPORT template <typename _T>
	concept weak_box = base_shared_box<_T> && std::decay_t<_T>::weak();

	IRON_MODULE_EXPORT template <typename _T>
	concept rc = shared_box<_T> && !std::decay_t<_T>::thread_safe();

	IRON_MODULE_EXPORT template <typename _T>
	concept rcw = weak_box<_T> && !std::decay_t<_T>::thread_safe();

	IRON_MODULE_EXPORT template <typename _T>
	concept arc = shared_box<_T> && std::decay_t<_T>::thread_safe();

	IRON_MODULE_EXPORT template <typename _T>
	concept arcw = weak_box<_T> && std::decay_t<_T>::thread_safe();

	IRON_MODULE_EXPORT template <typename _T>
	concept raw_vector = requires(_T) {
		typename std::decay_t<_T>::raw_vector_tag;
	};

	IRON_MODULE_EXPORT template <typename _T>
	concept list_node = requires(_T) {
		typename std::decay_t<_T>::list_node_tag;
	};

	IRON_MODULE_EXPORT template <typename _T>
	concept raw_list = requires(_T) {
		typename std::decay_t<_T>::raw_list_tag;
	};

	IRON_MODULE_EXPORT template <typename _T>
	concept adapter_box = requires(_T) {
		typename std::decay_t<_T>::adapter_box_tag;
	};

	IRON_MODULE_EXPORT template <typename _T>
	concept raw_hash_table = requires(_T) {
		typename std::decay_t<_T>::raw_hash_table_tag;
	};

	IRON_MODULE_EXPORT template <typename _T>
	concept btree_node = requires(_T) {
		typename std::decay_t<_T>::btree_node_tag;
	};

	IRON_MODULE_EXPORT template <typename _T>
	concept cfg = requires(_T) {
		typename std::decay_t<_T>::cfg_tag;
	};

	IRON_MODULE_EXPORT template <typename _T>
	concept chr = requires(_T) {
		typename std::decay_t<_T>::chr_tag;
	};

	IRON_MODULE_EXPORT template <typename _T>
	concept chr_sequence = requires(_T) {
		typename std::decay_t<_T>::chr_sequence_tag;
	};

	IRON_MODULE_EXPORT template <typename _T>
	concept base_mutex = requires(_T) {
		typename std::decay_t<_T>::mutex_tag;
	};

	IRON_MODULE_EXPORT template <typename _T>
	concept mutex = base_mutex<_T> && !std::decay_t<_T>::recursive;
	
	IRON_MODULE_EXPORT template <typename _T>
	concept recursive_mutex = base_mutex<_T> && std::decay_t<_T>::recursive;

	IRON_MODULE_EXPORT template <typename _T>
	concept base_spin_lock = requires(_T) {
		typename std::decay_t<_T>::spin_lock_tag;
	};

	IRON_MODULE_EXPORT template <typename _T>
	concept spin_lock = base_spin_lock<_T> && !std::decay_t<_T>::recursive;

	IRON_MODULE_EXPORT template <typename _T>
	concept recursive_spin_lock = base_spin_lock<_T> && std::decay_t<_T>::recursive;

	IRON_MODULE_EXPORT template <typename _T>
	concept option = requires(_T) {
		typename std::decay_t<_T>::option_tag;
	};

	IRON_MODULE_EXPORT template <typename _T>
	concept lock_guard = requires(_T) {
		typename std::decay_t<_T>::lock_guard_tag;
	};

	IRON_MODULE_EXPORT template <typename _T>
	concept result = requires(_T) {
		typename std::decay_t<_T>::result_tag;
	};

	IRON_MODULE_EXPORT template <typename _T>
	concept ref_wrapper = requires(_T) {
		typename std::decay_t<_T>::ref_wrapper_tag;
	};

	IRON_MODULE_EXPORT template <typename _T>
	concept security_checker = requires(_T) {
		typename std::decay_t<_T>::security_checker_tag;
	};

	IRON_MODULE_EXPORT template <typename _T>
	concept unicode_iterator = requires(_T) {
		typename std::decay_t<_T>::unicode_iterator_tag;
	};

	IRON_MODULE_EXPORT template <typename _T,typename _U>
	concept has_type = requires { typename _T::_U; };

}

