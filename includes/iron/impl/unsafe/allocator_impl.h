#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable:26408)
#pragma warning (disable:26474)
#pragma warning (disable:26481)
#pragma warning (disable:26485)
#endif

namespace iron::unsafe {

	template <typename _T>
	class base_allocator final {
	public:
		using value_t = _T;
		using value_type = value_t;
		using pointer = value_type*;
		static_assert(!std::is_reference_v<value_t>, "base_allocator value_type must not be a reference");
		static_assert(!std::is_array_v<value_t>, "base_allocator value_type must not be an array type");
		static_assert(!std::is_const_v<value_t>, "base_allocator value_type must not be const");
	private:
		using _constexpr_allocator_t = std::allocator<value_t>;
	public:
		using size_t = typename std::allocator_traits<_constexpr_allocator_t>::size_type;
		using size_type = size_t;
		using diff_t = typename std::allocator_traits<_constexpr_allocator_t>::difference_type;
		using difference_type = diff_t;
		using propagate_on_container_move_assignment = typename std::allocator_traits<_constexpr_allocator_t>::propagate_on_container_move_assignment;
		struct allocator_tag { allocator_tag() = delete; };
		constexpr base_allocator()noexcept = default;
		template< class _U>
		constexpr base_allocator(const base_allocator<_U>& _v)noexcept {
		}
		constexpr base_allocator(const base_allocator& _v)noexcept {
		}
		constexpr base_allocator(base_allocator&& _v)noexcept {
			_swap(_v);
		}
		constexpr base_allocator& operator=(const base_allocator& _v)noexcept {
			return *this;
		}
		constexpr base_allocator& operator=(base_allocator&& _v)noexcept {
			_swap(_v);
			return *this;
		}
		constexpr ~base_allocator() = default;
		constexpr void swap(base_allocator& _v)noexcept {
			_swap(_v);
		}
		template<typename _U>
		struct rebind {
			using other = base_allocator<_U>;
		};
		template <bool _init=false>
		[[nodiscard]] constexpr pointer allocate(size_t _n)noexcept {
			static_assert(sizeof(value_t) > 0, "base_allocator<T>::allocate() - value_type must be complete before calling");
			if (!_n) IRON_UNLIKELY
			{
				return {};
			}
			if (std::is_constant_evaluated()) {
#ifdef __clang__
				DEBUG_ASSERT(!std::has_virtual_destructor_v<value_t>); //"not use allocate/construct_at for bug in deallocate using base class");
#endif
				return _p_get_constexpr_allocator().allocate(_n);
			}
			else {
				auto _p=static_cast<pointer>(std::malloc(sizeof(value_t) * _n));
				if constexpr(_init) {
					if (_p) IRON_LIKELY
					{
						_memreset<true>(_p, _n);
					}
				}
				return _p;
			}
		}
		
		template <bool _reset=false>
		constexpr void deallocate(pointer _p, size_t _n)noexcept {
			static_assert(sizeof(value_t) > 0, "allocator<T>::deallocate() - value_type must be complete before calling");
			DEBUG_ASSERT_EQ((_p ? true : false), (_n ? true : false));
			if (_p) {
				if (std::is_constant_evaluated()) {
#ifdef __clang__
					DEBUG_ASSERT(!std::has_virtual_destructor_v<value_t>); //,"not use destroy_at/deallocate for bug in deallocate using base class - use classic allocator");
#endif
					_p_get_constexpr_allocator().deallocate(_p, _n);
				}
				else {
					if constexpr(_reset) {
						_memreset<false>(_p, _n);
					}
					std::free(_p);
				}
			}
		}

		[[nodiscard]] constexpr bool operator==(const base_allocator& _v)const noexcept {
			return true;
		}

	private:
		[[nodiscard]] static constexpr auto _p_get_constexpr_allocator()noexcept {
			if constexpr (std::is_same_v<value_type, typename _constexpr_allocator_t::value_type>) {
				return _constexpr_allocator_t{};
			}
			else {
				return _p_rebind();
			}
		}
		[[nodiscard]] static constexpr auto _p_rebind()noexcept {
			return typename std::allocator_traits<_constexpr_allocator_t>::template rebind_alloc<value_t>(_constexpr_allocator_t{});
		}
		template <bool _ALLOCATE>
		static void _memreset(value_t* _p, size_t _n)noexcept {
			if constexpr (_ALLOCATE) {
				std::memset(static_cast<void*>(_p), 0xFD, sizeof(value_t) * _n);
			}
			else {
				std::memset(static_cast<void*>(_p), 0xDF, sizeof(value_t) * _n);
			}
		}
		constexpr void _swap(base_allocator& _v)noexcept {
		}
		template <typename _U> friend class base_allocator;
	};


} //ns

static_assert(iron::concepts::allocator<iron::unsafe::base_allocator<int>>, "internal error");


#if defined(_MSC_VER)
#pragma warning(pop)
#endif
