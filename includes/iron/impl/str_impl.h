
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable:26429)
#pragma warning (disable:26430)
#pragma warning (disable:26446)
#pragma warning (disable:26472)
#pragma warning (disable:26481)
#pragma warning (disable:26482)
#pragma warning (disable:26485)
#pragma warning (disable:26490)
#pragma warning (disable:26492)
#pragma warning (disable:26494)
#pragma warning (disable:26820)
#endif

namespace iron {

	template <typename _T, typename _A, std::size_t _FIXED_SIZE,bool _SAFE>
	class base_str final {
	public:
		using value_t = _T;
		using value_type = value_t;
		using allocator_t = _A;
		using allocator_type = allocator_t;
		static_assert(!std::is_reference_v<value_t>, "string template parameter value_type  must not be a reference type");
		static_assert(!std::is_const_v<value_t>, "string template parameter value_type must not be const");
		static_assert(iron::concepts::charset<value_t>, "string template parameter value_type must be character");
	private:
		using _raw_str_t = iron::unsafe::str::raw_str<value_t, allocator_t, _FIXED_SIZE,_SAFE>;
		using _raw_adapter_box_t = typename _raw_str_t::adapter_box_t;
		using _rs_t = typename _raw_str_t::rs_t;
		using _common_t = typename _raw_str_t::common_t;
	public:
		using size_t = typename _raw_str_t::size_t;
		using size_type = size_t;
		using adapter_box_t = iron::adapter_box<_raw_adapter_box_t>;
		[[nodiscard]] static constexpr size_t max_size()noexcept { return _raw_str_t::max_size(); }
		[[nodiscard]] static constexpr size_t fixed_size()noexcept { return _raw_str_t::fixed_size(); }
		[[nodiscard]] static constexpr bool is_safe()noexcept { return _raw_str_t::is_safe(); }
		static_assert(fixed_size() < max_size(), "string template parameter stack_size must be less max_size");
		using view_t = iron::base_strview<value_t, _SAFE>;
		using view_type = view_t;
		using cview_t = iron::base_strview<const value_t, _SAFE>;
		using const_view_type = cview_t;
	private:
		using _raw_strview_t = iron::unsafe::str::raw_strview<value_t, _SAFE>;
		using _raw_iter_t = typename _raw_strview_t::raw_iter_t;
		using _rev_raw_iter_t = typename _raw_strview_t::rev_raw_iter_t;
		using _const_raw_iter_t = typename _raw_strview_t::const_raw_iter_t;
		using _const_rev_raw_iter_t = typename _raw_strview_t::const_rev_raw_iter_t;
		using _iter_t = typename _raw_strview_t::iter_t;
		using _const_iter_t = typename _raw_strview_t::const_iter_t;
	public:
		struct str_tag { str_tag() = delete; };
		using char_t = std::conditional_t<iron::concepts::fixed_charset<value_t>, value_t, char32_t>;
		using char_type = char_t;
		using diff_t = std::ptrdiff_t;
		using difference_type_t = diff_t;
		using ptr_t = value_t*;
		using cptr_t = const value_t*;
		using pointer = ptr_t;
		using ref_t = iron::ref_wrapper<value_t>;
		using cref_t = iron::ref_wrapper<const value_t>;
		using reference = value_t&;
		using const_reference = const value_t&;
		using iterator = _iter_t;
		using const_iterator = _const_iter_t;
		using error_handler_t = iron::error_handler;
		using error_handler_type = error_handler_t;
		using raw_seq_t = typename view_t::raw_seq_t;
		using raw_seq_type = raw_seq_t;
		using craw_seq_t = typename view_t::craw_seq_t;
		using const_raw_seq_type = craw_seq_t;
		using rev_raw_seq_t = typename view_t::rev_raw_seq_t;
		using rev_raw_seq_type = rev_raw_seq_t;
		using crev_raw_seq_t = typename view_t::crev_raw_seq_t;
		using const_rev_raw_seq_type = crev_raw_seq_t;
		using seq_t = typename view_t::seq_t;
		using seq_type = seq_t;
		using cseq_t = typename view_t::cseq_t;
		using const_seq_type = cseq_t;

		//
		//static members from 
		//
	
		[[nodiscard]] static constexpr base_str unchecked_from(allocator_t _a)noexcept { return base_str(_a); }

		//from char

		template <typename _U>
		[[nodiscard]] static constexpr iron::result<base_str,error_handler_t> try_unchecked_from(_U _v,size_t _n,allocator_t _a)noexcept
		requires (iron::concepts::fixed_charset<_U>)
		{
			auto _s = unchecked_from(_a);
			auto _h = _s._push_value(_n, _v); RR(_h);
			return iron::ok(std::move(_s));
		}
		template <typename _U>
		[[nodiscard]] static constexpr base_str unchecked_from(_U _v, size_t _n, allocator_t _a)noexcept
			requires (iron::concepts::fixed_charset<_U>)
		{
			auto _r = try_unchecked_from(_v, _n, _a); RP(_r);
		}

		template <typename _U>
		[[nodiscard]] static constexpr iron::result<base_str, error_handler_t> try_from(_U _v, size_t _n = 1)noexcept
			requires (iron::concepts::fixed_charset<_U>)
		{
			return try_unchecked_from(_v, _n, {});
		}

