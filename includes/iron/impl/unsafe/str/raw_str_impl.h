#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable:26429)
#pragma warning (disable:26481)
#pragma warning (disable:26495)
#endif

namespace iron::unsafe::str {

	template <typename _T,typename _A,std::size_t _FIXED_SIZE,bool _SAFE>
	class raw_str final {
	public:
		using value_t = _T;
		using size_t = std::size_t;
		using allocator_t = _A;
		using error_handler_t = iron::error_handler;
		using rs_t = iron::unsafe::raw_storage<value_t>;
		using adapter_box_t = iron::unsafe::raw_vector<value_t,allocator_t>;
		using view_t = iron::unsafe::str::raw_strview<value_t, _SAFE>;
		using cview_t = iron::unsafe::str::raw_strview<const value_t, _SAFE>;
		using common_t = typename view_t::common_t;
		[[nodiscard]] static constexpr size_t fixed_size()noexcept { return _FIXED_SIZE; }
		[[nodiscard]] static constexpr bool is_safe()noexcept { return _SAFE; }
		[[nodiscard]] static constexpr size_t max_size()noexcept { return rs_t::max_size() - 1; }
		struct raw_str_tag { raw_str_tag() = delete; };
		template <typename _U, std::size_t _N>
		[[nodiscard]] static constexpr size_t array_size(const _U(&_v)[_N])noexcept { return view_t::array_size(_v); }
		[[nodiscard]] static constexpr iron::result<raw_str, error_handler_t> from(adapter_box_t&& _box)noexcept {
			DEBUG_ASSERT(_box.size());
			raw_str _t(_box.allocator());
			if constexpr (is_safe()) {
				value_t* _p{};
				size_t _sz{};
				auto _boxp = _box.data();
				[[maybe_unused]] const auto _r = common_t:: template copy<true>(_p, _sz, _boxp, _boxp + _box.size());
				if (!_r) IRON_UNLIKELY
				{
					return iron::err(ERROR_HANDLER(iron::unsafe::errors::posix::code_t::illegal_byte_sequence));
				}
			}
			if (_box.capacity() == _box.size()) IRON_UNLIKELY
			{
#ifndef IRON_NOCHECK_SIZE
				if (_box.size() >= max_size()) IRON_UNLIKELY
				{
					return iron::err(ERROR_HANDLER(iron::unsafe::errors::posix::code_t::result_out_of_range));
				}
#endif 
				auto _h = _box.reserve(_box.capacity() + 1); RR(_h);
			}
			[[maybe_unused]] const auto _h = _box.push_back(value_t{}); //add \0
			DEBUG_ASSERT(_h);
			_t._m_sz = _box.size() - 1;
			_t._m_heap_area._capacity = _box.capacity();
			_t._m_heap_area._p = _box.release();
			return iron::ok(std::move(_t));
		}
		constexpr raw_str()noexcept {
			_init_stackarea();
		}
		constexpr explicit raw_str(allocator_t _a)noexcept
			:_m_a(_a)
		{
			_init_stackarea();
		}
		raw_str(const raw_str&) = delete;
		raw_str& operator=(const raw_str&) = delete;
		constexpr raw_str(raw_str&& _v)noexcept {
			_init_stackarea();
			_swap(_v);
		}
		constexpr raw_str& operator=(raw_str&& _v)noexcept {
			_swap(_v);
			return *this;
		}
		constexpr ~raw_str() {
			_reset();
		}
		constexpr void swap(raw_str& _v)noexcept {
			_swap(_v);
		}
		[[nodiscard]] constexpr allocator_t allocator()const noexcept { return _m_a; }
		[[nodiscard]] constexpr size_t size()const noexcept { return _m_sz; }
		[[nodiscard]] constexpr size_t capacity()const noexcept { return _m_heap_area._p ? _m_heap_area._capacity : fixed_size(); }
		[[nodiscard]] constexpr value_t* data()noexcept { return _data(); }
		[[nodiscard]] constexpr const value_t* data()const noexcept { return _data(); }
		[[nodiscard]] constexpr iron::result<raw_str,error_handler_t> try_clone(size_t _n = 1)const noexcept {
			return _clone(_n);
		}
		template <bool _EXACT>
		[[nodiscard]] constexpr error_handler_t check_size(size_t& _newc,size_t _n, size_t _addictional)const noexcept {
			return _check_size<_EXACT>(_newc,_n, _addictional);
		}
		[[nodiscard]] constexpr raw_str clone(size_t _n=1)const noexcept {
			auto _r = try_clone(_n); RP(_r);
		}
		[[nodiscard]] constexpr error_handler_t reserve(size_t _abs_capacity)noexcept {
			DEBUG_ASSERT(_abs_capacity > capacity());
			return _reserve(_abs_capacity);
		}
		constexpr void reset()noexcept {
			_reset();
		}
		template <bool _RESERVE>
		[[nodiscard]] constexpr error_handler_t push_back(size_t _n,value_t _u)noexcept
		{
			return _push_back<_RESERVE>(_n,_u);
		}
		template <bool _RESERVE,typename _ITER>
		[[nodiscard]] constexpr error_handler_t push_back(size_t _n,_ITER& _b, const _ITER& _e)noexcept {
			return _push_back<_RESERVE>(_n,_b, _e);
		}
		[[nodiscard]] constexpr  bool use_heap()const noexcept { return _m_heap_area._p ? true : false; }
		[[nodiscard]] constexpr error_handler_t realloc_on_heap()noexcept {
			if (!use_heap() && _m_sz) IRON_LIKELY
			{
				return _reserve(_m_sz + 1);
			}
			return {};
		}
		[[nodiscard]] constexpr iron::result<adapter_box_t, error_handler_t> release()noexcept {
			auto _h = realloc_on_heap(); RR(_h);
			adapter_box_t _t(_m_heap_area._p, _m_heap_area._capacity, _m_sz,_m_a);
			_m_heap_area._p = {};
			_m_heap_area._capacity = _m_sz = {};
			return iron::ok(std::move(_t));
		}
		[[nodiscard]] constexpr error_handler_t raw_resize(size_t _n, value_t _v = {})noexcept {
			return _raw_resize(_n, _v);
		}
		constexpr void clear()noexcept {
			_m_sz = {};
		}
		[[nodiscard]] constexpr value_t nth(size_t _idx)const noexcept {
			DEBUG_ASSERT(_idx < size());
			return _data()[_idx];
		}
		[[nodiscard]] constexpr view_t view()noexcept {
			return view_t(_data(), _m_sz);
		}
		[[nodiscard]] constexpr cview_t view()const noexcept {
			return cview_t(_data(), _m_sz);
		}
		template <typename _T1, typename _A1, std::size_t _FIXED_SIZE1, bool _SAFE1>
		[[nodiscard]] constexpr bool operator==(const raw_str<_T1, _A1, _FIXED_SIZE1, _SAFE1>& _v)const noexcept {
			return view() == _v.view();
		}
		template <typename _U, std::size_t _N>
		[[nodiscard]] constexpr bool operator==(const _U(&_v)[_N])const noexcept
		{
			return view() == _v;
		}
		template <typename _T1, typename _A1, std::size_t _FIXED_SIZE1, bool _SAFE1>
		[[nodiscard]] constexpr bool operator <(const raw_str<_T1, _A1, _FIXED_SIZE1, _SAFE1>& _v)const noexcept {
			return view() < _v.view();
		}
		template <typename _U, std::size_t _N>
		[[nodiscard]] constexpr bool operator <(const _U(&_v)[_N])const noexcept
		{
			return view() < _v;
		}

