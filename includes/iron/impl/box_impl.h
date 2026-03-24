
namespace iron {


	template <typename _T,typename _A>
	class box_deleter final {
	public:
		using value_t = _T;
		using value_type = value_t;
		using allocator_t = _A;
		using allocator_type = allocator_t;
	private:
		template <typename _T1>
		using _new_alloc_t=decltype(typename std::allocator_traits<allocator_t>::template rebind_alloc<_T1>(allocator_t{}));
	public:
		template <typename _T1>
		using rebind_t = box_deleter<_T1,_new_alloc_t<_T1>>;
		template <typename _T1>
		using rebind_type = rebind_t<_T1>;
		static_assert(!std::is_array_v<value_t>, "box_deleter value template parameter must not be an array type");
		static_assert(!std::is_const_v<value_t>, "box deleter value template parameter must not be const");
		static_assert(!std::is_reference_v<value_t>, "box deleter value template parameter must not be reference");
		static_assert(iron::concepts::allocator<allocator_t>, "box deleter allocator template parameter must be an allocator");
		constexpr box_deleter()noexcept = default;
		constexpr box_deleter(const box_deleter& _v)noexcept
		{
			IRON_SECURITY_CHECK_TEMPORARY(_v);
		}
		constexpr box_deleter(box_deleter&& _v)noexcept
		{
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_swap(_v);
			IRON_SECURITY_MARK_TEMPORARY(_v);
		}
		constexpr box_deleter& operator=(const box_deleter& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			auto _t = _v;
			_swap(_t);
			return *this;
		}
		constexpr box_deleter& operator=(box_deleter&& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_swap(_v);
			IRON_SECURITY_MARK_TEMPORARY(_v);
			return *this;
		}
		constexpr ~box_deleter() = default;
		[[nodiscard]] constexpr allocator_t unsafe_allocator()const noexcept { 
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return allocator_t{}; 
		}
		
		constexpr void operator()(value_t* _p)noexcept
		{ //not use require for compilation error in case value_t not set
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			static_assert(sizeof(value_t) > 0, "box_deleter::value_t is not set");
			static_assert(std::is_nothrow_destructible_v<value_t>, "box_deleter value_t is not nothrow destructible");
			if (_p)  IRON_LIKELY
			{
				_rw_t::destroy_at(_p);
				_rw_t::deallocate(_p, 1, unsafe_allocator());
			}
		}
		
		template <typename _U>
		[[nodiscard]] static rebind_t<_U> upcast()noexcept
			requires ((std::is_same_v<_U,value_t> || std::is_base_of_v<_U, value_t>) && std::has_virtual_destructor_v<_U>)
		{
			return {};
		}
		template <typename _U>
		[[nodiscard]] static rebind_t<_U> downcast()noexcept
			requires ((std::is_same_v<_U, value_t> || std::is_base_of_v<value_t,_U>) && std::has_virtual_destructor_v<_U>)
		{
			return {};
		}
		IRON_SECURITY_METHOD();
	private:
		using _rw_t = iron::unsafe::raw_storage<value_t>;
		constexpr void _swap(box_deleter& _v)noexcept {
			IRON_SECURITY_SWAP(_v);
		}
		IRON_SECURITY_DCL();
	};