		template <typename _U>
		[[nodiscard]] static constexpr base_str from(_U _v,size_t _n=1)noexcept 
			requires (iron::concepts::fixed_charset<_U>)
		{
			auto _r = try_from(_v,_n); RP(_r);
		}

		//from iter

		template <typename _ITER>
		[[nodiscard]] static constexpr iron::result<base_str, error_handler_t> try_unchecked_from(_ITER& _b, const _ITER& _e, size_t _n=1,allocator_t _a = {})noexcept 
			requires (iron::concepts::charset_forward_iterator<_ITER> && !std::is_const_v<_ITER>)
		{
			auto _s = unchecked_from(_a);
			auto _h = _s.try_unchecked_push_back(_b, _e,_n); RR(_h);
			return iron::ok(std::move(_s));
		}

		template <typename _ITER>
		[[nodiscard]] static constexpr base_str unchecked_from(_ITER& _b, const _ITER& _e,size_t _n=1,allocator_t _a = {})noexcept 
			requires (iron::concepts::charset_forward_iterator<_ITER> && !std::is_const_v<_ITER>)
		{
			auto _r = try_unchecked_from(_b, _e, _n,_a); RP(_r);
		}

		//from array 
		template <typename _U, size_t _N>
		[[nodiscard]] static constexpr iron::result<base_str, error_handler_t> try_unchecked_from(const _U(&_v)[_N], size_t _n, allocator_t _a)noexcept
			requires (iron::concepts::charset<_U>&& _N < max_size())
		{
			auto _s = unchecked_from(_a);
			auto _h = _s.try_push_back(_v, _n); RR(_h);
			return iron::ok(std::move(_s));
		}

		template <typename _U, size_t _N>
		[[nodiscard]] static constexpr iron::result<base_str, error_handler_t> try_from(const _U(&_v)[_N], size_t _n = 1)noexcept
		requires (iron::concepts::charset<_U>&& _N < max_size())
		{
			return try_unchecked_from(_v, _n, {});
		}
		template <typename _U, size_t _N>
		[[nodiscard]] static constexpr base_str unchecked_from(const _U(&_v)[_N], size_t _n, allocator_t _a)noexcept
			requires (iron::concepts::charset<_U>&& _N < max_size())
		{
			auto _r = try_unchecked_from(_v, _n, _a); RP(_r);
		}

		template <typename _U, size_t _N>
		[[nodiscard]] static constexpr base_str from(const _U(&_v)[_N], size_t _n = 1)noexcept 
		requires (iron::concepts::charset<_U> && _N < max_size())
		{
			return unchecked_from(_v, _n, {});
		}


		//from seq 
		template <typename _SEQ>
		[[nodiscard]] static constexpr iron::result<base_str, error_handler_t> try_from(const _SEQ& _seq, size_t _n=1)noexcept
			requires (iron::concepts::charset_sequence<_SEQ>)
		{
			IRON_SECURITY_CHECK_TEMPORARY(_seq);
			IRON_SECURITY_CHECK_MASTER_FOR_LIVE(_seq);
			auto _b = _seq.begin();
			return try_unchecked_from(_b, _seq.end(), _n, {});
		}
		template <typename _SEQ>
		[[nodiscard]] static constexpr base_str from(const _SEQ& _seq,size_t _n=1)noexcept 
			requires (iron::concepts::charset_sequence<_SEQ>)
		{
			auto _r = try_from(_seq,_n); RP(_r);
		}

		//from box_adapter

		[[nodiscard]] static constexpr result<base_str, error_handler> try_from(adapter_box_t&& _box)noexcept 
		{
			IRON_SECURITY_CHECK_TEMPORARY(_box);
			return _from(std::move(_box));
		}
		[[nodiscard]] static constexpr base_str from(adapter_box_t&& _box)noexcept {
			auto _r = try_from(std::move(_box)); RP(_r);
		}
		//constructors

		constexpr base_str()noexcept = default;
		constexpr base_str(base_str&& _v)noexcept 
		{
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_swap(_v);
			IRON_SECURITY_MARK_TEMPORARY(_v);
		}
		base_str(const base_str&) = delete;
		constexpr base_str& operator=(base_str&& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_swap(_v);
			IRON_SECURITY_MARK_TEMPORARY(_v);
			return *this;
		}
		base_str& operator=(const base_str&) = delete;
		constexpr ~base_str() = default;

