#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable:26429)
#pragma warning (disable:26459)
#pragma warning (disable:26481)
#pragma warning (disable:26496)
#endif

namespace iron::unsafe {

	template <typename _T, typename _A>
	class raw_vector final {
		public:
			using value_t = _T;
			using allocator_t = _A;
			using ptr_t = value_t*;
			using pointer = ptr_t;
			using cptr_t = const value_t*;
			using const_pointer = cptr_t;
			using size_t = std::size_t;
			using size_type = size_t;
			using diff_t = std::ptrdiff_t;
			using raw_storage_t = iron::unsafe::raw_storage<value_t>;
			using error_handler_t = raw_storage_t::error_handler_t;
			using reference = value_t&;
			using const_reference = const value_t&;
			[[nodiscard]] static constexpr size_t max_size()noexcept { return raw_storage_t::max_size(); }
			struct raw_vector_tag { raw_vector_tag() = delete; };
			constexpr raw_vector()noexcept = default;
			constexpr explicit  raw_vector(allocator_t _a)noexcept : _m_a(_a) {}
			raw_vector(const raw_vector&) = delete;
			raw_vector& operator=(const raw_vector&) = delete;
			constexpr raw_vector(raw_vector&& _v)noexcept
			{
				_swap(_v);
			}
			constexpr raw_vector& operator=(raw_vector&& _v)noexcept {
				_swap(_v);
				return *this;
			}
			constexpr ~raw_vector() { _reset(); }

			constexpr raw_vector(value_t* _p,size_t _capacity,size_t _sz,allocator_t _a)noexcept
			: _m_p(_p)
			, _m_c(_capacity)
			, _m_sz(_sz)
			, _m_a(_a)
			{
				DEBUG_ASSERT((_p && _capacity) || (!_p && !_capacity));
				DEBUG_ASSERT(_sz <= _capacity);
			}

			constexpr void swap(raw_vector& _v)noexcept {
				_swap(_v);
			}
			[[nodiscard]] constexpr size_t capacity()const noexcept {
				DEBUG_ASSERT(_m_c >= _m_sz);
				return _m_c;
			}
			[[nodiscard]] constexpr size_t size()const noexcept {
				DEBUG_ASSERT(_m_c >= _m_sz);
				return _m_sz;
			}
			[[nodiscard]] constexpr bool empty()const noexcept {
				return _m_p ? false : true;
			}
			[[nodiscard]] constexpr explicit operator bool()const noexcept { return !empty(); }
			constexpr raw_vector& reset()noexcept {
				_reset();
				return *this;
			}
			[[nodiscard]] constexpr cptr_t data()const noexcept {
				return _m_p;
			}
			[[nodiscard]] constexpr ptr_t data()noexcept {
				return _m_p;
			}

			[[nodiscard]] constexpr allocator_t allocator()const noexcept {
				return _m_a;
			}

			[[nodiscard]] constexpr error_handler_t reserve(size_t _abs_capacity)noexcept {
				DEBUG_ASSERT(_abs_capacity > capacity());
				return _reserve(_abs_capacity);
			}

			[[nodiscard]] constexpr ptr_t release()noexcept {
				return _release();
			}
			template <typename... _ARGS>
			[[nodiscard]] constexpr error_handler_t push_back(_ARGS&&... _args)noexcept
			{
				DEBUG_ASSERT(size() < capacity());
				return _push_emplace(std::forward<_ARGS>(_args)...);
			}
			template <bool _TRANSACTIONAL,typename _ITER>
			[[nodiscard]] constexpr iron::result<ptr_t, error_handler_t> range_push_back(_ITER& _b, const _ITER& _e)noexcept
			{
				return _range_push_back<_TRANSACTIONAL>(_b, _e);
			}
			[[nodiscard]] constexpr ptr_t nth(size_t _idx)noexcept {
				DEBUG_ASSERT(_idx < size());
				return _nth(_idx);
			}
			[[nodiscard]] constexpr const ptr_t nth(size_t _idx)const noexcept {
				DEBUG_ASSERT(_idx < size());
				return _nth(_idx);
			}

