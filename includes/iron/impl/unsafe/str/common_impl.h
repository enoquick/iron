#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable:26429)
#pragma warning (disable:26459)
#pragma warning (disable:26472)
#pragma warning (disable:26481)
#pragma warning (disable:26494)
#endif

namespace iron::unsafe::str {

	class common final {
	public:
		using unicode_cv_t = iron::unsafe::unicode_conversions;
		using diff_t = std::ptrdiff_t;
		using size_t = std::size_t;
		template <typename _T>
		[[nodiscard]] static constexpr auto begin(_T* _p, size_t _sz)noexcept {
			if constexpr (iron::concepts::fixed_charset<_T>) {
				return _p;
			}
			else if constexpr (iron::concepts::varying_charset<_T>) {
				return iron::unsafe::iterators::unicode_iterator<_T>(_p, _p + _sz);
			}
			else {
				static_assert(iron::unsafe::asserts::always_false<_T>, "internal error");
			}
		}
		template <typename _T>
		[[nodiscard]] static constexpr auto end(_T* _p, size_t _sz)noexcept {
			if constexpr (iron::concepts::fixed_charset<_T>) {
				return _p + _sz;
			}
			else if constexpr (iron::concepts::varying_charset<_T>) {
				_p += _sz;
				return iron::unsafe::iterators::unicode_iterator<_T>(_p, _p);
			}
			else {
				static_assert(iron::unsafe::asserts::always_false<_T>, "internal error");
			}
		}

		template <typename _T>
		[[nodiscard]] static constexpr std::uint32_t to_u32(_T _v)noexcept {
			using _t_t = std::decay_t<_T>;
			if constexpr (iron::concepts::basic_charset<_t_t>) {
				if (const auto _opt = iron::chr::from(_v)) {
					return _opt.unchecked_unwrap().to_u32();
				}
				return static_cast<std::uint32_t>(_v);
			}
			else if constexpr (iron::concepts::fixed_charset<_t_t>) {
				return static_cast<std::uint32_t>(_v);
			}
			else if constexpr (std::is_same_v<_t_t, std::uint32_t>) {
				return _v;
			}
			else if constexpr (iron::concepts::varying_charset<_t_t>) {
				static_assert(iron::unsafe::asserts::always_false<_T>, "internal error - is varying charset");
			}
			else {
				static_assert(iron::unsafe::asserts::always_false<_T>, "internal error - unknow charset");
			}
		}
		template <bool _SAFE,typename _T, typename _ITER>
		[[nodiscard]] static constexpr bool copy(_T* _pd, size_t& _sz, _ITER& _b, const _ITER& _e)noexcept {
			if (std::is_constant_evaluated()) {
				const auto _r=unicode_cv_t::template copy<_SAFE>(_pd, _sz, _b, _e);
				DEBUG_ASSERT_EQ(_b, _e);
				return _r;
			}
			else {
				if constexpr (std::contiguous_iterator<_ITER>) {
					const auto _r =_copy_contiguos<_SAFE>(_pd, _sz, _b, _e);
					_b = _e;
					return _r;
				}
				else if constexpr (iron::concepts::unicode_iterator<_ITER>) {
					DEBUG_ASSERT(_b._m_e == _e._m_b);
					const auto _r = _copy_contiguos_ui<_SAFE>(_pd, _sz, _b);
					_b = _e;
					return _r;
				}
				else {
					const auto _r=unicode_cv_t::template copy<_SAFE>(_pd, _sz, _b, _e);
					DEBUG_ASSERT_EQ(_b, _e);
					return _r;
				}
			}
		}
		template <typename _T>
		[[nodiscard]] static constexpr bool is_chr_boundary_varcharset(const _T* _p,size_t _idx,size_t _sz)noexcept
		requires(iron::concepts::varying_charset<_T>)
		{
			if (std::is_constant_evaluated()) {
				return _is_chr_boundary_varcharset_c(_p, _idx, _sz);
			}
			else {
				return _is_chr_boundary_varcharset_nc(_p, _idx, _sz);
			}
		}
		template <typename _T>
		[[nodiscard]] static constexpr std::pair<_T*, size_t> raw_slice(_T* _p, size_t _size, size_t _start, size_t _len)noexcept {
			if (_start >= _size || !_len) IRON_UNLIKELY
			{
				return {};
			}
			_size -= _start;
			if (_len > _size) IRON_UNLIKELY
			{
				_len = _size;
			}
			return std::make_pair(_p + _start, _len);
		}
		template <typename _T>
		[[nodiscard]] static constexpr std::pair<_T*, size_t> slice(_T* _p, size_t _sz, size_t  _start, size_t _len)noexcept {
			if constexpr (iron::concepts::fixed_charset<_T>) {
				return raw_slice(_p, _sz, _start, _len);
			}
			else if constexpr (iron::concepts::varying_charset<_T>) {
				if (std::is_constant_evaluated()) {
					return _slice_var_c(_p, _sz, _start, _len);
				}
				else {
					return _slice_var_nc(_p, _sz, _start, _len);
				}
			}
			else {
				static_assert(iron::unsafe::asserts::always_false<_T>, "unknow charset");
			}
		}
		template <typename _T,typename _F>
		[[nodiscard]] static constexpr std::pair<_T*, size_t> ltrim_if(_T* _p, size_t _sz, _F _f)noexcept {
			auto _b = begin(_p, _sz);
			const auto _e = end(_p, _sz);
			return _ltrim_if<_T>(_b, _e, _f);
		}
		template <typename _T>
		[[nodiscard]] static constexpr std::pair<_T*, size_t> ltrim(_T* _p, size_t _sz)noexcept {
			if (std::is_constant_evaluated()) {
				return _ltrim_c(_p, _sz);
			}
			else {
				return _ltrim_nc(_p, _sz);
			}
		}
		template <typename _T, typename _F>
		[[nodiscard]] static constexpr std::pair<_T*, size_t> rtrim_if(_T* _p, size_t _sz, _F _f)noexcept {
			if constexpr (iron::concepts::fixed_charset<_T>) {
				return _rtrim_if<_T>(_p, _p + _sz, _f);
			}
			else if constexpr (iron::concepts::varying_charset<_T>) {
				auto _pe = _p + _sz;
				iron::unsafe::iterators::unicode_iterator<_T> _b(_p, _pe);
				const iron::unsafe::iterators::unicode_iterator<_T> _e(_pe, _pe);
				return _rtrim_if<_T>(_b, _e, _f);
			}
			else {
				static_assert(iron::unsafe::asserts::always_false<_T>, "unknow charset");
			}
		}

		template <typename _T>
		[[nodiscard]] static constexpr std::pair<_T*, size_t> rtrim(_T* _p, size_t _sz)noexcept {
			if (std::is_constant_evaluated()) {
				return _rtrim_c(_p, _sz);
			}
			else {
				return _rtrim_nc(_p, _sz);
			}
		}
		template <typename _T,typename _U>
		[[nodiscard]] static constexpr std::pair<_T*, size_t> find_char(_T* _p, size_t _sz,_U _ch)noexcept {
			if (std::is_constant_evaluated()) {
				return _find_char_c(_p, _sz,to_u32(_ch));
			}
			else {
				return _find_char_nc(_p, _sz, to_u32(_ch));
			}
		}
		template <typename _T,typename _U>
		[[nodiscard]] static constexpr bool ends_with_char(const _T* _p, size_t _sz, _U _ch)noexcept {
			if constexpr (iron::concepts::fixed_charset<_T>) {
				if (_sz) IRON_LIKELY
				{
					return to_u32(_p[--_sz]) == to_u32(_ch);
				}
				return {};
			}
			else if constexpr(iron::concepts::varying_charset<_T>) {
				if (std::is_constant_evaluated()) {
					return _ends_with_char_var_c(_p, _sz,to_u32(_ch));
				}
				else {
					return _ends_with_char_var_nc(_p, _sz, to_u32(_ch));
				}
			}
			else {
				static_assert(iron::unsafe::asserts::always_false<_T>, "unknow charset");
			}
		}
		template <typename _T>
		[[nodiscard]] static constexpr bool ends_with(const _T* _p, size_t _sz,const _T* _pc,size_t _szc)noexcept {
			if (std::is_constant_evaluated()) {
				return _ends_with_c(_p, _sz, _pc, _szc);
			}
			else {
				return _ends_with_nc(_p, _sz, _pc, _szc);
			}
		}

