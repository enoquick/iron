
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable:26481)
#pragma warning (disable:26496)
#endif

namespace iron {

	template <typename _KEY,typename _MAPPED,typename _EQ,typename _HASHER,typename _A,bool _MULTI,bool _HAS_MAPPED>
	class hash_table final  {
	public:
		using key_t = _KEY;
		using key_type = key_t;
		using mapped_t = _MAPPED;
		using mapped_type = mapped_t;
		using allocator_t = _A;
		using allocator_type = allocator_t;
		using key_equal_t = _EQ;
		using key_equal_type = key_equal_t;
		using hasher_t = _HASHER;
		using hasher_type = hasher_t;
		static constexpr bool is_multi = _MULTI;
		static constexpr bool has_mapped = _HAS_MAPPED;
		static_assert(sizeof(key_t) > 0, "hash_table require sizeof key_t > 0");
		static_assert(!std::is_reference_v<key_t>, "hash_table key_type  must not be a reference type");
		static_assert(!std::is_array_v<key_t>, "hash_table key_type must not be an array type");
		static_assert(!std::is_const_v<key_t>, "hash_table key_type must not be const");
		static_assert(std::is_nothrow_destructible_v<key_t>, "hash_table key_type must be nothrow destructible");
		static_assert(std::is_nothrow_move_constructible_v<key_t>, "hash_table key_type must be nothrow move constructible");
		static_assert(std::is_nothrow_swappable_v<key_t>, "hash_table key_type must be nothrow swappable");
		static_assert(!has_mapped || !std::is_reference_v<mapped_t>, "hash_table mapped_type  must not be a reference type");
		static_assert(!has_mapped || !std::is_array_v<mapped_t>, "hash_table mapped_type must not be an array type");
		static_assert(!has_mapped || !std::is_const_v<mapped_t>, "hash_table mapped_type must not be const");
		static_assert(!has_mapped || std::is_nothrow_destructible_v<mapped_t>, "hash_table mapped_type nothrow destructible");
		static_assert(!has_mapped || std::is_nothrow_move_constructible_v<mapped_t>, "hash_table mapped_type must be nothrow move constructible");
		static_assert(!has_mapped || std::is_nothrow_swappable_v<mapped_t>, "hash_table mapped_type must be nothrow swappable");
		static_assert(iron::concepts::allocator<allocator_t>, "hash_table allocator_type must be an allocator");
		static_assert(iron::concepts::hashable<hasher_t, key_t>, "hasher function non appropriate for key_type");
		static_assert(iron::concepts::comparable<key_equal_t, key_t>, "key_equal function non appropriate for key_type");
	private:
		using _raw_hash_table_t = iron::unsafe::raw_hash_table<key_t, mapped_t, key_equal_t, hasher_t,allocator_t,is_multi, has_mapped>;
		using _raw_adapter_box_t = typename _raw_hash_table_t::adapter_box_t;
		using _list_t = typename _raw_hash_table_t::list_t;
		using _node_t = typename _raw_hash_table_t::node_t;
		using _iter_t = typename _raw_hash_table_t::iter_t;
		using _citer_t = typename _raw_hash_table_t::citer_t;
		using _posix_code_t = iron::unsafe::errors::posix::code_t;
	public:
		using value_t = typename _raw_hash_table_t::value_t;
		using value_type = value_t;
		using exported_value_t = typename _raw_hash_table_t::exported_value_t;
		using exported_value_type = exported_value_t;
		using size_t = std::size_t;
		using size_type = size_t;
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
		using load_factor_t = float;
		using load_factor_type = load_factor_t;
		using adapter_box_t = iron::adapter_box<_raw_adapter_box_t>;
		using iterator = _iter_t;
		using const_iterator = _citer_t;
		using seq_t = iron::sequence<iterator>;
		using cseq_t = iron::sequence<const_iterator>;
		using list_t = typename _raw_hash_table_t::list_t;
		struct hash_table_tag { hash_table_tag() = delete; };
		[[nodiscard]] static constexpr load_factor_t default_load_factor()noexcept { return 0.7F; };
		[[nodiscard]] static constexpr size_t max_size()noexcept { return _raw_hash_table_t::max_size(); }
		[[nodiscard]] static constexpr size_t max_bucket_count()noexcept { return _raw_hash_table_t::max_bucket_count(); }
		[[nodiscard]] static constexpr load_factor_t max_load_factor() { return _raw_hash_table_t::max_load_factor(); }
		[[nodiscard]] static constexpr iron::result<hash_table, error_handler_t> try_from(adapter_box_t&& _box, load_factor_t _threshold = default_load_factor())noexcept 
		{
			return _from(std::move(_box), _threshold,{});
		}
		[[nodiscard]] static constexpr hash_table from(adapter_box_t&& _box, load_factor_t _threshold = default_load_factor())noexcept
		{
			auto _r = try_from(std::move(_box), _threshold); RP(_r);
		}
		constexpr hash_table()noexcept
			: hash_table(allocator_t{})
		{}
		
