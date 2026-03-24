

namespace iron {

	template <typename _T,bool _SAFE>
	class base_strview final {
	public:
		using value_t = _T;
		using value_type = value_t;
		using stripped_value_t = std::decay_t<value_t>;
		using stripped_value_type = stripped_value_t;
		using size_t = std::size_t;
		using size_type = size_t;
		using diff_t = std::ptrdiff_t;
		using difference_type_t = diff_t;
	private:
		using _raw_strview_t = iron::unsafe::str::raw_strview<value_t, _SAFE>;
	public:
		static_assert(!std::is_reference_v<value_t>, "stringview template parameter value_type  must not be a reference type");
		static_assert(iron::concepts::charset<value_t>, "stringview template parameter value_type must be character");
		[[nodiscard]] static constexpr bool is_safe()noexcept { return _SAFE; }
		[[nodiscard]] static constexpr size_t max_size()noexcept { return _raw_strview_t::max_size(); }
		struct str_view_tag { str_view_tag() = delete; };
		using char_t = typename _raw_strview_t::char_t;
		using char_type = char_t;
		using stripped_char_t = std::decay_t<char_t>;
		using stripped_char_type = stripped_char_t;
	private:
		using _raw_iter_t = typename _raw_strview_t::raw_iter_t;
		using _rev_raw_iter_t = typename _raw_strview_t::rev_raw_iter_t;
		using _const_raw_iter_t = typename _raw_strview_t::const_raw_iter_t;
		using _const_rev_raw_iter_t = typename _raw_strview_t::const_rev_raw_iter_t;
		using _iter_t = typename _raw_strview_t::iter_t;
		using _const_iter_t = typename _raw_strview_t::const_iter_t;
		using _common_t = iron::unsafe::str::common;
	public:
		using ptr_t = value_t*;
		using cptr_t = const value_t*;
		using pointer = ptr_t;
		using const_pointer = cptr_t;
		using ref_t = iron::ref_wrapper<value_t>;
		using cref_t = iron::ref_wrapper<const value_t>;
		using reference = value_t&;
		using const_reference = const value_t&;
		using iterator = _iter_t;
		using const_iterator = _const_iter_t;
		using error_handler_t = iron::error_handler;
		using error_handler_type = error_handler_t;
		using raw_seq_t = iron::sequence<_raw_iter_t>;
		using raw_seq_type = raw_seq_t;
		using craw_seq_t = iron::sequence<_const_raw_iter_t>;
		using const_raw_seq_type = craw_seq_t;
		using rev_raw_seq_t = iron::sequence<_rev_raw_iter_t>;
		using rev_raw_seq_type = rev_raw_seq_t;
		using crev_raw_seq_t = iron::sequence<_const_rev_raw_iter_t>;
		using const_rev_raw_seq_type = crev_raw_seq_t;
		using seq_t = iron::sequence<_iter_t>;
		using seq_type = seq_t;
		using cseq_t = iron::sequence<_const_iter_t>;
		using const_seq_type = cseq_t;
		template <typename _MASTER>
		[[nodiscard]] static constexpr base_strview unchecked_from(const _MASTER& _master,ptr_t _v, size_t _sz)noexcept 
		IRON_SECURITY_REQ()
		{
			IRON_SECURITY_CHECK_TEMPORARY(_master);
			IRON_SECURITY_CHECK_MASTER_LIVE(_master);
			base_strview _t;
			_t._m_rs = _raw_strview_t(_v, _sz);
			IRON_SECURITY_MAKE_REF_TO(_t, _master);
			_t._reset_if();
			return _t;
		}
		constexpr base_strview& reset()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			_m_rs.reset();
			IRON_SECURITY_RESET_THIS();
			return *this;
		}