		template <typename _T>
		[[nodiscard]] static constexpr std::pair<_T*, size_t> find(_T* _p, size_t _sz,const _T* _pc,size_t _szc)noexcept {
			if (std::is_constant_evaluated()) {
				return _find_c(_p, _sz, _pc, _szc);
			}
			else {
				return _find_nc(_p, _sz, _pc, _szc);
			}
		}
		template <typename _T>
		[[nodiscard]] static constexpr bool starts_with(const _T* _p, size_t _sz,const _T* _pc,size_t _szc)noexcept {
			if (std::is_constant_evaluated()) {
				return _starts_with_c(_p, _sz, _pc, _szc);
			}
			else {
				return _starts_with_nc(_p, _sz, _pc, _szc);
			}
		}

		template <typename _T>
		[[nodiscard]] static constexpr iron::option<_T> get_raw(const _T* _p,size_t _sz,size_t _idx)noexcept {
			if (_idx < _sz) IRON_LIKELY
			{
				return iron::some(_p[_idx]);
			}
			return {};
		}
		template <typename _T>
		[[nodiscard]] static constexpr iron::option<char32_t> get_char(const _T* _p,size_t _sz,size_t _idx)noexcept
			requires(!iron::concepts::fixed_charset<_T>)
		{
			if (std::is_constant_evaluated()) {
				return _get_char_c(_p, _sz, _idx);
			}
			else {
				return _get_char_nc(_p, _sz, _idx);
			}
		}
		template <typename _T>
		[[nodiscard]] static constexpr iron::option<_T> get_raw_back(const _T* _p,size_t _sz)noexcept
		{
			if (_sz) IRON_LIKELY
			{
				return iron::some(_p[--_sz]);
			}
			return {};
		}
		template <typename _T>
		[[nodiscard]] static constexpr iron::option<char32_t> get_char_back(const _T* _p,size_t _sz)noexcept
			requires(!iron::concepts::fixed_charset<_T>)
		{
			if (std::is_constant_evaluated()) {
				return _get_char_back_c(_p, _sz);
			}
			else {
				return _get_char_back_nc(_p, _sz);
			}
		}
		template <bool _SAFE,typename _T,typename _F>
		static constexpr void make_base_transform(_T* _p, size_t _sz, _F _f)noexcept {
			using _v_t = std::decay_t<_T>;
			if constexpr (iron::concepts::basic_charset<_v_t>) { //char
#ifdef IRON_EBCDIC
#error not implemented for ebcdic
#else
				while (_sz--) IRON_LIKELY
				{
					_base_transform<_SAFE>(*_p++, _f);
				}
#endif
			}
			else if constexpr (iron::concepts::fixed_charset<_v_t>) { //utf32
				while (_sz--) IRON_LIKELY
				{
					_base_transform<_SAFE>(*_p++, _f);
				}
			}
			else if constexpr(iron::concepts::varying_charset<_v_t>) {
				while (_sz) IRON_LIKELY
				{
					const auto _n = _utf_sz(*_p);
					_base_transform<_SAFE>(*_p, _f);
					if (!_n) IRON_UNLIKELY
					{
						++_p;
						--_sz;
						continue;
					}
					if (_n >= _sz) IRON_UNLIKELY
					{
						break;
					}
					_p += _n;
					_sz -= _n;
				}
			}
			else {
				static_assert(iron::unsafe::asserts::always_false<_v_t>, "unknow charset");
			}
		}
		template <typename _T>
		[[nodiscard]]  static constexpr iron::option<size_t> chr_index(const _T* _p, size_t _sz,size_t _idx)noexcept
		requires(iron::concepts::varying_charset<_T>)
		{
			if (std::is_constant_evaluated()) {
				return _chr_index_c(_p, _sz, _idx);
			}
			else {
				return _chr_index_nc(_p, _sz, _idx);
			}
		}
		template <typename _T>
		[[nodiscard]] static constexpr iron::option<size_t> raw_index(const _T* _p, size_t _sz, size_t _idx)noexcept
		requires(iron::concepts::varying_charset<_T>)
		{
			if (std::is_constant_evaluated()) {
				return _raw_index_c(_p, _sz, _idx);
			}
			else {
				return _raw_index_nc(_p, _sz, _idx);
			}
		}

		template <typename _T>
		[[nodiscard]] static constexpr iron::option<size_t> raw_in(const _T* _p, size_t _sz, const _T* _pc, size_t _szc)noexcept {
			if (std::is_constant_evaluated()) {
				return _raw_in_c(_p, _sz, _pc, _szc);
			}
			else {
				return _raw_in_nc(_p, _sz, _pc, _szc);
			}
		}
		template <typename _T1, typename _T2>
		[[nodiscard]] static constexpr bool eq(const _T1* _pl, size_t _szl, const _T2* _pr, size_t _szr)noexcept {
			if constexpr (std::is_same_v<std::decay_t<_T1>, std::decay_t<_T2>>) {
				if (_szl == _szr) IRON_LIKELY
				{
					return std::equal(_pl,_pl + _szl,_pr);
				}
				return {};
			}
			else {
				if (std::is_constant_evaluated()) {
					return _eq_c(_pl, _szl, _pr, _szr);
				}
				else {
					return _eq_nc(_pl, _szl, _pr, _szr);
				}
			}
		}
		template <typename _T1, typename _T2>
		[[nodiscard]] static constexpr bool lt(const _T1* _pl, size_t _szl, const _T2* _pr, size_t _szr)noexcept {
			if (std::is_constant_evaluated()) {
				return _lt_c(_pl, _szl, _pr, _szr);
			}
			else {
				return _lt_nc(_pl, _szl, _pr, _szr);
			}
		}
		using compare_t = std::strong_ordering;
		template <typename _T1,typename _T2>
		[[nodiscard]] static constexpr compare_t compare(const _T1* _pl, size_t _szl, const _T2* _pr, size_t _szr)noexcept {
			if (std::is_constant_evaluated()) {
				return _compare_c(_pl, _szl, _pr, _szr);
			}
			else {
				return _compare_nc(_pl, _szl, _pr, _szr);
			}
		}
		typedef enum class enum_transform_str {
			upper
			, lower
		} transform_str_t;

		template <transform_str_t _TR,typename _T>
		static constexpr void transform_str(_T* _p, size_t& _sz, const _T* _pc, size_t _szc)noexcept {
			if constexpr (_TR == transform_str_t::upper) {
				if (std::is_constant_evaluated()) {
					_to_upper_c(_p, _sz, _pc, _szc);
				}
				else {
					_to_upper_nc(_p, _sz, _pc, _szc);
				}
			}
			else if constexpr (_TR == transform_str_t::lower) {
				if (std::is_constant_evaluated()) {
					_to_lower_c(_p, _sz, _pc, _szc);
				}
				else {
					_to_lower_nc(_p, _sz, _pc, _szc);
				}
			}
			else {
				static_assert(iron::unsafe::asserts::always_false<_T>, "unknow operation");
			}
		}

	private:
		template <typename _ITER>
		[[nodiscard]] static constexpr size_t _distance(const _ITER& _b, const _ITER& _e)noexcept 
		requires(std::contiguous_iterator<_ITER>)
		{
			return static_cast<size_t>(std::distance(_b, _e));
		}

		template <bool _SAFE,typename _T, typename _ITER>
		[[nodiscard]] static bool  _copy_contiguos(_T* _p,size_t& _sz, const _ITER& _b, const _ITER& _e)noexcept 
		requires(std::contiguous_iterator<_ITER>)
		{
			using _v_t = std::decay_t<decltype(*_b)>;
			const _v_t* _pb = &*_b;
			const _v_t* _pe = &*_e;
			if constexpr (_SAFE) {
				return _copy_safe(_p,_sz, _pb, _pe);
			}
			else {
				return _copy(_p,_sz, _pb, _pe);
			}
		}
		template <bool _SAFE, typename _T, typename _ITER>
		[[nodiscard]] static bool  _copy_contiguos_ui(_T* _p, size_t& _sz, const _ITER& _b)noexcept
			requires(iron::concepts::unicode_iterator<_ITER>)
		{
			if constexpr (_SAFE) {
				return _copy_safe(_p, _sz, _b._m_b, _b._m_e);
			}
			else {
				return _copy(_p, _sz, _b._m_b,_b._m_e);
			}
		}

