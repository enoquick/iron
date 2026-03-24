
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable:26481)
#endif

namespace iron {

	template <typename _T,typename _A>
	class base_vector final {
	public:
		using value_t = _T;
		using value_type = value_t;
		using allocator_t = _A;
		using allocator_type = allocator_t;
		static_assert(!std::is_reference_v<value_t>, "vector value_type must not be a reference type");
		static_assert(!std::is_array_v<value_t>, "vector value_type must not be an array type");
		static_assert(!std::is_const_v<value_t>, "vector value_type must not be const");
		static_assert(std::is_nothrow_move_constructible_v<value_t>, "vector value_type  must be move nothrow constructible");
		static_assert(std::is_nothrow_swappable_v<value_t>, "vector value_type  must be move nothrow swappable");
		static_assert(std::is_nothrow_destructible_v<value_t>, "vector value_type must be nothrow destructible");
		static_assert(iron::concepts::allocator<allocator_t>, "vector allocator_type is not an allocator");
		using size_t = std::size_t;
		using size_type = size_t;
		using diff_t = std::ptrdiff_t;
		using difference_type = diff_t;
		using ref_t = iron::ref_wrapper<value_t>;
		using cref_t = iron::ref_wrapper<const value_t>;
		using reference = value_t&;
		using const_reference = const value_t&;
		using ptr_t = value_t*;
		using cptr_t = const value_t*;
		using pointer = value_t*;
		using const_pointer = const value_t*;
	private:
		using _rs_t = iron::unsafe::raw_storage<value_t>;
		using _iter_t = iron::unsafe::iterators::contiguous_iterator<value_t>;
		using _riter_t = std::reverse_iterator<_iter_t>;
		using _citer_t = iron::unsafe::iterators::contiguous_iterator<const value_t>;
		using _criter_t = std::reverse_iterator<_citer_t>;
		using _raw_vector_t = iron::unsafe::raw_vector<value_t, allocator_t>;
	public:
		using iterator = _iter_t;
		using const_iterator = _citer_t;
		using reverse_iterator = _riter_t;
		using const_reverse_iterator = _criter_t;
		using seq_t = iron::sequence<_iter_t>;
		using seq_type = seq_t;
		using cseq_t = iron::sequence<_citer_t>;
		using const_seq_type = cseq_t;
		using rseq_t = iron::sequence<_riter_t>;
		using rev_seq_type = rseq_t;
		using crseq_t = iron::sequence<_criter_t>;
		using const_rev_seq_type = crseq_t;
		using error_handler_t = iron::error_handler;
		using error_handler_type = error_handler_t;
		using adapter_box_t = iron::adapter_box<_raw_vector_t>;
		using adapter_box_type = adapter_box_t;
		struct vector_tag { vector_tag() = delete; };
		[[nodiscard]] static constexpr base_vector unchecked_from(allocator_t _a)noexcept { 
			return base_vector(_a); 
		}
		[[nodiscard]] static constexpr base_vector from(adapter_box_t&& _box)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY(_box);
			return base_vector(std::move(_box));
		}
		constexpr base_vector()noexcept { }
		constexpr base_vector(base_vector&& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_swap(_v);
			IRON_SECURITY_MARK_TEMPORARY(_v);
		}
		base_vector(const base_vector&) = delete;
		constexpr base_vector& operator=(base_vector&& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_swap(_v);
			IRON_SECURITY_MARK_TEMPORARY(_v);
			return *this;
		}
		base_vector& operator=(const base_vector&) = delete;
		constexpr ~base_vector() = default;

