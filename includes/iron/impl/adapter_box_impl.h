namespace iron {

	template <typename _CONTAINER>
	class adapter_box final  {
	public:
		using container_t = _CONTAINER;
		using container_type = container_t;
		static_assert(iron::concepts::raw_vector<container_t>, "adapter_box - the container must be raw_vector");
		using value_t = typename container_t::value_t;
		using value_type = value_t;
		using allocator_t = typename container_t::allocator_t;
		using allocator_type = allocator_t;
		using error_handler_t = typename container_t::error_handler_t;
		using error_handler_type = error_handler_t;
		struct adapter_box_tag { adapter_box_tag() = delete; };
		[[nodiscard]] static constexpr adapter_box unchecked_from(container_t&& _v)noexcept 
		{
			return adapter_box(std::move(_v));
		}
		constexpr adapter_box()noexcept=default;
		adapter_box(const adapter_box&)=delete;
		adapter_box& operator=(const adapter_box&)=delete;
		constexpr adapter_box(adapter_box&& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_swap(_v);
			IRON_SECURITY_MARK_TEMPORARY(_v);
		}
		constexpr adapter_box& operator=(adapter_box&& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_swap(_v);
			IRON_SECURITY_MARK_TEMPORARY(_v);
			return *this;
		}
		constexpr ~adapter_box()=default;
		[[nodiscard]] constexpr size_t capacity()const noexcept 
		requires(iron::concepts::raw_vector<container_t>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _m_box.capacity();
		}

		[[nodiscard]] constexpr size_t size()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _m_box.size();
		}
		[[nodiscard]] constexpr bool empty()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _m_box.empty();
		}
		[[nodiscard]] constexpr explicit operator bool()const noexcept {
			return !empty();
		}
		[[nodiscard]] constexpr container_t unsafe_release()noexcept 
		requires(std::is_nothrow_move_constructible_v<container_t>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _release();
		}
		constexpr adapter_box& swap(adapter_box& _v)noexcept 
		requires(std::is_nothrow_swappable_v<container_t>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			_swap(_v);
			return *this;
		}
		[[nodiscard]] constexpr iron::result<adapter_box, error_handler_t> try_clone()const noexcept 
		requires(iron::concepts::cloneable<container_t>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			auto _r = _m_box.try_clone(); RR(_r);
			return iron::ok(adapter_box(std::move(_r.unchecked_unwrap())));
		}
		[[nodiscard]] constexpr adapter_box clone()const noexcept 
		requires(iron::concepts::cloneable<container_t>)
		{
			auto _r = try_clone(); RP(_r);
		}
		[[nodiscard]] constexpr allocator_t unsafe_allocator()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			return _m_box.allocator();
		}
		IRON_SECURITY_METHOD();
	private:
		constexpr explicit adapter_box(container_t&& _v)noexcept
			: _m_box(std::move(_v))
		{
		}
		[[nodiscard]] constexpr container_t _release()noexcept {
			container_t _t;
			_m_box.swap(_t);
			return _t;
		}
		constexpr void _swap(adapter_box& _v)noexcept {
			_m_box.swap(_v._m_box);
			IRON_SECURITY_SWAP(_v);
		}
		container_t _m_box;
		IRON_SECURITY_DCL();
	};
}

static_assert(iron::concepts::adapter_box<iron::adapter_box<iron::unsafe::raw_vector<int,iron::unsafe::allocator<int>>>>, "internal error");


