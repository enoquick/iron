#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable:26481)
#endif


namespace iron::unsafe::str {

	template <typename _T,bool _SAFE>
	class raw_strview final {
	private:
	public:
		using value_t = _T;
		using value_type = value_t;
		using stripped_value_t = std::decay_t<value_t>;
		[[nodiscard]] static constexpr bool is_safe()noexcept { return _SAFE; }
		struct raw_strview_tag { raw_strview_tag() = delete; };
		using char_t = std::conditional_t<iron::concepts::fixed_charset<value_t>, value_t, std::conditional_t<std::is_const_v<value_t>, const char32_t, char32_t>>;
		using stripped_char_t = std::decay_t<char_t>;
		using raw_storage_t = iron::unsafe::raw_storage<value_t>;
		using raw_iter_t = iron::unsafe::iterators::contiguous_iterator<std::conditional_t<is_safe(), const value_t, value_t>>;
		using rev_raw_iter_t = std::reverse_iterator<raw_iter_t>;
		using const_raw_iter_t = iron::unsafe::iterators::contiguous_iterator<const value_t>;
		using const_rev_raw_iter_t = std::reverse_iterator<const_raw_iter_t>;
		using iter_t = std::conditional_t<iron::concepts::fixed_charset<value_t>, raw_iter_t, iron::unsafe::iterators::unicode_iterator<value_t>>;
		using const_iter_t = std::conditional_t<iron::concepts::fixed_charset<value_t>, const_raw_iter_t, iron::unsafe::iterators::unicode_iterator<const value_t>>;
		using diff_t = std::ptrdiff_t;
		using difference_type_t = diff_t;
		using ptr_t = value_t*;
		using cptr_t = const value_t*;
		using pointer = ptr_t;
		using ref_t = iron::ref_wrapper<value_t>;
		using cref_t = iron::ref_wrapper<const value_t>;
		using reference = value_t&;
		using iterator = iter_t;
		using const_iterator = const_iter_t;
		using error_handler_t = iron::error_handler;
		using unicode_cv_t = iron::unsafe::unicode_conversions;
		using common_t = iron::unsafe::str::common;
		using size_t = std::size_t;
		constexpr raw_strview()noexcept {}
		constexpr raw_strview(raw_strview&& _v)noexcept
		{
			swap(_v);
		}
		constexpr raw_strview(const raw_strview& _v)noexcept
			: _m_p(_v._m_p)
			, _m_sz(_v._m_sz)
		{
		}
		constexpr raw_strview& operator=(raw_strview&& _v)noexcept {
			swap(_v);
			return *this;
		}
		constexpr raw_strview& operator=(const raw_strview& _v)noexcept {
			_m_p = _v._m_p;
			_m_sz = _v._m_sz;

			raw_strview _t(_v);
			swap(_t);
			return *this;
		}
		constexpr ~raw_strview() {}
		constexpr raw_strview(ptr_t _p, size_t _sz)noexcept
			: _m_p(_p)
			, _m_sz(_sz)
		{}
		[[nodiscard]] constexpr raw_iter_t raw_begin()noexcept { return raw_iter_t(_m_p); }
		[[nodiscard]] constexpr raw_iter_t raw_end()noexcept { return raw_iter_t(_m_p + _m_sz); }
		[[nodiscard]] constexpr const_raw_iter_t raw_begin()const noexcept { return const_raw_iter_t(_m_p); }
		[[nodiscard]] constexpr const_raw_iter_t raw_end()const noexcept { return const_raw_iter_t(_m_p + _m_sz); }
		[[nodiscard]] constexpr rev_raw_iter_t rev_raw_begin()noexcept { return rev_raw_iter_t(_m_p + _m_sz); }
		[[nodiscard]] constexpr rev_raw_iter_t rev_raw_end()noexcept { return rev_raw_iter_t(_m_p); }
		[[nodiscard]] constexpr const_rev_raw_iter_t rev_raw_begin()const noexcept { return const_rev_raw_iter_t(_m_p + _m_sz); }
		[[nodiscard]] constexpr const_rev_raw_iter_t rev_raw_end()const noexcept { return const_rev_raw_iter_t(_m_p); }
		[[nodiscard]] constexpr iter_t begin()noexcept {
			if constexpr (iron::concepts::fixed_charset<value_t>) {
				return raw_begin();
			}
			else {
				return iter_t(_m_p, _m_p + _m_sz);
			}
		}
		[[nodiscard]] constexpr iter_t end()noexcept {
			if constexpr (iron::concepts::fixed_charset<value_t>) {
				return raw_end();
			}
			else {
				auto _p = _m_p + _m_sz;
				return iter_t(_p, _p);
			}
		}
		[[nodiscard]] constexpr const_iter_t begin()const noexcept {
			if constexpr (iron::concepts::fixed_charset<value_t>) {
				return raw_begin();
			}
			else {
				return const_iter_t(_m_p, _m_p + _m_sz);
			}
		}
		[[nodiscard]] constexpr const_iter_t end()const noexcept {
			if constexpr (iron::concepts::fixed_charset<value_t>) {
				return raw_end();
			}
			else {
				auto _p = _m_p + _m_sz;
				return const_iter_t(_p, _p);
			}
		}
		[[nodiscard]] static constexpr size_t max_size()noexcept { return raw_storage_t::max_size(); }
		[[nodiscard]] constexpr size_t size()const noexcept { return _m_sz; }

