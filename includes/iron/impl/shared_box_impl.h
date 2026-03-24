#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable:26415)
#pragma warning (disable:26418)
#pragma warning (disable:26492)
#endif

namespace iron  {

	namespace _shared_impl {

		template <typename _T, typename _D, int _FLAGS>
		class _sh final {
		public:
			using _value_t = _T;
			using _deleter_t = _D;
			using _allocator_t = typename _deleter_t::allocator_t;
			using _counter_t = std::size_t;
			using _error_handler_t = error_handler;
			using _size_t = std::size_t;
			using _rwsh_t = iron::unsafe::raw_storage<_sh>;
			[[nodiscard]] static constexpr bool _thread_safe()noexcept { return (_FLAGS & _flag_thread_safe) != 0; }
			[[nodiscard]] static constexpr bool _weak()noexcept { return (_FLAGS & _flag_weak) != 0; }
			static_assert(!_weak(), "internal error");
			[[nodiscard]] static constexpr _counter_t _max_count()noexcept { return std::numeric_limits<_counter_t>::max(); }			
			constexpr explicit _sh(_value_t* _p)noexcept
				: _m_pvalue(_p)
				, _m_counter(1)
			{
				DEBUG_ASSERT(_p);
			}	
			_sh(const _sh&) = delete;
			_sh(_sh&&) = delete;
			_sh& operator=(const _sh&) = delete;
			_sh& operator=(_sh&&) = delete;
			constexpr ~_sh() {
				DEBUG_ASSERT(!_m_pvalue && !_m_counter && !_m_weak_counter);
			}
			template <bool _V>
			[[nodiscard]] constexpr _counter_t _counter()const noexcept {
				if constexpr (_V) {
					return _m_counter;
				}
				else {
					return _m_weak_counter;
				}
			}
			template <bool _V>
			constexpr void _inc_counter()noexcept {
				if constexpr (_V) {
					DEBUG_ASSERT(_m_counter < _max_count());
					++_m_counter;
				}
				else {
					DEBUG_ASSERT(_m_weak_counter < _max_count());
					++_m_weak_counter;
				}
			}
			template <bool _V>
			[[nodiscard]] constexpr _counter_t _dec_counter()noexcept {
				if constexpr (_V) {
					DEBUG_ASSERT(_m_counter);
					return --_m_counter;
				}
				else {
					DEBUG_ASSERT(_m_weak_counter);
					return --_m_weak_counter;
				}
			}
			static constexpr void _lock()noexcept {
				if constexpr (_thread_safe()) {
					_sm_lock._lock.lock();
				}
			}
			static constexpr void _unlock()noexcept {
				if constexpr (_thread_safe()) {
					_sm_lock._lock.unlock();
				}
			}
			[[nodiscard]] constexpr _value_t* const _get()noexcept { return _m_pvalue; }
			[[nodiscard]] constexpr const _value_t* const _get()const noexcept { return _m_pvalue; }
			
			constexpr void _destroy_value()noexcept {
				DEBUG_ASSERT(!_counter());
				_deleter_t{}(_m_pvalue);
				_m_pvalue = {};
			}
			
