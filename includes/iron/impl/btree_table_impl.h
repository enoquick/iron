
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable:26481)
#pragma warning (disable:26496)
#endif

namespace iron {
	template <typename _KEY, typename _MAPPED, typename _LT, std::size_t _ORDER, typename _A, bool _MULTI, bool _HAS_MAPPED>
	class btree_table final {
	public:
		using key_t = _KEY;
		using key_type = key_t;
		using mapped_t = _MAPPED;
		using mapped_type = mapped_t;
		using allocator_t = _A;
		using allocator_type = allocator_t;
		using key_less_t = _LT;
		using key_less_type = key_less_t;
		using size_t = std::size_t;
		using size_type = size_t;
		static_assert(sizeof(key_t) > 0, "btree table require sizeof key_t > 0");
		static constexpr size_t order = _ORDER;
		static_assert(order > 1, "btree table require order > 1");
		static constexpr bool is_multi = _MULTI;
		static constexpr bool has_mapped = _HAS_MAPPED;
		static_assert(!std::is_reference_v<key_t>, "btree_table key_type  must not be a reference type");
		static_assert(!std::is_array_v<key_t>, "btree_table key_type must not be an array type");
		static_assert(!std::is_const_v<key_t>, "btree_table key_type must not be const");
		static_assert(std::is_nothrow_destructible_v<key_t>, "btree_table key_type must be nothrow destructible");
		static_assert(std::is_nothrow_move_constructible_v<key_t>, "btree_table key_type must be nothrow move constructible");
		static_assert(std::is_nothrow_swappable_v<key_t>, "btree_table key_type must be nothrow swappable");
		static_assert(!has_mapped || !std::is_reference_v<mapped_t>, "btree_table mapped_type  must not be a reference type");
		static_assert(!has_mapped || !std::is_array_v<mapped_t>, "btree_table mapped_type must not be an array type");
		static_assert(!has_mapped || !std::is_const_v<mapped_t>, "btree_table mapped_type must not be const");
		static_assert(!has_mapped || std::is_nothrow_destructible_v<mapped_t>, "btree_table mapped_type nothrow destructible");
		static_assert(!has_mapped || std::is_nothrow_move_constructible_v<mapped_t>, "btree_table mapped_type must be nothrow move constructible");
		static_assert(!has_mapped || std::is_nothrow_swappable_v<mapped_t>, "btree_table mapped_type must be nothrow swappable");
		static_assert(iron::concepts::allocator<allocator_t>, "btree_table allocator_type must be an allocator");
		static_assert(iron::concepts::comparable<key_less_t, key_t>, "key_less function non appropriate for key_type");
	private:
		using _raw_btree_table_t = iron::unsafe::btree::raw_table<key_t, mapped_t, key_less_t, order, allocator_t, is_multi, has_mapped>;
		using _raw_adapter_box_t = typename _raw_btree_table_t::adapter_box_t;
		using _rollback_t = typename _raw_btree_table_t::rollback_t;
		using _iter_t = typename _raw_btree_table_t::iter_t;
		using _citer_t = typename _raw_btree_table_t::const_iter_t;
		using _riter_t = typename _raw_btree_table_t::riter_t;
		using _criter_t = typename _raw_btree_table_t::const_riter_t;
		using _posix_code_t = iron::unsafe::errors::posix::code_t;
	public:
		using value_t = typename _raw_btree_table_t::value_t;
		using value_type = value_t;
		using exported_value_t = typename _raw_btree_table_t::exported_value_t;
		using exported_value_type = exported_value_t;
		using ptr_t = value_t*;
		using pointer = ptr_t;
		using cptr_t = const value_t*;
		using const_pointer = cptr_t;
		using diff_t = std::ptrdiff_t;
		using difference_type = diff_t;
		using reference = value_t&;
		using const_reference = const value_t&;
		using ref_t = iron::ref_wrapper<value_t>;
		using cref_t = iron::ref_wrapper<const value_t>;
		using error_handler_t = iron::error_handler;
		using error_handler_type = error_handler_t;
		using adapter_box_t = iron::adapter_box<_raw_adapter_box_t>;
		using iterator = _iter_t;
		using const_iterator = _citer_t;
		using reverse_iterator = _riter_t;
		using const_reverse_iterator = _criter_t;
		using seq_t = iron::sequence<iterator>;
		using seq_type = seq_t;
		using cseq_t = iron::sequence<const_iterator>;
		using cseq_type = cseq_t;
		using rseq_t = iron::sequence<reverse_iterator>;
		using rseq_type = rseq_t;
		using crseq_t = iron::sequence<const_reverse_iterator>;
		using crseq_type = crseq_t;
		using tree_node_t = typename _raw_btree_table_t::tree_node_t;
		struct btree_table_tag { btree_table_tag() = delete; };
		[[nodiscard]] static constexpr size_t max_size()noexcept { return _raw_btree_table_t::max_size(); }
		[[nodiscard]] static constexpr iron::result<btree_table, error_handler_t> try_from(adapter_box_t&& _box)noexcept
		{
			return _from(std::move(_box));
		}
		[[nodiscard]] static constexpr btree_table from(adapter_box_t&& _box)noexcept
		{
			auto _r = try_from(std::move(_box)); RP(_r);
		}
		[[nodiscard]] static constexpr btree_table unchecked_from(allocator_t _a)noexcept {
			return btree_table(_a);
		}
		constexpr btree_table()noexcept
			: btree_table(allocator_t{})
		{}
	private:
		constexpr explicit btree_table(allocator_t _a)noexcept
		: _m_rh(_a)
		{}
	public:
		btree_table(const btree_table&) = delete;
		btree_table& operator=(const btree_table&) = delete;
		constexpr btree_table(btree_table&& _v)noexcept
		{
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_swap(_v);
			IRON_SECURITY_MARK_TEMPORARY(_v);
		}
		constexpr btree_table& operator=(btree_table&& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_swap(_v);
			IRON_SECURITY_MARK_TEMPORARY(_v);
			return *this;
		}
		constexpr ~btree_table() = default;