		[[nodiscard]] constexpr iron::result<base_vector, error_handler_t> try_clone(size_t _n = 1)const noexcept
		requires(iron::concepts::copyable_or_cloneable<value_t>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _clone(_n);
		}
		[[nodiscard]] constexpr base_vector clone(size_t _n = 1)const noexcept
		requires(iron::concepts::copyable_or_cloneable<value_t>)
		{
			auto _r = try_clone(_n); RP(_r);
		}
		[[nodiscard]] static constexpr size_t max_size()noexcept { return _raw_vector_t::max_size(); }
		[[nodiscard]] constexpr size_t size()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _m_box.size();
		}
		[[nodiscard]] constexpr bool empty()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _m_box.empty();
		}
		[[nodiscard]] constexpr explicit operator bool()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _m_box.size() ? true : false;
		}
		[[nodiscard]] constexpr size_t capacity()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _m_box.capacity();
		}
		[[nodiscard]] constexpr allocator_t unsafe_allocator()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _m_box.allocator();
		}
		[[nodiscard]] constexpr const value_t* unsafe_data()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _m_box.data();
		}
		[[nodiscard]] constexpr const value_t* unsafe_cdata()const noexcept {
			return unsafe_data();
		}
		[[nodiscard]] constexpr value_t* unsafe_data()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _m_box.data();
		}
		[[nodiscard]] constexpr error_handler_t try_reserve_at_least(size_t _additional, size_t _n = 1)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			return _reserve_at_least(_additional, _n);
		}
		constexpr base_vector& reserve_at_least(size_t _additional, size_t _n = 1)noexcept {
			const auto _h = try_reserve_at_least(_additional, _n); RVP(_h, *this);
		}
		[[nodiscard]] constexpr error_handler_t try_reserve_exact(size_t _additional, size_t _n = 1)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			return _reserve_exact(_additional, _n);
		}
		constexpr base_vector& reserve_exact(size_t _additional, size_t _n = 1)noexcept {
			auto _h = try_reserve_exact(_additional, _n); RVP(_h, *this);
		}
		template<typename... _ARGS >
		[[nodiscard]] constexpr iron::result<seq_t, error_handler_t> try_push_back(_ARGS&&... _args)noexcept
		requires (iron::concepts::constructible_at<value_t, _ARGS&&...>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			IRON_SECURITY_CHECK_TEMPORARY_ARGS(_args...);
#ifndef IRON_NOCHECK_SIZE
			if (size() == max_size()) IRON_UNLIKELY
			{
				return iron::err(ERROR_HANDLER(iron::unsafe::errors::posix::code_t::result_out_of_range));
			}
#endif
			if (size() == capacity()) IRON_UNLIKELY
			{
				auto _h=_reserve_at_least(1,1); RR(_h);
				DEBUG_ASSERT(_m_box.size() < _m_box.capacity());
			}
			auto _h = _m_box.push_back(std::forward<_ARGS>(_args)...); RR(_h);
			auto _p = _m_box.data();
			const auto _sz = _m_box.size();
			return iron::ok(seq_t::unchecked_from(*this, _iter_t(_p + (_sz - 1)), _iter_t(_p + _sz)));
		}
		template<typename... _ARGS >
		constexpr seq_t push_back(_ARGS&&... _args)noexcept 
		requires (iron::concepts::constructible_at<value_t, _ARGS&&...>)
		{
			auto _r = try_push_back(std::forward<_ARGS>(_args)...); RP(_r);
		}

		template <typename _SEQ>
		[[maybe_unused]] constexpr iron::result<seq_t, error_handler_t> try_push_back_range(_SEQ& _seq)noexcept
		requires(
			iron::concepts::sequence<_SEQ>
			&& !std::is_const_v<_SEQ>
			&& iron::concepts::constructible_at<value_t, std::decay_t<typename _SEQ::value_t>&&>
		)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_seq);
			const auto _sz = _seq.clone().count();
#ifndef IRON_NOCHECK_SIZE
			if (max_size() - _m_box.size() < _sz) IRON_UNLIKELY
			{
				return iron::err(ERROR_HANDLER(iron::unsafe::errors::posix::code_t::result_out_of_range));
			}
#endif
			if (_m_box.size() + _sz >=  _m_box.capacity()) IRON_UNLIKELY
			{
				auto _h = _reserve_at_least(_sz,1); RR(_h);
			}
			auto& _b = _seq.begin();
			const auto _e = _seq.end();
			constexpr bool _transactional = true;
			auto _r = _m_box.template range_push_back<_transactional>(_b, _e); RR(_r);
			DEBUG_ASSERT_EQ(_b, _e);
#if IRON_SECURITY_CHECKER
			_seq.reset();