			[[nodiscard]] static constexpr _error_handler_t _alloc(_sh*& _self,_value_t* _pv)noexcept
			{
				DEBUG_ASSERT(!_self);
				DEBUG_ASSERT(_pv);
				auto _h = _rwsh_t::allocate(_self, 1, _deleter_t{}.unsafe_allocator());
				if (_h)  IRON_LIKELY
				{
					DEBUG_ASSERT(_self);
					std::construct_at(_self, _pv);
				}
				return _h;
			}
			static constexpr void _dealloc_sh(_sh*& _p)noexcept {
				DEBUG_ASSERT(_p && !_p->_m_counter && !_p->_m_weak_counter);
				auto _pv = _p->_release();
				_rwsh_t::destroy_at(_p);
				_rwsh_t::deallocate(_p, 1, _deleter_t{}.unsafe_allocator());
				if (_pv)  IRON_LIKELY
				{
					_dealloc_v(_pv);
				}
				_p = {};
			}
			static constexpr void _dealloc_v(_value_t* _p)noexcept {
				DEBUG_ASSERT(_p);
				_deleter_t{}(_p);
			}
			[[nodiscard]] constexpr _value_t* _release()noexcept {
				_value_t* _t{};
				std::swap(_t, _m_pvalue);
				return _t;
			}
			template <typename _U>
			[[nodiscard]] constexpr _U* _assign()noexcept {
				using _v_t = std::decay_t<_U>;
				if constexpr (std::is_same_v<_v_t, _sh>) {
					return this;
				}
				else if constexpr (std::is_base_of_v<typename _v_t::_value_t,_value_t>) {
					return reinterpret_cast<_U*>(this);
				}
				else if constexpr (std::is_base_of_v<_value_t, typename _v_t::_value_t>) {
					return reinterpret_cast<_U*>(this);
				}
				else {
					PANIC("internal error");
				}
			}
		private:
			typedef struct _lock_true final {
				_lock_true()noexcept = default;
				iron::memory::recursive_spin_lock _lock{};
			} _lock_true_t;
			typedef struct _lock_false final {} _lock_false_t;
			using _lock_t = std::conditional_t<_thread_safe(), _lock_true_t, _lock_false_t>;
			_value_t* _m_pvalue{};
			_counter_t _m_counter{}, _m_weak_counter{};
			static inline _lock_t _sm_lock{};
		};

	} //ns

	template <typename _T,typename _D,int _FLAGS>
	class base_shared_box final {
	private:
		[[nodiscard]] static constexpr bool _thread_safe()noexcept { return (_FLAGS & _shared_impl::_flag_thread_safe) != 0; }
		[[nodiscard]] static constexpr bool _weak()noexcept { return (_FLAGS & _shared_impl::_flag_weak) != 0; }
		static constexpr int _flag = _thread_safe() ? _shared_impl::_flag_thread_safe : 0;
		using _sh_t = _shared_impl::_sh<_T, _D,_flag>;
	public:
		struct shared_box_tag { shared_box_tag() = delete; };
		using deleter_t = typename _sh_t::_deleter_t;
		using deleter_type = deleter_t;
		using value_t = _T;
		using value_type = value_t;
		using ptr_t = value_t* const;
		using pointer = ptr_t;
		using cptr_t = const value_t* const;
		using const_pointer = cptr_t;
		using counter_t = typename _sh_t::_counter_t;
		using counter_type = counter_t;
		using error_handler_t = iron::error_handler;
		using error_handler_type = error_handler_t;
		using box_t = base_box<value_t, deleter_t>;
		using size_t = std::size_t;
		using size_type = size_t;
		using ref_t = typename std::add_lvalue_reference<value_t>::type;
		using reference = ref_t;
		using cref_t = typename std::add_lvalue_reference<const value_t>::type;
		using const_reference = cref_t;
		template <typename _T1>
		using rebind_t = base_shared_box<_T1, typename deleter_t:: template rebind_t<_T1>,_FLAGS>;
		template <typename _T1>
		using rebind_type = rebind_t<_T1>;
		static_assert(!std::is_reference_v<value_t>, "shared box value_type  must not be a reference type");
		static_assert(!std::is_array_v<value_t>, "shared box value_type must not be an array type");
		static_assert(!std::is_const_v<value_t>, "shared box value_type must not be const");
		static_assert(!std::is_same_v<ptr_t, void*>, "shared box value_type must not be void");
		static_assert(iron::concepts::box_deleter<deleter_t>, "shared box deleter must be a box deleter");
		[[nodiscard]] static constexpr bool thread_safe()noexcept { return _thread_safe(); }
		[[nodiscard]] static constexpr bool weak()noexcept { return _weak(); }
		[[nodiscard]] static constexpr counter_t max_count()noexcept { return _sh_t::_max_count(); }
		using weak_t = base_shared_box<value_t, deleter_t,_flag | _shared_impl::_flag_weak>;
		using weak_type = weak_t;
		struct shared_box_mark { shared_box_mark() = delete; };
		constexpr base_shared_box()noexcept = default;
		constexpr explicit base_shared_box(std::nullptr_t)noexcept {}
		constexpr base_shared_box(const base_shared_box& _v)
		{
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			if (_v._m_p)  IRON_LIKELY
			{
				_copy_with_lock(*this, _v);
			}
		}
		constexpr base_shared_box(base_shared_box&& _v)noexcept
		{
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_swap(_v);
			IRON_SECURITY_MARK_TEMPORARY(_v);
		}
		constexpr base_shared_box& operator=(const base_shared_box& _v)noexcept
		requires(sizeof(value_t) > 0 && std::is_nothrow_destructible_v<value_t>)
		{
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			auto _t = _v;
			_swap(_t);
			return *this;
		}