		[[nodiscard]] static constexpr hash_table unchecked_from(allocator_t _a)noexcept {
			return hash_table(_a);
		}

		hash_table(const hash_table&) = delete;
		hash_table& operator=(const hash_table&) = delete;
		constexpr hash_table(hash_table&& _v)noexcept
		{
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_swap(_v);
			IRON_SECURITY_MARK_TEMPORARY(_v);
		}
		constexpr hash_table& operator=(hash_table&& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_swap(_v);
			IRON_SECURITY_MARK_TEMPORARY(_v);
			return *this;
		}
		constexpr ~hash_table() = default; 

		[[nodiscard]] constexpr load_factor_t load_factor_threshold()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _m_rh.load_factor_threshold();
		}
		[[nodiscard]] constexpr load_factor_t load_factor()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _m_rh.load_factor();
		}
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
		[[nodiscard]] constexpr size_t bucket_count()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _m_rh.bucket_count();
		}
		constexpr hash_table& swap(hash_table& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_swap(_v);
			return *this;
		}
		[[nodiscard]] constexpr iron::result<hash_table, error_handler_t> try_clone()const noexcept 
		requires(iron::concepts::copyable_or_cloneable<key_t> && iron::concepts::copyable_or_cloneable<mapped_t>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			auto _r = _m_rh.try_clone(); RR(_r);
			return iron::ok(hash_table(std::move(_r.unchecked_unwrap())));
		}
		[[nodiscard]] constexpr hash_table clone()const noexcept
		requires(iron::concepts::copyable_or_cloneable<key_t>&& iron::concepts::copyable_or_cloneable<mapped_t>)
		{
			auto _r = try_clone(); RP(_r);
		}
		[[nodiscard]] constexpr iron::result<hash_table, error_handler_t> try_clone(load_factor_t _lf)const noexcept
			requires(iron::concepts::copyable_or_cloneable<key_t>&& iron::concepts::copyable_or_cloneable<mapped_t>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			auto _h = _check_load_factor(_lf); RR(_h);
			auto _r = _m_rh.clone(_lf); RR(_r);
			return iron::ok(hash_table(std::move(_r.unchecked_unwrap())));
		}
		[[nodiscard]] constexpr hash_table clone(load_factor_t _lf)const noexcept
			requires(iron::concepts::copyable_or_cloneable<key_t>&& iron::concepts::copyable_or_cloneable<mapped_t>)
		{
			auto _r = try_clone(_lf); RP(_r);
		}

		[[nodiscard]] constexpr error_handler_t try_rehash(load_factor_t _lf)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			auto _h = _check_load_factor(_lf); RE(_h);
			if (_m_rh.size()) IRON_LIKELY 
			{
				_h = _check_reserve<_posix_code_t::result_out_of_range>(0, 0, _m_rh.size()); RE(_h);
				const auto _new_bc = _raw_hash_table_t::compute_bucket_count(_lf, _m_rh.size(), 0, 0);
				if (_new_bc != _m_rh.bucket_count()) IRON_LIKELY
				{
					_h = _m_rh.rehash(_new_bc, _lf); RE(_h);
				}
			}
			_m_rh.load_factor_threshold(_lf);
			return {};
		}
		constexpr hash_table& rehash(load_factor_t _lf)noexcept {
			auto _h = try_rehash(_lf); RVP(_h, *this);
		}
		[[nodiscard]] constexpr error_handler_t try_reserve(size_t _addictional)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			return _reserve<_posix_code_t::result_out_of_range>(_addictional);
		}
		constexpr hash_table& reserve(size_t _addictional)noexcept {
			auto _h = try_reserve(_addictional); RVP(_h, *this);
		}
		[[nodiscard]] constexpr allocator_t unsafe_allocator()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _m_rh.allocator();
		}
		[[nodiscard]] constexpr cseq_t cseq()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			auto _data = _m_rh.data();
			return _seq(_data, _data + _m_rh.bucket_count(), nullptr);
		}
		[[nodiscard]] constexpr cseq_t seq()const noexcept { return cseq(); }
		[[nodiscard]] constexpr seq_t seq()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			auto _data = _m_rh.data();
			return _seq(_data, _data + _m_rh.bucket_count(), nullptr);
		}

		[[nodiscard]] constexpr seq_t find(const key_t& _k)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_k);
			auto [_p, _node] = _m_rh.find(_k);
			if (_node) IRON_LIKELY
			{
				return _seq(_p, _m_rh.data() + _m_rh.bucket_count(), _node);
			}
			return {};
		}
		[[nodiscard]] constexpr cseq_t find(const key_t& _k)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_k);
			auto [_p, _node] = _m_rh.find(_k);
			if (_node) IRON_LIKELY
			{
				return _seq(_p, _m_rh.data() + _m_rh.bucket_count(), _node);
			}
			return {};
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
			&& std::constructible_from<const key_t, _K>
			&& std::constructible_from<mapped_t, _M>
			)
		{
			auto _r = try_insert(std::forward<_K>(_k),std::forward<_M>(_m)); RP(_r);
		}

		template <typename _K>
		[[nodiscard]] constexpr iron::result<std::pair<seq_t, bool>, error_handler_t> try_insert(_K&& _key)noexcept
			requires (
			!has_mapped
			&& std::constructible_from<const key_t, _K>
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
			&& std::constructible_from<const key_t, _K>
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
		//
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
//
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

		constexpr hash_table& clear()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			if (_m_rh.size()) IRON_LIKELY
			{
				_m_rh.clear();
			}
			return *this;
		}
		constexpr hash_table& reset()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			_m_rh.reset(default_load_factor());
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
			return _m_rh.erase(_key, _args...);
		}
		[[nodiscard]] constexpr iron::result<size_t,error_handler_t> try_merge(hash_table& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES(_v);
			return _merge(_v);
		}
		constexpr size_t merge(hash_table& _v)noexcept {
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
		[[nodiscard]] constexpr adapter_box_t release_list(const _ARGS&... _args)noexcept
		requires (std::is_nothrow_convertible_v<std::decay_t<_ARGS>, key_t>& ...)
		{
			auto _r = try_release_list(_args...); RP(_r);
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

		[[nodiscard]] constexpr const list_t* unsafe_data()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _m_rh.data();
		}
		[[nodiscard]] constexpr const list_t* unsafe_cdata()const noexcept {
			return unsafe_data();
		}
		[[nodiscard]] constexpr list_t* unsafe_data()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _m_rh.data();
		}
		IRON_SECURITY_METHOD();
	private:
		constexpr explicit hash_table(allocator_t _a)noexcept 
		: _m_rh(default_load_factor(),_a) 
		{}

		constexpr explicit hash_table(_raw_hash_table_t&& _v)noexcept
			: _m_rh(std::move(_v))
		{}
		constexpr void _swap(hash_table& _v)noexcept {
			_m_rh.swap(_v._m_rh);
			IRON_SECURITY_SWAP(_v);
		}
		[[nodiscard]] static constexpr error_handler_t _check_load_factor(load_factor_t _lf)noexcept {
			if (std::is_constant_evaluated()) { //is_nan() is not constexpr
				if (_lf <= 0.0F || max_load_factor() < _lf) IRON_UNLIKELY
				{
					return ERROR_HANDLER(_posix_code_t::argument_out_of_domain);
				}
			}
			else {
				if (std::isnan(_lf) || _lf <= 0.0F || max_load_factor() < _lf) IRON_UNLIKELY
				{
					return ERROR_HANDLER(_posix_code_t::argument_out_of_domain);
				}
			}
			return {};
		}
		[[nodiscard]] constexpr seq_t _seq(_list_t* _ps, _list_t* _pe, _node_t* _node)noexcept {
			using _i_t = typename seq_t::iter_t;
			_i_t _b(_ps, _pe, _node);
			auto _e = _i_t{};
			return seq_t::unchecked_from(*this, _b, _e);
		}
		[[nodiscard]] constexpr cseq_t _seq(const _list_t* _ps, const _list_t* _pe, const _node_t* _node)const noexcept {
			using _i_t = typename cseq_t::iter_t;
			_i_t _b(_ps, _pe, _node);
			auto _e = _i_t{};
			return cseq_t::unchecked_from(*this, _b, _e);
		}

		template <_posix_code_t _code>
		[[nodiscard]] static constexpr error_handler_t _check_reserve(size_t _bucket_count,size_t _sz,size_t _addictional)noexcept {
#ifndef IRON_NOCHECK_SIZE
			if (
				max_bucket_count() < _addictional
				|| max_bucket_count() - _addictional < _bucket_count
				|| max_size() < _addictional
				|| max_size() - _addictional < _sz
				) IRON_UNLIKELY
			{
				return ERROR_HANDLER(_code);
			}
#endif 
			return {};
		}

		template <_posix_code_t _code>
		[[nodiscard]] constexpr error_handler_t _reserve(size_t _addictional)noexcept {
			auto _h = _check_reserve<_code>(_m_rh.bucket_count(),_m_rh.size(),_addictional); RE(_h);
			return _m_rh.reserve(_addictional);
		}
		template <typename _K, typename _M>
		[[nodiscard]] constexpr iron::result<std::pair<seq_t, bool>, error_handler_t> _insert(_K&& _k, _M&& _m)noexcept {
			auto _h = _reserve<_posix_code_t::result_out_of_range>(1); RR(_h);
			auto [_p, _node, _inserted, _h1] = _m_rh.insert(std::forward<_K>(_k), std::forward<_M>(_m)); RR(_h1);
			return iron::ok(std::make_pair(_seq(_p, _m_rh.data() + _m_rh.bucket_count(), _node),_inserted));
		}

		[[nodiscard]] constexpr iron::result<size_t, error_handler_t> _merge(hash_table& _ht)noexcept {
			if (this == &_ht) IRON_UNLIKELY
			{
				return iron::err(ERROR_HANDLER(_posix_code_t::invalid_argument));
			}
			auto& _rh = _ht._m_rh;
			if (_rh.size()) IRON_LIKELY
			{
				if (_m_rh.empty()) IRON_UNLIKELY 
				{
					_swap(_ht);
					return iron::ok(_m_rh.size());
				}
				auto _h = _reserve<_posix_code_t::result_out_of_range>(_rh.size()); RR(_h);
				return iron::ok(_m_rh.merge(_rh));
			}
			return iron::ok(size_t{});
		}
		template <bool _TRANSACTIONAL,typename _SEQ>
		[[nodiscard]] constexpr iron::result<size_t, error_handler_t> _range_insert(_SEQ& _seq) 
		requires(!std::is_const_v<_SEQ>)
		{
			auto _c = _seq.clone().count();
			if (_c) IRON_LIKELY 
			{
				auto _h = _reserve<_posix_code_t::result_out_of_range>(_c); RR(_h);
				typename _raw_hash_table_t::nodes_t _rollback_nodes(_m_rh.allocator());
				if constexpr (_TRANSACTIONAL) {
					auto _rn = _rollback_nodes.reserve(_c); RR(_rn);
				}
				auto& _b = _seq.begin();
				const auto _e = _seq.end();
				constexpr bool _move = false;
				auto _r=_m_rh.template insert_range<_TRANSACTIONAL,_move>(_b, _e,_rollback_nodes);
				if (!_r) IRON_UNLIKELY 
				{
					if constexpr (_TRANSACTIONAL) {
						_m_rh.rollback(_rollback_nodes);
					}
					return _r;
				}
				_c = _r.unchecked_unwrap();
			}
			return iron::ok(_c);
		}
		[[nodiscard]] static constexpr iron::result<hash_table, error_handler_t> _from(adapter_box_t&& _box, load_factor_t _lf, allocator_t _a)noexcept {
			auto _rbox = _box.unsafe_release();
#if IRON_SECURITY_CHECKER 
			[[maybe_unused]] const auto _x = std::move(_box); //mark _box temporary
#endif 
			auto _h = _check_load_factor(_lf); RR(_h);
#ifdef _MSC_VER //for a bug in constant evaluation
			auto _ht = unchecked_from(_a);
#else 
			hash_table _ht(_a);
#endif 
			_ht._m_rh.load_factor_threshold(_lf);
			if (_rbox.size()) IRON_LIKELY 
			{
				_h = _ht._reserve<_posix_code_t::result_out_of_range>(_rbox.size()); RR(_h);
				typename _raw_hash_table_t::nodes_t _rollback_nodes(_a);
				constexpr bool _transactional = false;
				constexpr bool _move = true;
				auto _b = _rbox.data();
				auto _r = _ht._m_rh.template insert_range<_transactional, _move>(_b, _rbox.data() + _rbox.size(), _rollback_nodes); RR(_r);
			}
			return iron::ok(std::move(_ht));
		}
		_raw_hash_table_t _m_rh;
		IRON_SECURITY_DCL();
	};

} //ns

static_assert(iron::concepts::hash_map<iron::hash_map<int, int>>, "internal error");
static_assert(iron::concepts::multi_hash_map<iron::multi_hash_map<int, int>>, "internal error");
static_assert(iron::concepts::hash_set<iron::hash_set<int>>, "internal error");
static_assert(iron::concepts::multi_hash_set<iron::multi_hash_set<int>>, "internal error");


#if defined(_MSC_VER)
#pragma warning(pop)
#endif

