#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable:26481)
#pragma warning (disable:26820)
#endif 

namespace iron {

	class collector final {
	public:
		using error_handler_t=iron::error_handler;
		collector()=delete;

		template <typename _T,typename... _ARGS>
		[[nodiscard]] static constexpr iron::result<iron::vector<_T>,error_handler_t> try_make_vector(_ARGS&&... _args)noexcept 
		requires (!iron::concepts::sequence<_T> && !iron::concepts::adapter_box<_T>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_ARGS(_args...);
			iron::vector<_T> _t;
			if constexpr (sizeof...(_ARGS)) {
				auto _r = _t.try_reserve_at_least(sizeof...(_ARGS)); RR(_r);
				auto _h = _push_into(_t, std::forward<_ARGS>(_args)...); RR(_h);
			}
			return iron::ok(std::move(_t));
		}
		template <typename _T,typename... _ARGS>
		[[nodiscard]] static constexpr iron::vector<_T> make_vector(_ARGS&&... _args)noexcept 
		requires (!iron::concepts::sequence<_T> && !iron::concepts::adapter_box<_T>)
		{
			auto _r=try_make_vector<_T>(std::forward<_ARGS>(_args)...); RP(_r);
		}
		template <typename _T, typename... _ARGS>
		[[nodiscard]] static constexpr iron::result<iron::fwd_list<_T>, error_handler_t> try_make_fwd_list(_ARGS&&... _args)noexcept 
		requires (!iron::concepts::sequence<_T> && !iron::concepts::adapter_box<_T>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_ARGS(_args...);
			iron::fwd_list<_T> _t;
			if constexpr (sizeof...(_ARGS)) {
				auto _h = _push_into(_t, std::forward<_ARGS>(_args)...); RR(_h);
			}
			return iron::ok(std::move(_t));
		}
		template <typename _T, typename... _ARGS>
		[[nodiscard]] static constexpr iron::fwd_list<_T> make_fwd_list(_ARGS&&... _args)noexcept 
		requires (!iron::concepts::sequence<_T> && !iron::concepts::adapter_box<_T>)
		{
			auto _r = try_make_fwd_list<_T>(std::forward<_ARGS>(_args)...); RP(_r);
		}
		template <typename _T, typename... _ARGS>
		[[nodiscard]] static constexpr iron::result<iron::list<_T>, error_handler_t> try_make_list(_ARGS&&... _args)noexcept 
		requires (!iron::concepts::sequence<_T> && !iron::concepts::adapter_box<_T>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_ARGS(_args...);
			iron::list<_T> _t;
			if constexpr (sizeof...(_ARGS)) {
				auto _h = _push_into(_t, std::forward<_ARGS>(_args)...); RR(_h);
			}
			return iron::ok(std::move(_t));
		}
		template <typename _T, typename... _ARGS>
		[[nodiscard]] static constexpr iron::list<_T> make_list(_ARGS&&... _args)noexcept 
		requires (!iron::concepts::sequence<_T> && !iron::concepts::adapter_box<_T>)
		{
			auto _r = try_make_list<_T>(std::forward<_ARGS>(_args)...); RP(_r);
		}
		template <typename _T, std::size_t _N>
		[[nodiscard]] static constexpr iron::array<_T, _N> make_array()noexcept 
		requires(std::is_nothrow_default_constructible_v<_T>)
		{
			return iron::array<_T, _N>{};
		}