		constexpr base_shared_box& operator=(base_shared_box&& _v)noexcept
		{			
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_swap(_v);
			IRON_SECURITY_MARK_TEMPORARY(_v);
			return *this;
		}
		constexpr base_shared_box& operator=(std::nullptr_t)noexcept 
		requires(sizeof(value_t) > 0 && std::is_nothrow_destructible_v<value_t>)
		{
			base_shared_box _t;
			_swap(_t);
			return *this;
		}

		constexpr base_shared_box& reset()noexcept
		requires(sizeof(value_t) > 0 && std::is_nothrow_destructible_v<value_t>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			if (_m_p)  IRON_LIKELY
			{
				_reset();
			}
			return *this;
		}
		constexpr ~base_shared_box() {
			static_assert(sizeof(value_t) > 0, "shared_box::value_t is not set");
			static_assert(std::is_nothrow_destructible_v<value_t>, "shared_box::value_t is not nothrow destructible");
			if (_m_p)  IRON_LIKELY
			{
				_reset();
			}
		}
		[[nodiscard]] static constexpr iron::result<base_shared_box, error_handler_t> try_from(box_t&& _box)noexcept
		requires (!weak())
		{
			IRON_SECURITY_CHECK_TEMPORARY(_box);
			base_shared_box _sh;
			if (_box) IRON_LIKELY 
			{
				_sh_t* _p{};
				auto _vp = _box.unsafe_release();
				auto _h = _sh_t::_alloc(_p, _vp);
				if (!_h) IRON_UNLIKELY
				{
					_box = box_t::unchecked_from(_vp);
					return iron::err(std::move(_h));
				}
				DEBUG_ASSERT(_p && _p->_get() && _p-> template _counter<true>() == 1 && !_p-> template _counter<false>());
				_sh._m_p = _p;
			}
#if IRON_SECURITY_CHECKER
			[[maybe_unused]] const auto _t = std::move(_box); //mark _box temporary
#endif 
			return iron::ok(std::move(_sh));
		}
		[[nodiscard]] static constexpr base_shared_box from(box_t&& _v)noexcept 
		requires (!weak())
		{
			auto _r = try_from(std::move(_v)); RP(_r);
		}
		[[nodiscard]] constexpr counter_t use_count()const noexcept
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _counter_with_lock<true>();
		}
		[[nodiscard]] constexpr counter_t use_weak_count()const noexcept
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _counter_with_lock<false>();
		}
		[[nodiscard]] constexpr bool empty()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return !_counter_with_lock<true>();
		}
		[[nodiscard]] constexpr explicit operator bool()const noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _counter_with_lock<true>() ? true : false;
		}
		constexpr base_shared_box& swap(base_shared_box& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_swap(_v);
			return *this;
		}
		[[nodiscard]] constexpr ptr_t unsafe_data()noexcept 
		requires(!weak())
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _data<value_t>();
		}
		[[nodiscard]] constexpr cptr_t unsafe_data()const noexcept 
		requires(!weak())
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _data<value_t>();
		}
		[[nodiscard]] constexpr cptr_t unsafe_cdata()const noexcept 
		requires(!weak())
		{
			return unsafe_data();
		}

		[[nodiscard]] constexpr iron::option<iron::ref_wrapper<value_t>> value()noexcept 
		requires(!weak())
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			if (auto _p = _data<value_t>())  IRON_LIKELY
			{
				return iron::some(iron::make_ref(*this,*_p));
			}
			return {};
		}
		[[nodiscard]] constexpr iron::ref_wrapper<value_t> operator*()noexcept 
		requires(!weak())
		{
			return value().unwrap();
		}

		[[nodiscard]] constexpr iron::option<iron::ref_wrapper<const value_t>> value()const noexcept 
		requires(!weak())
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			if (auto _p = _data<value_t>())  IRON_LIKELY
			{
				return iron::some(iron::make_cref(*this,*_p));
			}
			return {};
		}

		[[nodiscard]] constexpr iron::option<iron::ref_wrapper<const value_t>> cvalue()const noexcept 
		requires(!weak())
		{
			return value();
		}
		[[nodiscard]] constexpr iron::ref_wrapper<const value_t> operator*()const noexcept 
		requires(!weak())
		{
			return value().unwrap();
		}
		[[nodiscard]] constexpr ref_t unchecked_value()noexcept 
		requires(!weak())
		{
			auto _p = unsafe_data();
			DEBUG_ASSERT(_p);
			return *_p;
		}

		[[nodiscard]] constexpr cref_t unchecked_value()const noexcept 
		requires(!weak())
		{
			auto _p = unsafe_data();
			DEBUG_ASSERT(_p);
			return *_p;
		}

		[[nodiscard]] constexpr cref_t unchecked_cvalue()const noexcept 
		requires(!weak())
		{
			return unchecked_value();
		}

		[[nodiscard]] constexpr value_t* operator->()noexcept
		requires(!weak() && sizeof(value_t) > 0 && std::is_class_v<value_t>)
		{
			auto _p = unsafe_data();
			PANIC_UNLESS(_p, "shared_box is not set");
			return _p;
		}
		[[nodiscard]] constexpr const value_t* operator->()const noexcept
			requires(!weak() && sizeof(value_t) > 0 && std::is_class_v<value_t>)
		{
			auto _p = unsafe_data();
			PANIC_UNLESS(_p, "shared_box is not set");
			return _p;
		}

		[[nodiscard]] constexpr weak_t make_weak()const noexcept 
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			weak_t _w;
			if (_m_p)  IRON_LIKELY
			{
				_copy_with_lock(_w, *this);
			}
			return _w;
		}
		[[nodiscard]] constexpr auto lock()const noexcept
		requires(weak())
		{
			base_shared_box<value_t, deleter_t, _flag> _t;
			if (_m_p) IRON_LIKELY
			{
				_copy_with_lock(_t, *this);
			}
			return _t;
		}
		[[nodiscard]] constexpr base_box<value_t, deleter_t> release()noexcept 
		requires(!weak())
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			if (_m_p)  IRON_LIKELY
			{
				value_t* _p{};
				_sh_t::_lock();
				if (_m_p-> template _counter<true>() == 1) {
					_p = _m_p->_release();
					_reset();
				}
				_sh_t::_unlock();
				return base_box<value_t, deleter_t>::unchecked_from(_p);
			}
			return {};
		}
		template <typename _U>
		[[nodiscard]] auto upcast()const noexcept
		requires (
			!weak() 
			&& (std::is_same_v<_U, value_t> || std::is_base_of_v<_U, value_t>)
			&& std::has_virtual_destructor_v<_U>
		)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			using _d_t = decltype(deleter_t::template upcast<_U>());
			using _new_t = base_shared_box<_U,_d_t, _FLAGS>;
			_new_t _t;
			if (_m_p)  IRON_LIKELY
			{
				_copy_with_lock(_t, *this);
			}
			return _t;
		}
		template <typename _U>
		[[nodiscard]] auto downcast()const noexcept
		requires (
			!weak()
			&& (std::is_same_v<_U, value_t> || std::is_base_of_v<value_t,_U>)
			&& std::has_virtual_destructor_v<_U>
		)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			using _d_t = decltype(deleter_t::template downcast<_U>());
			using _new_t = base_shared_box<_U, _d_t, _FLAGS>;
			_new_t _t;
			if (_m_p)  IRON_LIKELY
			{
				_copy_with_lock(_t, *this);
			}
			return _t;
		}
		template <typename _U>
		[[nodiscard]] auto cast()const noexcept
		requires(
			!weak()
			&& (std::is_same_v<_U, value_t> || std::is_base_of_v<value_t, _U> || std::is_base_of_v<_U, value_t>)
			&& std::has_virtual_destructor_v<_U>
		)
		{
			if constexpr (std::is_same_v<_U, value_t> || std::is_base_of_v<_U, value_t>) {
				return upcast<_U>();
			}
			else {
				return downcast<_U>();
			}
		}
		template <typename _T1,typename _D1,int _FLAGS1>
		[[nodiscard]] constexpr bool operator==(const base_shared_box<_T1,_D1,_FLAGS1>& _v)const noexcept 
		requires(
			(!weak() && !(base_shared_box<_T1, _D1, _FLAGS1>::weak()))
			&& (
				std::is_same_v<_T1,value_t>
				|| std::is_base_of_v<value_t,_T1> 
				|| std::is_base_of_v<_T1,value_t>
			)
		)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			const auto _pl = _data<value_t>();
			const auto _pr = _v.template _data<_T1>();
			return _pl == _pr;
		}
		IRON_SECURITY_METHOD()
	private:
		template <typename _T1, typename _D1,int _FLAGS1>
		friend class base_shared_box;

		constexpr void _swap(base_shared_box& _v)noexcept {
			std::swap(_m_p, _v._m_p);
			IRON_SECURITY_SWAP(_v);
		}

		template <bool _V>
		constexpr counter_t _counter_with_lock()const noexcept {
			_sh_t::_lock();
			const size_t _c = _m_p ? _m_p-> template _counter<_V>() : 0;
			_sh_t::_unlock();
			return _c;
		}
		constexpr void _reset()noexcept {
			DEBUG_ASSERT(_m_p);
			if constexpr (weak()) {
				_sh_t::_lock();
				if (!_m_p-> template _dec_counter<false>() && !_m_p->template _counter<true>()) IRON_LIKELY
				{
					_sh_t::_dealloc_sh(_m_p);
				}
				_sh_t::_unlock();
			}
			else {
				value_t* _p{};
				_sh_t::_lock();
				if (_m_p-> template _counter<true>()) IRON_LIKELY
				{
					if (!_m_p-> template _dec_counter<true>()) IRON_LIKELY
					{
						if (!_m_p-> template _counter<false>()) IRON_LIKELY
						{
							_sh_t::_dealloc_sh(_m_p);
						}
						else {
							_p = _m_p->_release(); //weak count active - not release _m_p but only the value
						}
					}
				}
				_sh_t::_unlock();
				if (_p) IRON_UNLIKELY
				{  //weak count active - not release _m_p but only the value
					_sh_t::_dealloc_v(_p);
				}
			}
			_m_p = {};
		}
		template <typename _T1>
		[[nodiscard]] constexpr _T1* _data()noexcept {
			if (_m_p)  IRON_LIKELY
			{
				value_t* _p = _m_p->_get();
				if (_p)  IRON_LIKELY
				{
					using _v_t = std::decay_t<_T1>;
					if constexpr (std::is_same_v<_v_t,value_t>) {
						return _p;
					}
					else if constexpr (std::is_base_of_v<_v_t, value_t>) {
						return _p;
					}
					else if constexpr (std::is_base_of_v<value_t, _v_t>) {
						return dynamic_cast<_T1*>(_p);
					}
					else {
						PANIC("internal error");
					}
				}
			}
			return {};
		}
		template <typename _T1>
		[[nodiscard]] constexpr const _T1* _data()const {
			return const_cast<base_shared_box*>(this)->_data<_T1>();
		}

		template <typename _T1>
		static constexpr void _copy(_T1& _out, const base_shared_box& _in)noexcept {
			DEBUG_ASSERT(!_out._m_p && _in._m_p);
			if (_in. template _data<typename _T1::value_t>())  IRON_LIKELY
			{
				constexpr auto _is_weak = _T1::weak();
#ifndef IRON_NOCHECK_SIZE
				if (_in._m_p-> template _counter<!_is_weak>() < _sh_t::_max_count()) 
#endif 					
				{ 
					using _sh1_t = typename _T1::_sh_t;
					_out._m_p = _in._m_p->template _assign<_sh1_t>();
					_out._m_p-> template _inc_counter<!_is_weak>();
				}
			}
		}

		template <typename _T1>
		static constexpr void _copy_with_lock(_T1& _out, const base_shared_box& _in)noexcept {
			_sh_t::_lock();
			_copy(_out, _in);
			_sh_t::_unlock();
		}
		_sh_t* _m_p{};
		IRON_SECURITY_DCL();
	};


	template <typename _T, typename... _ARGS>
	[[nodiscard]] inline constexpr iron::result<iron::rc<_T>, iron::error_handler> try_make_rc(_ARGS&&... _args)noexcept {
		IRON_SECURITY_CHECK_TEMPORARY_ARGS(_args...);
		auto _r = iron::try_make_box<_T>(std::forward<_ARGS>(_args)...); RR(_r);
		return iron::rc<_T>::try_from(std::move(_r.unchecked_unwrap()));
	}

	template <typename _T, typename... _ARGS>
	[[nodiscard]] inline constexpr iron::rc<_T> make_rc(_ARGS&&... _args)noexcept {
		auto _r = iron::try_make_rc<_T>(std::forward<_ARGS>(_args)...); RP(_r);
	}

	template <typename _T, typename... _ARGS>
	[[nodiscard]] inline iron::result<iron::arc<_T>, iron::error_handler> try_make_arc(_ARGS&&... _args)noexcept {
		IRON_SECURITY_CHECK_TEMPORARY_ARGS(_args...);
		auto _r = iron::try_make_box<_T>(std::forward<_ARGS>(_args)...); RR(_r);
		return iron::arc<_T>::try_from(std::move(_r.unchecked_unwrap()));
	}

	template <typename _T, typename... _ARGS>
	[[nodiscard]] inline iron::arc<_T> make_arc(_ARGS&&... _args)noexcept {
		auto _r = iron::try_make_arc<_T>(std::forward<_ARGS>(_args)...); RP(_r);
	}

}


static_assert(iron::concepts::base_shared_box<iron::rc<int>>, "internal error");
static_assert(iron::concepts::shared_box<iron::rc<int>>, "internal error");
static_assert(iron::concepts::weak_box<iron::rc<int>::weak_t>, "internal error");

static_assert(iron::concepts::base_shared_box<iron::arc<int>>, "internal error");
static_assert(iron::concepts::shared_box<iron::arc<int>>, "internal error");
static_assert(iron::concepts::weak_box<iron::arc<int>::weak_t>, "internal error");

static_assert(iron::concepts::rc<iron::rc<int>> && !iron::concepts::rc<iron::arc<int>>, "internal error");
static_assert(iron::concepts::rcw<iron::rc<int>::weak_t> && !iron::concepts::rcw<iron::arc<int>::weak_t>, "internal error");

static_assert(!iron::concepts::arc<iron::rc<int>>&& iron::concepts::arc<iron::arc<int>>, "internal error");
static_assert(!iron::concepts::arcw<iron::rc<int>::weak_t>&& iron::concepts::arcw<iron::arc<int>::weak_t>, "internal error");

#if defined(_MSC_VER)
#pragma warning(pop)
#endif
