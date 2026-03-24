
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable:26429)
#pragma warning (disable:26446)
#pragma warning (disable:26459)
#pragma warning (disable:26481)
#pragma warning (disable:26482)
#pragma warning (disable:26485)
#pragma warning (disable:26494)
#pragma warning (disable:26495)
#endif

namespace iron {

	template <typename _T, std::size_t _N>
	class array final {
	public:
		using value_t = _T;
		using value_type = value_t;
		using size_t = std::size_t;
		using size_type = size_t;
		using diff_t = std::ptrdiff_t;
		using difference_type = diff_t;
		using ptr_t = value_t*;
		using pointer = ptr_t;
		using reference = value_t&;
		using ref_t = iron::ref_wrapper<value_t>;
		using cref_t = iron::ref_wrapper<const value_t>;
	private:
		using _rw_t = iron::unsafe::raw_storage<value_t>;
		typedef struct _empty {} _empty_t;
		using _a_t = std::conditional_t< _N != 0,value_t[_N], _empty_t >;
	public:
		[[nodiscard]] static constexpr size_t max_size()noexcept {
			if constexpr (_N) {
				return _rw_t::max_size();
			}
			else {
				return 0;
			}
		}
		static_assert(_N <= max_size(), "array size must be <= max_size");
		static_assert(!std::is_reference_v<value_t>, "array value_type must not be a reference type");
		static_assert(!std::is_array_v<value_t>, "array value_type must not be an array type");
		static_assert(!std::is_const_v<value_t>, "array value_type must not be const");
		static_assert(std::is_nothrow_move_constructible_v<value_t>, "array value_type  must be move nothrow constructible");
		static_assert(std::is_nothrow_swappable_v<value_t>, "array value_type  must be move nothrow swappable");
		static_assert(std::is_nothrow_destructible_v<value_t>, "array value_type must be nothrow destructible");
		static_assert(std::is_nothrow_default_constructible_v<value_t>, "array value_type must be nothrow default constructible");
	private:
		using _iter_t = iron::unsafe::iterators::contiguous_iterator<value_t>;
		using _riter_t = std::reverse_iterator<_iter_t>;
		using _citer_t = iron::unsafe::iterators::contiguous_iterator<const value_t>;
		using _criter_t = std::reverse_iterator<_citer_t>;
		using _allocator_t = iron::unsafe::allocator<value_t>;
		template <typename _A>
		using _raw_vector_t = iron::unsafe::raw_vector<value_t, _A>;
	public:
		using iterator = _iter_t;
		using const_iterator = _citer_t;
		using reverse_iterator = _riter_t;
		using const_reverse_iterator = _criter_t;
		using seq_t = iron::sequence<_iter_t>;
		using seq_type = seq_t;
		using cseq_t = iron::sequence<_citer_t>;
		using cseq_type = cseq_t;
		using rseq_t = iron::sequence<_riter_t>;
		using rseq_type = rseq_t;
		using crseq_t = iron::sequence<_criter_t>;
		using crseq_type = crseq_t;
		using error_handler_t = iron::error_handler;
		using error_handler_type = error_handler_t;
		template <typename _A>
		using adapter_box_t = iron::adapter_box<_raw_vector_t<_A>>;
		struct array_tag { array_tag() = delete; };
		[[nodiscard]] static consteval size_t size()noexcept { return _N; }
		[[nodiscard]] static consteval bool empty()noexcept { return _N ? false : true; }
		[[nodiscard]] consteval explicit operator bool()const noexcept { return size() ? true : false; }
		[[nodiscard]] static constexpr array unchecked_from(value_t(&& _v)[_N])noexcept 
		requires(_N > 0)
		{
			return array(std::move(_v));
		}
		template <typename _A>
		[[nodiscard]] static constexpr iron::result<array, error_handler_t> try_from(adapter_box_t<_A>&& _box)noexcept 
		requires(iron::concepts::allocator<_A>)
		{
			IRON_SECURITY_CHECK_TEMPORARY(_box);
			if (_box.size() != _N) IRON_UNLIKELY {
				return iron::err(ERROR_HANDLER(iron::unsafe::errors::posix::code_t::argument_out_of_domain));
			}
			array _v;
			if constexpr (_N) {
				auto _d = &_v._m_v[0];
				auto _t = _box.unsafe_release();
#if IRON_SECURITY_CHECKER
				[[maybe_unused]] const auto _x = std::move(_box); //mark _box temporary
#endif 
				auto _p = _t.data();
				auto _n = _N;
				while (_n--) {
					if (std::is_constant_evaluated()) {
						*_d = std::move(*_p);
						if (_n) {
							++_d;
							++_p;
						}
					}
					else {
						*_d++ = std::move(*_p++);
					}
				}
			}
			else {
#if IRON_SECURITY_CHECKER
				[[maybe_unused]] const auto _x = std::move(_box); //mark _box temporary
#endif 
			}
			return iron::ok(std::move(_v));
		}
		template <typename _A>
		[[nodiscard]] static constexpr array from(adapter_box_t<_A>&& _box)noexcept 
		requires(iron::concepts::allocator<_A>)
		{
			auto _r = try_from<_A>(std::move(_box)); RP(_r);
		}
		constexpr array()noexcept 
		{
			_init();
		}
		array(const array&) = delete;
		constexpr array(array&& _v)noexcept 
		{
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_init();
			_swap(_v);
			IRON_SECURITY_MARK_TEMPORARY(_v);
		}
		array& operator=(const array&) = delete;
		constexpr array& operator=(array&& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_swap(_v);
			IRON_SECURITY_MARK_TEMPORARY(_v);
			return *this;
		}
		constexpr ~array()=default;
		constexpr void swap(array& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_swap(_v);
		}
		template <size_t _M=1>
		[[nodiscard]] constexpr iron::result<array<value_t,_M * _N>, error_handler_t> try_clone()const noexcept 
		requires ((_N == 0 || max_size() / _N >= _M) && iron::concepts::copyable_or_cloneable<value_t>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			constexpr auto _n = _M * _N;
			if constexpr (_n) {
				array<value_t,_n> _t;
				auto _p = &_t.unchecked_front();
				auto _m = _M;
				while (_m--) IRON_LIKELY
				{
					auto _h = _rw_t::assign_n(_p, _N, &_m_v[0]); RR(_h);
					if (_m) IRON_UNLIKELY
					{
						_p += _N;
					}
				}
				return iron::ok(std::move(_t));
			}
			else {
				return iron::ok(array<value_t, _n>{});
			}
		}
		template <size_t _M = 1>
		[[nodiscard]] constexpr array<value_t, _M * _N> clone()const noexcept 
		requires ((_N == 0 || max_size() / _N >= _M) && iron::concepts::copyable_or_cloneable<value_t>)
		{
			auto _r = try_clone<_M>(); RP(_r);
		}
		[[nodiscard]] constexpr iron::option<ref_t> nth(size_t _idx)noexcept 
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			if constexpr (_N) {
				if (_idx < _N) {
					return iron::some(iron::make_ref(*this, _m_v[_idx]));
				}
			}
			return {};
		}