		template <typename _T,typename... _ARGS>
		[[nodiscard]] static constexpr iron::result<iron::array<std::decay_t<_T>, sizeof...(_ARGS) + 1>,error_handler_t> try_make_array(_T&& _v,_ARGS&&... _args)noexcept 
		requires (
			(sizeof...(_ARGS) + 1 <= iron::unsafe::raw_storage<std::decay_t<_T>>::max_size())
			&&
			(std::convertible_to<std::decay_t<_ARGS>,std::decay_t<_T>>&& ...)
		)
		{
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			IRON_SECURITY_CHECK_TEMPORARY_ARGS(_args...);
			constexpr auto _n = sizeof...(_ARGS) + 1;
			using _t_t = std::decay_t<_T>;
			_t_t _a[_n]{};
			auto _p = &_a[0];
			auto _h = _assign(_p, std::forward<_T>(_v)); RR(_h);
			if constexpr (sizeof...(_ARGS) != 0) {
				_h = _assign_list(++_p, std::forward<_ARGS>(_args)...); RR(_h);
			}
			auto _b = iron::array<std::decay_t<_T>, _n>::unchecked_from(std::move(_a));
			return iron::ok(std::move(_b));
		}
		template <typename _T, typename... _ARGS>
		[[nodiscard]] static constexpr iron::array<std::decay_t<_T>, sizeof...(_ARGS) + 1> make_array(_T&& _v,_ARGS&&... _args)noexcept 
		requires (
			(sizeof...(_ARGS) + 1 <= iron::unsafe::raw_storage<std::decay_t<_T>>::max_size())
			&&
			(std::convertible_to<std::decay_t<_ARGS>,std::decay_t<_T>>&& ...)
		)
		{
			auto _r = try_make_array(std::forward<_T>(_v),std::forward<_ARGS>(_args)...); RP(_r);
		}