	template <typename _T,typename _D>
	class base_box final {
	public:
		using value_t=_T;
		using value_type = value_t;
		using deleter_t = _D;
		using deleter_type = deleter_t;
		using ptr_t = value_t*const;
		using pointer = ptr_t;
		using cptr_t = const value_t* const;
		using const_pointer = cptr_t;
		using size_t = std::size_t;
		using size_type = size_t;
		using ref_t = typename std::add_lvalue_reference<value_t>::type;
		using reference = ref_t;
		using cref_t = typename std::add_lvalue_reference<const value_t>::type;
		using const_reference = cref_t;
		using error_handler_t = iron::error_handler;
		using error_handler_type = error_handler_t;
		template <typename _T1>
		using rebind_t = base_box<_T1,typename deleter_t:: template rebind_t<_T1>>;
		template <typename _T1>
		using rebind_type=rebind_t<_T1>;
		static_assert(!std::is_reference_v<value_t>, "box value_type  must not be a reference type");
		static_assert(!std::is_array_v<value_t>, "box value_type must not be an array type");
		static_assert(!std::is_const_v<value_t>, "box value_type must not be const");
		static_assert(!std::is_same_v<value_t*,void*>, "box value_type must not be void");
		static_assert(iron::concepts::box_deleter<deleter_t>, "box deleter must be a box deleter");
		static_assert(std::is_same_v<value_t, typename deleter_t::value_t>, "invalid box deleter value type");
		struct box_tag { box_tag() = delete; };
		[[nodiscard]] static constexpr base_box unchecked_from(value_t* _p)noexcept {
			return base_box(_p);
		}
		template <typename... _ARGS >
		[[nodiscard]] static constexpr iron::result<base_box, error_handler_t> try_from(_ARGS&&... _args)noexcept
		requires (iron::concepts::constructible_at<value_t, _ARGS&&...>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_ARGS(_args...);
			return _emplace(std::forward<_ARGS>(_args)...);
		}
		template <typename... _ARGS >
		[[nodiscard]] static constexpr base_box from(_ARGS&&... _args)noexcept
			requires (iron::concepts::constructible_at<value_t, _ARGS&&...>)
		{
			auto _r = try_emplace(std::forward<_ARGS>(_args)...); RP(_r);
		}
		constexpr base_box()noexcept = default;
		constexpr explicit base_box(std::nullptr_t)noexcept {}
		base_box(const base_box&) = delete;
		base_box& operator=(const base_box&) = delete;
		constexpr base_box(base_box&& _v)noexcept
		{
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_swap(_v);
			IRON_SECURITY_MARK_TEMPORARY(_v);
		}
		constexpr base_box& operator=(base_box&& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_swap(_v);
			IRON_SECURITY_MARK_TEMPORARY(_v);
			return *this;
		}
		constexpr base_box& operator=(std::nullptr_t)noexcept 
		requires(sizeof(value_t) > 0 && std::is_nothrow_destructible_v<value_t>)
		{
			base_box _t;
			_swap(_t);
			return *this;
		}
		constexpr base_box& reset()noexcept
		requires(sizeof(value_t) > 0 && std::is_nothrow_destructible_v<value_t>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			_reset();
			return *this;
		}
		[[nodiscard]] constexpr ptr_t const unsafe_release()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			value_t* _p{};
			std::swap(_p, _m_p);
			return _p;
		}