		template <typename _U, std::size_t _N>
		[[nodiscard]] static constexpr size_t array_size(const _U(&_v)[_N])noexcept {
			if constexpr (!_N) {
				return _N;
			}
			else {
				return _v[_N - 1] == _U{} ? _N - 1 : _N;
			}
		}
		typedef enum class enum_transform {
			ucase
			,lcase
		} transform_t;
		template <transform_t _TR>
		constexpr void make_transform()noexcept {
			if constexpr (_TR == transform_t::ucase) {
				common_t::make_base_transform<is_safe()>(_m_p, _m_sz, [](value_t _v)noexcept->value_t {
						const auto _ch=iron::chr::unchecked_from_u32(static_cast<char32_t>(_v)).to_ascii_uppercase();
						return static_cast<value_t>(_ch.to_utf32());
					}
				);
			}
			else if constexpr (_TR == transform_t::lcase) {
				common_t::make_base_transform<is_safe()>(_m_p, _m_sz, [](value_t _v)noexcept->value_t {
						const auto _ch = iron::chr::unchecked_from_u32(static_cast<char32_t>(_v)).to_ascii_lowercase();
						return static_cast<value_t>(_ch.to_utf32());
					}
				);
			}
			else {
				static_assert(iron::unsafe::asserts::always_false<transform_t>, "internal error");
			}
		}
		[[nodiscard]] constexpr bool empty()const noexcept { return !size(); }

		[[nodiscard]] constexpr bool is_chr_boundary(size_t _idx)const noexcept {
			if constexpr (iron::concepts::fixed_charset<value_t>) {
				return _idx <= _m_sz;
			}
			else {
				return common_t::is_chr_boundary_varcharset(_m_p, _idx, _m_sz);
			}
		}
		constexpr raw_strview& reset()noexcept {
			_m_sz = {};
			_m_p = {};
			return *this;
		}
		[[nodiscard]] constexpr cptr_t data()const noexcept {
			return _m_p;
		}
		[[nodiscard]] constexpr ptr_t data()noexcept {
			return _m_p;
		}
		constexpr raw_strview& swap(raw_strview& _v)noexcept {
			_swap(_v);
			return *this;
		}
		[[nodiscard]] constexpr stripped_value_t unchecked_raw_nth(size_t _idx)const noexcept {
			DEBUG_ASSERT(_idx < size());
			return _m_p[_idx];
		}
		[[nodiscard]] constexpr raw_strview slice(size_t _start, size_t _len)const noexcept {
			return _slice(*this, _start, _len);
		}
		template <typename _F>
		[[nodiscard]] constexpr raw_strview ltrim_if(_F _f)const noexcept {
			return _ltrim_if(*this, _f);
		}
		[[nodiscard]] constexpr raw_strview ltrim()const noexcept {
			return _ltrim(*this);
		}
		template <typename _F>
		[[nodiscard]] constexpr raw_strview rtrim_if(_F _f)const noexcept {
			return _rtrim_if(*this, _f);
		}
		[[nodiscard]] constexpr raw_strview rtrim()const noexcept {
			return _rtrim(*this);
		}