		template <typename _T, typename _U>
		[[nodiscard]] static bool _copy(_T*, size_t&, const _U*, const _U*)noexcept;

		template <typename _T, typename _U>
		[[nodiscard]] static bool  _copy_safe(_T*, size_t&, const _U*, const _U*)noexcept;


		template <typename _T>
		[[nodiscard]] static constexpr bool _is_chr_boundary_varcharset_c(const _T* _p, size_t _idx, size_t _sz)noexcept {
			if (!_idx || _idx == _sz) IRON_UNLIKELY
			{
				return true;
			}
			if (_idx < _sz) IRON_LIKELY
			{
				size_t _current_idx = {};
				while (_sz) IRON_LIKELY
				{
					const auto _utfsz = _utf_sz(*_p);
					if (!_utfsz) IRON_UNLIKELY //invalid
					{
						break;
					}
					_current_idx += _utfsz;
					if (_idx == _current_idx) {
						return true;
					}
					if (_idx < _current_idx) IRON_UNLIKELY //false
					{
						break;
					}
					if (_sz > _utfsz) IRON_LIKELY
					{
						_sz -= _utfsz;
						_p += _utfsz;
						continue;
					}
					break;
				}
			}
			return {};
		}

		template <typename _T>
		[[nodiscard]] static bool _is_chr_boundary_varcharset_nc(const _T*, size_t,size_t)noexcept;

		template <typename _T>
		[[nodiscard]] static constexpr iron::option<char32_t> _get_char_c(const _T* _p, size_t _sz, size_t _idx)noexcept
		{
			if (_sz) IRON_LIKELY
			{
				auto _iter = begin(_p,_sz);
				while (_idx-- && _iter) IRON_LIKELY
				{
					++_iter;
				}
				if (_iter) IRON_LIKELY
				{
					return iron::some(*_iter);
				}
			}
			return {};
		}

		template <typename _T>
		[[nodiscard]] static iron::option<char32_t> _get_char_nc(const _T*, size_t, size_t)noexcept;

		template <typename _T>
		[[nodiscard]] static constexpr iron::option<char32_t> _get_char_back_c(const _T* _p, size_t _sz)noexcept
		{
			if (_sz) IRON_LIKELY
			{
				auto _iter = begin(_p,_sz);
				while (_iter) IRON_LIKELY
				{
					auto _next = _iter;
					++_next;
					if (!_next) IRON_UNLIKELY
					{
						return iron::some(*_iter);
					}
					_iter = _next;
				}
			}
			return {};
		}

		template <typename _T>
		[[nodiscard]] static iron::option<char32_t> _get_char_back_nc(const _T*, size_t)noexcept;

		template <bool _SAFE,typename _T,typename _F>
		static constexpr void _base_transform(_T& _v,_F _f)noexcept
		{
			if constexpr (_SAFE) {
#ifdef IRON_EBCDIC
#error not implemented for ebcdic
#else
				const auto _c = static_cast<char32_t>(_f(_v));
				if (iron::chr::unchecked_from_u32(_c).is_ascii()) {
					using _v_t = std::decay_t<_T>;
					_v = static_cast<_v_t>(_c);
				}
#endif
			}
			else {
#ifdef IRON_EBCDIC
#error _EBCDIC not implement 
#else
				_v = _f(_v);
#endif
			}
		}

		template <typename _T>
		[[nodiscard]] static constexpr size_t _utf_sz(_T _v)noexcept {
			using _v_t = std::decay_t<_T>;
			if constexpr (iron::concepts::utf8_charset<_v_t>) {
				return unicode_cv_t::size_utf8(_v);
			}
			else if constexpr (iron::concepts::utf16_charset<_v_t>) {
				return unicode_cv_t::size_utf16(_v);
			}
			else {
				static_assert(iron::unsafe::asserts::always_false<_v_t>, "is not varying charset");
			}
		}

		template <typename _T>
		[[nodiscard]] static constexpr std::pair<_T*, size_t> _slice_var_c(_T* _p, size_t _sz, size_t  _start, size_t _len)noexcept {
			if (!_len) IRON_UNLIKELY
			{
				return {};
			}
			while(_start-- && _sz) IRON_LIKELY
			{
				auto _size = _utf_sz(*_p);
				if (!_size) IRON_UNLIKELY
				{
					_size = 1;
				}
				else if (_size > _sz) IRON_UNLIKELY
				{
					_size = _sz;
				}
				_sz -= _size;
				_p += _size;
			}
			auto _pe = _p;
			while(_len-- && _sz) IRON_LIKELY
			{
				auto _size = _utf_sz(*_pe);
				if (!_size) IRON_UNLIKELY
				{
					_size = 1;
				}
				else if (_size > _sz) IRON_UNLIKELY
				{
					_size = _sz;
				}
				_sz -= _size;
				_pe += _size;
			}
			return std::make_pair(_p, _distance(_p,_pe));
		}

		template <typename _T>
		[[nodiscard]] static std::pair<_T*, size_t> _slice_var_nc(_T*, size_t, size_t, size_t)noexcept;

		template <typename _T,typename _ITER>
		[[nodiscard]] static constexpr std::pair<_T*, size_t> _make_pair(_ITER& _b, const _ITER& _e)noexcept {
			if constexpr (iron::concepts::fixed_charset<_T>) {
				return std::make_pair(_b,_distance(_b,_e));
			}
			else if constexpr (iron::concepts::varying_charset<_T>) {
				return std::make_pair(_b._m_b, _distance(_b._m_b, _e._m_b));
			}
			else {
				static_assert(iron::unsafe::asserts::always_false<_T>, "unknow charset");
			}
		}

		template <typename _T,typename _ITER,typename _F>
		[[nodiscard]] static constexpr std::pair<_T*, size_t> _ltrim_if(_ITER& _b, const _ITER& _e, _F _f)noexcept {
			while (_b != _e && _f(*_b)) IRON_LIKELY
			{
				++_b;
			}
			return _make_pair<_T>(_b, _e);
		}

		template <typename _T>
		[[nodiscard]] static constexpr std::pair<_T*, size_t> _ltrim_c(_T* _p, size_t _sz)noexcept {
			if constexpr (iron::concepts::basic_charset<_T>) {
				return ltrim_if(_p, _sz, [](char _v)noexcept -> bool { return iron::unsafe::unicode::is_ascii_whitespace(to_u32(_v)); });
			}
			else {
				return ltrim_if(_p, _sz, [](std::uint32_t _v)noexcept -> bool { return iron::chr::unchecked_from_u32(to_u32(_v)).is_whitespace(); });
			}
		}

		template <typename _T>
		[[nodiscard]]  static std::pair<_T*, size_t> _ltrim_nc(_T*, size_t)noexcept;

		template <typename _T, typename _ITER, typename _F>
		[[nodiscard]] static constexpr std::pair<_T*, size_t> _rtrim_if(_ITER& _b, const _ITER& _e, _F _f)noexcept {
			if constexpr (iron::concepts::fixed_charset<_T>) {
				static_assert(std::contiguous_iterator<_ITER>, "internal error - is not contiguous iter");
				auto _rb = std::reverse_iterator<_ITER>(_e);
				const auto _re = std::reverse_iterator<_ITER>(_b);
				while (_rb != _re) IRON_LIKELY
				{
					if (!_f(*_rb)) IRON_LIKELY
					{
						break;
					}
					++_rb;
				}
				return _make_pair<_T>(_b, _rb.base());
			}
			else if constexpr(iron::concepts::varying_charset<_T>) {
				auto _save = _b;
				while (_b != _e) IRON_LIKELY
				{
					if (_f(*_b)) IRON_UNLIKELY
					{
						auto _b1 = _b;
						++_b1;
						while (_b1 != _e) IRON_LIKELY
						{
							if (!_f(*_b1)) IRON_UNLIKELY
							{
								break;
							}
							++_b1;
						}
						if (_b1 == _e) IRON_LIKELY
						{
							break;
						}
						_b = _b1;
					}
					++_b;
				}
				return _make_pair<_T>(_save, _b);
			}
			else {
				static_assert(iron::unsafe::asserts::always_false<_T>, "unknow charset");
			}
		}