		constexpr ~base_box()
			//not use require for compiler error 
		{
			static_assert(sizeof(value_t) > 0, "box::value_t is not set");
			static_assert(std::is_nothrow_destructible_v<value_t>,"box::value_t is not nothrow destructible");
			_reset();
		}
		constexpr base_box& swap(base_box& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_swap(_v);
			return *this;
		}
		[[nodiscard]] constexpr bool empty()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _m_p ? false : true;
		}
		[[nodiscard]] constexpr explicit operator bool()const noexcept { return !empty(); }
		[[nodiscard]] constexpr ptr_t unsafe_data()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _m_p;
		}
		[[nodiscard]] constexpr cptr_t unsafe_data()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _m_p;
		}
		[[nodiscard]] constexpr cptr_t unsafe_cdata()const noexcept {
			return unsafe_data();
		}
		[[nodiscard]] constexpr iron::option<iron::ref_wrapper<value_t>> value()noexcept
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			if (_m_p)  IRON_LIKELY
			{
				return iron::some(iron::make_ref(*this,*_m_p));
			}
			return {};
		}
		[[nodiscard]] constexpr iron::option<iron::ref_wrapper<const value_t>> value()const noexcept
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			if (_m_p)  IRON_LIKELY
			{
				return iron::some(iron::make_cref(*this,*_m_p));
			}
			return {};
		}
		[[nodiscard]] constexpr iron::option<iron::ref_wrapper<const value_t>> cvalue()const noexcept
		{
			return value();
		}

		[[nodiscard]] constexpr iron::ref_wrapper<value_t> operator*()noexcept
		{
			return value().unwrap();
		}
		[[nodiscard]] constexpr iron::ref_wrapper<const value_t> operator*()const noexcept
		{
			return value().unwrap();
		}
		[[nodiscard]] constexpr value_t* operator->()noexcept
		requires(sizeof(value_t) > 0 && std::is_class_v<value_t>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			PANIC_UNLESS(_m_p, "box is not set");
			return _m_p;
		}
		[[nodiscard]] constexpr const value_t* operator->()const noexcept
			requires(sizeof(value_t) > 0 && std::is_class_v<value_t>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			PANIC_UNLESS(_m_p, "box is not set");
			return _m_p;
		}
		[[nodiscard]] constexpr ref_t unchecked_value()noexcept
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			DEBUG_ASSERT(_m_p);
			return *_m_p;
		}
		[[nodiscard]] constexpr cref_t unchecked_value()const noexcept
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			DEBUG_ASSERT(_m_p);
			return *_m_p;
		}
		[[nodiscard]] constexpr cref_t unchecked_cvalue()const noexcept
		{
			return unchecked_value();
		}
		template <typename _U>
		[[nodiscard]] constexpr auto upcast()noexcept
			requires (
				sizeof(value_t) > 0 
				&& (std::is_same_v<_U,value_t> || std::is_base_of_v<_U, value_t>)
				&& std::has_virtual_destructor_v<_U>
			)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			using _d_t = decltype(deleter_t::template upcast<_U>());
			using _new_box_t = base_box<_U, _d_t>;
			_new_box_t _t;
			_t._m_p = _m_p;
			IRON_SECURITY_SWAP(_t);
			_m_p = {};
			return _t;
		}
		template <typename _U>
		[[nodiscard]] constexpr auto downcast()noexcept
			requires (
				sizeof(value_t) > 0 
				&& (std::is_same_v<_U, value_t> || std::is_base_of_v<value_t,_U>)
				&& std::has_virtual_destructor_v<value_t>
			)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			using _d_t = decltype(deleter_t::template downcast<_U>());
			using _new_box_t = base_box<_U, _d_t>;
			_new_box_t _t;
			if constexpr (std::is_same_v<value_t,_U>) {
				std::swap(_t._m_p, _m_p);
				IRON_SECURITY_SWAP(_t);
			}
			else {
				_t._m_p = dynamic_cast<_U*>(_m_p);
				if (_t._m_p)  IRON_LIKELY
				{
					_m_p = {};
					IRON_SECURITY_SWAP(_t);
				}
			}
			return _t;
		}
		template <typename _U>
		[[nodiscard]] constexpr auto cast()noexcept
		requires (
			sizeof(value_t) > 0
			&& (std::is_same_v<_U, value_t> || std::is_base_of_v<value_t, _U> || std::is_base_of_v<_U, value_t>)
			&& std::has_virtual_destructor_v<value_t>
			)
		{
			if constexpr (std::is_same_v<_U, value_t> || std::is_base_of_v<_U, value_t>) {
				return upcast<_U>();
			}
			else {
				return downcast<_U>();
			}
		}
		[[nodiscard]] constexpr iron::result<value_t,error_handler_t>  try_release_value()noexcept
		requires (
			sizeof(value_t) > 0 
			&& std::is_nothrow_move_constructible_v<value_t>
			&& std::is_nothrow_destructible_v<value_t>
		)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			if (_m_p) IRON_LIKELY
			{
				value_t _t{ std::move(*_m_p) };
				_reset();
				return iron::ok(std::move(_t));
			}
			return iron::err(ERROR_HANDLER(iron::unsafe::errors::posix::code_t::result_out_of_range));
		}
		[[nodiscard]] constexpr value_t release_value()noexcept
		requires (
			sizeof(value_t) > 0 
			&& std::is_nothrow_move_constructible_v<value_t>
			&& std::is_nothrow_destructible_v<value_t>
		)
		{
			auto _r = try_release_value(); RP(_r);
		}
		[[nodiscard]] constexpr value_t release_value_or_default()noexcept
			requires (
				sizeof(value_t) > 0 
				&& std::is_nothrow_move_constructible_v<value_t> 
				&& std::is_nothrow_destructible_v<value_t>
				&& std::is_nothrow_default_constructible_v<value_t>
			)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_REF_ACTIVES_THIS();
			if (_m_p)  IRON_LIKELY
			{
				value_t _t{ std::move(*_m_p) };
				_reset();
				return _t;
			}
			return {};
		}

		[[nodiscard]] constexpr iron::result<base_box, error_handler_t> try_clone()const noexcept
		requires (iron::concepts::copyable_or_cloneable<value_t>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _clone();
		}
		[[nodiscard]] constexpr base_box clone()const noexcept
		requires (iron::concepts::copyable_or_cloneable<value_t>)
		{
			auto _r = try_clone(); RP(_r);
		}
		IRON_SECURITY_METHOD();
	private:
		template <typename _T1,typename _D1>
		friend class base_box;
		using _rw_t = iron::unsafe::raw_storage<value_t>;
		constexpr explicit base_box(value_t* _p)noexcept : _m_p(_p) {}
		[[nodiscard]] static constexpr error_handler_t _allocate(value_t*& _p)noexcept {
			DEBUG_ASSERT(!_p);
			return _rw_t::allocate(_p, 1, deleter_t{}.unsafe_allocator());
		}
		static constexpr void _deallocate(value_t* _p)noexcept {
			_rw_t::deallocate(_p, 1, deleter_t{}.unsafe_allocator());
		}
		template <typename... _ARGS >
		[[nodiscard]] static constexpr iron::result<base_box, error_handler_t> _emplace(_ARGS&&... _args)noexcept {
			value_t* _p{};
			auto _h = _allocate(_p); RR(_h);
			_h = _rw_t::construct_at(_p, std::forward<_ARGS>(_args)...);
			if (!_h) IRON_UNLIKELY 
			{
				_deallocate(_p);
				return iron::err(std::move(_h));
			}
			return iron::ok(unchecked_from(_p));
		}
		[[nodiscard]] static constexpr iron::result<base_box, error_handler_t> _from(value_t&& _v)noexcept 
		requires(std::is_nothrow_move_constructible_v<value_t>)
		{
			value_t* _p{};
			auto _h = _allocate(_p); RR(_h);
			std::construct_at(_p, std::move(_v));
			return iron::ok(unchecked_from(_p));
		}
		[[nodiscard]] static constexpr iron::result<base_box, error_handler_t> _from(const value_t& _v)noexcept 
		requires(std::is_nothrow_copy_constructible_v<value_t>)
		{
			value_t* _p{};
			auto _h = _allocate(_p); RR(_h);
			std::construct_at(_p,_v);
			return iron::ok(unchecked_from(_p));
		}
		[[nodiscard]] constexpr iron::result<base_box, error_handler_t> _clone()const noexcept {
			if (_m_p) IRON_LIKELY 
			{
				if constexpr (std::is_nothrow_copy_constructible_v<value_t>) {
					return _from(*_m_p);
				}
				else {
					auto _r = iron::clone::try_clone_from(*_m_p); RR(_r);
					return _from(std::move(_r.unchecked_unwrap()));
				}
			}
			return {};
		}
		constexpr void _swap(base_box& _v)noexcept {
			std::swap(_m_p, _v._m_p);
			IRON_SECURITY_SWAP(_v);
		}
		constexpr void _reset()noexcept {
			deleter_t{}(_m_p);
			_m_p = {};
		}
		value_t* _m_p{};
		IRON_SECURITY_DCL();
	};

	template <typename _T>
	[[nodiscard]] inline constexpr iron::box<_T> make_empty_box()noexcept {
		return iron::box<_T>{};
	}

	template <typename _T, typename... _ARGS>
	[[nodiscard]] inline constexpr iron::result<iron::box<_T>, iron::error_handler> try_make_box(_ARGS&&... _args)noexcept
	{
		IRON_SECURITY_CHECK_TEMPORARY_ARGS(_args...);
		return iron::box<_T>::try_from(std::forward<_ARGS>(_args)...);
	}

	template <typename _T, typename... _ARGS>
	[[nodiscard]] inline constexpr iron::box<_T> make_box(_ARGS&&... _args)noexcept
	{
		auto _r = try_make_box<_T>(std::forward<_ARGS>(_args)...); RP(_r);
	}

} //ns

static_assert(iron::concepts::box<iron::box<int>>, "internal error");

