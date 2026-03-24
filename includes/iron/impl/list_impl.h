
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable:26429)
#pragma warning (disable:26430)
#pragma warning (disable:26465)
#pragma warning (disable:26496)
#endif

namespace iron {

	template <typename _T,typename _A, bool _ISBD>
	class base_list final {
	public:
		using value_t = _T;
		using value_type = value_t;
		using allocator_t = _A;
		using allocator_type = allocator_t;
		static_assert(!std::is_reference_v<value_t>, "list value_type must not be a reference type");
		static_assert(!std::is_array_v<value_t>, "list value_type must not be an array type");
		static_assert(!std::is_const_v<value_t>, "list value_type must not be const");
		static_assert(std::is_nothrow_move_constructible_v<value_t>, "list value_type  must be move nothrow constructible");
		static_assert(std::is_nothrow_swappable_v<value_t>, "list value_type  must be nothrow swappable");
		static_assert(std::is_nothrow_destructible_v<value_t>, "list value_type must be nothrow destructible");
		static_assert(iron::concepts::allocator<allocator_t>, "list allocator_type is not an allocator");
		static constexpr bool is_bidirectional = _ISBD;
	private:
		using _node_t = iron::unsafe::list_node<value_t, is_bidirectional>;
		using _raw_list_t = iron::unsafe::raw_list<_node_t, allocator_t>;
	public:
		using size_t = typename _node_t::size_t;
		using size_type = size_t;
		using diff_t = std::ptrdiff_t;
		using difference_type = diff_t;
		using ref_t = iron::ref_wrapper<value_t>;
		using cref_t = iron::ref_wrapper<const value_t>;
		using ptr_t = value_t*;
		using cptr_t = const value_t*;
		using reference = value_t&;
		using const_reference = const value_t&;
		using pointer = value_t*;
		using const_pointer = const value_t*;
	private:
		using _iter_t = iron::unsafe::iterators::list_iterator<value_t,_raw_list_t>;
		using _riter_t = std::reverse_iterator<_iter_t>;
		using _citer_t = iron::unsafe::iterators::list_iterator<const value_t,_raw_list_t>;
		using _criter_t = std::reverse_iterator<_citer_t>;
	public:
		using iterator = _iter_t;
		using const_iterator = _citer_t;
		using seq_t = iron::sequence<iterator>;
		using cseq_t = iron::sequence<const_iterator>;
		using rseq_t = iron::sequence<_riter_t>;
		using crseq_t = iron::sequence<_criter_t>;
		using error_handler_t = typename _node_t::error_handler_t;
		using error_handler_type = error_handler_t;
		using adapter_box_t = iron::adapter_box<typename _raw_list_t::adapter_box_t>;
		using adapter_box_type = adapter_box_t;
		struct list_tag { list_tag() = delete; };
		constexpr base_list()noexcept = default;
		constexpr base_list(base_list&& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_swap(_v);
			IRON_SECURITY_MARK_TEMPORARY(_v);
		}
		base_list(const base_list&) = delete;
		constexpr base_list& operator=(base_list&& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_swap(_v);
			IRON_SECURITY_MARK_TEMPORARY(_v);
			return *this;
		}
		base_list& operator=(const base_list&) = delete;
		constexpr ~base_list()=default;
		[[nodiscard]] static constexpr base_list unchecked_from(allocator_t _a)noexcept {
			return base_list(_a);
		}
		[[nodiscard]] constexpr static iron::result<base_list, error_handler_t> try_from(adapter_box_t&& _box)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY(_box);
			return _from(std::move(_box));
		}
		[[nodiscard]] constexpr static base_list from(adapter_box_t&& _box)noexcept {
			auto _r = try_from(std::move(_box)); RP(_r);
		}
		[[nodiscard]] constexpr result<base_list, error_handler_t> try_clone(size_t _n = 1)const noexcept 
		requires(iron::concepts::copyable_or_cloneable<value_t>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _clone(_n);
		}
		[[nodiscard]] constexpr base_list clone(size_t _n = 1)const noexcept {
			auto _r = try_clone(_n); RP(_r);
		}
		[[nodiscard]] static constexpr size_t max_size()noexcept { return _raw_list_t::max_size(); }