		template <typename _T>
		[[nodiscard]] static constexpr std::pair<_T*, size_t> _rtrim_c(_T* _p, size_t _sz)noexcept {
			if constexpr (iron::concepts::basic_charset<_T>) {
				return rtrim_if(_p, _sz, [](char _v)noexcept -> bool { return iron::unsafe::unicode::is_ascii_whitespace(to_u32(_v)); });
			}
			else {
				return rtrim_if(_p, _sz, [](std::uint32_t _v)noexcept -> bool { return iron::chr::unchecked_from_u32(to_u32(_v)).is_whitespace(); });
			}
		}

		template <typename _T>
		[[nodiscard]]  static std::pair<_T*, size_t> _rtrim_nc(_T*, size_t)noexcept;

		template <typename _T>
		[[nodiscard]] static constexpr std::pair<_T*, size_t> _find_char_c(_T* _p, size_t _sz,std::uint32_t _ch)noexcept {
			auto _b = begin(_p, _sz);
			const auto _e = end(_p, _sz);
			while (_b != _e && to_u32(*_b) != _ch) IRON_LIKELY
			{
				++_b;
			}
			return _make_pair<_T>(_b,_e);
		}
		template <typename _T>
		[[nodiscard]] static std::pair<_T*, size_t> _find_char_nc(_T*, size_t,std::uint32_t)noexcept;

		template <typename _ITER, typename _ITER1>
		[[nodiscard]] static constexpr bool _find_m(_ITER _b, const _ITER& _e, _ITER1 _b1, const _ITER1& _e1)noexcept {
			while(_b != _e && _b1 != _e1) IRON_LIKELY
			{
				if (!(*_b == *_b1)) {
					return {};
				}
				++_b;
				++_b1;
			}
			return _b1 == _e1;
		}

		template <typename _T,typename _T1>
		[[nodiscard]] static constexpr std::pair<_T*, size_t> _find_c(_T* _p, size_t _sz,_T1* _pc,size_t _szc)noexcept
		requires(std::is_same_v<std::decay_t<_T>,std::decay_t<_T1>>)
		{
			auto _b1 = begin(_p, _sz);
			const auto _e1 = end(_p, _sz);
			const auto _b2 = begin(_pc, _szc);
			const auto _e2 = end(_pc, _szc);
			while (_b1 != _e1) IRON_LIKELY
			{
				if (_find_m(_b1,_e1,_b2,_e2)) IRON_UNLIKELY
				{
					break;
				}
				++_b1;
			}
			return _make_pair<_T>(_b1, _e1);
		}
		template <typename _T,typename _T1>
		[[nodiscard]] static std::pair<_T*, size_t> _find_nc(_T* _p, size_t _sz,_T1*, size_t)noexcept;

		template <typename _T>
		[[nodiscard]] static constexpr iron::option<size_t> _raw_in_c(const _T* _p, size_t _sz, const _T* _pc, size_t _szc)noexcept {
			if (_sz && _szc) IRON_LIKELY
			{
				auto _start = _p;
				while (_sz && _p != _pc) IRON_LIKELY
				{
					++_p;
					--_sz;
				}
				if (_sz < _szc) IRON_UNLIKELY
				{
					return {};
				}
				return iron::some(_distance(_start, _pc));
			}
			return {};
		}
		template <typename _T>
		[[nodiscard]] static iron::option<size_t> _raw_in_nc(const _T*, size_t, const _T*, size_t)noexcept;

		template <typename _T>
		[[nodiscard]] static iron::option<size_t> _chr_index_nc(const _T*, size_t, size_t)noexcept;

		template <typename _T>
		[[nodiscard]] static constexpr iron::option<size_t> _chr_index_c(const _T* _p, size_t _sz, size_t _raw_idx)noexcept
			requires(iron::concepts::varying_charset<_T>)
		{
			auto _b = begin(_p, _sz);
			const auto _e = end(_p, _sz);
			size_t _idx{};
			while (_b != _e) IRON_LIKELY
			{
				if (_distance(_p,_b._m_b) == _raw_idx) IRON_UNLIKELY
				{
					return iron::some(_idx);
				}
				++_idx;
				++_b;
			}
			return {};
		}

		template <typename _T>
		[[nodiscard]] static iron::option<size_t> _raw_index_nc(const _T*, size_t, size_t)noexcept;

		template <typename _T>
		[[nodiscard]] static constexpr iron::option<size_t> _raw_index_c(const _T* _p, size_t _sz, size_t _idx)noexcept
			requires(iron::concepts::varying_charset<_T>)
		{
			auto _b = begin(_p, _sz);
			const auto _e = end(_p, _sz);
			size_t _count{};
			while (_b != _e) IRON_LIKELY
			{
				if (_count == _idx) IRON_UNLIKELY
				{
					return iron::some(_distance(_p, _b._m_b));
				}
				++_b;
				++_count;
			}
			return {};
		}
		template <typename _T>
		[[nodiscard]] static bool _ends_with_char_var_nc(const _T*, size_t, std::uint32_t)noexcept;

		template <typename _T>
		[[nodiscard]] static constexpr bool _ends_with_char_var_c(const _T* _p, size_t _sz,std::uint32_t _ch)noexcept
		requires(iron::concepts::varying_charset<_T>)
		{
			auto _b = begin(_p, _sz);
			const auto _e = end(_p, _sz);
			while (_b != _e) IRON_LIKELY
			{
				if (to_u32(*_b) == _ch) IRON_UNLIKELY
				{
					++_b;
					if (_b == _e) IRON_UNLIKELY
					{
						return true;
					}
					continue;
				}
				++_b;
			}
			return {};
		}
		template <typename _T>
		[[nodiscard]] static constexpr bool _starts_with_c(const _T* _p, size_t _sz, const _T* _pc, size_t _szc)noexcept {
			if (_sz >= _szc) IRON_LIKELY
			{
				auto _b1 = begin(_p, _sz);
				const auto _e1 = end(_p, _sz);
				auto _b2 = begin(_pc, _szc);
				const auto _e2 = end(_pc, _szc);
				while (_b1 != _e1 && _b2 != _e2) IRON_LIKELY
				{
					if (*_b1 != *_b2) IRON_UNLIKELY
					{
						return {};
					}
					++_b1;
					++_b2;
				}
				return _b2 == _e2;
			}
			return {};
		}
		template <typename _T>
		[[nodiscard]] static bool _starts_with_nc(const _T*, size_t, const _T*, size_t)noexcept;

		template <typename _ITER>
		[[nodiscard]] static constexpr size_t _count(_ITER _b, const _ITER& _e)noexcept 
		requires(!std::contiguous_iterator<_ITER>)
		{
			size_t _c{};
			while (_b != _e) IRON_LIKELY
			{
				++_c;
				++_b;
			}
			return _c;
		}
		template <typename _ITER>
		[[nodiscard]] static constexpr bool _eq(_ITER& _b, const _ITER& _e, _ITER& _b1)noexcept {
			while (_b != _e) IRON_LIKELY
			{
				if (*_b != *_b1) IRON_UNLIKELY
				{
					return {};
				}
				++_b;
				++_b1;
			}
			return true;
		}
		template <typename _ITER>
		static constexpr void _advance(_ITER& _b, size_t _c)noexcept
		requires(!std::contiguous_iterator<_ITER>)
		{
			while (_c--) IRON_LIKELY
			{
				++_b;
			}
		}
		template <typename _T>
		[[nodiscard]] static constexpr bool _ends_with_c(const _T* _p, size_t _sz, const _T* _pc, size_t _szc)noexcept {
			if (!_szc) IRON_UNLIKELY
			{
				return true;
			}
			if (_sz < _szc) IRON_UNLIKELY
			{
				return {};
			}
			if constexpr (iron::concepts::fixed_charset<_T>) {
				_p += _sz - _szc;
				return _eq(_p, _p + _szc, _pc);
			}
			else if constexpr (iron::concepts::varying_charset<_T>) {
				auto _b1 = begin(_p, _sz);
				const auto _e1 = end(_p, _sz);
				auto _b2 = begin(_pc, _szc);
				const auto _e2 = end(_pc, _szc);
				const auto _c1 = _count(_b1, _e1);
				const auto _c2 = _count(_b2, _e2);
				_advance(_b1, _c1 - _c2);
				return _eq(_b1, _e1,_b2);
			}
			else {
				static_assert(iron::unsafe::asserts::always_false<_T>, "unknow charset");
			}
		}
		template <typename _T>
		[[nodiscard]] static bool _ends_with_nc(const _T*, size_t, const _T*, size_t)noexcept;