		constexpr base_strview()noexcept = default;
		constexpr base_strview(base_strview&& _v)noexcept
		{
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			IRON_SECURITY_CHECK_MASTER_LIVE(_v);
			_swap(_v);
			IRON_SECURITY_MARK_TEMPORARY(_v);
		}
		constexpr base_strview(const base_strview& _v)noexcept
			: _m_rs(_v._m_rs)
#if IRON_SECURITY_CHECKER
			, _m_ck(_v._m_ck)
#endif
		{
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			IRON_SECURITY_CHECK_MASTER_LIVE(_v);
		}
		constexpr base_strview& operator=(base_strview&& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			IRON_SECURITY_CHECK_MASTER_LIVE(_v);
			_swap(_v);
			IRON_SECURITY_MARK_TEMPORARY(_v);
			return *this;
		}
		constexpr base_strview& operator=(const base_strview& _v)noexcept {
			base_strview _t(_v);
			_swap(_t);
			return *this;
		}
		constexpr ~base_strview() = default;

		template <typename _T1, bool _SAFE1>
		[[nodiscard]] constexpr bool operator==(const base_strview<_T1, _SAFE1>& _v)const noexcept 
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			IRON_SECURITY_CHECK_MASTER_LIVE(_v);
			return  _m_rs == _v._m_rs;
		}
		template <typename _U, std::size_t _N>
		[[nodiscard]] constexpr bool operator==(const _U(&_v)[_N])const noexcept
			requires(iron::concepts::charset<_U> && _N < max_size())
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			return _m_rs == _v;
		}

		template <typename _T1, bool _SAFE1>
		[[nodiscard]] constexpr bool operator <(const base_strview<_T1, _SAFE1>& _v)const noexcept
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			IRON_SECURITY_CHECK_MASTER_LIVE(_v);
			return  _m_rs < _v._m_rs;
		}
		template <typename _U, std::size_t _N>
		[[nodiscard]] constexpr bool operator <(const _U(&_v)[_N])const noexcept
			requires(iron::concepts::charset<_U> && _N < max_size())
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			return _m_rs < _v;
		}
		template <typename _T1, bool _SAFE1>
		[[nodiscard]] constexpr std::strong_ordering operator <=>(const base_strview<_T1, _SAFE1>& _v)const noexcept 
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			IRON_SECURITY_CHECK_MASTER_LIVE(_v);
			return _m_rs <=> _v._m_rs;
		}
		template <typename _U, std::size_t _N>
		[[nodiscard]] constexpr std::strong_ordering operator <=>(const _U(&_v)[_N])const noexcept
			requires(iron::concepts::charset<_U> && _N < max_size())
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			return _m_rs <=> _v;
		}

		[[nodiscard]] constexpr size_t size()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			return _m_rs.size();
		}

		[[nodiscard]] constexpr size_t count()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			if constexpr (iron::concepts::varying_charset<value_t>) {
				return seq().count();
			}
			else {
				return size();
			}
		}
		[[nodiscard]] constexpr explicit operator bool()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			return _m_rs.size() ? true : false;
		}
		[[nodiscard]] constexpr iron::option<size_t> raw_index(size_t _ch_index)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			return _m_rs.raw_index(_ch_index);
		}
		[[nodiscard]] constexpr bool empty()const noexcept { return !size(); }

		[[nodiscard]] constexpr bool is_chr_boundary(size_t _idx)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			return _m_rs.is_chr_boundary(_idx);
		}
		[[nodiscard]] constexpr cptr_t unsafe_data()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			return _m_rs.data();
		}
		[[nodiscard]] constexpr const cptr_t unsafe_cdata()const noexcept {
			return unsafe_data();
		}
		[[nodiscard]] constexpr ptr_t unsafe_data()noexcept
			requires(!is_safe())
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _m_rs.data();
		}

		constexpr base_strview& swap(base_strview& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			IRON_SECURITY_CHECK_MASTER_LIVE(_v);
			_swap(_v);
			return *this;
		}
		[[nodiscard]] constexpr value_t& unchecked_raw_nth(size_t _idx)noexcept 
		requires(!is_safe())
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			DEBUG_ASSERT(_idx < size());
			return _m_rs.data()[_idx];
		}

		[[nodiscard]] constexpr stripped_value_t unchecked_raw_nth(size_t _idx)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			DEBUG_ASSERT(_idx < size());
			return _m_rs.data()[_idx];
		}
		[[nodiscard]] constexpr iron::option<stripped_value_t> raw_nth(size_t _idx)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			return _get_raw(_idx);
		}
		[[nodiscard]] constexpr iron::option<stripped_value_t> raw_front()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			return _get_raw(0);
		}
		[[nodiscard]] constexpr iron::option<stripped_value_t> raw_back()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			return _get_raw_back();
		}
		[[nodiscard]] constexpr iron::option<stripped_char_t> nth(size_t _idx)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			if constexpr (iron::concepts::fixed_charset<value_t>) {
				return _get_raw(_idx);
			}
			else {
				return _get_char(_idx);
			}
		}
		[[nodiscard]] constexpr iron::option<stripped_char_t> front()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			constexpr size_t _idx = {};
			if constexpr (iron::concepts::fixed_charset<value_t>) {
				return _get_raw(_idx);
			}
			else {
				return _get_char(_idx);
			}
		}
		[[nodiscard]] constexpr iron::option<stripped_char_t> back()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			if constexpr (iron::concepts::fixed_charset<value_t>) {
				return _get_raw_back();
			}
			else {
				return _get_char_back();
			}
		}
		[[nodiscard]] constexpr raw_seq_t raw_seq()noexcept
			requires (!is_safe())
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			return raw_seq_t::unchecked_from(*this, _m_rs.raw_begin(), _m_rs.raw_end());
		}
		[[nodiscard]] constexpr craw_seq_t raw_seq()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			return craw_seq_t::unchecked_from(*this, _m_rs.raw_begin(), _m_rs.raw_end());
		}
		[[nodiscard]] constexpr craw_seq_t craw_seq()const noexcept { 
			return raw_seq(); 
		}
		[[nodiscard]] constexpr rev_raw_seq_t raw_rseq()noexcept
			requires(!is_safe())
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			return rev_raw_seq_t::unchecked_from(*this, _m_rs.rev_raw_begin(), _m_rs.rev_raw_end());
		}

		[[nodiscard]] constexpr crev_raw_seq_t raw_rseq()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			return crev_raw_seq_t::unchecked_from(*this, _m_rs.rev_raw_begin(), _m_rs.rev_raw_end());
		}
		[[nodiscard]] constexpr crev_raw_seq_t craw_rseq()const noexcept { 
			return raw_rseq(); 
		}
		[[nodiscard]] constexpr cseq_t seq()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			return cseq_t::unchecked_from(*this, _m_rs.begin(), _m_rs.end());
		}
		[[nodiscard]] constexpr seq_t seq()noexcept
			requires (!is_safe())
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			return seq_t::unchecked_from(*this, _m_rs.begin(), _m_rs.end());
		}
		[[nodiscard]] constexpr cseq_t cseq()const noexcept { 
			return seq(); 
		}
		[[nodiscard]] constexpr crev_raw_seq_t rseq()const noexcept
			requires (iron::concepts::fixed_charset<value_t>)
		{
			return raw_seq();
		}
		[[nodiscard]] constexpr rev_raw_seq_t rseq()noexcept
			requires (!is_safe() && iron::concepts::fixed_charset<value_t>)
		{
			return raw_seq();
		}
		[[nodiscard]] constexpr crev_raw_seq_t crseq()const noexcept
			requires (iron::concepts::fixed_charset<value_t>)
		{
			return rseq();
		}
		constexpr base_strview& make_base_uppercase()noexcept 
		requires(!std::is_const_v<value_t>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			_m_rs.template make_transform<_raw_strview_t::transform_t::ucase>();
			return *this;
		}
		constexpr base_strview& make_base_lowercase()noexcept
			requires(!std::is_const_v<value_t>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			_m_rs.template make_transform<_raw_strview_t::transform_t::lcase>();
			return *this;
		}
		[[nodiscard]] constexpr base_strview slice(size_t _start, size_t _len=max_size())const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			return _slice(*this, _start, _len);
		}
		template <typename _F>
		[[nodiscard]] constexpr base_strview ltrim_if(_F _f)const noexcept
			requires (iron::concepts::bool_predicate<_F, stripped_char_t>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			return _ltrim_if(*this, _f);
		}
		[[nodiscard]] constexpr base_strview ltrim()const noexcept
			requires(iron::concepts::basic_charset<stripped_char_t>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			return _ltrim(*this);
		}
		[[nodiscard]] base_strview ltrim()const noexcept
			requires(!iron::concepts::basic_charset<stripped_char_t>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			return _ltrim(*this);
		}

		template <typename _F>
		[[nodiscard]] constexpr base_strview rtrim_if(_F _f)const noexcept
			requires (iron::concepts::bool_predicate<_F, stripped_char_t>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			return _rtrim_if(*this, _f);
		}
		[[nodiscard]] constexpr base_strview rtrim()const noexcept
			requires (iron::concepts::basic_charset<stripped_char_t>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			return _rtrim(*this);
		}
		[[nodiscard]] base_strview rtrim()const noexcept
			requires (!iron::concepts::basic_charset<stripped_char_t>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			return _rtrim(*this);
		}
		template <typename _U>
		[[nodiscard]]constexpr  base_strview find(_U _ch)const noexcept
		requires(iron::concepts::fixed_charset<_U>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			return _find_char(*this, _ch);
		}
		[[nodiscard]] constexpr base_strview find(const base_strview& _w)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_w);
			IRON_SECURITY_CHECK_MASTER_LIVE(_w);
			return _find(*this, _w._m_rs);
		}
		template <typename _U>
		[[nodiscard]] constexpr  bool starts_with(_U _ch)const noexcept
			requires(iron::concepts::fixed_charset<_U>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			return _m_rs.starts_with(_ch);
		}
		[[nodiscard]] constexpr bool starts_with(const base_strview& _w)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_w);
			IRON_SECURITY_CHECK_MASTER_LIVE(_w);
			return _m_rs.starts_with(_w._m_rs);
		}
		template <typename _U>
		[[nodiscard]] constexpr  bool contains(_U _ch)const noexcept
			requires(iron::concepts::fixed_charset<_U>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			return _m_rs.contains(_ch);
		}
		[[nodiscard]] constexpr bool contains(const base_strview& _w)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_w);
			IRON_SECURITY_CHECK_MASTER_LIVE(_w);
			return _m_rs.contains(_w._m_rs);
		}
		template <typename _U>
		[[nodiscard]] constexpr  bool ends_with(_U _ch)const noexcept
			requires(iron::concepts::fixed_charset<_U>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			return _m_rs.ends_with(_ch);
		}
		[[nodiscard]] constexpr bool ends_with(const base_strview& _w)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_w);
			IRON_SECURITY_CHECK_MASTER_LIVE(_w);
			return _m_rs.ends_with(_w._m_rs);
		}
		//
		[[nodiscard]] constexpr iron::option<size_t> raw_in(const base_strview& _w)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_w);
			IRON_SECURITY_CHECK_MASTER_LIVE(_w);