		[[nodiscard]] constexpr size_t size()const noexcept 
		requires (is_bidirectional) 
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _m_data.size();
		}
		[[nodiscard]] constexpr size_t count()const noexcept
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _m_data.count();
		}

		[[nodiscard]] constexpr bool empty()const noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _empty();
		}
		[[nodiscard]] constexpr explicit operator bool()const noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return !_empty(); 
		}
		[[nodiscard]] constexpr allocator_t unsafe_allocator()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _m_data.allocator();
		}

		[[nodiscard]] constexpr value_t& unchecked_front()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _ufront();
		}
		[[nodiscard]] constexpr const value_t& unchecked_front()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _ufront();
		}
		[[nodiscard]] constexpr const value_t& unchecked_cfront()const noexcept {
			return unchecked_front();
		}
		[[nodiscard]] constexpr iron::option<ref_t> front()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			if (!_empty()) IRON_LIKELY 
			{
				return iron::some(iron::make_ref(*this, _ufront()));
			}
			return {};
		}
		[[nodiscard]] constexpr iron::option<cref_t> front()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			if (!_empty()) IRON_LIKELY
			{
				return iron::some(iron::make_cref(*this, _ufront()));
			}
			return {};
		}
		[[nodiscard]] constexpr iron::option<cref_t> cfront()const noexcept {
			return front();
		}
		[[nodiscard]] constexpr value_t& unchecked_back()noexcept 
		requires (is_bidirectional) 
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			DEBUG_ASSERT(!empty());
			return _uback();
		}

		[[nodiscard]] constexpr const value_t& unchecked_back()const noexcept 
		requires (is_bidirectional) 
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _uback();
		}
		[[nodiscard]] constexpr const value_t& unchecked_cback()const noexcept 
		requires (is_bidirectional)
		{
			return unchecked_back();
		}
		[[nodiscard]] constexpr iron::option<ref_t> back()noexcept 
		requires (is_bidirectional)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			if (!_empty()) IRON_LIKELY
			{
				return iron::some(iron::make_ref(*this, _uback()));
			}
			return {};
		}

		[[nodiscard]] constexpr iron::option<cref_t> back()const noexcept 
		requires (is_bidirectional)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			if (!_empty()) IRON_LIKELY
			{
				return iron::some(iron::make_cref(*this, _uback()));
			}
			return {};
		}
		[[nodiscard]] constexpr iron::option<cref_t> cback()const noexcept
		requires (is_bidirectional)
		{
			return back();
		}

		template<typename... _ARGS >
		[[nodiscard]] constexpr result<seq_t, error_handler_t> try_push_front(_ARGS&&... _args) 
		requires (iron::concepts::constructible_at<value_t, _ARGS&&...>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY_ARGS(_args...);
			return _push_front(std::forward<_ARGS>(_args)...);
		}

		template<typename... _ARGS >
		constexpr seq_t push_front(_ARGS&&... _args) 
		requires (iron::concepts::constructible_at<value_t, _ARGS&&...>)
		{
			auto _r = try_push_front(std::forward<_ARGS>(_args)...); RP(_r);
		}

		template<typename _SEQ>
		[[nodiscard]] constexpr result<size_t, error_handler_t> try_push_front_range(_SEQ& _seq)noexcept
			requires(
			iron::concepts::sequence<_SEQ>
			&& !std::is_const_v<_SEQ>
			&& iron::concepts::constructible_at<value_t, std::decay_t<typename _SEQ::value_t>&&>
			)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_seq);
			constexpr bool _transactional = true;
			constexpr bool _back = false;
			return _push_range<_transactional,_back>(_seq);
		}

		template<typename _SEQ>
		constexpr size_t push_front_range(_SEQ& _seq)noexcept
			requires(
			iron::concepts::sequence<_SEQ>
			&& !std::is_const_v<_SEQ>
			&& iron::concepts::constructible_at<value_t, std::decay_t<typename _SEQ::value_t>&&>
			)
		{
			auto _r = try_push_front_range(_seq); RP(_r);
		}

		template<typename _SEQ>
		[[nodiscard]] constexpr result<size_t, error_handler_t> try_push_back_range(_SEQ& _seq)noexcept
			requires(
			is_bidirectional
			&& iron::concepts::sequence<_SEQ>
			&& !std::is_const_v<_SEQ>
			&& iron::concepts::constructible_at<value_t, std::decay_t<typename _SEQ::value_t>&&>
			)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_seq);
			constexpr bool _transactional = true;
			constexpr bool _back = true;
			return _push_range<_transactional,_back>(_seq);
		}

		template<typename _SEQ>
		constexpr size_t push_back_range(_SEQ& _seq)noexcept
			requires(
			is_bidirectional
			&& iron::concepts::sequence<_SEQ>
			&& !std::is_const_v<_SEQ>
			&& iron::concepts::constructible_at<value_t, std::decay_t<typename _SEQ::value_t>&&>
			)
		{
			auto _r = try_push_back_range(_seq); RP(_r);
		}

		constexpr iron::option<value_t> pop_front()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			if (!_empty()) IRON_LIKELY
			{
				return iron::some(_pop_front());
			}
			return {};
		}

		constexpr value_t unchecked_pop_front()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			DEBUG_ASSERT(!empty());
			return _pop_front();
		}

		template<typename... _ARGS>
		[[nodiscard]] constexpr result<seq_t, error_handler_t> try_push_back(_ARGS&&... _args)noexcept 
		requires (is_bidirectional && iron::concepts::constructible_at<value_t, _ARGS&&...>) {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY_ARGS(_args...);
			return _push_back(std::forward<_ARGS>(_args)...);
		}

		template<typename... _ARGS>
		constexpr seq_t push_back(_ARGS&&... _args)noexcept 
		requires (is_bidirectional && iron::concepts::constructible_at<value_t, _ARGS&&...>)
		{
			auto _r = try_push_back(std::forward<_ARGS>(_args)...); RP(_r);
		}

		constexpr iron::option<value_t> pop_back()noexcept 
		requires (is_bidirectional)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			if (!_empty())  IRON_LIKELY
			{
				return iron::some(_pop_back());
			}
			return {};
		}
		constexpr value_t unchecked_pop_back()noexcept 
		requires (is_bidirectional) 
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			DEBUG_ASSERT(!empty());
			return _pop_back();
		}
		[[nodiscard]] constexpr iron::result<adapter_box_t,error_handler_t> try_release()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			auto _r = _m_data.release(); RR(_r);
			return iron::ok(adapter_box_t::unchecked_from(std::move(_r.unchecked_unwrap())));
		}
		[[nodiscard]] constexpr adapter_box_t release()noexcept {
			auto _r = try_release(); RP(_r);
		}
		template <typename _CONTAINER_SEQ, typename _SEQ>
		[[nodiscard]] constexpr iron::result<size_t, error_handler_t> try_insert_range_before(const _CONTAINER_SEQ& _container_seq, _SEQ& _seq)noexcept
		requires(
				iron::concepts::sequence<_CONTAINER_SEQ>
				&& iron::concepts::sequence<_SEQ>
				&& !std::is_const_v<_SEQ>
				&& iron::concepts::constructible_at<value_t, std::decay_t<typename _SEQ::value_t>&&>
		)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_container_seq);
			IRON_SECURITY_CHECK_TEMPORARY(_seq);