		template <size_t _N,typename _T>
		[[nodiscard]] static constexpr iron::result<iron::array<std::decay_t<_T>, _N>, error_handler_t> try_make_array_fill(_T&& _v)noexcept 	
		{
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			using _t_t = std::decay_t<_T>;
			iron::array<_t_t, _N> _t;
			if constexpr (_N) {
				if constexpr (std::is_nothrow_copy_constructible_v<_T>) {
					_t.fill(_v);
				}
				else {
					auto _h = _t.try_fill(std::forward<_T>(_v)); RR(_h);
				}
			}
			return iron::ok(std::move(_t));
		}
		template <size_t _N,typename _T>
		[[nodiscard]] static constexpr iron::array<std::decay_t<_T>, _N> make_array_fill(_T&& _v)noexcept 
		{
			auto _r = try_make_array_fill<_N>(std::forward<_T>(_v)); RP(_r);
		}
		template <typename _K,typename _M,typename... _ARGS>
		[[nodiscard]] static constexpr iron::result<iron::hash_map<_K,_M>, error_handler_t> try_make_hash_map(_ARGS&&... _args)noexcept 
			requires (!iron::concepts::sequence<_K> && !iron::concepts::adapter_box<_K> && !iron::concepts::sequence<_M> && !iron::concepts::adapter_box<_M>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_ARGS(_args...);
			iron::hash_map<_K,_M> _t;
			if constexpr (sizeof...(_ARGS)) {
				auto _h = _push_into_map(_t, std::forward<_ARGS>(_args)...); RR(_h);
			}
			return iron::ok(std::move(_t));
		}
		template <typename _K,typename _M,typename... _ARGS>
		[[nodiscard]] static constexpr iron::hash_map<_K,_M> make_hash_map(_ARGS&&... _args)noexcept 
		requires (!iron::concepts::sequence<_K> && !iron::concepts::adapter_box<_K> && !iron::concepts::sequence<_M> && !iron::concepts::adapter_box<_M>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_ARGS(_args...);
			auto _r = try_make_hash_map<_K,_M>(std::forward<_ARGS>(_args)...); RP(_r);
		}
		template <typename _K, typename _M, typename... _ARGS>
		[[nodiscard]] static constexpr iron::result<iron::multi_hash_map<_K, _M>, error_handler_t> try_make_multi_hash_map(_ARGS&&... _args)noexcept 
		requires (!iron::concepts::sequence<_K> && !iron::concepts::adapter_box<_K> && !iron::concepts::sequence<_M> && !iron::concepts::adapter_box<_M>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_ARGS(_args...);
			iron::multi_hash_map<_K, _M> _t;
			if constexpr (sizeof...(_ARGS)) {
				auto _h = _push_into_map(_t, std::forward<_ARGS>(_args)...); RR(_h);
			}
			return iron::ok(std::move(_t));
		}
		template <typename _K, typename _M, typename... _ARGS>
		[[nodiscard]] static constexpr iron::multi_hash_map<_K, _M> make_multi_hash_map(_ARGS&&... _args)noexcept 
		requires (!iron::concepts::sequence<_K> && !iron::concepts::adapter_box<_K> && !iron::concepts::sequence<_M> && !iron::concepts::adapter_box<_M>)
		{
			auto _r = try_make_multi_hash_map<_K, _M>(std::forward<_ARGS>(_args)...); RP(_r);
		}
		template <typename _K,typename... _ARGS>
		[[nodiscard]] static constexpr iron::result<iron::hash_set<_K>, error_handler_t> try_make_hash_set(_ARGS&&... _args)noexcept
			requires (!iron::concepts::sequence<_K> && !iron::concepts::adapter_box<_K>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_ARGS(_args...);
			iron::hash_set<_K> _t;
			if constexpr (sizeof...(_ARGS)) {
				auto _h = _push_into(_t, std::forward<_ARGS>(_args)...); RR(_h);
			}
			return iron::ok(std::move(_t));
		}
		template <typename _K, typename... _ARGS>
		[[nodiscard]] static constexpr iron::hash_set<_K> make_hash_set(_ARGS&&... _args)noexcept
			requires (!iron::concepts::sequence<_K> && !iron::concepts::adapter_box<_K>)
		{
			auto _r = try_make_hash_set<_K>(std::forward<_ARGS>(_args)...); RP(_r);
		}
		template <typename _K, typename... _ARGS>
		[[nodiscard]] static constexpr iron::result<iron::multi_hash_set<_K>, error_handler_t> try_make_multi_hash_set(_ARGS&&... _args)noexcept
			requires (!iron::concepts::sequence<_K> && !iron::concepts::adapter_box<_K>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_ARGS(_args...);
			iron::multi_hash_set<_K> _t;
			if constexpr (sizeof...(_ARGS)) {
				auto _h = _push_into(_t, std::forward<_ARGS>(_args)...); RR(_h);
			}
			return iron::ok(std::move(_t));
		}
		template <typename _K, typename... _ARGS>
		[[nodiscard]] static constexpr iron::multi_hash_set<_K> make_multi_hash_set(_ARGS&&... _args)noexcept
			requires (!iron::concepts::sequence<_K> && !iron::concepts::adapter_box<_K>)
		{
			auto _r = try_make_multi_hash_set<_K>(std::forward<_ARGS>(_args)...); RP(_r);
		}
		template <typename _K, typename _M, typename... _ARGS>
		[[nodiscard]] static constexpr iron::result<iron::btree_map<_K, _M>, error_handler_t> try_make_btree_map(_ARGS&&... _args)noexcept
			requires (!iron::concepts::sequence<_K> && !iron::concepts::adapter_box<_K> && !iron::concepts::sequence<_M> && !iron::concepts::adapter_box<_M>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_ARGS(_args...);
			iron::btree_map<_K, _M> _t;
			if constexpr (sizeof...(_ARGS)) {
				auto _h = _push_into_map(_t, std::forward<_ARGS>(_args)...); RR(_h);
			}
			return iron::ok(std::move(_t));
		}
		template <typename _K, typename _M, typename... _ARGS>
		[[nodiscard]] static constexpr iron::btree_map<_K, _M> make_btree_map(_ARGS&&... _args)noexcept
			requires (!iron::concepts::sequence<_K> && !iron::concepts::adapter_box<_K> && !iron::concepts::sequence<_M> && !iron::concepts::adapter_box<_M>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_ARGS(_args...);
			auto _r = try_make_btree_map<_K, _M>(std::forward<_ARGS>(_args)...); RP(_r);
		}
		template <typename _K, typename _M, typename... _ARGS>
		[[nodiscard]] static constexpr iron::result<iron::multi_btree_map<_K, _M>, error_handler_t> try_make_multi_btree_map(_ARGS&&... _args)noexcept
			requires (!iron::concepts::sequence<_K> && !iron::concepts::adapter_box<_K> && !iron::concepts::sequence<_M> && !iron::concepts::adapter_box<_M>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_ARGS(_args...);
			iron::multi_btree_map<_K, _M> _t;
			if constexpr (sizeof...(_ARGS)) {
				auto _h = _push_into_map(_t, std::forward<_ARGS>(_args)...); RR(_h);
			}
			return iron::ok(std::move(_t));
		}
		template <typename _K, typename _M, typename... _ARGS>
		[[nodiscard]] static constexpr iron::multi_btree_map<_K, _M> make_multi_btree_map(_ARGS&&... _args)noexcept
			requires (!iron::concepts::sequence<_K> && !iron::concepts::adapter_box<_K> && !iron::concepts::sequence<_M> && !iron::concepts::adapter_box<_M>)
		{
			auto _r = try_make_multi_btree_map<_K, _M>(std::forward<_ARGS>(_args)...); RP(_r);
		}
		template <typename _K, typename... _ARGS>
		[[nodiscard]] static constexpr iron::result<iron::btree_set<_K>, error_handler_t> try_make_btree_set(_ARGS&&... _args)noexcept
			requires (!iron::concepts::sequence<_K> && !iron::concepts::adapter_box<_K>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_ARGS(_args...);
			iron::btree_set<_K> _t;
			if constexpr (sizeof...(_ARGS)) {
				auto _h = _push_into(_t, std::forward<_ARGS>(_args)...); RR(_h);
			}
			return iron::ok(std::move(_t));
		}
		template <typename _K, typename... _ARGS>
		[[nodiscard]] static constexpr iron::btree_set<_K> make_btree_set(_ARGS&&... _args)noexcept
			requires (!iron::concepts::sequence<_K> && !iron::concepts::adapter_box<_K>)
		{
			auto _r = try_make_btree_set<_K>(std::forward<_ARGS>(_args)...); RP(_r);
		}
		template <typename _K, typename... _ARGS>
		[[nodiscard]] static constexpr iron::result<iron::multi_btree_set<_K>, error_handler_t> try_make_multi_btree_set(_ARGS&&... _args)noexcept
			requires (!iron::concepts::sequence<_K> && !iron::concepts::adapter_box<_K>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_ARGS(_args...);
			iron::multi_btree_set<_K> _t;
			if constexpr (sizeof...(_ARGS)) {
				auto _h = _push_into(_t, std::forward<_ARGS>(_args)...); RR(_h);
			}
			return iron::ok(std::move(_t));
		}
		template <typename _K, typename... _ARGS>
		[[nodiscard]] static constexpr iron::multi_btree_set<_K> make_multi_btree_set(_ARGS&&... _args)noexcept
			requires (!iron::concepts::sequence<_K> && !iron::concepts::adapter_box<_K>)
		{
			auto _r = try_make_multi_btree_set<_K>(std::forward<_ARGS>(_args)...); RP(_r);
		}
		template <typename _T,typename... _ARGS>
		[[nodiscard]] static constexpr iron::result<_T, error_handler_t> try_make_str(_ARGS&&... _args)noexcept 
		requires (iron::concepts::str<_T>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_ARGS(_args...);
			_T _t;
			if constexpr (sizeof...(_ARGS)) {
				auto _h = _push_into(_t, std::forward<_ARGS>(_args)...); RR(_h);
			}
			return iron::ok(std::move(_t));
		}
		template <typename _T=iron::ustrb,typename... _ARGS>
		[[nodiscard]] static constexpr iron::result<_T, error_handler_t> try_make_ustrb(_ARGS&&... _args)noexcept
		requires(std::is_same_v<_T,iron::ustrb>)
		{
			return try_make_str<_T>(std::forward<_ARGS>(_args)...);
		}
		template <typename _T=iron::ustr8,typename... _ARGS>
		[[nodiscard]] static constexpr iron::result<_T, error_handler_t> try_make_ustr8(_ARGS&&... _args)noexcept
		requires(std::is_same_v<_T,iron::ustr8>)
		{
			return try_make_str<_T>(std::forward<_ARGS>(_args)...);
		}
		template <typename _T=iron::ustr16,typename... _ARGS>
		[[nodiscard]] static constexpr iron::result<_T, error_handler_t> try_make_ustr16(_ARGS&&... _args)noexcept
		requires(std::is_same_v<_T,iron::ustr16>)
		{
			return try_make_str<_T>(std::forward<_ARGS>(_args)...);
		}
		template <typename _T=iron::ustrw,typename... _ARGS>
		[[nodiscard]] static constexpr iron::result<_T, error_handler_t> try_make_ustrw(_ARGS&&... _args)noexcept
		requires(std::is_same_v<_T,iron::ustrw>)
		{
			return try_make_str<_T>(std::forward<_ARGS>(_args)...);
		}
		template <typename _T=iron::ustr32,typename... _ARGS>
		[[nodiscard]] static constexpr iron::result<_T, error_handler_t> try_make_ustr32(_ARGS&&... _args)noexcept
		requires(std::is_same_v<_T,iron::ustr32>)
		{
			return try_make_str<_T>(std::forward<_ARGS>(_args)...);
		}
		template <typename _T=iron::strb,typename... _ARGS>
		[[nodiscard]] static constexpr iron::result<_T, error_handler_t> try_make_strb(_ARGS&&... _args)noexcept
		requires(std::is_same_v<_T,iron::strb>)
		{
			return try_make_str<_T>(std::forward<_ARGS>(_args)...);
		}
		template <typename _T=iron::str8,typename... _ARGS>
		[[nodiscard]] static constexpr iron::result<_T, error_handler_t> try_make_str8(_ARGS&&... _args)noexcept
		requires(std::is_same_v<_T,iron::str8>)
		{
			return try_make_str<_T>(std::forward<_ARGS>(_args)...);
		}
		template <typename _T=iron::str16,typename... _ARGS>
		[[nodiscard]] static constexpr iron::result<_T, error_handler_t> try_make_str16(_ARGS&&... _args)noexcept
		requires(std::is_same_v<_T,iron::str16>)
		{
			return try_make_str<_T>(std::forward<_ARGS>(_args)...);
		}
		template <typename _T=iron::strw,typename... _ARGS>
		[[nodiscard]] static constexpr iron::result<_T, error_handler_t> try_make_strw(_ARGS&&... _args)noexcept
		requires(std::is_same_v<_T,iron::strw>)
		{
			return try_make_str<_T>(std::forward<_ARGS>(_args)...);
		}
		template <typename _T=iron::str32,typename... _ARGS>
		[[nodiscard]] static constexpr iron::result<_T, error_handler_t> try_make_str32(_ARGS&&... _args)noexcept
		requires(std::is_same_v<_T,iron::str32>)
		{
			return try_make_str<_T>(std::forward<_ARGS>(_args)...);
		}
		template <typename _T, typename... _ARGS>
		[[nodiscard]] static constexpr _T make_str(_ARGS&&... _args)noexcept
		requires (iron::concepts::str<_T>)
		{
			auto _r = try_make_str<_T>(std::forward<_ARGS>(_args)...); RP(_r);
		}
		template <typename _T=iron::ustrb,typename... _ARGS>
		[[nodiscard]] static constexpr _T make_ustrb(_ARGS&&... _args)noexcept
		requires(std::is_same_v<_T,iron::ustrb>)
		{
			return make_str<_T>(std::forward<_ARGS>(_args)...);
		}
		template <typename _T=iron::ustr8,typename... _ARGS>
		[[nodiscard]] static constexpr _T make_ustr8(_ARGS&&... _args)noexcept
		requires(std::is_same_v<_T,iron::ustr8>)
		{
			return make_str<_T>(std::forward<_ARGS>(_args)...);
		}
		template <typename _T=iron::ustr16,typename... _ARGS>
		[[nodiscard]] static constexpr _T make_ustr16(_ARGS&&... _args)noexcept
		requires(std::is_same_v<_T,iron::ustr16>)
		{
			return make_str<_T>(std::forward<_ARGS>(_args)...);
		}
		template <typename _T=iron::ustrw,typename... _ARGS>
		[[nodiscard]] static constexpr _T make_ustrw(_ARGS&&... _args)noexcept
		requires(std::is_same_v<_T,iron::ustrw>)
		{
			return make_str<_T>(std::forward<_ARGS>(_args)...);
		}
		template <typename _T=iron::ustr32,typename... _ARGS>
		[[nodiscard]] static constexpr _T make_ustr32(_ARGS&&... _args)noexcept
		requires(std::is_same_v<_T,iron::ustr32>)
		{
			return make_str<_T>(std::forward<_ARGS>(_args)...);
		}
		template <typename _T=iron::strb,typename... _ARGS>
		[[nodiscard]] static constexpr _T make_strb(_ARGS&&... _args)noexcept
		requires(std::is_same_v<_T,iron::strb>)
		{
			return make_str<_T>(std::forward<_ARGS>(_args)...);
		}
		template <typename _T=iron::str8,typename... _ARGS>
		[[nodiscard]] static constexpr _T make_str8(_ARGS&&... _args)noexcept
		requires(std::is_same_v<_T,iron::str8>)
		{
			return make_str<_T>(std::forward<_ARGS>(_args)...);
		}
		template <typename _T=iron::str16,typename... _ARGS>
		[[nodiscard]] static constexpr _T make_str16(_ARGS&&... _args)noexcept
		requires(std::is_same_v<_T,iron::str16>)
		{
			return make_str<_T>(std::forward<_ARGS>(_args)...);
		}
		template <typename _T=iron::strw,typename... _ARGS>
		[[nodiscard]] static constexpr _T make_strw(_ARGS&&... _args)noexcept
		requires(std::is_same_v<_T,iron::strw>)
		{
			return make_str<_T>(std::forward<_ARGS>(_args)...);
		}
		template <typename _T=iron::str32,typename... _ARGS>
		[[nodiscard]] static constexpr _T make_str32(_ARGS&&... _args)noexcept
		requires(std::is_same_v<_T,iron::str32>)
		{
			return make_str<_T>(std::forward<_ARGS>(_args)...);
		}
	private: 
#if 0
		template <std::size_t _SZ>
		static constexpr void _bit_set(bit_set<_SZ>&)noexcept {}