#if IRON_SECURITY_CHECKER
			PANIC_UNLESS(is_same_container(_w), "the view does not belong to the container");
#endif 
			return _m_rs.raw_in(_w._m_rs);
		}

		[[nodiscard]] constexpr iron::option<size_t> in(const base_strview& _w)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_w);
			IRON_SECURITY_CHECK_MASTER_LIVE(_w);
#if IRON_SECURITY_CHECKER
			PANIC_UNLESS(is_same_container(_w), "the view does not belong to the container");
#endif 
			return _m_rs.in(_w._m_rs);
		}
#if IRON_SECURITY_CHECKER
		[[nodiscard]] constexpr bool is_same_container(const base_strview& _w)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_w);
			if (!_w) IRON_UNLIKELY 
			{
				return true;
			}
			return _m_ck.same_data(_w.checker());
		}
#endif

		IRON_SECURITY_METHOD()
	private:
		constexpr void _reset_if()noexcept {
#if IRON_SECURITY_CHECKER
			if (_m_rs.empty()) {
				IRON_SECURITY_RESET_THIS();
			}
#endif 
		}
		constexpr void _swap(base_strview& _v)noexcept {
			_m_rs.swap(_v._m_rs);
			IRON_SECURITY_SWAP(_v);
		}
		template <typename _RVIEW>
		[[nodiscard]] static constexpr base_strview _make(const base_strview& _w,_RVIEW&& _rs)noexcept 
		requires(iron::concepts::raw_strview<_RVIEW> && !std::is_reference_v<_RVIEW>) //_rs by rvalue
		{
			base_strview _t;
			_t._m_rs.swap(_rs);
#if IRON_SECURITY_CHECKER
			if (_t._m_rs.empty()) {
				_t._m_rs.reset(); //reset internal pointer if is not null
			}
			else {
				_t._m_ck = _w._m_ck;
			}
#endif 
			return _t;
		}
		template <typename _VIEW>
		[[nodiscard]] static constexpr base_strview _slice(_VIEW& _w,size_t _start, size_t _len)noexcept {
			return _make(_w, _w._m_rs.slice(_start, _len));
		}
		template <typename _VIEW, typename _F>
		[[nodiscard]] static constexpr base_strview _ltrim_if(_VIEW& _w, _F _f)noexcept {
			return _make(_w, _w._m_rs.ltrim_if(_f));
		}
		template <typename _VIEW>
		[[nodiscard]] static constexpr base_strview _ltrim(_VIEW& _w)noexcept {
			return _make(_w, _w._m_rs.ltrim());
		}
		template <typename _VIEW, typename _F>
		[[nodiscard]] static constexpr base_strview _rtrim_if(_VIEW& _w, _F _f)noexcept {
			return _make(_w, _w._m_rs.rtrim_if(_f));
		}
		template <typename _VIEW>
		[[nodiscard]] static constexpr base_strview _rtrim(_VIEW& _w)noexcept {
			return _make(_w, _w._m_rs.rtrim());
		}
		template <typename _VIEW>
		[[nodiscard]] static constexpr base_strview _find(_VIEW& _w,const _raw_strview_t& _c)noexcept {
			return _make(_w, _w._m_rs.find(_c));
		}
		template <typename _VIEW,typename _U>
		[[nodiscard]] static constexpr base_strview _find_char(_VIEW& _w,_U _c)noexcept {
			return _make(_w, _w._m_rs.find(_c));
		}
		[[nodiscard]] constexpr iron::option<stripped_value_t> _get_raw(size_t _idx)const noexcept {
			return _common_t::get_raw(_m_rs.data(), _m_rs.size(), _idx);
		}
		[[nodiscard]] constexpr iron::option<stripped_char_t> _get_char(size_t _idx)const noexcept
			requires(!iron::concepts::fixed_charset<value_t>)
		{
			return _common_t::get_char(_m_rs.data(), _m_rs.size(), _idx);
		}
		[[nodiscard]] constexpr iron::option<stripped_value_t> _get_raw_back()const noexcept
		{
			return _common_t::get_raw_back(_m_rs.data(), _m_rs.size());
		}
		[[nodiscard]] constexpr iron::option<stripped_char_t> _get_char_back()const noexcept
			requires(!iron::concepts::fixed_charset<value_t>)
		{
			return _common_t::get_char_back(_m_rs.data(), _m_rs.size());
		}
		_raw_strview_t _m_rs;
		IRON_SECURITY_DCL();
	};



} //ns