		template <typename _T1,bool _SAFE1>
		[[nodiscard]] constexpr bool operator==(const raw_strview<_T1, _SAFE1>& _v)const noexcept {
			return common_t::eq(_m_p, _m_sz, _v._m_p, _v._m_sz);
		}
		template <typename _U, std::size_t _N>
		[[nodiscard]] constexpr bool operator==(const _U(&_v)[_N])const noexcept {
			using _view_t = iron::unsafe::str::raw_strview<const _U, false>;
			const auto _sz = _view_t::array_size(_v);
			const _view_t  _t(&_v[0], _sz);
			return *this == _t;
		}

		template <typename _T1, bool _SAFE1>
		[[nodiscard]] constexpr bool operator <(const raw_strview<_T1, _SAFE1>& _v)const noexcept {
			return common_t::lt(_m_p, _m_sz, _v._m_p, _v._m_sz);
		}
		template <typename _U, std::size_t _N>
		[[nodiscard]] constexpr bool operator <(const _U(&_v)[_N])const noexcept {
			using _view_t = iron::unsafe::str::raw_strview<const _U, false>;
			const auto _sz = _view_t::array_size(_v);
			const _view_t  _t(&_v[0], _sz);
			return *this < _t;
		}

		template <typename _T1, bool _SAFE1>
		[[nodiscard]] constexpr std::strong_ordering operator <=>(const raw_strview<_T1,_SAFE1>& _v)const noexcept {
			return common_t::compare(_m_p, _m_sz, _v._m_p, _v._m_sz);
		}
		template <typename _U, std::size_t _N>
		[[nodiscard]] constexpr std::strong_ordering operator <=>(const _U(&_v)[_N])const noexcept {
			using _view_t = iron::unsafe::str::raw_strview<const _U, false>;
			const auto _sz = _view_t::array_size(_v);
			const _view_t  _t(&_v[0], _sz);
			return *this <=> _t;
		}

		template <typename _U>
		[[nodiscard]] constexpr raw_strview find(_U _c)const noexcept 
		requires(iron::concepts::fixed_charset<_U>)
		{
			return _find_char(*this, _c);
		}
		[[nodiscard]] constexpr raw_strview find(const raw_strview& _w)const noexcept {
			return _find(*this, _w);
		}
		//
		template <typename _U>
		[[nodiscard]] constexpr  bool starts_with(_U _c)const noexcept
		requires(iron::concepts::fixed_charset<_U>)
		{
			const auto _b = begin();
			return _b != end() && static_cast<std::uint32_t>(*_b) == static_cast<std::uint32_t>(_c);
		}
		template <typename _U>
		[[nodiscard]] constexpr  bool contains(_U _c)const noexcept
			requires(iron::concepts::fixed_charset<_U>)
		{
			const auto _pair = common_t::find_char(_m_p, _m_sz, _c);
			return _pair.second ? true : false;
		}
		template <typename _U>
		[[nodiscard]] constexpr  bool ends_with(_U _c)const noexcept
			requires(iron::concepts::fixed_charset<_U>)
		{
			return common_t::ends_with_char(_m_p, _m_sz, _c);
		}
		[[nodiscard]] constexpr bool starts_with(const raw_strview& _w)const noexcept {
			return common_t::starts_with(_m_p, _m_sz, _w._m_p, _w._m_sz);
		}
		[[nodiscard]] constexpr bool contains(const raw_strview& _w)const noexcept {
			const auto _pair = common_t::find(_m_p, _m_sz, _w._m_p, _w._m_sz);
			return _pair.second ? true : false;
		}
		[[nodiscard]] constexpr bool ends_with(const raw_strview& _w)const noexcept {
			return common_t::ends_with(_m_p,_m_sz,_w._m_p,_w._m_sz);
		}