		[[nodiscard]] constexpr ref_t at(size_t _idx)noexcept 
		requires(_N > 0)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			if constexpr (_N) {
				if (_idx < _N) IRON_LIKELY
				{
					return iron::make_ref(*this, _m_v[_idx]);
				}
			}
			PANIC("index out of bound");
		}
		[[nodiscard]] constexpr ref_t operator[](size_t _idx)noexcept 
		requires(_N > 0)
		{
			return at(_idx);
		}
		[[nodiscard]] constexpr iron::option<cref_t> nth(size_t _idx)const noexcept 
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			if constexpr (_N) {
				if (_idx < _N) {
					return iron::some(iron::make_cref(*this, _m_v[_idx]));
				}
			}
			return {};
		}
		[[nodiscard]] constexpr iron::option<cref_t> cnth(size_t _idx)const noexcept {
			return nth(_idx);
		}
		[[nodiscard]] constexpr cref_t at(size_t _idx)const noexcept 
		requires(_N > 0)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			if constexpr (_N) {
				if (_idx < _N) IRON_LIKELY
				{
					return iron::make_cref(*this, _m_v[_idx]);
				}
			}
			PANIC("index out of bound");
		}
		[[nodiscard]] constexpr cref_t operator[](size_t _idx)const noexcept 
		requires(_N > 0)
		{
			return at(_idx);
		}
		[[nodiscard]] constexpr value_t& unchecked_nth(size_t _idx) noexcept 
		requires(_N > 0)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			DEBUG_ASSERT(_idx < size());
			return _m_v[_idx];
		}
		[[nodiscard]] constexpr const value_t& unchecked_nth(size_t _idx)const noexcept 
		requires(_N > 0)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			DEBUG_ASSERT(_idx < size());
			return _m_v[_idx];
		}
		[[nodiscard]] constexpr const value_t& unchecked_cnth(size_t _idx)const noexcept
			requires(_N > 0)
		{
			return unchecked_nth(_idx);
		}

		[[nodiscard]] constexpr cref_t front()const noexcept 
		requires(_N > 0)
		{ 
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return iron::make_cref(*this, _m_v[0]);

		}
		[[nodiscard]] constexpr cref_t cfront()const noexcept
		requires(_N > 0)
		{
			return front();
		}
		[[nodiscard]] constexpr const value_t& unchecked_front()const noexcept 
		requires(_N > 0)
		{ 
			return unchecked_nth(0); 
		}
		[[nodiscard]] constexpr const value_t& unchecked_cfront()const noexcept
			requires(_N > 0)
		{
			return unchecked_front();
		}
		[[nodiscard]] constexpr ref_t front()noexcept 
		requires(_N > 0)
		{ 
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return iron::make_ref(*this, _m_v[0]);	
		}
		[[nodiscard]] constexpr value_t& unchecked_front()noexcept 
		requires(_N > 0)
		{ 
			return unchecked_nth(0); 
		}
		[[nodiscard]] constexpr cref_t back()const noexcept 
			requires(_N > 0)
		{ 
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return iron::make_cref(*this, _m_v[_N - 1]);
		}
		[[nodiscard]] constexpr cref_t cback()const noexcept 
		requires(_N > 0)
		{
			return back();
		}
		[[nodiscard]] constexpr const value_t& unchecked_back()const noexcept 
		requires(_N > 0)
		{ 
			return unchecked_nth(_N - 1); 
		}
		[[nodiscard]] constexpr const value_t& unchecked_cback()const noexcept
			requires(_N > 0)
		{
			return unchecked_back();
		}
		[[nodiscard]] constexpr ref_t back()noexcept 
		requires(_N > 0)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return iron::make_ref(*this, _m_v[_N - 1]);
		}
		[[nodiscard]] constexpr value_t& unchecked_back()noexcept 
		requires(_N > 0)
		{ 
			return unchecked_nth(_N - 1); 
		}
		[[nodiscard]] constexpr error_handler_t try_fill(value_t&& _v)noexcept 
		requires(iron::concepts::copyable_or_cloneable<value_t>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			if constexpr (_N) {
				_a_t _t{};
				auto _p = &_t[0];
				auto _h = _rw_t::assign_fill(_p, _N, std::move(_v)); RE(_h);
				std::swap(_m_v, _t);
			}
			return {};
		}
		constexpr array& fill(value_t&& _v)noexcept 
		requires(iron::concepts::copyable_or_cloneable<value_t>)
		{
			auto _h = try_fill(std::move(_v)); RVP(_h, *this);
		}
		constexpr array& fill(const value_t& _v)noexcept
		requires(std::is_nothrow_copy_constructible_v<value_t>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			auto _vv = _v;
			auto _p = &_m_v[0];
			auto _h = _rw_t::assign_fill(_p, _N, std::move(_vv));
			DEBUG_ASSERT(_h);
			return *this;
		}
		[[nodiscard]] constexpr cseq_t cseq()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			if constexpr (_N) {
				auto _p = &_m_v[0];			
				return cseq_t::unchecked_from(*this, _citer_t(_p), _citer_t(_p + _N));
			}
			else {
				return {};
			}
		}
		[[nodiscard]] constexpr crseq_t crseq()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			if constexpr (_N) {
				auto _p = &_m_v[0];
				return crseq_t::unchecked_from(*this, _criter_t(_citer_t(_p + _N)), _criter_t(_citer_t(_p)));
			}
			else {
				return {};
			}
		}
		[[nodiscard]] constexpr seq_t seq()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			if constexpr (_N) {
				auto _p = &_m_v[0];
				return seq_t::unchecked_from(*this, _iter_t(_p), _iter_t(_p + _N));
			}
			else {
				return {};
			}
		}
		[[nodiscard]] constexpr rseq_t rseq()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			if constexpr (_N) {
				auto _p = &_m_v[0];
				return rseq_t::unchecked_from(*this, _riter_t(_iter_t(_p + _N)), _riter_t(_iter_t(_p)));
			}
			else {
				return {};
			}
		}
		[[nodiscard]] constexpr cseq_t seq()const noexcept { return cseq(); }
		[[nodiscard]] constexpr crseq_t rseq()const noexcept { return crseq(); }
		
		template <typename _A=iron::unsafe::allocator<value_t>>
		[[nodiscard]] constexpr iron::result<adapter_box_t<_A>, error_handler_t> try_release()noexcept 
		requires(iron::concepts::allocator<_A>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			typename adapter_box_t<_A>::container_t _rw(_A{});
			if constexpr (_N) {
				auto _h = _rw.reserve(_N); RR(_h);
				auto _p = &_m_v[0];
				auto _n = _N;
				while (_n--) IRON_LIKELY
				{
					_h = _rw.push_back(std::move(*_p));
					DEBUG_ASSERT(_h);
					*_p = {};
					if (std::is_constant_evaluated()) {
						if (_n) {
							++_p;
						}
					}
					else {
						++_p;
					}
				}
			}
			return iron::ok(adapter_box_t<_A>::unchecked_from(std::move(_rw)));
		}
		template <typename _A=iron::unsafe::allocator<value_t>>
		[[nodiscard]] constexpr adapter_box_t<_A> release()noexcept 
		requires(iron::concepts::allocator<_A>)
		{
			auto _r = try_release<_A>(); RP(_r);
		}
		constexpr array& clear()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			if constexpr (_N) {
				auto _n=_N;
				auto _p = &_m_v[0];
				while(_n--) IRON_LIKELY
				{
					*_p++={};
				}
			}
			return *this;
		}
		[[nodiscard]] constexpr bool operator==(const array<value_t, _N>& _r)const noexcept 
		requires(iron::concepts::nothrow_equality_comparable<value_t>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_r);
			if constexpr (_N) {
				auto _pl = &_m_v[0];
				auto _pr = &_r._m_v[0];
				return std::equal(_pl, _pl + _N, _pr);
			}
			else {
				return true;
			}
		}

		IRON_SECURITY_METHOD();
	private:
		constexpr explicit array(value_t(&& _v)[_N])noexcept 
		requires(_N > 0)
		{
			_init();
			std::swap(_m_v,_v);
		}
		constexpr void _init() {
			if constexpr (_N) {
				if constexpr (std::is_trivially_constructible_v<value_t>) {
					auto _p = &_m_v[0];
					[[maybe_unused]] const auto _h = _rw_t::assign_fill(_p, _N, value_t{});
					DEBUG_ASSERT(_h);
				}
			}
		}
		constexpr void _swap(array& _v) 
		{
			if constexpr (_N) {
				std::swap(_m_v, _v._m_v);
			}
			IRON_SECURITY_SWAP(_v);
		}
		IRON_NO_UNIQUE_ADDRESS _a_t _m_v;
		IRON_SECURITY_DCL();
	};

} //ns

static_assert(iron::concepts::array<iron::array<int, 0>>, "internal error");
static_assert(iron::concepts::array<iron::array<int, 1>>, "internal error");

#if defined(_MSC_VER)
#pragma warning(pop)
#endif