		template <std::size_t _SZ, typename... _ARGS>
		static constexpr void _bit_set(bit_set<_SZ>& _s, std::size_t _v, const _ARGS&... _args)noexcept {
			_s.set(_v);
			_bit_set(_s, _args...);
		}

		template <typename _T>
		static constexpr void _enum_set(iron::enum_set<_T>&)noexcept {}

		template <typename _T,typename... _ARGS>
		static constexpr void _enum_set(iron::enum_set<_T>& _s,_T _v,_ARGS... _args)noexcept {
			_s.set(_v);
			_enum_set(_s, _args...);
		}
#endif 
		template <typename _T>
		[[nodiscard]] static constexpr error_handler_t _push_into(_T&)noexcept { return {};}

		template <typename _T, typename _U, typename... _ARGS >
		[[nodiscard]] static constexpr error_handler_t _push_into(_T& _container,_U&& _element,_ARGS&&... _args)noexcept {
			if constexpr (iron::concepts::sequence<_U>) {
				auto _h = _push_sequence(_container,std::forward<_U>(_element)); RE(_h);
				return _push_into(_container, std::forward<_ARGS>(_args)...);
			}
			else if constexpr (iron::concepts::adapter_box<_U>) {
				static_assert(!std::is_reference_v<_U>, "adapter must be passed rvalue");
				auto _h = _push_adapter(_container, std::move(_element)); RE(_h);
				return _push_into(_container, std::forward<_ARGS>(_args)...);
			}
			else if constexpr (
				iron::concepts::hash_map<_T> || iron::concepts::multi_hash_map<_T>
				|| iron::concepts::btree_map<_T> || iron::concepts::multi_btree_map<_T>
				) {
				return _push_into_map(_container, std::forward<_U>(_element), std::forward<_ARGS>(_args)...);
			}
			else {
				auto _h = _push_value(_container, std::forward<_U>(_element)); RE(_h);
				return _push_into(_container, std::forward<_ARGS>(_args)...);
			}
		}
		