		//misc 
		[[nodiscard]] constexpr size_t size()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _m_rs.size();
		}
		[[nodiscard]] constexpr size_t count()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _count();
		}
		[[nodiscard]] constexpr bool use_fixed()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return !_m_rs.use_heap();
		}

		[[nodiscard]] constexpr size_t capacity()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _m_rs.capacity();
		}
		[[nodiscard]] constexpr bool empty()const noexcept { return !size(); }
		[[nodiscard]] constexpr explicit operator bool()const noexcept { return !empty(); }
		[[nodiscard]] constexpr const value_t* unsafe_data()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _m_rs.data();
		}
		[[nodiscard]] constexpr const value_t* unsafe_cdata()const noexcept {
			return unsafe_data();
		}
		[[nodiscard]] constexpr value_t* unsafe_data()noexcept 
		requires(!is_safe())
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _m_rs.data();
		}

		constexpr base_str& swap(base_str& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_swap(_v);
			return *this;
		}
		constexpr base_str& clear()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			_m_rs.clear();
			return *this;
		}

		constexpr base_str& reset()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			auto _t = unchecked_from(_m_rs.allocator());
			_swap(_t);
			return *this;
		}

		//clone 
		[[nodiscard]] constexpr result<base_str,error_handler_t> try_clone(size_t _n = 1)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			auto _r = _m_rs.try_clone(_n); RR(_r);
			return iron::ok(base_str(std::move(_r.unchecked_unwrap())));
		}

		[[nodiscard]] constexpr base_str clone(size_t _n = 1)const noexcept {
			auto _r = try_clone(_n); RP(_r);
		}

		template <typename _T1, typename _A1, std::size_t _FIXED_SIZE1,bool _SAFE1>
		[[nodiscard]] static constexpr result<base_str, error_handler_t> try_clone_from(const base_str<_T1, _A1, _FIXED_SIZE1, _SAFE1>& _s,size_t _n=1)noexcept 
		{
			IRON_SECURITY_CHECK_TEMPORARY(_s);
			if constexpr (std::is_same_v<base_str, std::decay_t<decltype(_s)>>) {
				return _s.try_clone(_n);
			}
			else {
				auto _b = _s.unsafe_data();
				return base_str::try_unchecked_from(_b, _b + _s.size(), _n);
			}
		}

		template <typename _T1, typename _A1, std::size_t _FIXED_SIZE1,bool _SAFE1>
		[[nodiscard]] static constexpr base_str clone_from(const base_str<_T1, _A1, _FIXED_SIZE1, _SAFE1>& _s,size_t _n=1)noexcept 
		{
			auto _r = try_clone_from(_s, _n); RP(_r);
		}

		//reserve
		[[nodiscard]] constexpr error_handler_t try_reserve_at_least(size_t _additional, size_t _n = 1)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			constexpr bool _exact = {};
			return _reserve<_exact>(_n, _additional);
		}
		constexpr base_str& reserve_at_least(size_t _additional, size_t _n = 1)noexcept {
			auto _h = try_reserve_at_least(_additional, _n); RVP(_h,*this);
		}
		[[nodiscard]] constexpr error_handler_t try_reserve_exact(size_t _additional, size_t _n = 1)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			constexpr bool _exact = true;
			return _reserve<_exact>(_n, _additional);
		}
		constexpr base_str& reserve_exact(size_t _additional, size_t _n = 1)noexcept {
			auto _h = try_reserve_exact(_additional, _n); RVP(_h, *this);
		}

		//release 
		[[nodiscard]] constexpr iron::result<adapter_box_t, error_handler_t> try_release()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			auto _r = _m_rs.release(); RR(_r);
			return iron::ok(adapter_box_t::unchecked_from(std::move(_r.unchecked_unwrap())));
		}

		[[nodiscard]] constexpr adapter_box_t release()noexcept {
			auto _r = try_release(); RP(_r);
		}

		//get allocator
		[[nodiscard]] constexpr allocator_t unsafe_allocator()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _m_rs.allocator();
		}

		//shrink 
		[[nodiscard]] constexpr error_handler_t try_shrink_to_fit()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			return _m_rs.shrink_to_fit();
		}
		constexpr base_str& shrink_to_fit()noexcept {
			[[maybe_unused]] const auto _h = try_shrink_to_fit();
			//the error is ignored
			return *this;
		}

		//raw resize 
		[[nodiscard]] constexpr error_handler_t try_raw_resize(size_t _n, value_t _v = {})noexcept 
		requires (!is_safe())
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			if (_n >= max_size()) IRON_UNLIKELY
			{
				return ERROR_HANDLER(iron::unsafe::errors::posix::code_t::invalid_argument);
			}
			return _m_rs.raw_resize(_n, _v);
		}

		constexpr base_str& raw_resize(size_t _n, value_t _v = {})noexcept 
		requires (!is_safe())
		{
			const auto _h = try_raw_resize(_n, _v); RVP(_h, *this);
		}

		//chr resize
		[[nodiscard]] constexpr error_handler_t try_resize(size_t _n)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			if (_n >= max_size()) IRON_UNLIKELY
			{
				return ERROR_HANDLER(iron::unsafe::errors::posix::code_t::invalid_argument);
			}
			constexpr char_t _v{};
			return _chr_resize(_n, _v);
		}
		constexpr base_str& resize(size_t _n)noexcept {
			const auto _h = try_resize(_n); RVP(_h, *this);
		}

		template <typename _U>
		[[nodiscard]] constexpr error_handler_t try_resize(size_t _n,_U _v)noexcept 
		requires iron::concepts::fixed_charset<_U>
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			if (_n >= max_size()) IRON_UNLIKELY
			{
				return ERROR_HANDLER(iron::unsafe::errors::posix::code_t::invalid_argument);
			}
			return _chr_resize(_n, _v);
		}

		template <typename _U>
		constexpr base_str& resize(size_t _n,_U _v)noexcept 
		requires iron::concepts::fixed_charset<_U>
		{
			const auto _h = try_resize(_n,_v); RVP(_h, *this);
		}
		[[nodiscard]] constexpr error_handler_t try_realloc_on_heap()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			return _m_rs.realloc_on_heap(); 
		}
		base_str& realloc_on_heap()noexcept {
			auto _h = try_realloc_on_heap(); RVP(_h,*this);
		}
		[[nodiscard]] constexpr value_t& unchecked_raw_nth(size_t _idx)noexcept 
		requires(!is_safe())
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			DEBUG_ASSERT(_idx < size());
			return _m_rs.data()[_idx];
		}

		[[nodiscard]] constexpr value_t unchecked_raw_nth(size_t _idx)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			DEBUG_ASSERT(_idx < size());
			return _m_rs.data()[_idx];
		}

		[[nodiscard]] constexpr iron::option<value_t> raw_nth(size_t _idx)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _get_raw(_idx);
		}

		[[nodiscard]] constexpr iron::option<value_t> raw_front()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _get_raw(0);
		}
		[[nodiscard]] constexpr iron::option<value_t> raw_back()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _get_raw_back();
		}

		[[nodiscard]] constexpr iron::option<char_t> nth(size_t _idx)const noexcept{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			if constexpr (iron::concepts::fixed_charset<value_t>) {
				return _get_raw(_idx);
			}
			else {
				return _get_char(_idx);
			}
		}

		[[nodiscard]] constexpr iron::option<char_t> front()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			constexpr size_t _idx = {};
			if constexpr (iron::concepts::fixed_charset<value_t>) {
				return _get_raw(_idx);
			}
			else {
				return _get_char(_idx);
			}
		}
		[[nodiscard]] constexpr iron::option<char_t> back()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			if constexpr (iron::concepts::fixed_charset<value_t>) {
				return _get_raw_back();
			}
			else {
				return _get_char_back();
			}
		}


		//make view
		[[nodiscard]] constexpr cview_t view()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _view();
		}
		[[nodiscard]] constexpr cview_t cview()const noexcept { return view(); }

		[[nodiscard]] constexpr view_t view()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _view();
		}

		[[nodiscard]] constexpr raw_seq_t raw_seq()noexcept 
			requires (!is_safe())
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return view().raw_seq();
		}
		[[nodiscard]] constexpr craw_seq_t raw_seq()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return view().raw_seq();
		}

		[[nodiscard]] constexpr craw_seq_t craw_seq()const noexcept { return raw_seq(); }


		[[nodiscard]] constexpr rev_raw_seq_t raw_rseq()noexcept
			requires (!is_safe())
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return view().raw_rseq();
		}
		[[nodiscard]] constexpr crev_raw_seq_t raw_rseq()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return view().raw_rseq();
		}
		[[nodiscard]] constexpr crev_raw_seq_t craw_rseq()const noexcept { return raw_rseq(); }

		[[nodiscard]] constexpr cseq_t seq()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return view().seq();
		}
		[[nodiscard]] constexpr cseq_t cseq()const noexcept {
			return seq();
		}

		[[nodiscard]] constexpr seq_t seq()noexcept 
			requires (!is_safe())
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return view().seq();
		}

		[[nodiscard]] constexpr rev_raw_seq_t rseq()noexcept
		requires (!is_safe() && iron::concepts::fixed_charset<value_t>)
		{ 
			return raw_rseq(); 
		}

		[[nodiscard]] constexpr craw_seq_t rseq()const noexcept
			requires (iron::concepts::fixed_charset<value_t>)
		{ 
			return raw_rseq(); 
		}

		[[nodiscard]] constexpr craw_seq_t crseq()const noexcept
			requires (iron::concepts::fixed_charset<value_t>)
		{ 
			return rseq(); 
		}

		template <typename _U>
		[[nodiscard]] constexpr error_handler_t try_push_back(_U _v,size_t _n=1)noexcept 
		requires (iron::concepts::fixed_charset<_U>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			return _push_value(_n, _v);
		}

		template <typename _U>
		constexpr base_str& push_back(_U _v,size_t _n=1)noexcept 
		requires (iron::concepts::fixed_charset<_U>)
		{
			const auto _h = try_push_back(_v,_n); RVP(_h, *this);
		}

		//push back iter 

		template <typename _ITER>
		[[nodiscard]] constexpr error_handler_t try_unchecked_push_back(_ITER& _b, const _ITER& _e, size_t _n=1)noexcept 
		requires (iron::concepts::charset_forward_iterator<_ITER> && !std::is_const_v<_ITER>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			return _push_iter(_n, _b, _e);
		}

		template <typename _ITER>
		constexpr base_str& unchecked_push_back(_ITER& _b, const _ITER& _e, size_t _n=1)noexcept 
		requires (iron::concepts::charset_forward_iterator<_ITER> && !std::is_const_v<_ITER>)
		{
			const auto _h = try_unchecked_push_back(_b, _e,_n); RVP(_h, *this);
		}


		//push back seq 
		template <typename _SEQ>
		[[nodiscard]] constexpr error_handler_t try_push_back(_SEQ& _seq, size_t _n=1)noexcept 
		requires (iron::concepts::charset_sequence<_SEQ> && !std::is_const_v<_SEQ>)
		{
			auto& _b = _seq.begin();
			const auto _e = _seq.end();
			return try_unchecked_push_back(_b, _e, _n);
		}

		template <typename _SEQ>
		[[nodiscard]] constexpr base_str& push_back(_SEQ& _seq,size_t _n=1)noexcept 
			requires (iron::concepts::charset_sequence<_SEQ> && !std::is_const_v<_SEQ>)
		{
			const auto _h = try_push_back(_seq,_n); RVP(_h, *this);
		}


		//push back from array

		template <typename _U, size_t _N>
		[[nodiscard]] constexpr error_handler_t try_push_back(const _U(&_v)[_N], size_t _n=1)noexcept
			requires(iron::concepts::charset<_U> && _N < max_size())
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			const auto _sz = _raw_str_t::array_size(_v);
			auto _p = &_v[0];
			return _push_iter(_n, _p, _p + _sz);
		}

		template <typename _U, size_t _N>
		constexpr base_str& push_back(const _U(&_v)[_N], size_t _n = 1)noexcept 
			requires(iron::concepts::charset<_U> && _N < max_size())
		{
			const auto _h = try_push_back(_v, _n); RVP(_h, *this);
		}
		
		
		//upper 

		[[nodiscard]] constexpr iron::result<base_str, error_handler_t> try_to_upper()const noexcept 
		requires(iron::concepts::basic_charset<value_t>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _to_upper();
		}

		[[nodiscard]] iron::result<base_str, error_handler_t> try_to_upper()const noexcept
		requires(!iron::concepts::basic_charset<value_t>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _to_upper();
		}

		[[nodiscard]] constexpr base_str to_upper()const noexcept
		requires(iron::concepts::basic_charset<value_t>)
		{
			auto _r = try_to_upper(); RP(_r);
		}

		[[nodiscard]] base_str to_upper()const noexcept
			requires(!iron::concepts::basic_charset<value_t>)
		{
			auto _r = try_to_upper(); RP(_r);
		}

		//lower 

		[[nodiscard]] constexpr iron::result<base_str, error_handler_t> try_to_lower()const noexcept
		requires(iron::concepts::basic_charset<value_t>)

		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _to_lower();
		}
		[[nodiscard]] iron::result<base_str, error_handler_t> try_to_lower()const noexcept
		requires(!iron::concepts::basic_charset<value_t>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _to_lower();
		}

		[[nodiscard]] constexpr base_str to_lower()const noexcept
		requires(iron::concepts::basic_charset<value_t>)
		{
			auto _r = try_to_lower(); RP(_r);
		}
		[[nodiscard]] base_str to_lower()const noexcept
			requires(!iron::concepts::basic_charset<value_t>)
		{
			auto _r = try_to_lower(); RP(_r);
		}

		//equal

		template <typename _T1, typename _A1, std::size_t _FIXED_SIZE1, bool _SAFE1>
		[[nodiscard]] constexpr bool operator==(const base_str<_T1, _A1, _FIXED_SIZE1, _SAFE1>& _v)const noexcept
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			return _m_rs == _v._m_rs;
		}
		template <typename _U, std::size_t _N>
		[[nodiscard]] constexpr bool operator==(const _U(&_v)[_N])const noexcept
		requires(iron::concepts::charset<_U> && _N < max_size())
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _m_rs == _v;
		}
		// less 
		template <typename _T1, typename _A1, std::size_t _FIXED_SIZE1, bool _SAFE1>
		[[nodiscard]] constexpr bool operator <(const base_str<_T1, _A1, _FIXED_SIZE1, _SAFE1>& _v)const noexcept
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			return _m_rs < _v._m_rs;
		}

		template <typename _U, std::size_t _N>
		[[nodiscard]] constexpr bool operator <(const _U(&_v)[_N])const noexcept
		requires(iron::concepts::charset<_U>&& _N < max_size())
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _m_rs < _v;
		}
		template <typename _T1, typename _A1, std::size_t _FIXED_SIZE1, bool _SAFE1>
		[[nodiscard]] constexpr std::strong_ordering operator <=>(const base_str<_T1, _A1, _FIXED_SIZE1, _SAFE1>& _v)const noexcept 
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			return _m_rs <=> _v._m_rs;
		}
		template <typename _U, std::size_t _N>
		[[nodiscard]] constexpr std::strong_ordering operator <=>(const _U(&_v)[_N])const noexcept
		requires(iron::concepts::charset<_U> && _N < max_size())
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _m_rs <=> _v;
		}

		IRON_SECURITY_METHOD()
	private:
		template <typename _T1, typename _A1, std::size_t _FIXED_SIZE1, bool _SAFE1>
		friend class base_str;
		explicit constexpr base_str(const allocator_t& _a)noexcept
			: _m_rs(_a)
		{
		}
		constexpr explicit base_str(_raw_str_t&& _v)noexcept
			: _m_rs(std::move(_v))
		{}
		constexpr void _swap(base_str& _v)noexcept {
			_m_rs.swap(_v._m_rs);
			IRON_SECURITY_SWAP(_v);
		}
		[[nodiscard]] static constexpr iron::result<base_str, error_handler_t> _from(adapter_box_t&& _box)noexcept {
			auto _t = unchecked_from(_box.unsafe_allocator());
			if (_box.size()) IRON_LIKELY
			{
				auto _r = _raw_str_t::from(_box.unsafe_release()); RR(_r);
				_t = base_str(std::move(_r.unchecked_unwrap()));
			}
			return iron::ok(std::move(_t));
		}
		template <bool _EXACT>
		[[maybe_unused]] constexpr error_handler_t _reserve(size_t _n, size_t _additional)noexcept {
			size_t _newc{};
			auto _h = _m_rs.template check_size<_EXACT>(_newc, _n, _additional); RE(_h);
			if (_m_rs.capacity() < _newc) IRON_LIKELY
			{
				return _m_rs.reserve(_newc);
			}
			return {};
		}