#if IRON_SECURITY_CHECKER
			PANIC_UNLESS(this->seq().is_same_container(_container_seq), "the sequence does not belong to the container");
#endif 
			auto _h = _check_maxsize(_seq.clone().count()); RR(_h);
			auto _node = _node_from_seq(_container_seq);
			if (!_node) IRON_UNLIKELY 
			{
				return iron::err(ERROR_HANDLER(iron::unsafe::errors::posix::code_t::argument_out_of_domain));
			}
			auto& _b = _seq.begin();
			const auto _e = _seq.end();
			constexpr bool _transactional = true;
			return _m_data.template insert_range_before<_transactional>(_node, _b, _e);
		}

		template <typename _CONTAINER_SEQ,typename _SEQ>
		constexpr size_t insert_range_before(const _CONTAINER_SEQ& _container_seq,_SEQ& _seq)noexcept
		requires(
			iron::concepts::sequence<_CONTAINER_SEQ>
			&& !std::is_const_v<_SEQ>
			&& iron::concepts::sequence<_SEQ>
			&& iron::concepts::constructible_at<value_t, std::decay_t<typename _SEQ::value_t>&&>
		)
		{
			auto _r = try_insert_range_before(_container_seq,_seq); RP(_r);
		}

		template <typename _CONTAINER_SEQ, typename _SEQ>
		[[nodiscard]] constexpr iron::result<size_t, error_handler_t> try_insert_range_after(const _CONTAINER_SEQ& _container_seq, _SEQ& _seq)noexcept
		requires(
			iron::concepts::sequence<_CONTAINER_SEQ>
			&& !std::is_const_v<_SEQ>
			&& iron::concepts::sequence<_SEQ>
			&& iron::concepts::constructible_at<value_t, std::decay_t<typename _SEQ::value_t>&&>
			)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_container_seq);
			IRON_SECURITY_CHECK_TEMPORARY(_seq);
#if IRON_SECURITY_CHECKER
			PANIC_UNLESS(this->seq().is_same_container(_container_seq), "the sequence does not belong to the container");
#endif 
			auto _h = _check_maxsize(_seq.clone().count()); RR(_h);
			auto _node = _node_from_seq(_container_seq);
			if (!_node) IRON_UNLIKELY 
			{
				return iron::err(ERROR_HANDLER(iron::unsafe::errors::posix::code_t::argument_out_of_domain));
			}
			auto& _b = _seq.begin();
			const auto _e = _seq.end();
			constexpr bool _transactional = true;
			return _m_data.template insert_range_after<_transactional>(_node, _b, _e);
		}

		template <typename _CONTAINER_SEQ, typename _SEQ>
		constexpr size_t insert_range_after(const _CONTAINER_SEQ& _container_seq, _SEQ& _seq)noexcept
		requires(
			iron::concepts::sequence<_CONTAINER_SEQ>
			&& !std::is_const_v<_SEQ>
			&& iron::concepts::sequence<_SEQ>
			&& iron::concepts::constructible_at<value_t, std::decay_t<typename _SEQ::value_t>&&>
			)
		{
			auto _r = try_insert_range_after(_container_seq, _seq); RP(_r);
		}

		template <typename _SEQ,typename... _ARGS>
		[[nodiscard]] constexpr iron::result<seq_t, error_handler_t> try_insert_before(const _SEQ& _seq, _ARGS&&... _args)noexcept
		requires (
			iron::concepts::sequence<_SEQ>
			&& iron::concepts::constructible_at<value_t, _ARGS&&...>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_seq);
			IRON_SECURITY_CHECK_TEMPORARY_ARGS(_args...);
#if IRON_SECURITY_CHECKER
			PANIC_UNLESS(this->seq().is_same_container(_seq), "the sequence does not belong to the container");
#endif 
			auto _node = _node_from_seq(_seq);
			if (!_node) IRON_UNLIKELY 
			{
				return iron::err(ERROR_HANDLER(iron::unsafe::errors::posix::code_t::argument_out_of_domain));
			}
			if constexpr (iron::concepts::reverse_iterator<typename _SEQ::iter_t>) {
				_node = _node->next();
				if (!_node) IRON_UNLIKELY 
				{
					return _push_back(std::forward<_ARGS>(_args)...);
				}
			}
			else {
				_node = _node->prev();
				if (!_node) IRON_UNLIKELY 
				{
					return _push_front(std::forward<_ARGS>(_args)...);
				}
			}
			return _insert_after(_node, std::forward<_ARGS>(_args)...);
		}

		template <typename _SEQ, typename... _ARGS>
		constexpr seq_t insert_before(const _SEQ& _seq, _ARGS&&... _args)noexcept
		requires (
			iron::concepts::sequence<_SEQ>
			&& iron::concepts::constructible_at<value_t, _ARGS&&...>
		)
		{
			auto _r = try_insert_before(_seq, std::forward<_ARGS>(_args)...); RP(_r);
		}

		template <typename _SEQ, typename... _ARGS>
		[[nodiscard]] constexpr iron::result<seq_t, error_handler_t> try_insert_after(const _SEQ& _seq, _ARGS&&... _args)noexcept
		requires (
			iron::concepts::sequence<_SEQ>
			&& iron::concepts::constructible_at<value_t, _ARGS&&...>
		)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_seq);
			IRON_SECURITY_CHECK_TEMPORARY_ARGS(_args...);
#if IRON_SECURITY_CHECKER
			PANIC_UNLESS(this->seq().is_same_container(_seq), "the sequence does not belong to the container");