		template <typename _T,typename _SEQ>
		[[nodiscard]] static constexpr error_handler_t _push_sequence(_T& _container, _SEQ&& _seq)noexcept {
			if constexpr (!std::is_reference_v<_SEQ>) { //rvalue
				auto _t = std::move(_seq); //mark _seq temporary
				return _push_sequence(_container, _t);
			}
			else {
				auto _seq1 = _seq;
				if constexpr (iron::concepts::vector<_T> || iron::concepts::list<_T>) {
					auto _r = _container.try_push_back_range(_seq1); RE(_r);
				}
				else if constexpr (iron::concepts::fwd_list<_T>) {
					auto _r = _container.try_push_front_range(_seq1); RE(_r);
				}
				else if constexpr (iron::concepts::hash_table<_T> || iron::concepts::btree_table<_T>) {
					auto _r = _container.try_insert_range(_seq1); RE(_r);
				}
				else if constexpr (iron::concepts::str<_T>) {
					using _seq_t = std::decay_t<decltype(_seq1)>;
					using _chr_t = typename _seq_t::value_t;
					if constexpr (iron::concepts::charset<_chr_t>) {
						auto _h = _container.try_push_back(_seq1); RE(_h);
					}
					else {
						static_assert(iron::unsafe::asserts::always_false<_chr_t>, "sequence value must be a charset");
					}
				}
				else {
					static_assert(iron::unsafe::asserts::always_false<_T>, "unknow container");
				}
				return {};
			}
		}