		template <typename _T1, typename _A1, std::size_t _FIXED_SIZE1, bool _SAFE1>
		[[nodiscard]] constexpr std::strong_ordering operator <=>(const raw_str<_T1, _A1, _FIXED_SIZE1, _SAFE1>& _v)const noexcept {
			return view() <=> _v.view();
		}
		template <typename _U, std::size_t _N>
		[[nodiscard]] constexpr std::strong_ordering operator <=>(const _U(&_v)[_N])const noexcept
		{
			return view() <=> _v;
		}

		[[nodiscard]] constexpr iron::result<raw_str, error_handler_t> to_upper()const noexcept {
			return _transform_str<common_t::transform_str_t::upper>();
		}
		[[nodiscard]] constexpr iron::result<raw_str, error_handler_t> to_lower()const noexcept {
			return _transform_str<common_t::transform_str_t::lower>();
		}
		[[nodiscard]]  constexpr error_handler_t shrink_to_fit()noexcept {
			if (auto& _p = _m_heap_area._p) IRON_LIKELY
			{
				DEBUG_ASSERT(use_heap() && _m_heap_area._capacity);
				return rs_t::shrink_to_fit(_p, _m_heap_area._capacity, _m_sz + 1, _m_a);
			}
			return {};
		}
	private:
		constexpr void _init_stackarea()noexcept {
			if constexpr (fixed_size()) {
				[[maybe_unused]] const auto _h = rs_t::uninitialized_fill(&(_m_stack_area._v[0]), fixed_size(), value_t{});
				DEBUG_ASSERT(_h);
			}
		}
		constexpr void _swap(raw_str& _v)noexcept {
			if constexpr (fixed_size()) {
				std::swap(_m_stack_area._v, _v._m_stack_area._v);
			}
			std::swap(_m_heap_area._p, _v._m_heap_area._p);
			std::swap(_m_heap_area._capacity, _v._m_heap_area._capacity);
			std::swap(_m_sz, _v._m_sz);
			std::swap(_m_a, _v._m_a);
		}
		constexpr void _reset()noexcept {
			if (_m_heap_area._p) IRON_UNLIKELY 
			{
				DEBUG_ASSERT(_m_heap_area._capacity);
				if (std::is_constant_evaluated()) {
					rs_t::destroy_n(_m_heap_area._p, _m_heap_area._capacity);
				}
				rs_t::deallocate(_m_heap_area._p, _m_heap_area._capacity,_m_a);
				_m_heap_area._p = {};
				_m_heap_area._capacity = {};
			}
			else {
				if (std::is_constant_evaluated()) {
					if constexpr (fixed_size()) {
						rs_t::destroy_n(&(_m_stack_area._v[0]), fixed_size());
					}
				}
			}
			_m_sz = {};
		}
		[[nodiscard]] constexpr value_t* _data()noexcept {
			if (_m_heap_area._p) IRON_UNLIKELY
			{
				return _m_heap_area._p;
			}
			if constexpr (fixed_size()) {
				return &(_m_stack_area._v[0]);
			}
			return nullptr;
		}
		[[nodiscard]] constexpr const value_t* _data()const noexcept {
			if (_m_heap_area._p) IRON_UNLIKELY
			{
				return _m_heap_area._p;
			}
			if constexpr (fixed_size()) {
				return &(_m_stack_area._v[0]);
			}
			return nullptr;
		}
		template <bool _EXACT>
		[[nodiscard]] constexpr error_handler_t _check_size(size_t& _newc,size_t _n, size_t _addictional)const noexcept {
#ifndef IRON_NOCHECK_SIZE
			using _usizeb_t = iron::usizeb;
			const auto _x = (_usizeb_t(_n) * _usizeb_t(_addictional)) + _usizeb_t(_m_sz) + _usizeb_t(1); //+1 is for \0 final
			if (!_x || _x.unchecked_value() > max_size()) IRON_UNLIKELY
			{
				return ERROR_HANDLER(iron::unsafe::errors::posix::code_t::result_out_of_range);
			}
			_newc = _x.unchecked_value();
#else 
			_newc = _n * _addictional + _m_rs.size() + 1;
#endif 
			if constexpr (!_EXACT) {
				if (_newc < max_size() / 2) IRON_LIKELY
				{
					_newc *= 2;
				}
			}
			return {};
		}
		[[nodiscard]] constexpr error_handler_t _reserve_c(size_t _abs_capacity)noexcept {
			value_t* _pd{};
			auto _h = rs_t::allocate(_pd, _abs_capacity, _m_a); RE(_h);
			if (std::is_constant_evaluated()) {
				_h = rs_t::uninitialized_fill(_pd, _abs_capacity, value_t{});
				DEBUG_ASSERT(_h);
			}
			auto _ps = _data();
			_h = rs_t::assign_n(_pd, _m_sz + 1, _ps);
			DEBUG_ASSERT(_h);
			if (_m_heap_area._p) IRON_UNLIKELY
			{
				if (std::is_constant_evaluated()) {
					rs_t::destroy_n(_m_heap_area._p, _m_heap_area._capacity);
				}
				rs_t::deallocate(_m_heap_area._p, _m_heap_area._capacity,_m_a);
			}
			_m_heap_area._p = _pd;
			_m_heap_area._capacity = _abs_capacity;
			return _h;
		}
		[[nodiscard]] _IRON_DLL_EXPORT error_handler_t _reserve_nc(size_t)noexcept;
		[[nodiscard]] constexpr error_handler_t _reserve(size_t _abs_capacity)noexcept {
			if (std::is_constant_evaluated()) {
				return _reserve_c(_abs_capacity);
			}
			else {
				if constexpr (
					fixed_size() == iron::str_default_fixedsize 
					&& iron::concepts::iron_allocator<allocator_t> 
					&& std::is_same_v<typename allocator_t::value_t,value_t>
				) 
				{
					return _reserve_nc(_abs_capacity);
				}
				else {
					return _reserve_c(_abs_capacity);
				}
			}
		}
		[[nodiscard]] constexpr iron::result<raw_str, error_handler_t> _clone(size_t _n)const noexcept {
			raw_str _t(_m_a);
			if (_n && _m_sz) IRON_LIKELY 
			{
				size_t _newc{};
				constexpr bool _exact = true;
				auto _h = _t.template _check_size<_exact>(_newc, _n, _m_sz); RR(_h);
				if (_newc > _t.capacity()) IRON_UNLIKELY 
				{
					_h = _t.reserve(_newc); RR(_h);
				}
				auto _ps = _data();
				auto _pd = _t._data();
				while (_n--) IRON_LIKELY 
				{
					_pd = _assign(_pd, _ps, _ps + _m_sz);
				}
				*_pd = {};
				_t._m_sz = _newc - 1;
			}
			return iron::ok(std::move(_t));
		}
		constexpr void _push_back_same(size_t _n,value_t _c)noexcept {
			DEBUG_ASSERT(_m_sz + _n < capacity());
			auto _p = _data() + _m_sz;
			[[maybe_unused]] const auto _h = rs_t::assign_fill(_p, _n, value_t{ _c });
			DEBUG_ASSERT(_h);
			_p[_n] = {};
			_m_sz += _n;
		}
		template <bool _RESERVE>
		[[nodiscard]] constexpr error_handler_t _push_back(size_t _n,value_t _v)noexcept
		{
			if constexpr (_RESERVE) {
				size_t _newc{};
				constexpr bool _exact = true;
				auto _h = _check_size<_exact>(_newc, _n,1); RR(_h);
				if (capacity() < _newc) IRON_LIKELY
				{
					_h = _reserve(_newc); RE(_h);
				}
			}
			_push_back_same(_n, _v);
			return {};
		}
		template <typename _ITER>
		[[nodiscard]] static constexpr value_t* _assign(value_t* _p,_ITER _b, const _ITER& _e)noexcept {
			while (_b != _e) IRON_LIKELY
			{
				*_p++ = *_b++;
			}
			return _p;
		}
		template <bool _RESERVE,typename _ITER>
		[[nodiscard]] constexpr error_handler_t _push_back(size_t _n,_ITER& _b,const _ITER& _e)noexcept
		requires(std::is_same_v<value_t, std::decay_t<decltype(*_b)>>)
		{
			DEBUG_ASSERT(_n);
			DEBUG_ASSERT(_b != _e);
			if constexpr (_RESERVE) {
				value_t* _pd{};
				size_t _sz{};
				auto _bb = _b;
				[[maybe_unused]] const auto _r = common_t::template copy<is_safe()>(_pd, _sz, _bb, _e);
				if constexpr (is_safe()) {
					if (!_r) IRON_UNLIKELY
					{
						return ERROR_HANDLER(iron::unsafe::errors::posix::code_t::illegal_byte_sequence);
					}
				}
				constexpr bool _exact = true;
				size_t _newc{};
				auto _h = check_size<_exact>(_newc, _n, _sz); RE(_h);
				if (_newc > capacity()) IRON_UNLIKELY
				{
					_h = _reserve(_newc); RE(_h);
				}
			}
			else if constexpr (is_safe()) {
				value_t* _pd{};
				size_t _sz{};
				auto _bb = _b;
				const auto _r = common_t:: template copy<true>(_pd, _sz, _bb, _e);
				if (!_r) IRON_UNLIKELY 
				{
					return ERROR_HANDLER(iron::unsafe::errors::posix::code_t::illegal_byte_sequence);
				}
			}
			auto _p = _data() + _m_sz;
			while (_n--) IRON_LIKELY
			{
				auto _pd = _assign(_p, _b, _e);
				_m_sz += static_cast<size_t>(_pd - _p);
				_p = _pd;
			}
			*_p = {};
			return {};
		}
		[[nodiscard]] constexpr error_handler_t _raw_resize(size_t _n, value_t _v = {})noexcept {
			if (_n != _m_sz) IRON_LIKELY
			{
				if (_n > _m_sz) {
					const auto _diff = _n - _m_sz;
					constexpr bool _reserve = true;
					return _push_back<_reserve>(_diff, _v); 
				}
				_m_sz = _n;
				auto _p = _data();
				_p[_m_sz] = {};
			}
			return {};
		}
		template <common_t::transform_str_t _TR>
		[[nodiscard]] constexpr iron::result<raw_str, error_handler_t> _transform_str()const noexcept {
			raw_str _t(_m_a);
			if (size()) IRON_LIKELY
			{
				if (size() >= fixed_size()) IRON_UNLIKELY
				{
					auto _h = _t.reserve(size() + 1); RR(_h);
				}
				auto _pd = _t._data();
				auto _szd = size();
				auto _ps = _data();
				common_t::template transform_str<_TR>(_pd, _szd, _ps, size());
				if (_szd > size() && _szd >= fixed_size()) IRON_UNLIKELY
				{
					auto _h = _t.reserve(_szd + 1); RR(_h);
					_pd = _t._data();
					common_t::template transform_str<_TR>(_pd, _szd, _ps, size());
				}
				_pd[_szd] = {};
				_t._m_sz = _szd;
			}
			return iron::ok(std::move(_t));
		}