#endif 
			auto _node = _node_from_seq(_seq);
			if (!_node) IRON_UNLIKELY 
			{
				return iron::err(ERROR_HANDLER(iron::unsafe::errors::posix::code_t::argument_out_of_domain));
			}
			return _insert_after(_node, std::forward<_ARGS>(_args)...);
		}

		template <typename _SEQ, typename... _ARGS>
		constexpr seq_t insert_after(const _SEQ& _seq,_ARGS&&... _args)noexcept 
		requires (
		iron::concepts::sequence<_SEQ>
		&& iron::concepts::constructible_at<value_t, _ARGS&&...>)
		{
			auto _r = try_insert_after(_seq, std::forward<_ARGS>(_args)...); RP(_r);
		}
		constexpr size_t erase_n(seq_t& _seq, size_t _n)noexcept 
		requires(is_bidirectional)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_seq);
#if IRON_SECURITY_CHECKER
			PANIC_UNLESS(this->seq().is_same_container(_seq), "the sequence does not belong to the container");
			ASSERT(!has_servant_actives(_seq));
#endif 
			if (_seq && _n) IRON_LIKELY
			{
				constexpr bool _after = false;
				return _erase_n<_after>(_seq, _n);
			}
			return {};
		}
		constexpr size_t erase_n_after(seq_t& _seq, size_t _n)noexcept
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_seq);
#if IRON_SECURITY_CHECKER
			PANIC_UNLESS(this->seq().is_same_container(_seq), "the sequence does not belong to the container");
			ASSERT(!has_servant_actives(_seq));
#endif 
			if (_seq && _n) IRON_LIKELY
			{
				constexpr bool _after = true;
				return _erase_n<_after>(_seq, _n);
			}
			return {};
		}

		constexpr size_t erase_n(rseq_t& _seq, size_t _n)noexcept 
		requires(is_bidirectional)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_seq);
#if IRON_SECURITY_CHECKER
			PANIC_UNLESS(this->seq().is_same_container(_seq), "the sequence does not belong to the container");
			ASSERT(!has_servant_actives(_seq));
#endif 
			if (_seq && _n) IRON_LIKELY
			{
				constexpr bool _after = false;
				return _erase_n<_after>(_seq, _n);
			}
			return {};
		}

		constexpr size_t erase_all(seq_t& _seq)noexcept 
		requires(is_bidirectional)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_seq);
#if IRON_SECURITY_CHECKER
			PANIC_UNLESS(this->seq().is_same_container(_seq), "the sequence does not belong to the container");
			ASSERT(!has_servant_actives(_seq));
#endif 
			constexpr bool _after = false;
			return _erase_all<_after>(_seq);
		}
		constexpr size_t erase_all_after(seq_t& _seq)noexcept
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_seq);
#if IRON_SECURITY_CHECKER
			PANIC_UNLESS(this->seq().is_same_container(_seq), "the sequence does not belong to the container");
			ASSERT(!has_servant_actives(_seq));
#endif 
			constexpr bool _after = true;
			return _erase_all<_after>(_seq);
		}

		constexpr size_t erase_all(rseq_t& _seq)noexcept 
		requires(is_bidirectional) {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_seq);
#if IRON_SECURITY_CHECKER
			PANIC_UNLESS(this->seq().is_same_container(_seq), "the sequence does not belong to the container");
			ASSERT(!has_servant_actives(_seq));
#endif 
			constexpr bool _after = false;
			return _erase_all<_after>(_seq);
		}

		constexpr iron::option<value_t> erase_first(seq_t& _seq)noexcept
			requires(is_bidirectional)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_seq);
#if IRON_SECURITY_CHECKER
			PANIC_UNLESS(this->seq().is_same_container(_seq), "the sequence does not belong to the container");
			ASSERT(!has_servant_actives(_seq));
#endif 
			if (_seq) IRON_LIKELY
			{
				constexpr bool _after = false;
				return iron::some(_erase_first<_after>(_seq));
			}
			return {};
		}
		constexpr iron::option<value_t> erase_first_after(seq_t& _seq)noexcept
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_seq);
#if IRON_SECURITY_CHECKER
			PANIC_UNLESS(this->seq().is_same_container(_seq), "the sequence does not belong to the container");
			ASSERT(!has_servant_actives(_seq));
#endif 
			if (_seq) IRON_LIKELY
			{
				constexpr bool _after = true;
				return iron::some(_erase_first<_after>(_seq));
			}
			return {};
		}

		constexpr value_t unchecked_erase_first(seq_t& _seq)noexcept
			requires(is_bidirectional)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_seq);
#if IRON_SECURITY_CHECKER
			PANIC_UNLESS(this->seq().is_same_container(_seq), "the sequence does not belong to the container");
#endif 
			DEBUG_ASSERT(_seq);
			constexpr bool _after = false;
			return _erase_first<_after>(_seq);
		}
		constexpr value_t unchecked_erase_first_after(seq_t& _seq)noexcept
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_seq);
#if IRON_SECURITY_CHECKER
			PANIC_UNLESS(this->seq().is_same_container(_seq), "the sequence does not belong to the container");
#endif 
			DEBUG_ASSERT(_seq);
#ifndef NDEBUG
			auto _t = _seq; ++_t;
			DEBUG_ASSERT(_t);
#endif 
			constexpr bool _after = true;
			return _erase_first<_after>(_seq);
		}

		constexpr iron::option<value_t> erase_first(rseq_t& _seq)noexcept 
		requires(is_bidirectional)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_seq);
#if IRON_SECURITY_CHECKER
			PANIC_UNLESS(this->seq().is_same_container(_seq), "the sequence does not belong to the container");
			ASSERT(!has_servant_actives(_seq));
#endif 
			if (_seq) IRON_LIKELY
			{
				constexpr bool _after = false;
				return iron::some(_erase_first<_after>(_seq));
			}
			return {};
		}

		constexpr value_t unchecked_erase_first(rseq_t& _seq)noexcept 
		requires(is_bidirectional)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_seq);
#if IRON_SECURITY_CHECKER
			PANIC_UNLESS(this->seq().is_same_container(_seq), "the sequence does not belong to the container");