			[[nodiscard]] constexpr ptr_t front()noexcept {
				DEBUG_ASSERT(size());
				return _nth(0);
			}
			[[nodiscard]] constexpr const ptr_t front()const noexcept {
				DEBUG_ASSERT(size());
				return _nth(0);
			}
			[[nodiscard]] constexpr ptr_t back()noexcept {
				DEBUG_ASSERT(size());
				return nth(_m_sz - 1);
			}
			[[nodiscard]] constexpr const ptr_t back()const noexcept {
				DEBUG_ASSERT(size());
				return nth(_m_sz - 1);
			}
			constexpr value_t pop_back()noexcept
			requires (std::is_nothrow_move_constructible_v<value_t>)
			{
				DEBUG_ASSERT(size());
				return _pop_back();
			}
			[[nodiscard]] constexpr error_handler_t shrink_to_fit()noexcept {
				return raw_storage_t::shrink_to_fit(_m_p, _m_c, _m_sz, _m_a);
			}
			constexpr raw_vector& clear()noexcept {
				_clear();
				DEBUG_ASSERT(!_m_sz);
				return *this;
			}
			[[nodiscard]] constexpr iron::result<raw_vector, error_handler_t> try_clone(size_t _n=1)const noexcept
			requires(iron::concepts::copyable_or_cloneable<value_t>)
			{
				DEBUG_ASSERT(size() && _n);
				return _clone(_n);
			}
			[[nodiscard]] constexpr raw_vector clone(size_t _n = 1)const noexcept
			requires(iron::concepts::copyable_or_cloneable<value_t>)
			{
				auto _r = try_clone(_n); RP(_r);
			}

			[[nodiscard]] constexpr error_handler_t resize(size_t _n)noexcept
			requires(std::is_default_constructible_v<value_t>)
			{
				DEBUG_ASSERT(_n && _n != size());
				return _resize(_n);
			}
			[[nodiscard]] constexpr error_handler_t resize(size_t _n,const value_t& _v)noexcept
			requires(iron::concepts::copyable_or_cloneable<value_t>)
			{
				DEBUG_ASSERT(_n && _n != size());
				return _resize(_n,_v);
			}

			template <typename _A1>
			[[nodiscard]] inline constexpr bool operator==(const raw_vector<value_t, _A1>& _v)const noexcept
				requires (iron::concepts::nothrow_equality_comparable<value_t> && iron::concepts::iron_allocator<_A1>)
			{
				return size() == _v.size()
					? std::equal(_m_p,_m_p + _m_sz, _v._m_p)
					: false
					;
			}

		private:
			template <typename _T1,typename _A1>
			friend class raw_vector;
			[[nodiscard]] constexpr value_t* const _release()noexcept {
				value_t* _p{};
				std::swap(_p, _m_p);
				_m_c = _m_sz = {};
				return _p;
			}
			[[nodiscard]] constexpr ptr_t _nth(size_t _idx)noexcept {
				return _m_p + _idx;
			}
			[[nodiscard]] constexpr const ptr_t _nth(size_t _idx)const noexcept {
				return _m_p + _idx;
			}
			constexpr void _swap(raw_vector& _v)noexcept {
				std::swap(_m_p, _v._m_p);
				std::swap(_m_c, _v._m_c);
				std::swap(_m_sz, _v._m_sz);
				std::swap(_m_a, _v._m_a);
			}
			template <typename... _ARGS >
			[[nodiscard]] constexpr error_handler_t _push_emplace(_ARGS&&... _args)noexcept {
				DEBUG_ASSERT(_m_sz < _m_c);
				auto _h = raw_storage_t::construct_at(_m_p + _m_sz, std::forward<_ARGS>(_args)...); RE(_h);
				++_m_sz;
				return {};
			}
			template <typename _ITER>
			[[nodiscard]] constexpr error_handler_t _range_push_back_x(size_t& _c,_ITER& _b, const _ITER& _e)noexcept {
				while (_b != _e) IRON_LIKELY {
					if constexpr (iron::concepts::constructible_at<value_t,decltype(*_b)>) {
						auto _h = _push_emplace(*_b); RE(_h);
					}
					else {
						auto _r = iron::clone::try_clone_from(*_b); RE(_r);
						auto _h = _push_emplace(std::move(_r.unchecked_unwrap())); RE(_h);
					}
					++_c;
					++_b;
				}
				return {};
			}