		typedef struct stack_area_empty {} stack_area_empty_t;
		typedef struct stack_area_sized {
			value_t _v[fixed_size()];
		} stack_area_sized_t;
		using stack_area_t = std::conditional_t<fixed_size() != 0, stack_area_sized_t, stack_area_empty_t>;
		typedef struct heap_area {
			value_t* _p{};
			size_t _capacity{};
		} heap_area_t;
		heap_area_t _m_heap_area{};
		size_t _m_sz{};
		IRON_NO_UNIQUE_ADDRESS stack_area_t _m_stack_area;
		IRON_NO_UNIQUE_ADDRESS allocator_t _m_a{};
	};


	template <> _IRON_DLL_EXPORT auto raw_str<char, iron::unsafe::allocator<char>, iron::str_default_fixedsize, false>::_reserve_nc(size_t)noexcept -> error_handler_t;
	template <> _IRON_DLL_EXPORT auto raw_str<char, iron::unsafe::allocator<char>, iron::str_default_fixedsize, true>::_reserve_nc(size_t)noexcept -> error_handler_t;

	template <> _IRON_DLL_EXPORT auto raw_str<char8_t, iron::unsafe::allocator<char8_t>, iron::str_default_fixedsize, false>::_reserve_nc(size_t)noexcept -> error_handler_t;
	template <> _IRON_DLL_EXPORT auto raw_str<char8_t, iron::unsafe::allocator<char8_t>, iron::str_default_fixedsize, true>::_reserve_nc(size_t)noexcept -> error_handler_t;