#define _IRON_CHECK_SAFE(_b) \
		do { \
			if constexpr (is_safe()) { \
				if (!_b) IRON_UNLIKELY \
				{ \
					return ERROR_HANDLER(iron::unsafe::errors::posix::code_t::illegal_byte_sequence); \
				} \
			} \
		} while(0)

		template <typename _U>
		[[nodiscard]] constexpr error_handler_t _push_value(const size_t _n, _U _u)noexcept {
			if (_n) IRON_LIKELY
			{
				if constexpr (std::is_same_v<value_t,std::decay_t<_U>>) {
					if constexpr (is_safe()) {
						value_t* _pd{};
						size_t _sz{};
						const value_t* _ps = &_u;
						const auto _r = _common_t:: template copy<true>(_pd, _sz, _ps, _ps + 1);
						_IRON_CHECK_SAFE(_r);
					}
					size_t _sz{};
					constexpr bool _exact = true;
					auto _h = _m_rs.template check_size<_exact>(_sz, _n, 1); RE(_h);
					if (_m_rs.capacity() < _sz) IRON_UNLIKELY
					{
						_h = _m_rs.reserve(_sz); RE(_h);
					}
					constexpr bool _reserve = false;
					return _m_rs.template push_back<_reserve>(_n,_u);
				}
				else {
					value_t _buff[5];
					auto _sz = sizeof(_buff);
					auto _pd = &(_buff[0]);
					auto _ps = &_u;
					[[maybe_unused]] const auto _r = _common_t:: template copy<is_safe()>(_pd, _sz, _ps, _ps + 1);
					DEBUG_ASSERT(_sz <= sizeof(_buff));
					_IRON_CHECK_SAFE(_r);
					return _push_iter_same(_n, _pd, _pd + _sz);
				}
			}
			return {};
		}
		template <typename _ITER>
		[[nodiscard]] constexpr error_handler_t _push_iter_same(const size_t _n, _ITER& _b, const _ITER& _e)noexcept 
		requires (std::contiguous_iterator<_ITER> && std::is_same_v<value_t, std::decay_t<decltype(*_b)>>)
		{
			DEBUG_ASSERT(_b != _e);
			const size_t _sz = static_cast<size_t>(std::distance(_b, _e));
			constexpr bool _exact = true;
			size_t _newc{};
			auto _h = _m_rs.template check_size<_exact>(_newc, _n, _sz); RE(_h);
			if (_m_rs.capacity() < _newc) IRON_UNLIKELY
			{
				_h = _m_rs.reserve(_newc); RE(_h);
			}
			constexpr bool _reserve = false;
			return _m_rs.template push_back<_reserve>(_n, _b, _e);
		}
		template <typename _ITER>
		[[nodiscard]] constexpr error_handler_t _push_iter(const size_t _n,_ITER& _b, const _ITER& _e)noexcept {
			if (_n && _b != _e) IRON_LIKELY
			{
				using _v_t = std::decay_t<decltype(*_b)>;
				if constexpr (std::is_same_v<value_t, _v_t>) {
					if constexpr (std::contiguous_iterator<_ITER>) {
						return _push_iter_same(_n, _b, _e);
					}
					else {
						constexpr bool _reserve = true;
						return _m_rs.template push_back<_reserve>(_n,_b, _e);
					}
				}
				else {
					size_t _sz{};
					value_t* _pd{};
					if constexpr (
						iron::concepts::fixed_charset<value_t>
						&& iron::concepts::fixed_charset<_v_t>
						&& !is_safe()
						&& std::contiguous_iterator<_ITER>
					)
					{
						_sz = static_cast<size_t>(std::distance(_b, _e));
					}
					else {
						auto _bb = _b;
						const auto _r = _common_t::template copy<is_safe()>(_pd, _sz, _bb, _e);
						_IRON_CHECK_SAFE(_r);
					}
					if (_sz) IRON_LIKELY 
					{
						auto _h = _rs_t::allocate(_pd, _sz, _m_rs.allocator()); RE(_h);
						if (std::is_constant_evaluated()) {
							_h = _rs_t::uninitialized_fill(_pd,_sz, value_t{});
							DEBUG_ASSERT(_h);
						}
						[[maybe_unused]] const auto _r = _common_t::template copy<false>(_pd, _sz, _b, _e);
						DEBUG_ASSERT(_r);
						_h = _push_iter_same(_n, _pd, _pd + _sz);
						if (std::is_constant_evaluated()) {
							_rs_t::destroy_n(_pd,_sz);
						}
						_rs_t::deallocate(_pd, _sz, _m_rs.allocator());
						return _h;
					}
				}
			}
			return {};
		}