			template <bool _TRANSACTIONAL,typename _ITER>
			[[nodiscard]] constexpr iron::result<ptr_t,error_handler_t> _range_push_back(_ITER& _b,const _ITER& _e)noexcept {
				const auto _n = _m_sz;
				size_t _c{};
				auto _h = _range_push_back_x(_c, _b, _e);
				if (!_h) IRON_UNLIKELY 
				{
					if constexpr (_TRANSACTIONAL) {
						raw_storage_t::destroy_n(_m_p + _n,_c);
						_m_sz = _n;
					}
					return iron::err(std::move(_h));
				}
				return iron::ok(_m_p + _n);
			}
			[[nodiscard]] constexpr value_t _pop_back()noexcept {
				auto _p = _m_p + --_m_sz;
				auto _v = std::move(*_p);
				raw_storage_t::destroy_at(_p);
				return _v;
			}
			constexpr void _clear()noexcept {
				raw_storage_t::destroy_n(_m_p, _m_sz);
				_m_sz = {};
			}
			constexpr void _reset()noexcept {
				_clear();
				raw_storage_t::deallocate(_m_p, _m_c, _m_a);
				_m_p = {};
				_m_c = {};
			}
			[[nodiscard]] constexpr iron::result<raw_vector, error_handler_t> _clone(size_t _n)const noexcept {
				raw_vector _t(std::allocator_traits<allocator_t>::select_on_container_copy_construction(_m_a));
				const auto _newc = _n * _m_sz;
				auto _r = _t._reserve(_newc); RR(_r);
				DEBUG_ASSERT(_m_p);
				auto _d = _t._m_p;
				DEBUG_ASSERT(_d);
				while (_n) {
					auto _h = raw_storage_t::uninitialized_copy_n(_d, _m_sz, _m_p);
					if (!_h) IRON_UNLIKELY{
						raw_storage_t::destroy_n(_t._m_p, _d - _t._m_p);
						return iron::err(std::move(_h));
					}
					_d += _m_sz;
					--_n;
				}
				_t._m_sz = _newc;
				return iron::ok(std::move(_t));
			}
			[[nodiscard]] constexpr error_handler_t _reserve(size_t _abs_capacity)noexcept {
				return raw_storage_t::reallocate(_m_p,_m_c,_m_sz,_abs_capacity,_m_a);
			}
			[[nodiscard]] constexpr error_handler_t _resize(size_t _n)noexcept {
				if (_n < _m_sz) {
					raw_storage_t::destroy_n(_m_p + _n, _m_sz - _n);
					_m_sz =_n;
				}
				else if (_m_c >= _n) {
					auto _h = raw_storage_t::uninitialized_fill(_m_p + _m_sz, _n - _m_sz); RE(_h);
					_m_sz = _n;
				}
				else {
					auto _h = _reserve(_n); RE(_h);
					return _resize(_n);
				}
				return {};
			}
			[[nodiscard]] constexpr error_handler_t _resize(size_t _n,const value_t& _v)noexcept {
				if (_n < _m_sz) {
					raw_storage_t::destroy_n(_m_p + _n, _m_sz - _n);
					_m_sz = _n;
				}
				else if (_m_c >= _n) {
					auto _r = iron::clone::try_clone_from(_v); RE(_r);
					auto _h = raw_storage_t::uninitialized_fill(_m_p + _m_sz, _n - _m_sz,std::move(_r.unchecked_unwrap())); RE(_h);
					_m_sz = _n;
				}
				else {
					auto _h = _reserve(_n); RE(_h);
					return _resize(_n,_v);
				}
				return {};
			}
			value_t* _m_p{};
			size_t _m_c{}, _m_sz{};
			IRON_NO_UNIQUE_ADDRESS allocator_t _m_a{};
	};


} //ns

static_assert(iron::concepts::raw_vector<iron::unsafe::raw_vector<int, iron::unsafe::allocator<int>>>, "internal error");

#if defined(_MSC_VER)
#pragma warning(pop)
#endif