		[[nodiscard]] constexpr iron::option<size_t> raw_in(const raw_strview& _w)const noexcept {
			return common_t::raw_in(_m_p, _m_sz, _w._m_p, _w._m_sz);
		}
		[[nodiscard]] constexpr iron::option<size_t> in(const raw_strview& _w)const noexcept {
			auto _opt = raw_in(_w);
			if constexpr (iron::concepts::varying_charset<value_t>) {
				if (_opt) IRON_LIKELY
				{
					return common_t::chr_index(_m_p, _m_sz, _opt.unchecked_unwrap());
				}
			}
			return _opt;
		}
		[[nodiscard]] constexpr iron::option<size_t> raw_index(size_t _ch_index)const noexcept {
			if constexpr (iron::concepts::fixed_charset<value_t>) {
				if (_ch_index < _m_sz) IRON_LIKELY
				{
					return iron::some(_ch_index);
				}
				return {};
			}
			else {
				return common_t::raw_index(_m_p, _m_sz, _ch_index);
			}
		}

	private:
		template <typename _T1, bool _SAFE1>
		friend class raw_strview;

		constexpr void _swap(raw_strview& _v)noexcept {
			std::swap(_m_p, _v._m_p);
			std::swap(_m_sz, _v._m_sz);
		}
		template <typename _VIEW>
		[[nodiscard]] static constexpr raw_strview _slice(_VIEW& _w,size_t _start, size_t _len)noexcept {
			const auto _pair = common_t::slice(_w._m_p, _w._m_sz,_start,_len);
			return raw_strview(_pair.first, _pair.second);
		}
		template <typename _VIEW,typename _F>
		[[nodiscard]] static constexpr raw_strview _ltrim_if(_VIEW& _w,_F _f)noexcept {
			const auto _pair = common_t::ltrim_if(_w._m_p, _w._m_sz,_f);
			return raw_strview(_pair.first, _pair.second);
		}
		template <typename _VIEW>
		[[nodiscard]] static constexpr raw_strview _ltrim(_VIEW& _w)noexcept {
			const auto _pair = common_t::ltrim(_w._m_p, _w._m_sz);
			return raw_strview(_pair.first, _pair.second);
		}
		template <typename _VIEW, typename _F>
		[[nodiscard]] static constexpr raw_strview _rtrim_if(_VIEW& _w, _F _f)noexcept {
			const auto _pair = common_t::rtrim_if(_w._m_p, _w._m_sz, _f);
			return raw_strview(_pair.first, _pair.second);
		}
		template <typename _VIEW>
		[[nodiscard]] static constexpr raw_strview _rtrim(_VIEW& _w)noexcept {
			const auto _pair = common_t::rtrim(_w._m_p, _w._m_sz);
			return raw_strview(_pair.first, _pair.second);
		}
		template <typename _VIEW,typename _U>
		[[nodiscard]] static constexpr raw_strview _find_char(_VIEW& _w,_U _ch)noexcept {
			const auto _pair = common_t::find_char(_w._m_p, _w._m_sz,_ch);
			return raw_strview(_pair.first, _pair.second);
		}

		template <typename _VIEW>
		[[nodiscard]] static constexpr raw_strview _find(_VIEW& _w, const raw_strview& _c)noexcept {
			const auto _pair = common_t::find(_w._m_p, _w._m_sz,_c._m_p,_c._m_sz);
			return raw_strview(_pair.first, _pair.second);
		}
		ptr_t _m_p{};
		size_t _m_sz{};
	};



} //ns

#if defined(_MSC_VER)
#pragma warning(pop)
#endif