		template <typename _T,typename _U>
		[[nodiscard]] static constexpr error_handler_t _push_adapter(_T& _container,_U&& _adapter)noexcept {
			if (_container) {
				if constexpr (
					iron::concepts::hash_map<_T> || iron::concepts::multi_hash_map<_T>
					|| iron::concepts::btree_map<_T> || iron::concepts::multi_btree_map<_T>
				) {
					auto _r = _T::try_from(std::move(_adapter)); RE(_r);
					auto _r1 = _container.try_merge(_r.unchecked_unwrap()); RE(_r1);
				}
				else {
					if constexpr (iron::concepts::vector<_T>) {
						auto _h = _container.try_reserve_at_least(_adapter.size()); RE(_h);
					}
					auto _raw_adapter = _adapter.unsafe_release();
#if IRON_SECURITY_CHECKER 
					[[maybe_unused]] const auto _x = std::move(_adapter); //mark _adapter temporary
#endif 
					auto _p = _raw_adapter.data();
					if constexpr (iron::concepts::str<_T>) {
						using _char_t = decltype(*_p);
						if constexpr (iron::concepts::charset<_char_t>) {
							auto _h = _container.try_unchecked_push_back(_p, _p + _raw_adapter.size()); RE(_h);
						}
						else {
							static_assert(iron::unsafe::asserts::always_false<_char_t>, "adapter not have value_type charset");
						}
					}
					else {
						auto _sz = _raw_adapter.size();
						while (_sz--) {
							auto& _v = *_p++;
							auto _h = _push_value(_container, std::move(_v)); RE(_h);
						}
					}
				}
			}
			else { //container empty
				if constexpr (iron::concepts::vector<_T>) {
					_container = _T::from(std::move(_adapter));
				}
				else if constexpr (iron::concepts::str<_T>) {
					using _adapter_t = std::decay_t<_U>;
					using _a_t = typename _adapter_t::value_t;
					using _v_t = typename _T::value_t;
					if constexpr (std::is_same_v<_a_t,_v_t>) {
						auto _r = _T::try_from(std::move(_adapter)); RE(_r);
						_r.unchecked_unwrap().swap(_container);
					}
					else if constexpr(iron::concepts::charset<_a_t>) {
						auto _raw_adapter = _adapter.unsafe_release();
#if IRON_SECURITY_CHECKER 
						[[maybe_unused]] const auto _x = std::move(_adapter); //mark _adapter temporary
#endif 
						auto _p = _raw_adapter.data();
						auto _h = _container.try_unchecked_push_back(_p, _p + _raw_adapter.size()); RE(_h);
					}
					else {
						static_assert(iron::unsafe::asserts::always_false<_a_t>, "adapter not have value_type charset");
					}
				}
				else {
					auto _r = _T::try_from(std::move(_adapter)); RE(_r);
					_r.unchecked_unwrap().swap(_container);
				}
			}
			return {};
		}
		template <typename _T, typename _U>
		[[nodiscard]] static constexpr error_handler_t _push_value(_T& _container, _U&& _v)noexcept {
			if constexpr (iron::concepts::vector<_T> || iron::concepts::list<_T>) {
				auto _r = _container.try_push_back(std::forward<_U>(_v)); RE(_r);
			}
			else if constexpr (iron::concepts::fwd_list<_T>) {
				auto _r = _container.try_push_front(std::forward<_U>(_v)); RE(_r);
			}
			else if constexpr (
				iron::concepts::hash_set<_T> || iron::concepts::multi_hash_set<_T>
				|| iron::concepts::btree_set<_T> || iron::concepts::multi_btree_set<_T>
				) 
			{
				auto _r = _container.try_insert(std::forward<_U>(_v)); RE(_r);
			}
			else if constexpr (iron::concepts::str<_T>) {
				return _push_str_value(_container,std::forward<_U>(_v));
			}
			else {
				static_assert(iron::unsafe::asserts::always_false<_T>, "unknow container");
				//PANIC("todo");
			}
			return {};
		}
		template <typename _T, typename _U>
		[[nodiscard]] static constexpr error_handler_t _push_str_value(_T& _container, _U&& _v)noexcept 
		{
			if constexpr (iron::concepts::fixed_charset<_U>) {
				auto _h = _container.try_push_back(_v); RE(_h);
			}
			else if constexpr (iron::concepts::charset<_U>) {
				static_assert(iron::unsafe::asserts::always_false<_U>, "value must be a fixed charset");
			}
			else if constexpr (std::is_array_v<std::remove_reference_t<_U>>) {
				if constexpr (iron::concepts::charset<decltype(_v[0])>) {
					auto _h = _container.try_push_back(_v); RE(_h);
				}
				else {
					static_assert(iron::unsafe::asserts::always_false<_U>, "value is an array but the single element is not a charset");
				}
			}
			else {
				static_assert(iron::unsafe::asserts::always_false<_U>, "value is not an array");
//				PANIC("todo");
			}
			return {};
		}
		template <typename _T, typename _U>
		[[nodiscard]] static constexpr error_handler_t _assign(_T* _p, _U&& _element)noexcept
		{
			return iron::unsafe::raw_storage<_T>::assign(*_p, std::forward<_U>(_element));
		}
		template <typename _T,typename _U,typename... _ARGS>
		[[nodiscard]] static constexpr error_handler_t _assign_list(_T* _p, _U&& _element, _ARGS&&... _args)noexcept {
			DEBUG_ASSERT(_p);
			auto _h = _assign(_p, std::forward<_U>(_element)); RE(_h);
			if constexpr (sizeof...(_ARGS) != 0) {
				_h = _assign_list(++_p, std::forward<_ARGS>(_args)...); RE(_h);
			}
			return _h;
		}
		template <typename _T>
		[[nodiscard]] static constexpr error_handler_t _push_into_map(_T&)noexcept {
			return {};
		}
		template <typename _T, typename _K>
		[[nodiscard]] static constexpr error_handler_t _push_into_map(_T& _container, _K&& _k)noexcept {
			if constexpr (iron::concepts::sequence<_K> || iron::concepts::adapter_box<_K>) {
				auto _h = _push_into(_container, std::forward<_K>(_k)); RE(_h);
			}
			else {
				auto _r = _container.try_insert(std::forward<_K>(_k), typename _T::mapped_t{}); RE(_r);
			}
			return {};
		}
		template <typename _T, typename _K, typename _M>
		[[nodiscard]] static constexpr error_handler_t _push_into_map_key_mapped(_T& _container, _K&& _k,_M&& _m)noexcept {
			static_assert(!iron::concepts::sequence<_K> && !iron::concepts::adapter_box<_K>, "internal error");
			static_assert(!iron::concepts::sequence<_M> && !iron::concepts::adapter_box<_M>, "internal error");
			auto _r = _container.try_insert(std::forward<_K>(_k),std::forward<_M>(_m)); RE(_r);
			return {};
		}

		template <typename _T,typename _K,typename _M, typename... _ARGS>
		[[nodiscard]] static constexpr error_handler_t _push_into_map(_T& _container, _K&& _k, _M&& _m, _ARGS&&... _args)noexcept 
		{
			if constexpr (iron::concepts::sequence<_K> || iron::concepts::adapter_box<_K>) {
				auto _h = _push_into(_container, std::forward<_K>(_k)); RE(_h);
				return _push_into_map(_container, std::forward<_M>(_m), std::forward<_ARGS>(_args)...);
			}
			else if constexpr (iron::concepts::sequence<_M> || iron::concepts::adapter_box<_M>) {
				auto _h = _push_into_map(_container, std::forward<_K>(_k)); RE(_h);
				_h = _push_into(_container, std::forward<_M>(_m)); RE(_h);
				return _push_into_map(_container, std::forward<_ARGS>(_args)...);
			}
			else {
				auto _h = _push_into_map_key_mapped(_container,std::forward<_K>(_k), std::forward<_M>(_m)); RE(_h);
				return _push_into_map(_container, std::forward<_ARGS>(_args)...);
			}
		}
	};
}

#if defined(_MSC_VER)
#pragma warning(pop)
#endif 

