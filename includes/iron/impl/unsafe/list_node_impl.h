#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable:26492)
#endif

namespace iron::unsafe {

	template <typename _T,bool _ISBD>
	class  list_node final {
	public:
		using value_t = _T;
		static_assert(!std::is_reference_v<value_t>, "list_node value_type  must not be a reference type");
		static_assert(!std::is_array_v<value_t>, "list_node value_type must not be an array type");
		static_assert(std::is_nothrow_destructible_v<value_t>, "list_node value_type nothrow destructible");
		using error_handler_t = iron::error_handler;
		static constexpr bool is_bidirectional=_ISBD;
		using raw_storage_node_t = iron::unsafe::raw_storage<list_node>;
		using raw_storage_value_t = iron::unsafe::raw_storage<value_t>;
		using size_t = typename raw_storage_value_t::size_t;
		struct list_node_tag { list_node_tag() = delete; };
		static constexpr auto max_size() { return raw_storage_node_t::max_size();}
		list_node()=delete;
		constexpr explicit list_node(value_t&& _v)noexcept
		requires(std::is_constant_evaluated())
		: _m_next()
		, _m_prev()
		, _m_v(std::move(_v))
		{
		}
		template <typename _F, typename _S>
		constexpr list_node(_F&& _f,_S&& _s)noexcept
			requires(std::is_constant_evaluated())
		: _m_next()
		, _m_prev()
		, _m_v(
				std::piecewise_construct
				, std::forward_as_tuple(std::forward<_F>(_f))
				, std::forward_as_tuple(std::forward<_S>(_s))
			)
		{}
		list_node(const list_node&) = delete;
		list_node(list_node&&) = delete;
		list_node& operator=(const list_node&) = delete;
		list_node& operator=(list_node&&) = delete;
		constexpr ~list_node() {
			DEBUG_ASSERT(!_m_next); //unlink before
			if constexpr (is_bidirectional) {
				DEBUG_ASSERT(!_m_prev);//unlink before
			}
		}
		template <typename _A,typename... _ARGS>
		[[nodiscard]] static constexpr error_handler_t from_value(list_node*& _p,_A _a,_ARGS&&... _args)noexcept
		requires (
			iron::concepts::allocator<_A>
			&& iron::concepts::constructible_at<value_t, _ARGS&&...>)
		{
			DEBUG_ASSERT(!_p);
			if (std::is_constant_evaluated()) {
				const auto _h=raw_storage_node_t::allocate(_p, 1, _a);
				DEBUG_ASSERT(_h);
				value_t _v(std::forward<_ARGS>(_args)...);
				std::construct_at(_p,std::move(_v));
				return {};
			}
			else {
				auto _h = raw_storage_node_t::allocate(_p, 1, _a); RE(_h);
				_h=raw_storage_value_t::construct_at(&(_p->_m_v), std::forward<_ARGS>(_args)...);
				if (!_h) IRON_UNLIKELY
				{
					raw_storage_node_t::deallocate(_p, 1, _a);
					_p={};
					return _h;
				}
				_p->_m_next = {};
				if constexpr (is_bidirectional) {
					_p->_m_prev = {};
				}
				return _h;
			}
		}
		template <typename _A, typename _ITER>
		[[nodiscard]] static constexpr error_handler_t from_iter(list_node*& _p, _A _a,const _ITER& _b)noexcept {
			if constexpr (iron::concepts::constructible_at<value_t, decltype(*_b)>) {
				return from_value(_p, _a, *_b);
			}
			else {
				auto _r = iron::clone::try_clone_from(*_b); RE(_r);
				return from_value(_p, _a, std::move(_r.unchecked_unwrap()));
			}
		}

		template <typename _A, typename _F,typename _S>
		[[nodiscard]] static constexpr error_handler_t from_pair(list_node*& _p, _A _a, _F&& _first, _S&& _second)noexcept
		requires (
			iron::concepts::allocator<_A>
			&& iron::concepts::std_pair<typename list_node::value_t>
			&& iron::concepts::constructible_at<typename list_node::value_t::first_type, _F&&>
			&& iron::concepts::constructible_at<typename list_node::value_t::second_type, _S&&>
		)
		{
			DEBUG_ASSERT(!_p);
			if (std::is_constant_evaluated()) {
				const auto _h = raw_storage_node_t::allocate(_p, 1, _a);
				DEBUG_ASSERT(_h);
				std::construct_at(_p, std::forward<_F>(_first), std::forward<_S>(_second));
				return {};
			}
			else {
				auto _h = raw_storage_node_t::allocate(_p, 1, _a); RE(_h);
				_h=iron::unsafe::raw_storage<value_t>::construct_at(
					&(_p->_m_v)
					,std::piecewise_construct
					,std::forward_as_tuple(std::forward<_F>(_first))
					,std::forward_as_tuple(std::forward<_S>(_second))
				);
				if (!_h) {
					raw_storage_node_t::deallocate(_p, 1, _a);
					_p = {};
					return _h;
				}
				std::construct_at(&(_p->_m_next), nullptr);
				if constexpr (is_bidirectional) {
					std::construct_at(&(_p->_m_prev), nullptr);
				}
				return _h;
			}
		}

		static constexpr void destroy(list_node* _p)noexcept {
			raw_storage_node_t::destroy_at(_p);
		}

		template <typename _A>
		static constexpr void deallocate(list_node* _p, _A _a)noexcept
		requires (iron::concepts::allocator<_A>)
		{
			raw_storage_node_t::deallocate(_p, 1, _a);
		}
		constexpr void swap(list_node& _v)noexcept
		requires (std::is_nothrow_swappable_v<value_t>)
		{
			std::swap(_m_next, _v._m_next);
			if constexpr (is_bidirectional) {
				std::swap(_m_prev, _v._m_prev);
			}
			std::swap(_m_v, _v._m_v);
		}
		[[nodiscard]] constexpr value_t& value()noexcept { return _m_v;}
		[[nodiscard]] constexpr const value_t& value()const noexcept { return _m_v;}
		[[nodiscard]] constexpr list_node*& next()noexcept { return _m_next;}
		[[nodiscard]] constexpr const list_node*const next()const noexcept { return _m_next;}
		[[nodiscard]] constexpr list_node*& prev()noexcept
		requires(is_bidirectional)
		{
			return _m_prev;
		}
		[[nodiscard]] constexpr const list_node*const prev()const noexcept
		requires(is_bidirectional)
		{
			return _m_prev;
		}
		constexpr void reset_links()noexcept {
			_m_next = {};
			if constexpr (is_bidirectional) {
				_m_prev = {};
			}
		}
	private:
		typedef struct _pnode_empty {} _pnode_empty_t;
		using _pnode_prev_t=std::conditional_t<is_bidirectional,list_node*,_pnode_empty_t>;
		list_node* _m_next;
		_pnode_prev_t _m_prev;
		value_t _m_v;
	};

}

#ifdef _IRON_CHECK_LIST
static_assert(iron::concepts::list_node<iron::unsafe::list_node<int, true>>, "internal error");
static_assert(iron::concepts::list_node<iron::unsafe::list_node<int, false>>, "internal error");
#endif

#if defined(_MSC_VER)
#pragma warning(pop)
#endif