#endif 
			return iron::ok(seq_t::unchecked_from(*this, _iter_t(_r.unchecked_unwrap()), _iter_t(_m_box.data() + _m_box.size())));
		}
		template <typename _SEQ>
		constexpr seq_t push_back_range(_SEQ& _seq)noexcept
		requires(
			iron::concepts::sequence<_SEQ>
			&& !std::is_const_v<_SEQ>
			&& iron::concepts::constructible_at<value_t, std::decay_t<typename _SEQ::value_t>&&>
		)
		{
			auto _r = try_push_back_range(_seq); RP(_r);
		}
		constexpr value_t unchecked_pop_back()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			DEBUG_ASSERT(!empty());
			return _pop_back();
		}
		constexpr iron::option<value_t> pop_back()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			if (_m_box.size()) {
				return iron::some(_pop_back());
			}
			return {};
		}
		[[nodiscard]] constexpr iron::option<ref_t> nth(size_t _idx)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			if (_idx < _m_box.size()) IRON_LIKELY
			{
				return iron::some(_nth(_idx));
			}
			return {};
		}
		[[nodiscard]] constexpr ref_t at(size_t _idx)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			if (_idx < _m_box.size()) IRON_LIKELY
			{
				return _nth(_idx);
			}
			PANIC("index out of bound");
		}
		[[nodiscard]] constexpr ref_t operator[](size_t _idx)noexcept {
			return at(_idx);
		}
		[[nodiscard]] constexpr option<cref_t> nth(size_t _idx)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			if (_idx < _m_box.size()) IRON_LIKELY
			{
				return iron::some(_nth(_idx));
			}
			return {};
		}
		[[nodiscard]] constexpr option<cref_t> cnth(size_t _idx)const noexcept {
			return nth(_idx);
		}
		[[nodiscard]] constexpr cref_t at(size_t _idx)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			if (_idx < _m_box.size()) IRON_LIKELY
			{
				return _nth(_idx);
			}
			PANIC("index out of bound");
		}
		[[nodiscard]] constexpr cref_t operator[](size_t _idx)const noexcept {
			return at(_idx);
		}
		[[nodiscard]] constexpr value_t& unchecked_nth(size_t _idx) noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return *(_m_box.nth(_idx));
		}
		[[nodiscard]] constexpr const value_t& unchecked_nth(size_t _idx)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return *(_m_box.nth(_idx));
		}
		[[nodiscard]] constexpr const value_t& unchecked_cnth(size_t _idx)const noexcept {
			return unchecked_nth(_idx);
		}

		[[nodiscard]] constexpr iron::option<cref_t> front()const noexcept { return nth(0); }
		[[nodiscard]] constexpr iron::option<ref_t> front()noexcept { return nth(0); }
		[[nodiscard]] constexpr iron::option<cref_t> cfront()const noexcept { return front(); }

		[[nodiscard]] constexpr const value_t& unchecked_front()const noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _m_box.front(); 
		}
		[[nodiscard]] constexpr const value_t& unchecked_cfront()const noexcept {
			return  unchecked_front();
		}
		[[nodiscard]] constexpr value_t& unchecked_front()noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _m_box.front();
		}
		[[nodiscard]] constexpr iron::option<cref_t> back()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			if (auto _sz=_m_box.size()) IRON_LIKELY 
			{
				return iron::some(_nth(--_sz));
			}
			return {};
		}
		[[nodiscard]] constexpr iron::option<cref_t> cback()const noexcept {
			return back();
		}
		[[nodiscard]] constexpr iron::option<ref_t> back()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			if (auto _sz=_m_box.size()) IRON_LIKELY 
			{
				return iron::some(_nth(--_sz));
			}
			return {};
		}
		[[nodiscard]] constexpr const value_t& unchecked_back()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return *(_m_box.back());
		}
		[[nodiscard]] constexpr const value_t& unchecked_cback()const noexcept {
			return  unchecked_back();
		}
		[[nodiscard]] constexpr value_t& unchecked_back() noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return *(_m_box.back());
		}
		[[nodiscard]] constexpr error_handler_t try_shrink_to_fit()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			return _m_box.shrink_to_fit();
		}
		constexpr base_vector& shrink_to_fit()noexcept {
			[[maybe_unused]] const auto _h = try_shrink_to_fit();
			//ignore the error
			return *this;
		}
		[[nodiscard]] constexpr adapter_box_t release()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			_raw_vector_t _box;
			_box.swap(_m_box);
			return adapter_box_t::unchecked_from(std::move(_box));
		}
		constexpr base_vector& clear()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			_m_box.clear();
			return *this;
		}
		constexpr base_vector& reset()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			_m_box.reset();
			return *this;
		}
		[[nodiscard]] constexpr error_handler_t try_resize(size_t _n)noexcept
		requires(std::is_default_constructible_v<value_t>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			return _resize(_n);
		}
		constexpr base_vector& resize(size_t _n)noexcept
		requires(std::is_default_constructible_v<value_t>)
		{
			const auto _h = try_resize(_n); RVP(_h,*this);
		}
		[[nodiscard]] constexpr error_handler_t try_resize(size_t _n,const value_t& _v)noexcept
		requires(iron::concepts::copyable_or_cloneable<value_t>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			return _resize(_n,_v);
		}
		constexpr base_vector& resize(size_t _n,const value_t& _v)noexcept
		requires(iron::concepts::copyable_or_cloneable<value_t>)
		{
			const auto _h = try_resize(_n, _v); RVP(_h, *this);
		}
		constexpr base_vector& swap(base_vector& _v) noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_swap(_v);
			return *this;
		}
		[[nodiscard]] constexpr cseq_t cseq()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			auto _p = _m_box.data();
			return cseq_t::unchecked_from(*this, _citer_t(_p), _citer_t(_p + _m_box.size()));
		}
		[[nodiscard]] constexpr crseq_t crseq()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			auto _p = _m_box.data();
			return crseq_t::unchecked_from(*this, _criter_t(_citer_t(_p + _m_box.size())), _criter_t(_citer_t(_p)));
		}
		[[nodiscard]] constexpr seq_t seq()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			auto _p = _m_box.data();
			return seq_t::unchecked_from(*this, _iter_t(_p), _iter_t(_p + _m_box.size()));
		}
		[[nodiscard]] constexpr rseq_t rseq()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			auto _p = _m_box.data();
			return rseq_t::unchecked_from(*this, _riter_t(_iter_t(_p + _m_box.size())), _riter_t(_iter_t(_p)));
		}
		[[nodiscard]] constexpr cseq_t seq()const noexcept { return cseq(); }
		[[nodiscard]] constexpr crseq_t rseq()const noexcept { return crseq(); }

		template <typename _A1>
		[[nodiscard]] constexpr bool operator==(const base_vector<value_t, _A1>& _v)const noexcept
		requires (iron::concepts::nothrow_equality_comparable<value_t>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			return _m_box == _v._m_box;
		}
		IRON_SECURITY_METHOD()
	private:
		template <typename _T1,typename _A1>
		friend class base_vector;
		constexpr explicit base_vector(allocator_t _a)noexcept
			: _m_box(_a)
		{}
		constexpr explicit base_vector(_raw_vector_t&& _v)noexcept
			: _m_box(std::move(_v))
		{}
		constexpr explicit base_vector(adapter_box_t&& _box)noexcept