		template <typename _T1, typename _T2>
		[[nodiscard]] static constexpr bool _eq_c(const _T1* _pl, size_t _szl, const _T2* _pr, size_t _szr)noexcept {
			auto _bl = begin(_pl, _szl);
			const auto _el = end(_pl, _szl);
			auto _br = begin(_pr, _szr);
			const auto _er = end(_pr, _szr);
			while (_bl != _el && _br != _er) IRON_LIKELY
			{
				const auto _c1 = to_u32(*_bl);
				const auto _c2 = to_u32(*_br);
				if (_c1 != _c2)
				{
					return {};
				}
				++_bl;
				++_br;
			}
			return _bl == _el && _br == _er;
		}

		template <typename _T1, typename _T2>
		[[nodiscard]] static bool _eq_nc(const _T1*, size_t, const _T2*, size_t)noexcept;

		template <typename _T1, typename _T2>
		[[nodiscard]] static constexpr bool _lt_c(const _T1* _pl, size_t _szl, const _T2* _pr, size_t _szr)noexcept {
			auto _bl = begin(_pl, _szl);
			const auto _el = end(_pl, _szl);
			auto _br = begin(_pr, _szr);
			const auto _er = end(_pr, _szr);
			while (_bl != _el && _br != _er) IRON_LIKELY
			{
				const auto _c1 = to_u32(*_bl);
				const auto _c2 = to_u32(*_br);
				using _s_t = std::strong_ordering;
				const auto _r = _c1 <=> _c2;
				if (_r == _s_t::less) {
					return true;
				}
				if (_r == _s_t::greater) {
					return {};
				}
				DEBUG_ASSERT(_r == _s_t::equal);
				++_bl;
				++_br;
			}
			return _bl == _el && _br != _er;
		}
		template <typename _T1, typename _T2>
		[[nodiscard]] static bool _lt_nc(const _T1* _pl, size_t _szl, const _T2* _pr, size_t _szr)noexcept;

		template <typename _T1, typename _T2>
		[[nodiscard]] static constexpr compare_t _compare_c(const _T1* _pl, size_t _szl, const _T2* _pr, size_t _szr)noexcept {
			auto _bl = begin(_pl, _szl);
			const auto _el = end(_pl, _szl);
			auto _br = begin(_pr, _szr);
			const auto _er = end(_pr, _szr);
			using _s_t = std::strong_ordering;
			while (_bl != _el && _br != _er) IRON_LIKELY
			{
				const auto _c1 = to_u32(*_bl);
				const auto _c2 = to_u32(*_br);
				const auto _r = _c1 <=> _c2;
				if (_r == _s_t::less || _r == _s_t::greater) {
					return _r;
				}
				DEBUG_ASSERT(_r == _s_t::equal);
				++_bl;
				++_br;
			}
			if (_bl == _el) {
				return _br == _er ? _s_t::equal : _s_t::less;
			}
			return _s_t::greater;
		}

		template <typename _T1, typename _T2>
		[[nodiscard]] static compare_t _compare_nc(const _T1*, size_t, const _T2*, size_t)noexcept;

		template <transform_str_t _TR,bool _BASECP,size_t _N>
		[[nodiscard]]  static constexpr size_t _case_utf32(char32_t(&_out)[_N],std::uint32_t _in)noexcept 
		requires(_N > 0)
		{
			if constexpr (_BASECP) {
				const auto _ch = iron::chr::unchecked_from_u32(_in);
#ifdef IRON_EBCDIC
#error - convert ascii to ebcdic
#else

				if constexpr (_TR == transform_str_t::upper) {
					_out[0] = _ch.to_ascii_uppercase().to_utf32();
				}
				else if constexpr (_TR == transform_str_t::lower) {
					_out[0] = _ch.to_ascii_lowercase().to_utf32();
				}
				else {
					PANIC("unknow operation");
				}
#endif 
				return 1;
			}
			else {
				using _u_t = iron::unsafe::unicode;
				_u_t::seq_t _pair_iter;
				if constexpr (_TR == transform_str_t::upper) {
					_pair_iter = _u_t::uppers(_in);
				}
				else if constexpr (_TR == transform_str_t::lower) {
					_pair_iter = _u_t::lowers(_in);
				}
				else {
					PANIC("unknow operation");
				}
				auto _b = _pair_iter.first;
				const auto _e = _pair_iter.second;
				const auto _n = static_cast<size_t>(std::distance(_b, _e));
				if (!_n) IRON_UNLIKELY
				{
					_out[0] = static_cast<char32_t>(_in);
					return 1;
				}
				DEBUG_ASSERT(_n < _N);
				auto _p = &_out[0];
				while (_b != _e) IRON_LIKELY
				{
					*_p++ = static_cast<char32_t>(*_b++);
				}
				return _n;
			}
		}
		template <typename _T>
		[[nodiscard]] static constexpr size_t _append(_T*& _pd, size_t& _szd,const char32_t* _b)noexcept {
			constexpr size_t _max_size = 5;
			_T _codes[_max_size];
			auto _sz = _max_size;
			[[maybe_unused]] const auto _r=iron::unsafe::unicode_conversions::copy<false>(&_codes[0], _sz,_b,_b + 1);
			DEBUG_ASSERT(_r);
			DEBUG_ASSERT(_sz < _max_size);
			auto _p = &_codes[0];
			const auto _ret_sz = _sz;
			while (_sz--) IRON_LIKELY
			{
				if (_szd) IRON_LIKELY
				{
					if constexpr (iron::concepts::basic_charset<_T>) {
#ifdef IRON_EBCDIC
#error EBCDIC not implemented
#else 
						* _pd++ = *_p++;
#endif 
					}
					else {
						*_pd++ = *_p++;
					}
					--_szd;
				}
			}
			return _ret_sz;
		}
		template <transform_str_t _TR,typename _T>
		static constexpr void _case(_T* _p, size_t& _sz, const _T* _pc, size_t _szc)noexcept {
			auto _b = begin(_pc, _szc);
			const auto _e = end(_pc, _szc);
			size_t _count{};
			while (_b != _e) {
				const auto _c = to_u32(*_b++);
				char32_t _chars[10];
				auto _chars_sz=_case_utf32<_TR,iron::concepts::basic_charset<_T>>(_chars,_c);
				DEBUG_ASSERT(_chars_sz);
				auto _pch = &_chars[0];
				while (_chars_sz--) {
					_count += _append(_p, _sz, _pch++);
				}
			}
			_sz = _count;
		}
		template <typename _T>
		constexpr static void _to_upper_c(_T* _p, size_t& _sz, const _T* _pc, size_t _szc)noexcept {
			return _case<transform_str_t::upper>(_p, _sz, _pc, _szc);
		}

		template <typename _T>
		static void _to_upper_nc(_T* _p, size_t& _sz, const _T* _pc, size_t _szc)noexcept;

		template <typename _T>
		constexpr static void _to_lower_c(_T* _p, size_t& _sz, const _T* _pc, size_t _szc)noexcept {
			return _case<transform_str_t::lower>(_p, _sz, _pc, _szc);
		}
		template <typename _T>
		static void _to_lower_nc(_T* _p, size_t& _sz, const _T* _pc, size_t _szc)noexcept;

		template <bool _SAFE, typename _T, typename _U>
		[[nodiscard]] static bool _icopy(_T*, size_t&, const _U*, const _U*)noexcept;

	};