		[[nodiscard]] constexpr bool empty()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _m_rh.empty();
		}
		[[nodiscard]] constexpr explicit operator bool()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _m_rh ? true : false;
		}
		[[nodiscard]] constexpr size_t size()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _m_rh.size();
		}
		constexpr btree_table& swap(btree_table& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_swap(_v);
			return *this;
		}
		[[nodiscard]] constexpr iron::result<btree_table, error_handler_t> try_clone(size_t _n=1)const noexcept
			requires(iron::concepts::copyable_or_cloneable<key_t>&& iron::concepts::copyable_or_cloneable<mapped_t>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			auto _r = _m_rh.try_clone(_n); RR(_r);
			return iron::ok(btree_table(std::move(_r.unchecked_unwrap())));
		}
		[[nodiscard]] constexpr btree_table clone(size_t _n=1)const noexcept
			requires(iron::concepts::copyable_or_cloneable<key_t>&& iron::concepts::copyable_or_cloneable<mapped_t>)
		{
			auto _r = try_clone(_n); RP(_r);
		}
		[[nodiscard]] constexpr allocator_t unsafe_allocator()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _m_rh.allocator();
		}
		[[nodiscard]] constexpr seq_t seq()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return seq_t::unchecked_from(*this,_m_rh.begin(), _m_rh.end());
		}
		[[nodiscard]] constexpr cseq_t seq()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return cseq_t::unchecked_from(*this,_m_rh.begin(), _m_rh.end());
		}
		[[nodiscard]] constexpr cseq_t cseq()const noexcept { return seq(); }

		[[nodiscard]] constexpr rseq_t rseq()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return rseq_t::unchecked_from(*this, _m_rh.rbegin(), _m_rh.rend());
		}
		[[nodiscard]] constexpr crseq_t rseq()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return crseq_t::unchecked_from(*this, _m_rh.rbegin(), _m_rh.rend());
		}
		[[nodiscard]] constexpr crseq_t crseq()const noexcept { return rseq(); }

		[[nodiscard]] constexpr seq_t find(const key_t& _k)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_k);
			auto _iter = _m_rh.find(_k);
			return seq_t::unchecked_from(*this, _iter, _m_rh.end());
		}
		[[nodiscard]] constexpr cseq_t find(const key_t& _k)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_k);
			auto _iter=_m_rh.find(_k);
			return cseq_t::unchecked_from(*this, _iter, _m_rh.end());
		}
		[[nodiscard]] constexpr cseq_t cfind(const key_t& _k)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY(_k);
			return find(_k);
		}
		template <typename _K,typename _M>
		[[nodiscard]] constexpr iron::result<std::pair<seq_t,bool>, error_handler_t> try_insert(_K&& _key,_M&& _mapped)noexcept
		requires (
			has_mapped
			&& std::constructible_from<key_t,_K>
			&& std::constructible_from<mapped_t,_M>
		)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_key);
			IRON_SECURITY_CHECK_TEMPORARY(_mapped);
			return _insert(std::forward<_K>(_key), std::forward<_M>(_mapped));
		}

		template <typename _K, typename _M>
		constexpr std::pair<seq_t,bool> insert(_K&& _k,_M&& _m)noexcept
		requires (
			has_mapped
			&& std::constructible_from<key_t, _K>
			&& std::constructible_from<mapped_t, _M>
			)
		{
			auto _r = try_insert(std::forward<_K>(_k),std::forward<_M>(_m)); RP(_r);
		}

		template <typename _K>
		[[nodiscard]] constexpr iron::result<std::pair<seq_t, bool>, error_handler_t> try_insert(_K&& _key)noexcept
			requires (
			!has_mapped
			&& std::constructible_from<key_t, _K>
			)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_key);
			return _insert(std::forward<_K>(_key), mapped_t{});
		}
		template <typename _K>
		constexpr std::pair<seq_t, bool> insert(_K&& _k)noexcept
			requires (
			!has_mapped
			&& std::constructible_from<key_t, _K>
			)
		{
			auto _r = try_insert(std::forward<_K>(_k)); RP(_r);
		}
		template <typename _SEQ>
		[[nodiscard]] constexpr iron::result<size_t, error_handler_t> try_insert_range(_SEQ& _seq)noexcept
			requires(
			has_mapped
			&& iron::concepts::sequence<_SEQ>
			&& !std::is_const_v<_SEQ>
			&& iron::concepts::std_pair<typename _SEQ::value_t>
			&& iron::concepts::constructible_at<key_t, std::decay_t<typename _SEQ::value_t::first_type>&&>
			&& iron::concepts::constructible_at<mapped_t, std::decay_t<typename _SEQ::value_t::second_type>&&>
			)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_seq);
			constexpr bool _transactional = false;
			return _range_insert<_transactional>(_seq);
		}
		template <typename _SEQ>
		constexpr size_t insert_range(_SEQ& _seq)noexcept
		requires(
			has_mapped
			&& iron::concepts::sequence<_SEQ>
			&& iron::concepts::std_pair<typename _SEQ::value_t>
			&& iron::concepts::constructible_at<key_t, std::decay_t<typename _SEQ::value_t::first_type>&&>
			&& iron::concepts::constructible_at<mapped_t,std::decay_t<typename _SEQ::value_t::second_type>&&>
			)
		{

			auto _r = try_insert_range(_seq); RP(_r);
		}
		
		template <typename _SEQ>
		[[nodiscard]] constexpr iron::result<size_t, error_handler_t> try_transactional_insert_range(_SEQ& _seq)noexcept
			requires(
			has_mapped
			&& iron::concepts::sequence<_SEQ>
			&& !std::is_const_v<_SEQ>
			&& iron::concepts::std_pair<typename _SEQ::value_t>
			&& iron::concepts::constructible_at<key_t, std::decay_t<typename _SEQ::value_t::first_type>&&>
			&& iron::concepts::constructible_at<mapped_t, std::decay_t<typename _SEQ::value_t::second_type>&&>
			)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_seq);
			constexpr bool _transactional = true;
			return _range_insert<_transactional>(_seq);
		}
		template <typename _SEQ>
		constexpr size_t transactional_insert_range(_SEQ& _seq)noexcept
			requires(
			has_mapped
			&& iron::concepts::sequence<_SEQ>
			&& !std::is_const_v<_SEQ>
			&& iron::concepts::std_pair<typename _SEQ::value_t>
			&& iron::concepts::constructible_at<key_t, std::decay_t<typename _SEQ::value_t::first_type>&&>
			&& iron::concepts::constructible_at<mapped_t, std::decay_t<typename _SEQ::value_t::second_type>&&>
			)
		{

			auto _r = try_transactional_insert_range(_seq); RP(_r);
		}

		template <typename _SEQ>
		[[nodiscard]] constexpr iron::result<size_t, error_handler_t> try_insert_range(_SEQ& _seq)noexcept
			requires(
			!has_mapped
			&& iron::concepts::sequence<_SEQ>
			&& !std::is_const_v<_SEQ>
			&& iron::concepts::constructible_at<key_t, std::decay_t<typename _SEQ::value_t>&&>
			)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_seq);
			constexpr bool _transactional = false;
			return _range_insert<_transactional>(_seq);
		}

		template <typename _SEQ>
		constexpr size_t insert_range(_SEQ& _seq)noexcept
		requires(
			!has_mapped
			&& iron::concepts::sequence<_SEQ>
			&& !std::is_const_v<_SEQ>
			&& iron::concepts::constructible_at<key_t,std::decay_t<typename _SEQ::value_t>&&>
			)
		{
			auto _r = try_insert_range(_seq); RP(_r);
		}

		template <typename _SEQ>
		[[nodiscard]] constexpr iron::result<size_t, error_handler_t> try_transactional_insert_range(_SEQ& _seq)noexcept
			requires(
			!has_mapped
			&& iron::concepts::sequence<_SEQ>
			&& !std::is_const_v<_SEQ>
			&& iron::concepts::constructible_at<key_t, std::decay_t<typename _SEQ::value_t>&&>
			)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_seq);
			constexpr bool _transactional = true;
			return _range_insert<_transactional>(_seq);
		}

		template <typename _SEQ>
		constexpr size_t transactional_insert_range(_SEQ& _seq)noexcept
			requires(
			!has_mapped
			&& iron::concepts::sequence<_SEQ>
			&& !std::is_const_v<_SEQ>
			&& iron::concepts::constructible_at<key_t, std::decay_t<typename _SEQ::value_t>&&>
			)
		{
			auto _r = try_transactional_insert_range(_seq); RP(_r);
		}
		constexpr btree_table& reset()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			if (_m_rh.size()) IRON_LIKELY
			{
				_m_rh.reset();
			}
			return *this;
		}

		template <typename _K,typename... _ARGS >
		constexpr size_t erase(const _K& _key,const _ARGS&... _args)noexcept
		requires (
			std::is_nothrow_convertible_v<std::decay_t<_K>, key_t>
			&&
			(std::is_nothrow_convertible_v<std::decay_t<_ARGS>, key_t>& ...)
		)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_key);
			IRON_SECURITY_CHECK_TEMPORARY_ARGS(_args...);
			return _m_rh.erase_keys(_key, _args...);
		}
		[[nodiscard]] constexpr iron::result<size_t,error_handler_t> try_merge(btree_table& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES(_v);
			constexpr bool _transactional{};
			_rollback_t _rollback;
			return _merge<_transactional>(_v,_rollback);
		}
		constexpr size_t merge(btree_table& _v)noexcept {
			auto _r = try_merge(_v); RP(_r);
		}
		template <typename... _ARGS >
		[[nodiscard]] constexpr iron::result<adapter_box_t, error_handler_t> try_release_list(const _ARGS&... _keys)noexcept
		requires (std::is_nothrow_convertible_v<std::decay_t<_ARGS>, key_t>& ...)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			IRON_SECURITY_CHECK_TEMPORARY_ARGS(_keys...);
			if constexpr (sizeof...(_ARGS) != 0) {
				auto _r = _m_rh.release_list(_keys...); RR(_r);
				return iron::ok(adapter_box_t::unchecked_from(std::move(_r.unchecked_unwrap())));
			}
			else {
				return {};
			}
		}
		template <typename... _ARGS>
		[[nodiscard]] constexpr adapter_box_t release_list(const _ARGS&... _keys)noexcept
		requires (std::is_nothrow_convertible_v<std::decay_t<_ARGS>, key_t>& ...)
		{
			auto _r = try_release_list(_keys...); RP(_r);
		}
		[[nodiscard]] constexpr iron::result<adapter_box_t, error_handler_t> try_release_all()noexcept
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			auto _r = _m_rh.release_all(); RR(_r);
			return iron::ok(adapter_box_t::unchecked_from(std::move(_r.unchecked_unwrap())));
		}
		[[nodiscard]] constexpr adapter_box_t release_all()noexcept
		{
			auto _r = try_release_all(); RP(_r);
		}

		[[nodiscard]] constexpr const tree_node_t* unsafe_data()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _m_rh.data();
		}
		[[nodiscard]] constexpr const tree_node_t* unsafe_cdata()const noexcept {
			return unsafe_data();
		}
		[[nodiscard]] constexpr tree_node_t* unsafe_data()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _m_rh.data();
		}

		template <typename _KEY1, typename _MAPPED1, typename _LT1, std::size_t _ORDER1, typename _A1, bool _MULTI1, bool _HAS_MAPPED1>
		[[nodiscard]] constexpr bool operator==(const btree_table<_KEY1,_MAPPED1,_LT1, _ORDER1, _A1,_MULTI1,_HAS_MAPPED1>& _r)const noexcept 
		requires(
			has_mapped == _HAS_MAPPED1
			&& std::is_same_v<key_t,_KEY1>
			&& std::is_same_v<key_less_t, _LT1>
			&& (!has_mapped || std::is_same_v<mapped_t,_MAPPED1>)
			&& (!has_mapped || iron::concepts::nothrow_equality_comparable<mapped_t>)
		)
		{
			return _m_rh == _r._m_rh;
		}
		
		IRON_SECURITY_METHOD();
	private:
		template <typename _KEY1, typename _MAPPED1, typename _LT1, std::size_t _ORDER1, typename _A1, bool _MULTI1, bool _HAS_MAPPED1>
		friend class btree_table;
		constexpr explicit btree_table(_raw_btree_table_t&& _v)noexcept
			: _m_rh(std::move(_v))
		{}
		constexpr void _swap(btree_table& _v)noexcept {
			_m_rh.swap(_v._m_rh);
			IRON_SECURITY_SWAP(_v);
		}
		template <typename _K, typename _M>
		[[nodiscard]] constexpr iron::result<std::pair<seq_t, bool>, error_handler_t> _insert(_K&& _k, _M&& _m)noexcept {
			auto _r = _m_rh.insert(std::forward<_K>(_k), std::forward<_M>(_m)); RR(_r);
			auto& _pair = _r.unchecked_unwrap();
			return iron::ok(std::make_pair(seq_t::unchecked_from(*this,_pair.first, _m_rh.end()), _pair.second));
		}

		template <bool _TRANSACTIONAL>
		[[nodiscard]] constexpr iron::result<size_t, error_handler_t> _merge(btree_table& _ht,_rollback_t& _rollback)noexcept {
			if (this == &_ht) IRON_UNLIKELY{
				return iron::err(ERROR_HANDLER(_posix_code_t::invalid_argument));
			}
			auto& _rh = _ht._m_rh;
			if (_rh.size()) IRON_LIKELY{
				if (_m_rh.empty()) IRON_UNLIKELY {
					_swap(_ht);
					return iron::ok(_m_rh.size());
				}
				return _m_rh. template merge<_TRANSACTIONAL>(_rh,_rollback);
			}
			return iron::ok(size_t{});
		}
		template <bool _TRANSACTIONAL,typename _SEQ>
		[[nodiscard]] constexpr iron::result<size_t, error_handler_t> _range_insert(_SEQ& _seq) 
		requires(!std::is_const_v<_SEQ>)
		{

			size_t _c{};
			if (_seq) IRON_LIKELY 
			{
				_rollback_t _rollback(_m_rh.allocator());
				if constexpr (_TRANSACTIONAL) {
					auto _rn = _rollback.reserve(_seq.clone().count()); RR(_rn);
				}
				auto& _b = _seq.begin();
				const auto _e = _seq.end();
				constexpr bool _move{};
				auto _r=_m_rh.template insert_range<_TRANSACTIONAL,_move>(_b, _e,_rollback);
				if (!_r) IRON_UNLIKELY 
				{
					if constexpr (_TRANSACTIONAL) {
						_m_rh.rollback(_rollback);
					}
					return _r;
				}
				_c = _r.unchecked_unwrap();
			}
			return iron::ok(_c);
		}
		[[nodiscard]] static constexpr iron::result<btree_table, error_handler_t> _from(adapter_box_t&& _box)noexcept {
			auto _rbox = _box.unsafe_release();
#if IRON_SECURITY_CHECKER
			[[maybe_unused]] const auto _x = std::move(_box); //mark _box temporary
#endif

#ifdef _MSC_VER //for a bug in costexpr eval not use private constructor
			auto _ht = unchecked_from(_rbox.allocator());
#else 
			btree_table _ht(_rbox.allocator());
#endif 
			if (_rbox.size()) {
				_rollback_t _rollback_nodes(_rbox.allocator());
				constexpr bool _transactional = false;
				constexpr bool _move = true;
				auto _b = _rbox.data();
				auto _r = _ht._m_rh.template insert_range<_transactional,_move>(_b, _rbox.data() + _rbox.size(), _rollback_nodes); RR(_r);
			}
			return iron::ok(std::move(_ht));
		}
		_raw_btree_table_t _m_rh;
		IRON_SECURITY_DCL();
	};


} //ns

#ifdef _IRON_CHECK_BTREE
static_assert(iron::concepts::btree_map<iron::btree_map<int, int>>, "internal error");
static_assert(iron::concepts::multi_btree_map<iron::multi_btree_map<int, int>>, "internal error");
static_assert(iron::concepts::btree_set<iron::btree_set<int>>, "internal error");
static_assert(iron::concepts::multi_btree_set<iron::multi_btree_set<int>>, "internal error");
#endif 

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