#endif 
			DEBUG_ASSERT(_seq);
			constexpr bool _after = false;
			return _erase_first<_after>(_seq);
		}

		template <typename _F>
		constexpr size_t erase_if(_F _f)noexcept
		requires(iron::concepts::bool_predicate<_F, value_t>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			if (auto _seq = this->seq()) IRON_LIKELY
			{
				constexpr bool _after = false;
				return _erase_if<_after>(_seq, _f);
			}
			return {};
		}
		template <typename _F>
		constexpr size_t erase_if(seq_t& _seq,_F _f)noexcept
		requires(is_bidirectional && iron::concepts::bool_predicate<_F, value_t>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_seq);
#if IRON_SECURITY_CHECKER
			PANIC_UNLESS(this->seq().is_same_container(_seq), "the sequence does not belong to the container");
			ASSERT(!has_servant_actives(_seq));
#endif 
			if (_seq) IRON_LIKELY
			{
				constexpr bool _after = false;
				return _erase_if<_after>(_seq, _f);
			}
			return {};
		}
		template <typename _F>
		constexpr size_t erase_if_after(seq_t& _seq, _F _f)noexcept
		requires(iron::concepts::bool_predicate<_F, value_t>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_seq);
#if IRON_SECURITY_CHECKER
			PANIC_UNLESS(this->seq().is_same_container(_seq), "the sequence does not belong to the container");
			ASSERT(!has_servant_actives(_seq));
#endif 
			if (_seq) IRON_LIKELY
			{
				constexpr bool _after = true;
				return _erase_if<_after>(_seq, _f);
			}
			return {};
		}

		template <typename _F>
		constexpr size_t erase_if(rseq_t& _seq, _F _f)noexcept
		requires(is_bidirectional && iron::concepts::bool_predicate<_F, value_t>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_seq);
#if IRON_SECURITY_CHECKER
			PANIC_UNLESS(this->seq().is_same_container(_seq), "the sequence does not belong to the container");
			ASSERT(!has_servant_actives(_seq));
#endif 
			if (_seq) IRON_LIKELY
			{
				constexpr bool _after = false;
				return _erase_if<_after>(_seq, _f);
			}
			return {};
		}
		constexpr size_t erase(const value_t& _v)noexcept 
		requires (iron::concepts::nothrow_equality_comparable<value_t>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			if (auto _seq = this->seq()) IRON_LIKELY
			{
				constexpr bool _after = false;
				return _erase_if<_after>(_seq,[&](const value_t& _v1)constexpr noexcept ->bool { return _v == _v1; });
			}
			return {};
		}
		constexpr size_t erase(seq_t& _seq, const value_t& _v)noexcept
		requires (is_bidirectional && iron::concepts::nothrow_equality_comparable<value_t>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_seq);
			IRON_SECURITY_CHECK_TEMPORARY(_v);
#if IRON_SECURITY_CHECKER
			PANIC_UNLESS(this->seq().is_same_container(_seq), "the sequence does not belong to the container");
			ASSERT(!has_servant_actives(_seq));
#endif 
			constexpr bool _after = false;
			return _erase_if<_after>(_seq, [&](const value_t& _v1)constexpr noexcept ->bool { return _v == _v1; });
		}
		constexpr size_t erase(rseq_t& _seq, const value_t& _v)noexcept
		requires (is_bidirectional && iron::concepts::nothrow_equality_comparable<value_t>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_seq);
			IRON_SECURITY_CHECK_TEMPORARY(_v);
#if IRON_SECURITY_CHECKER
			PANIC_UNLESS(this->seq().is_same_container(_seq), "the sequence does not belong to the container");
			ASSERT(!has_servant_actives(_seq));
#endif 
			constexpr bool _after = false;
			return _erase_if<_after>(_seq, [&](const value_t& _v1)constexpr noexcept ->bool { return _v == _v1; });
		}
		constexpr size_t erase_after(seq_t& _seq, const value_t& _v)noexcept
		requires (iron::concepts::nothrow_equality_comparable<value_t>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_seq);
			IRON_SECURITY_CHECK_TEMPORARY(_v);
#if IRON_SECURITY_CHECKER
			PANIC_UNLESS(this->seq().is_same_container(_seq), "the sequence does not belong to the container");
			ASSERT(!has_servant_actives(_seq));