	template <> _IRON_DLL_EXPORT bool common::_copy(char*, size_t&, const char*, const char*)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_copy(char*, size_t&, const char8_t*, const char8_t*)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_copy(char*, size_t&, const char16_t*, const char16_t*)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_copy(char*, size_t&, const wchar_t*, const wchar_t*)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_copy(char*, size_t&, const char32_t*, const char32_t*)noexcept;

	template <> _IRON_DLL_EXPORT bool common::_copy(char8_t*, size_t&, const char*, const char*)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_copy(char8_t*, size_t&, const char8_t*, const char8_t*)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_copy(char8_t*, size_t&, const char16_t*, const char16_t*)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_copy(char8_t*, size_t&, const wchar_t*, const wchar_t*)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_copy(char8_t*, size_t&, const char32_t*, const char32_t*)noexcept;

	template <> _IRON_DLL_EXPORT bool common::_copy(char16_t*, size_t&, const char*, const char*)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_copy(char16_t*, size_t&, const char8_t*, const char8_t*)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_copy(char16_t*, size_t&, const char16_t*, const char16_t*)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_copy(char16_t*, size_t&, const wchar_t*, const wchar_t*)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_copy(char16_t*, size_t&, const char32_t*, const char32_t*)noexcept;

	template <> _IRON_DLL_EXPORT bool common::_copy(wchar_t*, size_t&, const char*, const char*)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_copy(wchar_t*, size_t&, const char8_t*, const char8_t*)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_copy(wchar_t*, size_t&, const char16_t*, const char16_t*)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_copy(wchar_t*, size_t&, const wchar_t*, const wchar_t*)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_copy(wchar_t*, size_t&, const char32_t*, const char32_t*)noexcept;

	template <> _IRON_DLL_EXPORT bool common::_copy(char32_t*, size_t&, const char*, const char*)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_copy(char32_t*, size_t&, const char8_t*, const char8_t*)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_copy(char32_t*, size_t&, const char16_t*, const char16_t*)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_copy(char32_t*, size_t&, const wchar_t*, const wchar_t*)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_copy(char32_t*, size_t&, const char32_t*, const char32_t*)noexcept;

	template <> _IRON_DLL_EXPORT bool common::_copy_safe(char*, size_t&, const char*, const char*)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_copy_safe(char*, size_t&, const char8_t*, const char8_t*)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_copy_safe(char*, size_t&, const char16_t*, const char16_t*)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_copy_safe(char*, size_t&, const wchar_t*, const wchar_t*)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_copy_safe(char*, size_t&, const char32_t*, const char32_t*)noexcept;

	template <> _IRON_DLL_EXPORT bool common::_copy_safe(char8_t*, size_t&, const char*, const char*)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_copy_safe(char8_t*, size_t&, const char8_t*, const char8_t*)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_copy_safe(char8_t*, size_t&, const char16_t*, const char16_t*)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_copy_safe(char8_t*, size_t&, const wchar_t*, const wchar_t*)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_copy_safe(char8_t*, size_t&, const char32_t*, const char32_t*)noexcept;

	template <> _IRON_DLL_EXPORT bool common::_copy_safe(char16_t*, size_t&, const char*, const char*)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_copy_safe(char16_t*, size_t&, const char8_t*, const char8_t*)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_copy_safe(char16_t*, size_t&, const char16_t*, const char16_t*)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_copy_safe(char16_t*, size_t&, const wchar_t*, const wchar_t*)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_copy_safe(char16_t*, size_t&, const char32_t*, const char32_t*)noexcept;

	template <> _IRON_DLL_EXPORT bool common::_copy_safe(wchar_t*, size_t&, const char*, const char*)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_copy_safe(wchar_t*, size_t&, const char8_t*, const char8_t*)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_copy_safe(wchar_t*, size_t&, const char16_t*, const char16_t*)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_copy_safe(wchar_t*, size_t&, const wchar_t*, const wchar_t*)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_copy_safe(wchar_t*, size_t&, const char32_t*, const char32_t*)noexcept;

	template <> _IRON_DLL_EXPORT bool common::_copy_safe(char32_t*, size_t&, const char*, const char*)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_copy_safe(char32_t*, size_t&, const char8_t*, const char8_t*)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_copy_safe(char32_t*, size_t&, const char16_t*, const char16_t*)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_copy_safe(char32_t*, size_t&, const wchar_t*, const wchar_t*)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_copy_safe(char32_t*, size_t&, const char32_t*, const char32_t*)noexcept;


	template <> _IRON_DLL_EXPORT bool common::_is_chr_boundary_varcharset_nc(const char8_t*, size_t, size_t)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_is_chr_boundary_varcharset_nc(const char16_t*, size_t, size_t)noexcept;
#ifdef _WIN32
	template <> _IRON_DLL_EXPORT bool common::_is_chr_boundary_varcharset_nc(const wchar_t*, size_t, size_t)noexcept;
#endif

	template <> _IRON_DLL_EXPORT iron::option<char32_t> common::_get_char_nc(const char8_t*, size_t, size_t)noexcept;
	template <> _IRON_DLL_EXPORT iron::option<char32_t> common::_get_char_nc(const char16_t*, size_t, size_t)noexcept;
#ifdef _WIN32
	template <> _IRON_DLL_EXPORT iron::option<char32_t> common::_get_char_nc(const wchar_t*, size_t, size_t)noexcept;
#endif