	template <> _IRON_DLL_EXPORT auto raw_str<char16_t, iron::unsafe::allocator<char16_t>, iron::str_default_fixedsize, false>::_reserve_nc(size_t)noexcept -> error_handler_t;
	template <> _IRON_DLL_EXPORT auto raw_str<char16_t, iron::unsafe::allocator<char16_t>, iron::str_default_fixedsize, true>::_reserve_nc(size_t)noexcept -> error_handler_t;

	template <> _IRON_DLL_EXPORT auto raw_str<wchar_t, iron::unsafe::allocator<wchar_t>, iron::str_default_fixedsize, false>::_reserve_nc(size_t)noexcept -> error_handler_t;
	template <> _IRON_DLL_EXPORT auto raw_str<wchar_t, iron::unsafe::allocator<wchar_t>, iron::str_default_fixedsize, true>::_reserve_nc(size_t)noexcept -> error_handler_t;

	template <> _IRON_DLL_EXPORT auto raw_str<char32_t, iron::unsafe::allocator<char32_t>, iron::str_default_fixedsize, false>::_reserve_nc(size_t)noexcept -> error_handler_t;
	template <> _IRON_DLL_EXPORT auto raw_str<char32_t, iron::unsafe::allocator<char32_t>, iron::str_default_fixedsize, true>::_reserve_nc(size_t)noexcept -> error_handler_t;
}

#if defined(_MSC_VER)
#pragma warning(pop)
#endif