#undef _IRON_CHECK_SAFE

		template <typename _U>
		[[nodiscard]] constexpr error_handler_t _chr_resize(size_t _n,_U _v)noexcept {
			const auto _c = this->cview().count();
			if (_c != _n) IRON_LIKELY
			{
				if (_c > _n) {
					auto _s = seq().slice(0, _n);
					auto _t = unchecked_from(_m_rs.allocator());
					for (const auto _ch : _s) IRON_LIKELY
					{
						auto _h = _t.try_push_back(_ch); RE(_h);
					}
					_swap(_t);
				}
				else {
					auto _h = try_push_back(_v, _n - _c); RE(_h);
				}
			}
			return {};
		}
		[[nodiscard]] constexpr iron::option<value_t> _get_raw(size_t _idx)const noexcept {
			return _common_t::get_raw(_m_rs.data(), _m_rs.size(), _idx);
		}
		[[nodiscard]] constexpr iron::option<char_t> _get_char(size_t _idx)const noexcept 
		requires(!iron::concepts::fixed_charset<value_t>)
		{
			return _common_t::get_char(_m_rs.data(), _m_rs.size(), _idx);
		}
		[[nodiscard]] constexpr iron::option<value_t> _get_raw_back()const noexcept 
		{
			return _common_t::get_raw_back(_m_rs.data(), _m_rs.size());
		}
		[[nodiscard]] constexpr iron::option<char_t> _get_char_back()const noexcept 
			requires(!iron::concepts::fixed_charset<value_t>)
		{
			return _common_t::get_char_back(_m_rs.data(), _m_rs.size());
		}
		[[nodiscard]] constexpr iron::result<base_str, error_handler_t> _to_upper()const noexcept {
			auto _r = _m_rs.to_upper(); RR(_r);
			auto _t = unchecked_from(_m_rs.allocator());
			_t._m_rs.swap(_r.unchecked_unwrap());
			return iron::ok(std::move(_t));
		}
		[[nodiscard]] constexpr iron::result<base_str, error_handler_t> _to_lower()const noexcept {
			auto _r = _m_rs.to_lower(); RR(_r);
			auto _t = unchecked_from(_m_rs.allocator());
			_t._m_rs.swap(_r.unchecked_unwrap());
			return iron::ok(std::move(_t));
		}
		[[nodiscard]] constexpr cview_t _view()const noexcept {
			return cview_t::unchecked_from(*this, _m_rs.data(), _m_rs.size());
		}
		[[nodiscard]] constexpr view_t _view()noexcept {
			return view_t::unchecked_from(*this, _m_rs.data(), _m_rs.size());
		}
		[[nodiscard]] constexpr size_t _count()noexcept {
			if constexpr (iron::concepts::fixed_charset<value_t>) {
				return _m_rs.size();
			}
			else {
				return _view().count();
			}
		}
		_raw_str_t _m_rs;
		IRON_SECURITY_DCL();
	};

	namespace literals {
		namespace _impl {
			template <typename _S,typename _C>
			inline constexpr _S _make(_C _v, std::size_t _len)noexcept {
				DEBUG_ASSERT(_v);
				auto _r = _S::try_unchecked_from(_v, _v + _len); RP(_r);
			}
		}
		inline constexpr ustrb operator""_ub(const char* _v, std::size_t _len)noexcept {
			return _impl::_make<ustrb>(_v, _len);
		}
		inline constexpr strb operator""_sb(const char* _v, std::size_t _len)noexcept {
			return _impl::_make<strb>(_v, _len);
		}
		inline constexpr ustr8 operator""_u8(const char* _v, std::size_t _len)noexcept {
			return _impl::_make<ustr8>(_v, _len);
		}
		inline constexpr ustr8 operator""_u8(const char8_t* _v, std::size_t _len)noexcept {
			return _impl::_make<ustr8>(_v, _len);
		}
		inline constexpr ustr8 operator""_u8(const wchar_t* _v, std::size_t _len)noexcept {
			return _impl::_make<ustr8>(_v, _len);
		}
		inline constexpr ustr8 operator""_u8(const char16_t* _v, std::size_t _len)noexcept {
			return _impl::_make<ustr8>(_v, _len);
		}
		inline constexpr ustr8 operator""_u8(const char32_t* _v, std::size_t _len)noexcept {
			return _impl::_make<ustr8>(_v, _len);
		}
		inline constexpr str8 operator""_s8(const char* _v, std::size_t _len)noexcept {
			return _impl::_make<str8>(_v, _len);
		}
		inline constexpr str8 operator""_s8(const char8_t* _v, std::size_t _len)noexcept {
			return _impl::_make<str8>(_v, _len);
		}
		inline constexpr str8 operator""_s8(const wchar_t* _v, std::size_t _len)noexcept {
			return _impl::_make<str8>(_v, _len);
		}
		inline constexpr str8 operator""_s8(const char16_t* _v, std::size_t _len)noexcept {
			return _impl::_make<str8>(_v, _len);
		}
		inline constexpr str8 operator""_s8(const char32_t* _v, std::size_t _len)noexcept {
			return _impl::_make<str8>(_v, _len);
		}
		inline constexpr ustr16 operator""_u16(const char* _v, std::size_t _len)noexcept {
			return _impl::_make<ustr16>(_v, _len);
		}
		inline constexpr ustr16 operator""_u16(const char8_t* _v, std::size_t _len)noexcept {
			return _impl::_make<ustr16>(_v, _len);
		}
		inline constexpr ustr16 operator""_u16(const wchar_t* _v, std::size_t _len)noexcept {
			return _impl::_make<ustr16>(_v, _len);
		}
		inline constexpr ustr16 operator""_u16(const char16_t* _v, std::size_t _len)noexcept {
			return _impl::_make<ustr16>(_v, _len);
		}
		inline constexpr ustr16 operator""_u16(const char32_t* _v, std::size_t _len)noexcept {
			return _impl::_make<ustr16>(_v, _len);
		}
		inline constexpr str16 operator""_s16(const char* _v, std::size_t _len)noexcept {
			return _impl::_make<str16>(_v, _len);
		}
		inline constexpr str16 operator""_s16(const char8_t* _v, std::size_t _len)noexcept {
			return _impl::_make<str16>(_v, _len);
		}
		inline constexpr str16 operator""_s16(const wchar_t* _v, std::size_t _len)noexcept {
			return _impl::_make<str16>(_v, _len);
		}
		inline constexpr str16 operator""_s16(const char16_t* _v, std::size_t _len)noexcept {
			return _impl::_make<str16>(_v, _len);
		}
		inline constexpr str16 operator""_s16(const char32_t* _v, std::size_t _len)noexcept {
			return _impl::_make<str16>(_v, _len);
		}
		inline constexpr ustrw operator""_uw(const char* _v, std::size_t _len)noexcept {
			return _impl::_make<ustrw>(_v, _len);
		}
		inline constexpr ustrw operator""_uw(const char8_t* _v, std::size_t _len)noexcept {
			return _impl::_make<ustrw>(_v, _len);
		}
		inline constexpr ustrw operator""_uw(const wchar_t* _v, std::size_t _len)noexcept {
			return _impl::_make<ustrw>(_v, _len);
		}
		inline constexpr ustrw operator""_uw(const char16_t* _v, std::size_t _len)noexcept {
			return _impl::_make<ustrw>(_v, _len);
		}
		inline constexpr ustrw operator""_uw(const char32_t* _v, std::size_t _len)noexcept {
			return _impl::_make<ustrw>(_v, _len);
		}
		inline constexpr strw operator""_sw(const char* _v, std::size_t _len)noexcept {
			return _impl::_make<strw>(_v, _len);
		}
		inline constexpr strw operator""_sw(const char8_t* _v, std::size_t _len)noexcept {
			return _impl::_make<strw>(_v, _len);
		}
		inline constexpr strw operator""_sw(const wchar_t* _v, std::size_t _len)noexcept {
			return _impl::_make<strw>(_v, _len);
		}
		inline constexpr strw operator""_sw(const char16_t* _v, std::size_t _len)noexcept {
			return _impl::_make<strw>(_v, _len);
		}
		inline constexpr strw operator""_sw(const char32_t* _v, std::size_t _len)noexcept {
			return _impl::_make<strw>(_v, _len);
		}
		inline constexpr ustr32 operator""_u32(const char* _v, std::size_t _len)noexcept {
			return _impl::_make<ustr32>(_v, _len);
		}
		inline constexpr ustr32 operator""_u32(const char8_t* _v, std::size_t _len)noexcept {
			return _impl::_make<ustr32>(_v, _len);
		}
		inline constexpr ustr32 operator""_u32(const wchar_t* _v, std::size_t _len)noexcept {
			return _impl::_make<ustr32>(_v, _len);
		}
		inline constexpr ustr32 operator""_u32(const char16_t* _v, std::size_t _len)noexcept {
			return _impl::_make<ustr32>(_v, _len);
		}
		inline constexpr ustr32 operator""_u32(const char32_t* _v, std::size_t _len)noexcept {
			return _impl::_make<ustr32>(_v, _len);
		}
		inline constexpr str32 operator""_s32(const char* _v, std::size_t _len)noexcept {
			return _impl::_make<str32>(_v, _len);
		}
		inline constexpr str32 operator""_s32(const char8_t* _v, std::size_t _len)noexcept {
			return _impl::_make<str32>(_v, _len);
		}
		inline constexpr str32 operator""_s32(const wchar_t* _v, std::size_t _len)noexcept {
			return _impl::_make<str32>(_v, _len);
		}
		inline constexpr str32 operator""_s32(const char16_t* _v, std::size_t _len)noexcept {
			return _impl::_make<str32>(_v, _len);
		}
		inline constexpr str32 operator""_s32(const char32_t* _v, std::size_t _len)noexcept {
			return _impl::_make<str32>(_v, _len);
		}

	}

} //ns

#if defined(_MSC_VER)
#pragma warning(pop)
#endif