#if !IRON_SECURITY_CHECKER
			: _m_box(_box.unsafe_release())
#endif
		{
#if IRON_SECURITY_CHECKER
			auto _t = std::move(_box); //mark _box temporary
			_m_box = _t.unsafe_release();
#endif
		}
		constexpr void _swap(base_vector& _v)noexcept {
			_m_box.swap(_v._m_box);
			IRON_SECURITY_SWAP(_v);
		}
		[[nodiscard]] constexpr error_handler_t _reserve_at_least(size_t _additional, size_t _n)noexcept {
#ifndef IRON_NOCHECK_SIZE
			const auto _x = iron::usizeb(_n) * iron::usizeb(_additional);
			if (!_x || _x.unchecked_value() > _raw_vector_t::max_size()) IRON_UNLIKELY
			{
				return ERROR_HANDLER(iron::unsafe::errors::posix::code_t::result_out_of_range);
			}
			const auto _y = _x.unchecked_value();
#else
			const auto _y = _n * _additional;
#endif
			if (_y > _m_box.capacity() - _m_box.size()) IRON_LIKELY 
			{
				size_t _c{};
				auto _h = _rs_t::default_extension_policy(_c, _m_box.capacity(), _y); RE(_h);
				return _m_box.reserve(_c);
			}
			return {};
		}

		[[nodiscard]] constexpr error_handler_t _reserve_exact(size_t _additional, size_t _n)noexcept {
#ifndef IRON_NOCHECK_SIZE
			const auto _x = iron::usizeb(_n) * iron::usizeb(_additional);
			if (!_x || _x.unchecked_value() > _raw_vector_t::max_size()) IRON_UNLIKELY
			{
				return ERROR_HANDLER(iron::unsafe::errors::posix::code_t::result_out_of_range);
			}
			const auto _free = _m_box.capacity() - _m_box.size();
			if (_x.unchecked_value() > _free) IRON_LIKELY
			{
				const auto _c = iron::usizeb(_m_box.capacity() - _free) + _x;
				if (!_c || _c.unchecked_value() > _raw_vector_t::max_size()) IRON_UNLIKELY 
				{
					return ERROR_HANDLER(iron::unsafe::errors::posix::code_t::result_out_of_range);
				}
				return _m_box.reserve(_c.unchecked_value());
			}
#else
			const auto _x = _n * _additional;
			const auto _free = _m_box.capacity() - _m_box.size();
			if (_x > _free) IRON_LIKELY {
				const auto _c = _m_box.capacity() - _free + _x;
				return _m_box.reserve(_c);
			}
#endif
			return {};
		}
		[[nodiscard]] constexpr iron::result<base_vector, error_handler_t> _clone(size_t _n)const noexcept
		{
			if (_m_box.size() && _n) IRON_LIKELY 
			{
#ifndef IRON_NOCHECK_SIZE
				const auto _x = iron::usizeb(_n) * iron::usizeb(_m_box.size());
				if (!_x || _x.unchecked_value() > max_size()) IRON_UNLIKELY
				{
					return iron::err(ERROR_HANDLER(iron::unsafe::errors::posix::code_t::result_out_of_range));
				}
#endif
				auto _r = _m_box.try_clone(_n); RR(_r);
				return iron::ok(base_vector(std::move(_r.unchecked_unwrap())));
			}
			return iron::ok(base_vector(_m_box.allocator()));
		}

		[[nodiscard]] constexpr value_t _pop_back()noexcept {
			return _m_box.pop_back();
		}
		[[nodiscard]] constexpr error_handler_t _resize(size_t _n)noexcept {
#ifndef IRON_NOCHECK_SIZE
			if (_n > _raw_vector_t::max_size()) IRON_UNLIKELY
			{
				return ERROR_HANDLER(iron::unsafe::errors::posix::code_t::argument_out_of_domain);
			}
#endif
			if (!_n) IRON_UNLIKELY 
			{
				_m_box.reset();
				return {};
			}
			if (_n != _m_box.size()) IRON_LIKELY 
			{
				return _m_box.resize(_n);
			}
			return {};
		}
		[[nodiscard]] constexpr error_handler_t _resize(size_t _n,const value_t& _v)noexcept {
#ifndef IRON_NOCHECK_SIZE
			if (_n > _raw_vector_t::max_size()) IRON_UNLIKELY
			{
				return ERROR_HANDLER(iron::unsafe::errors::posix::code_t::argument_out_of_domain);
			}
#endif
			if (!_n) IRON_UNLIKELY
			{
				_m_box.reset();
				return {};
			}
			if (_n != _m_box.size()) IRON_LIKELY
			{
				return _m_box.resize(_n,_v);
			}
			return {};
		}
		[[nodiscard]] constexpr ref_t _nth(size_t _idx)noexcept {
			value_t& _v = *(_m_box.nth(_idx));
			return iron::make_ref(*this, _v);
		}
		[[nodiscard]] constexpr cref_t _nth(size_t _idx)const noexcept {
			const value_t& _v = *(_m_box.nth(_idx));
			return iron::make_cref(*this, _v);
		}
		_raw_vector_t _m_box{};
		IRON_SECURITY_DCL();
	};


} //ns

static_assert(iron::concepts::vector<iron::vector<int>>, "internal error");

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