#endif 
			constexpr bool _after = true;
			return _erase_if<_after>(_seq, [&](const value_t& _v1)constexpr noexcept ->bool { return _v == _v1; });
		}
		constexpr size_t unique()noexcept 
		requires(iron::concepts::nothrow_equality_comparable<value_t>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			if (auto _seq = this->seq()) IRON_LIKELY 
			{
				return _unique(_seq);
			}
			return {};
		}
		constexpr base_list& reverse()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			_m_data.reverse();
			return *this;
		}
		constexpr base_list& reset()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			if (!_empty()) IRON_LIKELY
			{
				_reset();
			}
			return *this;
		}
		[[nodiscard]] constexpr error_handler_t try_resize(size_t _sz)noexcept
		requires(std::is_default_constructible_v<value_t>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
#ifndef IRON_NOCHECK_SIZE
			if (_sz > max_size()) {
				return ERROR_HANDLER(iron::unsafe::errors::posix::code_t::argument_out_of_domain);
			}
#endif 
			constexpr bool _transactional = true;
			return _m_data.template resize<_transactional>(_sz, value_t{});
		}
		constexpr base_list& resize(size_t _sz)noexcept
		requires(std::is_default_constructible_v<value_t>)
		{
			auto _h = try_resize(_sz); RVP(_h,*this);
		}
		[[nodiscard]] constexpr error_handler_t try_resize(size_t _sz,const value_t& _v)noexcept
		requires(iron::concepts::copyable_or_cloneable<value_t>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
#ifndef IRON_NOCHECK_SIZE
			if (_sz > max_size()) {
				return ERROR_HANDLER(iron::unsafe::errors::posix::code_t::argument_out_of_domain);
			}
#endif 
			constexpr bool _transactional = true;
			return _m_data.template resize<_transactional>(_sz,_v);
		}
		constexpr base_list& resize(size_t _sz,const value_t& _v)noexcept
		requires(iron::concepts::copyable_or_cloneable<value_t>)
		{
			auto _h = try_resize(_sz,_v); RVP(_h, *this);
		}

		constexpr base_list& swap(base_list& _v) noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_swap(_v);
			return *this;
		}
		[[nodiscard]] constexpr cseq_t cseq()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			using _i_t = typename cseq_t::iter_type;
			_i_t _b(const_cast<_node_t*>(_m_data.front()),const_cast<_raw_list_t&>(_m_data),false);
			auto _e = _i_t{};
			return cseq_t::unchecked_from(*this, _b, _e);
		}
		[[nodiscard]] constexpr crseq_t crseq()const noexcept 
		requires (is_bidirectional) {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			using _i_t = typename crseq_t::iter_type;
			auto& _d = const_cast<_raw_list_t&>(_m_data);
			const auto _b = _citer_t(const_cast<_node_t*>(_m_data.front()),_d,false);
			const auto _e = _citer_t(nullptr,_d, true);
			return crseq_t::unchecked_from(*this, _i_t(_e), _i_t(_b));
		}
		[[nodiscard]] constexpr seq_t seq()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _seq(_m_data.front(), false);
		}
		[[nodiscard]] constexpr rseq_t rseq()noexcept 
		requires (is_bidirectional) {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			using _i_t = rseq_t::iter_type;
			const auto _b = _iter_t(_m_data.front(),_m_data,false);
			const auto _e = _iter_t(nullptr,_m_data,true);
			return rseq_t::unchecked_from(*this, _i_t(_e), _i_t(_b));
		}
		[[nodiscard]] constexpr cseq_t seq()const noexcept { return cseq(); }
		
		[[nodiscard]] constexpr crseq_t rseq()const noexcept 
		requires (is_bidirectional) 
		{ 
			return crseq(); 
		}
		IRON_SECURITY_METHOD()
#if IRON_SECURITY_CHECKER
		template <typename _SEQ>
		[[nodiscard]] constexpr bool has_servant_actives(const _SEQ& _seq)const noexcept 
		requires (iron::concepts::sequence<_SEQ>) 
		{
			auto _c = checker().ref_count();
			if (_c && !_seq.empty()) IRON_LIKELY
			{
				--_c;
			}
			return _c ? true : false;
		}