	template <> _IRON_DLL_EXPORT iron::option<char32_t> common::_get_char_back_nc(const char8_t*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT iron::option<char32_t> common::_get_char_back_nc(const char16_t*, size_t)noexcept;
#ifdef _WIN32
	template <> _IRON_DLL_EXPORT iron::option<char32_t> common::_get_char_back_nc(const wchar_t*, size_t)noexcept;
#endif

	template <> _IRON_DLL_EXPORT std::pair<char8_t*, size_t> common::_slice_var_nc(char8_t*, size_t, size_t, size_t)noexcept;
	template <> _IRON_DLL_EXPORT std::pair<const char8_t*, size_t> common::_slice_var_nc(const char8_t*, size_t, size_t, size_t)noexcept;
	template <> _IRON_DLL_EXPORT std::pair<char16_t*, size_t> common::_slice_var_nc(char16_t*, size_t, size_t, size_t)noexcept;
	template <> _IRON_DLL_EXPORT std::pair<const char16_t*, size_t> common::_slice_var_nc(const char16_t*, size_t, size_t, size_t)noexcept;
#ifdef _WIN32
	template <> _IRON_DLL_EXPORT std::pair<wchar_t*, size_t> common::_slice_var_nc(wchar_t*, size_t,size_t, size_t)noexcept;
	template <> _IRON_DLL_EXPORT std::pair<const wchar_t*, size_t> common::_slice_var_nc(const wchar_t*, size_t, size_t, size_t)noexcept;
#endif

	template <> _IRON_DLL_EXPORT std::pair<char*, size_t> common::_ltrim_nc(char*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT std::pair<const char*, size_t> common::_ltrim_nc(const char*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT std::pair<char8_t*, size_t> common::_ltrim_nc(char8_t*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT std::pair<const char8_t*, size_t> common::_ltrim_nc(const char8_t*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT std::pair<char16_t*, size_t> common::_ltrim_nc(char16_t*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT std::pair<const char16_t*, size_t> common::_ltrim_nc(const char16_t*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT std::pair<wchar_t*, size_t> common::_ltrim_nc(wchar_t*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT std::pair<const wchar_t*, size_t> common::_ltrim_nc(const wchar_t*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT std::pair<char32_t*, size_t> common::_ltrim_nc(char32_t*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT std::pair<const char32_t*, size_t> common::_ltrim_nc(const char32_t*, size_t)noexcept;

	template <> _IRON_DLL_EXPORT std::pair<char*, size_t> common::_rtrim_nc(char*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT std::pair<const char*, size_t> common::_rtrim_nc(const char*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT std::pair<char8_t*, size_t> common::_rtrim_nc(char8_t*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT std::pair<const char8_t*, size_t> common::_rtrim_nc(const char8_t*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT std::pair<char16_t*, size_t> common::_rtrim_nc(char16_t*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT std::pair<const char16_t*, size_t> common::_rtrim_nc(const char16_t*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT std::pair<wchar_t*, size_t> common::_rtrim_nc(wchar_t*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT std::pair<const wchar_t*, size_t> common::_rtrim_nc(const wchar_t*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT std::pair<char32_t*, size_t> common::_rtrim_nc(char32_t*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT std::pair<const char32_t*, size_t> common::_rtrim_nc(const char32_t*, size_t)noexcept;

	template <> _IRON_DLL_EXPORT std::pair<char*, size_t> common::_find_char_nc(char*, size_t, std::uint32_t)noexcept;
	template <> _IRON_DLL_EXPORT std::pair<const char*, size_t> common::_find_char_nc(const char*, size_t, std::uint32_t)noexcept;
	template <> _IRON_DLL_EXPORT std::pair<char8_t*, size_t> common::_find_char_nc(char8_t*, size_t, std::uint32_t)noexcept;
	template <> _IRON_DLL_EXPORT std::pair<const char8_t*, size_t> common::_find_char_nc(const char8_t*, size_t, std::uint32_t)noexcept;
	template <> _IRON_DLL_EXPORT std::pair<char16_t*, size_t> common::_find_char_nc(char16_t*, size_t, std::uint32_t)noexcept;
	template <> _IRON_DLL_EXPORT std::pair<const char16_t*, size_t> common::_find_char_nc(const char16_t*, size_t, std::uint32_t)noexcept;
	template <> _IRON_DLL_EXPORT std::pair<wchar_t*, size_t> common::_find_char_nc(wchar_t*, size_t, std::uint32_t)noexcept;
	template <> _IRON_DLL_EXPORT std::pair<const wchar_t*, size_t> common::_find_char_nc(const wchar_t*, size_t, std::uint32_t)noexcept;
	template <> _IRON_DLL_EXPORT std::pair<char32_t*, size_t> common::_find_char_nc(char32_t*, size_t, std::uint32_t)noexcept;
	template <> _IRON_DLL_EXPORT std::pair<const char32_t*, size_t> common::_find_char_nc(const char32_t*, size_t, std::uint32_t)noexcept;

	template <> _IRON_DLL_EXPORT std::pair<char*, size_t> common::_find_nc(char*, size_t, const char*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT std::pair<const char*, size_t> common::_find_nc(const char*, size_t, const char*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT std::pair<char8_t*, size_t> common::_find_nc(char8_t*, size_t, const char8_t*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT std::pair<const char8_t*, size_t> common::_find_nc(const char8_t*, size_t, const char8_t*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT std::pair<char16_t*, size_t> common::_find_nc(char16_t*, size_t, const char16_t*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT std::pair<const char16_t*, size_t> common::_find_nc(const char16_t*, size_t, const char16_t*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT std::pair<wchar_t*, size_t> common::_find_nc(wchar_t*, size_t, const wchar_t*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT std::pair<const wchar_t*, size_t> common::_find_nc(const wchar_t*, size_t, const wchar_t*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT std::pair<char32_t*, size_t> common::_find_nc(char32_t*, size_t, const char32_t*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT std::pair<const char32_t*, size_t> common::_find_nc(const char32_t*, size_t, const char32_t*, size_t)noexcept;

	template <> _IRON_DLL_EXPORT bool common::_ends_with_char_var_nc(const char8_t*, size_t, std::uint32_t)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_ends_with_char_var_nc(const char16_t*, size_t, std::uint32_t)noexcept;
#ifdef _WIN32
	template <> _IRON_DLL_EXPORT bool common::_ends_with_char_var_nc(const wchar_t*, size_t, std::uint32_t)noexcept;
#endif

	template <> _IRON_DLL_EXPORT bool common::_ends_with_nc(const char*, size_t, const char*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_ends_with_nc(const char8_t*, size_t, const char8_t*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_ends_with_nc(const char16_t*, size_t, const char16_t*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_ends_with_nc(const wchar_t*, size_t, const wchar_t*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_ends_with_nc(const char32_t*, size_t, const char32_t*, size_t)noexcept;

	template <> _IRON_DLL_EXPORT bool common::_starts_with_nc(const char*, size_t, const char*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_starts_with_nc(const char8_t*, size_t, const char8_t*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_starts_with_nc(const char16_t*, size_t, const char16_t*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_starts_with_nc(const wchar_t*, size_t, const wchar_t*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_starts_with_nc(const char32_t*, size_t, const char32_t*, size_t)noexcept;

	template <> _IRON_DLL_EXPORT auto common::_chr_index_nc(const char8_t*, size_t, size_t)noexcept -> iron::option<size_t>;
	template <> _IRON_DLL_EXPORT auto common::_chr_index_nc(const char16_t*, size_t, size_t)noexcept -> iron::option<size_t>;

#ifdef _WIN32
	template <> _IRON_DLL_EXPORT auto common::_chr_index_nc(const wchar_t*, size_t, size_t)noexcept -> iron::option<size_t>;
#endif

	template <> _IRON_DLL_EXPORT auto  common::_raw_in_nc(const char*, size_t, const char*, size_t)noexcept -> iron::option<size_t>;
	template <> _IRON_DLL_EXPORT auto  common::_raw_in_nc(const char8_t*, size_t, const char8_t*, size_t)noexcept -> iron::option<size_t>;
	template <> _IRON_DLL_EXPORT auto  common::_raw_in_nc(const char16_t*, size_t, const char16_t*, size_t)noexcept -> iron::option<size_t>;
	template <> _IRON_DLL_EXPORT auto  common::_raw_in_nc(const wchar_t*, size_t, const wchar_t*, size_t)noexcept -> iron::option<size_t>;
	template <> _IRON_DLL_EXPORT auto  common::_raw_in_nc(const char32_t*, size_t, const char32_t*, size_t)noexcept -> iron::option<size_t>;

	template <> _IRON_DLL_EXPORT auto  common::_raw_index_nc(const char8_t*, size_t, size_t)noexcept-> iron::option<size_t>;
	template <> _IRON_DLL_EXPORT auto  common::_raw_index_nc(const char16_t*, size_t, size_t)noexcept-> iron::option<size_t>;

#ifdef _WIN32
	template <> _IRON_DLL_EXPORT auto  common::_raw_index_nc(const wchar_t*, size_t, size_t)noexcept-> iron::option<size_t>;
#endif

	template <> _IRON_DLL_EXPORT bool common::_eq_nc(const char*, size_t, const char8_t*, size_t)noexcept;
	template <> inline bool common::_eq_nc(const char8_t* _pl, size_t _szl, const char* _pr, size_t _szr)noexcept {
		return _eq_nc(_pr, _szr, _pl, _szl);
	}
	template <> _IRON_DLL_EXPORT bool common::_eq_nc(const char*, size_t, const char16_t*, size_t)noexcept;
	template <> inline bool common::_eq_nc(const char16_t* _pl, size_t _szl, const char* _pr, size_t _szr)noexcept {
		return _eq_nc(_pr, _szr, _pl, _szl);
	}
	template <> _IRON_DLL_EXPORT bool common::_eq_nc(const char*, size_t, const wchar_t*, size_t)noexcept;
	template <> inline bool common::_eq_nc(const wchar_t* _pl, size_t _szl, const char* _pr, size_t _szr)noexcept {
		return _eq_nc(_pr, _szr, _pl, _szl);
	}
	template <> _IRON_DLL_EXPORT bool common::_eq_nc(const char*, size_t, const char32_t*, size_t)noexcept;
	template <> inline bool common::_eq_nc(const char32_t* _pl, size_t _szl, const char* _pr, size_t _szr)noexcept {
		return _eq_nc(_pr, _szr, _pl, _szl);
	}
	template <> _IRON_DLL_EXPORT bool common::_eq_nc(const char8_t*, size_t, const char16_t*, size_t)noexcept;
	template <> inline bool common::_eq_nc(const char16_t* _pl, size_t _szl, const char8_t* _pr, size_t _szr)noexcept {
		return _eq_nc(_pr, _szr, _pl, _szl);
	}
	template <> _IRON_DLL_EXPORT bool common::_eq_nc(const char8_t*, size_t, const wchar_t*, size_t)noexcept;
	template <> inline bool common::_eq_nc(const wchar_t* _pl, size_t _szl, const char8_t* _pr, size_t _szr)noexcept {
		return _eq_nc(_pr, _szr, _pl, _szl);
	}
	template <> _IRON_DLL_EXPORT bool common::_eq_nc(const char8_t*, size_t, const char32_t*, size_t)noexcept;
	template <> inline bool common::_eq_nc(const char32_t* _pl, size_t _szl, const char8_t* _pr, size_t _szr)noexcept {
		return _eq_nc(_pr, _szr, _pl, _szl);
	}
	template <> _IRON_DLL_EXPORT bool common::_eq_nc(const wchar_t*, size_t, const char16_t*, size_t)noexcept;
	template <> inline bool common::_eq_nc(const char16_t* _pl, size_t _szl, const wchar_t* _pr, size_t _szr)noexcept {
		return _eq_nc(_pr, _szr, _pl, _szl);
	}
	template <> _IRON_DLL_EXPORT bool common::_eq_nc(const wchar_t*, size_t, const char32_t*, size_t)noexcept;
	template <> inline bool common::_eq_nc(const char32_t* _pl, size_t _szl, const wchar_t* _pr, size_t _szr)noexcept {
		return _eq_nc(_pr, _szr, _pl, _szl);
	}
	template <> _IRON_DLL_EXPORT bool common::_eq_nc(const char16_t*, size_t, const char32_t*, size_t)noexcept;
	template <> inline bool common::_eq_nc(const char32_t* _pl, size_t _szl, const char16_t* _pr, size_t _szr)noexcept {
		return _eq_nc(_pr, _szr, _pl, _szl);
	}

	template <> _IRON_DLL_EXPORT bool common::_lt_nc(const char*, size_t, const char*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_lt_nc(const char*, size_t, const char8_t*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_lt_nc(const char*, size_t, const char16_t*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_lt_nc(const char*, size_t, const wchar_t*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_lt_nc(const char*, size_t, const char32_t*, size_t)noexcept;

	template <> _IRON_DLL_EXPORT bool common::_lt_nc(const char8_t*, size_t, const char*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_lt_nc(const char8_t*, size_t, const char8_t*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_lt_nc(const char8_t*, size_t, const char16_t*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_lt_nc(const char8_t*, size_t, const wchar_t*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_lt_nc(const char8_t*, size_t, const char32_t*, size_t)noexcept;

	template <> _IRON_DLL_EXPORT bool common::_lt_nc(const char16_t*, size_t, const char*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_lt_nc(const char16_t*, size_t, const char8_t*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_lt_nc(const char16_t*, size_t, const char16_t*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_lt_nc(const char16_t*, size_t, const wchar_t*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_lt_nc(const char16_t*, size_t, const char32_t*, size_t)noexcept;

	template <> _IRON_DLL_EXPORT bool common::_lt_nc(const wchar_t*, size_t, const char*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_lt_nc(const wchar_t*, size_t, const char8_t*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_lt_nc(const wchar_t*, size_t, const char16_t*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_lt_nc(const wchar_t*, size_t, const wchar_t*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_lt_nc(const wchar_t*, size_t, const char32_t*, size_t)noexcept;

	template <> _IRON_DLL_EXPORT bool common::_lt_nc(const char32_t*, size_t, const char*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_lt_nc(const char32_t*, size_t, const char8_t*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_lt_nc(const char32_t*, size_t, const char16_t*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_lt_nc(const char32_t*, size_t, const wchar_t*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT bool common::_lt_nc(const char32_t*, size_t, const char32_t*, size_t)noexcept;

	template <> _IRON_DLL_EXPORT auto common::_compare_nc(const char*, size_t, const char*, size_t)noexcept-> compare_t;
	template <> _IRON_DLL_EXPORT auto common::_compare_nc(const char*, size_t, const char8_t*, size_t)noexcept-> compare_t;
	template <> _IRON_DLL_EXPORT auto common::_compare_nc(const char*, size_t, const char16_t*, size_t)noexcept-> compare_t;
	template <> _IRON_DLL_EXPORT auto common::_compare_nc(const char*, size_t, const wchar_t*, size_t)noexcept-> compare_t;
	template <> _IRON_DLL_EXPORT auto common::_compare_nc(const char*, size_t, const char32_t*, size_t)noexcept-> compare_t;

	template <> _IRON_DLL_EXPORT auto common::_compare_nc(const char8_t*, size_t, const char*, size_t)noexcept-> compare_t;
	template <> _IRON_DLL_EXPORT auto common::_compare_nc(const char8_t*, size_t, const char8_t*, size_t)noexcept-> compare_t;
	template <> _IRON_DLL_EXPORT auto common::_compare_nc(const char8_t*, size_t, const char16_t*, size_t)noexcept-> compare_t;
	template <> _IRON_DLL_EXPORT auto common::_compare_nc(const char8_t*, size_t, const wchar_t*, size_t)noexcept-> compare_t;
	template <> _IRON_DLL_EXPORT auto common::_compare_nc(const char8_t*, size_t, const char32_t*, size_t)noexcept-> compare_t;

	template <> _IRON_DLL_EXPORT auto common::_compare_nc(const char16_t*, size_t, const char*, size_t)noexcept-> compare_t;
	template <> _IRON_DLL_EXPORT auto common::_compare_nc(const char16_t*, size_t, const char8_t*, size_t)noexcept-> compare_t;
	template <> _IRON_DLL_EXPORT auto common::_compare_nc(const char16_t*, size_t, const char16_t*, size_t)noexcept-> compare_t;
	template <> _IRON_DLL_EXPORT auto common::_compare_nc(const char16_t*, size_t, const wchar_t*, size_t)noexcept-> compare_t;
	template <> _IRON_DLL_EXPORT auto common::_compare_nc(const char16_t*, size_t, const char32_t*, size_t)noexcept-> compare_t;

	template <> _IRON_DLL_EXPORT auto common::_compare_nc(const wchar_t*, size_t, const char*, size_t)noexcept-> compare_t;
	template <> _IRON_DLL_EXPORT auto common::_compare_nc(const wchar_t*, size_t, const char8_t*, size_t)noexcept-> compare_t;
	template <> _IRON_DLL_EXPORT auto common::_compare_nc(const wchar_t*, size_t, const char16_t*, size_t)noexcept-> compare_t;
	template <> _IRON_DLL_EXPORT auto common::_compare_nc(const wchar_t*, size_t, const wchar_t*, size_t)noexcept-> compare_t;
	template <> _IRON_DLL_EXPORT auto common::_compare_nc(const wchar_t*, size_t, const char32_t*, size_t)noexcept-> compare_t;

	template <> _IRON_DLL_EXPORT auto common::_compare_nc(const char32_t*, size_t, const char*, size_t)noexcept-> compare_t;
	template <> _IRON_DLL_EXPORT auto common::_compare_nc(const char32_t*, size_t, const char8_t*, size_t)noexcept-> compare_t;
	template <> _IRON_DLL_EXPORT auto common::_compare_nc(const char32_t*, size_t, const char16_t*, size_t)noexcept-> compare_t;
	template <> _IRON_DLL_EXPORT auto common::_compare_nc(const char32_t*, size_t, const wchar_t*, size_t)noexcept-> compare_t;
	template <> _IRON_DLL_EXPORT auto common::_compare_nc(const char32_t*, size_t, const char32_t*, size_t)noexcept-> compare_t;

	template <> _IRON_DLL_EXPORT void common::_to_upper_nc(char*, size_t&, const char*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT void common::_to_upper_nc(char8_t*, size_t&, const char8_t*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT void common::_to_upper_nc(char16_t*, size_t&, const char16_t*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT void common::_to_upper_nc(wchar_t*, size_t&, const wchar_t*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT void common::_to_upper_nc(char32_t*, size_t&, const char32_t*, size_t)noexcept;

	template <> _IRON_DLL_EXPORT void common::_to_lower_nc(char*, size_t&, const char*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT void common::_to_lower_nc(char8_t*, size_t&, const char8_t*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT void common::_to_lower_nc(char16_t*, size_t&, const char16_t*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT void common::_to_lower_nc(wchar_t*, size_t&, const wchar_t*, size_t)noexcept;
	template <> _IRON_DLL_EXPORT void common::_to_lower_nc(char32_t*, size_t&, const char32_t*, size_t)noexcept;

} //ns


#if defined(_MSC_VER)
#pragma warning(pop)
#endif