#endif 
		template <typename _A1,bool _ISBD1>
		[[nodiscard]] constexpr bool operator==(const base_list<value_t, _A1,_ISBD1>& _r)const noexcept
		requires(iron::concepts::nothrow_equality_comparable<value_t>)
		{
			if constexpr (is_bidirectional && _ISBD1) {
				return size() == _r.size()
					? std::equal(cseq().begin(), cseq().end(), _r.cseq().begin())
					: false
					;
			}
			else {
				auto _lseq = cseq();
				auto _rseq = _r.cseq();
				return  _lseq.eq(_rseq);
			}
		}

	private:
		using _iter_node_t = iron::unsafe::iterators::_impl::_node;
		constexpr explicit base_list(allocator_t _a)noexcept
		: _m_data(_a)
		{}
		constexpr explicit base_list(_raw_list_t&& _d) : _m_data(std::move(_d)) {}
		[[nodiscard]] constexpr bool _empty()const noexcept { return _m_data.empty(); }
		constexpr void _reset()noexcept { _m_data.reset(); }
		[[nodiscard]] constexpr value_t _release_value(_node_t* _node)noexcept {
			DEBUG_ASSERT(_node);
			auto _t = std::move(_node->value());
#ifndef NDEBUG
			_node->reset_links();
#endif 
			_node_t::destroy(_node);
			_node_t::deallocate(_node, _m_data.allocator());
			return _t;
		}
		[[nodiscard]] constexpr value_t _pop_front()noexcept {
			auto _node = _m_data.pop_front();
			return _release_value(_node);
		}
		[[nodiscard]] constexpr value_t _pop_back()noexcept 
		requires(is_bidirectional)
		{
			auto _node = _m_data.pop_back();
			return _release_value(_node);
		}
		constexpr void _swap(base_list& _v)noexcept {
			_m_data.swap(_v._m_data);
			IRON_SECURITY_SWAP(_v);
		}
		[[nodiscard]] constexpr value_t& _ufront()noexcept {
			auto _node = _m_data.front();
			DEBUG_ASSERT(_node);
			return _node->value();
		}
		[[nodiscard]] constexpr const value_t& _ufront()const noexcept {
			auto _node = _m_data.front();
			DEBUG_ASSERT(_node);
			return _node->value();
		}
		[[nodiscard]] constexpr value_t& _uback()noexcept 
		requires (is_bidirectional)
		{
			auto _node = _m_data.back();
			DEBUG_ASSERT(_node);
			return _node->value();
		}
		[[nodiscard]] constexpr const value_t& _uback()const noexcept {
			auto _node = _m_data.back();
			DEBUG_ASSERT(_node);
			return _node->value();
		}
		template<typename... _ARGS >
		[[nodiscard]] constexpr result<seq_t, error_handler_t> _push_front(_ARGS&&... _args)noexcept 
		{
			auto _h = _check_maxsize(1); RR(_h);
			_node_t* _node{};
			_h = _node_t::from_value(_node,_m_data.allocator(),std::forward<_ARGS>(_args)...); RR(_h);
			_m_data.push_front(_node);
			return iron::ok(_seq(_m_data.front(), false));
		}
		template<typename... _ARGS >
		[[nodiscard]] constexpr iron::result<seq_t, error_handler_t> _push_back(_ARGS&&... _args)noexcept 
		requires (is_bidirectional)
		{
			auto _h = _check_maxsize(1); RR(_h);
			_node_t* _node{};
			_h = _node_t::from_value(_node,_m_data.allocator(),std::forward<_ARGS>(_args)...); RR(_h);
			_m_data.push_back(_node);
			return iron::ok(_seq(_node, false));
		}
		template <bool _TRANSACTIONAL,bool _BACK,typename _SEQ>
		[[nodiscard]] constexpr iron::result<size_t, error_handler_t> _push_range(_SEQ& _seq)noexcept 
		requires(!std::is_const_v<_SEQ>)
		{
			auto _h = _check_maxsize(_seq.clone().count()); RR(_h);
			auto& _b = _seq.begin();
			const auto _e = _seq.end();
			if constexpr (_BACK) {
				return _m_data.template push_back_range<_TRANSACTIONAL>(_b, _e);
			}
			else {
				return _m_data.template push_front_range<_TRANSACTIONAL>(_b, _e);
			}
		}

		template <typename... _ARGS>
		[[nodiscard]] constexpr iron::result<seq_t, error_handler_t> _insert_after(_node_t* _current_node, _ARGS&&... _args)noexcept
		{
			DEBUG_ASSERT(_current_node);
			auto _h = _check_maxsize(1); RR(_h);
			_node_t* _node{};
			_h = _node_t::from_value(_node, _m_data.allocator(), std::forward<_ARGS>(_args)...); RR(_h);
			_m_data.insert(_current_node, _node);
			return iron::ok(_seq(_node, false));
		}
		[[nodiscard]] constexpr iron::result<base_list, error_handler_t> _clone(size_t _n)const noexcept
		{
			if constexpr (is_bidirectional) {
				const auto _x = usizeb(_n) * usizeb(_m_data.size());
				if (!_x || _x.unchecked_value() > max_size()) IRON_UNLIKELY {
					return iron::err(ERROR_HANDLER(iron::unsafe::errors::posix::code_t::result_out_of_range));
				}
			}
			else {
				if (_n > max_size()) IRON_UNLIKELY {
					return iron::err(ERROR_HANDLER(iron::unsafe::errors::posix::code_t::argument_out_of_domain));
				}
			}
			auto _list=unchecked_from(std::allocator_traits<allocator_t>::select_on_container_copy_construction(_m_data.allocator()));
			if (!_m_data.empty() && _n) IRON_LIKELY{
				auto _r = _m_data.try_clone(_n); RR(_r);
				_list._m_data = std::move(_r.unchecked_unwrap());
			}
			return iron::ok(std::move(_list));
		}

		[[nodiscard]] constexpr static iron::result<base_list, error_handler_t> _from(adapter_box_t&& _box)noexcept {
			auto _raw_vector = _box.unsafe_release();
#if IRON_SECURITY_CHECKER
			[[maybe_unused]] const auto _t = std::move(_box); //mark temporary
#endif 
			const auto _a = _raw_vector.allocator();
			auto _r = _raw_list_t::from(std::move(_raw_vector)); RR(_r);
#ifdef _MSC_VER //bug in constant evaluation
			auto _list=unchecked_from(_a);
#else 
			base_list _list{ _a };
#endif 
			_list._m_data = std::move(_r.unchecked_unwrap());
			return iron::ok(std::move(_list));
		}
		[[nodiscard]] constexpr error_handler_t _check_maxsize(size_t _n)noexcept {
#ifndef IRON_NOCHECK_SIZE
			if constexpr (is_bidirectional) {
				if (max_size() - _n < _m_data.size()) IRON_UNLIKELY {
					return ERROR_HANDLER(iron::unsafe::errors::posix::code_t::result_out_of_range);
				}
			}
			else {
#ifdef _IRON_CHECK_LIST
				if (max_size() - _n < _m_data.count()) IRON_UNLIKELY{
					return ERROR_HANDLER(iron::unsafe::errors::posix::code_t::result_out_of_range);
				}
#endif 
			}
#endif 
			return {};
		}
		template <typename _SEQ>
		[[nodiscard]] constexpr _node_t* _node_for_detach(_SEQ& _seq)noexcept 
		requires(!std::is_const_v<_SEQ>)
		{
			auto& _b = _seq.begin();  
			if constexpr (iron::concepts::reverse_iterator<decltype(_b)>) {
				auto _bb = std::prev(_b.base());
				auto _node = _iter_node_t::node(_bb);
				DEBUG_ASSERT(_node);
				if (!_node->prev()) {
					_seq.reset();
				}
				return _node;
			}
			else {
				auto _node = _iter_node_t::node(_b);
				++_b;
				return _node;
			}
		}
		template <typename _SEQ>
		[[nodiscard]] constexpr _node_t* _node_from_seq(const _SEQ& _seq)noexcept {
			if (_seq) {
				auto _b = _seq.begin();
				if constexpr (iron::concepts::reverse_iterator<decltype(_b)>) {
					auto _bb = std::prev(_b.base());
					auto _node = _iter_node_t::node(_bb);
					DEBUG_ASSERT(_node);
					return _node;
				}
				else {
					auto _node = _iter_node_t::node(_b);
					DEBUG_ASSERT(_node);
					return _node;
				}
			}
			return {};
		}

		template <bool _AFTER, typename _SEQ>
		[[nodiscard]] constexpr _SEQ _next_seq(_SEQ& _seq)noexcept 
		requires(!std::is_const_v<_SEQ>)
		{
			_SEQ _prev_seq{};
			if constexpr (!is_bidirectional) {
				_prev_seq = _seq;
			}
			if constexpr (_AFTER) {
				++_seq;
			}
			else {
				if constexpr (!is_bidirectional) {
					DEBUG_ASSERT(_iter_node_t::node(_prev_seq.begin()) == _m_data.front());
					_prev_seq = {};
				}
			}
			return _prev_seq;
		}

		template <bool _AFTER,typename _SEQ>
		[[nodiscard]] constexpr size_t _erase_n(_SEQ& _seq, size_t _n)noexcept 
		requires(!std::is_const_v<_SEQ>)
		{
			DEBUG_ASSERT(_seq && _n);
			size_t _count{};
			auto _prev_seq=_next_seq<_AFTER>(_seq);
			while(_seq && _n--) {
				auto _node = _node_for_detach(_seq); //increment seq
				if constexpr (is_bidirectional) {
					_m_data.detach(_node);
				}
				else {
					auto _prev_node = _iter_node_t::node(_prev_seq.begin());
					_m_data.detach(_prev_node, _node);
				}
#ifndef NDEBUG
				_node->reset_links();
#endif 
				_node_t::destroy(_node);
				_node_t::deallocate(_node, _m_data.allocator());
				++_count;
				_m_data.check();
			}
			return _count;
		}
		template <bool _AFTER,typename _SEQ>
		[[nodiscard]] constexpr size_t _erase_all(_SEQ& _seq)noexcept 
		requires(!std::is_const_v<_SEQ>)
		{
			DEBUG_ASSERT(_seq);
			auto _prev_seq = _next_seq<_AFTER>(_seq);
			size_t _count{};
			while (_seq) {
				auto _node = _node_for_detach(_seq);
				if constexpr (is_bidirectional) {
					_m_data.detach(_node);
				}
				else {
					auto _prev_node = _iter_node_t::node(_prev_seq.begin());
					_m_data.detach(_prev_node, _node);
				}
#ifndef NDEBUG
				_node->reset_links();
#endif 
				_node_t::destroy(_node);
				_node_t::deallocate(_node, _m_data.allocator());
				++_count;
			}
			return _count;
		}

		template <bool _AFTER,typename _SEQ>
		[[nodiscard]] constexpr value_t _erase_first(_SEQ& _seq)noexcept 
		requires(!std::is_const_v<_SEQ>)
		{
			DEBUG_ASSERT(_seq);
			auto _prev_seq = _next_seq<_AFTER>(_seq);
			auto _node = _node_for_detach(_seq);
			if constexpr (is_bidirectional) {
				_m_data.detach(_node);
			}
			else {
				auto _prev_node = iron::unsafe::iterators::_impl::_node::node(_prev_seq.begin());
				_m_data.detach(_prev_node, _node);
			}
			return _release_value(_node);
		}
		template <bool _AFTER,typename _SEQ,typename _F>
		[[nodiscard]] constexpr size_t _erase_if(_SEQ& _seq,_F _f)noexcept 
		requires(!std::is_const_v<_SEQ>)
		{
			DEBUG_ASSERT(_seq);
			auto _prev_seq = _next_seq<_AFTER>(_seq);
			size_t _count{};
			while (_seq) {
				const auto _ref = *_seq;
				if (_f(*_ref)) {
					auto _node = _node_for_detach(_seq);
					if constexpr (is_bidirectional) {
						_m_data.detach(_node);
					}
					else {
						auto _prev_node = iron::unsafe::iterators::_impl::_node::node(_prev_seq.begin());
						_m_data.detach(_prev_node, _node);
					}
#ifndef NDEBUG
					_node->reset_links();
#endif 
					_node_t::destroy(_node);
					_node_t::deallocate(_node, _m_data.allocator());
					++_count;
				}
				else {
					++_seq;
					if constexpr (!is_bidirectional) {
						if (!_prev_seq) {
							_prev_seq = this->seq();
						}
						else {
							++_prev_seq;
						}
					}
				}
			}
			return _count;
		}
		template <typename _SEQ>
		[[nodiscard]] constexpr size_t _unique(_SEQ& _seq)noexcept
		requires(!std::is_const_v<_SEQ>)
		{
			DEBUG_ASSERT(_seq);
			size_t _count{ 1 };
			auto _prev_seq = _seq++;
			while (_seq) {
				if (**_prev_seq == **_seq) { //drop _seq
					auto _node = _node_for_detach(_seq); //_seq advance
					if constexpr (is_bidirectional) {
						_m_data.detach(_node);
					}
					else {
						auto _prev_node = iron::unsafe::iterators::_impl::_node::node(_prev_seq.begin());
						_m_data.detach(_prev_node, _node);
					}
#ifndef NDEBUG
					_node->reset_links();
#endif 
					_node_t::destroy(_node);
					_node_t::deallocate(_node, _m_data.allocator());
				}
				else {
					++_count;
					_prev_seq = _seq++;
				}
			}
			_m_data.check();
			return _count;
		}
		[[nodiscard]] constexpr seq_t _seq(_node_t* _node,bool _tail_activate)noexcept {
			using _i_t = typename seq_t::iter_type;
			auto _b = _i_t(_node, _m_data,_tail_activate);
			auto _e = _i_t{ nullptr,_m_data,_tail_activate };
			return seq_t::unchecked_from(*this,std::move(_b), std::move(_e));
		}
		_raw_list_t _m_data{};
		IRON_SECURITY_DCL();
	};



} //ns

#ifdef _IRON_CHECK_LIST
static_assert(iron::concepts::list<iron::list<int>>, "internal error");
static_assert(!iron::concepts::list<iron::fwd_list<int>>, "internal error");
static_assert(iron::concepts::fwd_list<iron::fwd_list<int>>, "internal error");
static_assert(!iron::concepts::fwd_list<iron::list<int>>, "internal error");
#endif 

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

