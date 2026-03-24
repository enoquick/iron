
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable:26429)
#pragma warning (disable:26472)
#pragma warning (disable:26481)
#pragma warning (disable:26800)
#endif

namespace iron::unsafe {

template <typename _T>
class raw_storage final {
public:
	using value_t = _T;
	using size_t = std::size_t;
	using ptrdiff_t = std::ptrdiff_t;
	using difference_type = ptrdiff_t;
	using error_handler_t = iron::error_handler;
	static_assert(sizeof(value_t) < static_cast<std::size_t>(std::numeric_limits<std::ptrdiff_t>::max()), "T sizeof is too big");
	static_assert(!std::is_reference_v<value_t>, "raw_storage template parameter value_type  must not be a reference type");
	static_assert(!std::is_array_v<value_t>, "raw_storage template parameter value_type must not be an array type");
	static_assert(std::is_nothrow_destructible_v<value_t>, "raw_storage template parameter value_type must be nothrow destructible");
	raw_storage() = delete;
	[[nodiscard]] static constexpr size_t max_size()noexcept { return _max() / sizeof(value_t); }
	[[nodiscard]] static constexpr error_handler_t default_extension_policy(size_t& _new_capacity, size_t _current_capacity, size_t _additional)noexcept {
		DEBUG_ASSERT(max_size() >= _current_capacity);
		if (max_size() - _current_capacity >= _additional) IRON_LIKELY
		{
			const auto _c = _current_capacity + _additional;
			if (!_c) IRON_UNLIKELY
			{
				constexpr size_t _optimum_size = 20;
				if constexpr (sizeof(value_t) < _optimum_size) {
					_new_capacity = _optimum_size / sizeof(value_t);
				}
				else {
					_new_capacity = 1;
				}
				return {};
			}
			DEBUG_ASSERT(!_is_hbit(_c));
			const auto _nc = _c << 1; // _c *= 2;
			_new_capacity = _nc <= max_size() ? _nc : max_size();
			return {};
		}
		return ERROR_HANDLER(iron::unsafe::errors::posix::code_t::argument_out_of_domain);
	}
	template <typename _A>
	[[nodiscard]] static constexpr error_handler_t allocate(value_t*& _p, size_t _sz,_A _allocator)noexcept
	requires iron::concepts::allocator<_A>
	{
		DEBUG_ASSERT(!_p);
		if (_sz) IRON_LIKELY 
		{
			auto _a = typename std::allocator_traits<_A>::template rebind_alloc<value_t>(_allocator);
			_p = _a.allocate(_sz);
			if (!_p)  IRON_UNLIKELY
			{
				return ERROR_HANDLER(iron::unsafe::errors::posix::code_t::not_enough_memory);
			}
		}
		return {};
	}
	template <bool _reset = false,typename _A>
	static constexpr void deallocate(value_t* _p, const size_t _n,_A _allocator)noexcept
	requires (iron::concepts::allocator<_A>)
	{
		auto _a = typename std::allocator_traits<_A>::template rebind_alloc<value_t>(_allocator);
		_a. template deallocate<_reset>(_p, _n);
	}
	template <typename _A>
	[[nodiscard]] static constexpr error_handler_t reallocate(value_t*& _p, size_t& _current_capacity, const size_t _n, size_t _new_capacity,_A _a)noexcept
	requires (iron::concepts::allocator<_A> && std::is_nothrow_move_constructible_v<value_t>)
	{
		//preconditions
		DEBUG_ASSERT(_current_capacity <= max_size());
		DEBUG_ASSERT(_new_capacity <= max_size());
		DEBUG_ASSERT(_n <= _current_capacity);
		DEBUG_ASSERT(_new_capacity > _current_capacity);
		//
		if (!_p) IRON_UNLIKELY
		{
			DEBUG_ASSERT(!_current_capacity);
			DEBUG_ASSERT(!_n);
			auto _h = allocate(_p, _new_capacity, _a); RE(_h);
		}
		else {
			value_t* _new_p{};
			auto _h = allocate(_new_p, _new_capacity, _a); RE(_h);
			uninitialized_move_and_destroy_n(_new_p, _n, _p);
			deallocate(_p, _current_capacity, _a);
			_p = _new_p;
		}
		_current_capacity = _new_capacity;
		return {};
	}

	template <typename _A>
	[[nodiscard]] static constexpr error_handler_t shrink_to_fit(value_t*& _current_p, size_t& _current_capacity, const size_t _n,_A _a)noexcept
		requires (iron::concepts::allocator<_A> && std::is_nothrow_move_constructible_v<value_t>)
	{
		DEBUG_ASSERT(_n <= _current_capacity);
		if (!_n) IRON_UNLIKELY
		{
			if (_current_capacity) IRON_LIKELY
			{
				deallocate(_current_p, _current_capacity, _a);
				_current_p = {};
				_current_capacity = {};
			}
			else {
				DEBUG_ASSERT(!_current_p);
			}
		}
		else if (_current_capacity > _n) IRON_LIKELY
		{
			value_t* _p{};
			auto _h = allocate(_p, _n, _a); RE(_h);
			uninitialized_move_and_destroy_n(_p, _n, _current_p);
			deallocate(_current_p, _current_capacity, _a);
			_current_p = _p;
			_current_capacity = _n;
		}
		return {};
	}
	static constexpr value_t* uninitialized_move_and_destroy_n(value_t* _to, const size_t _n, value_t* _from)noexcept
		requires std::is_nothrow_move_constructible_v<value_t>
	{
		DEBUG_ASSERT(!_n || (_to && _from));
		if (std::is_constant_evaluated()) {
			auto _s = _from;
			DEBUG_ASSERT(_s); //ms analyzer error
			auto _c = _n;
			while (_c--) {
				std::construct_at(_to, std::move(*_s));
				if (_c) {
					++_to;
					++_s;
				}
			}
			destroy_n(_from,_n);
		}
		else if constexpr (std::is_trivially_copy_assignable_v<value_t>) {
			std::memcpy(_to, _from, _n * sizeof(value_t));
			_to += _n;
		}
		else {
			auto _s = _from;
			auto _c = _n;
			while (_c--) IRON_LIKELY
			{
				std::construct_at(_to++, std::move(*_s++));
			}
			destroy_n(_from,_n);
		}
		return _to;
	}
	static constexpr value_t* uninitialized_move_n(value_t* _to, size_t _n, value_t* _from)noexcept
		requires std::is_nothrow_move_constructible_v<value_t>
	{
		DEBUG_ASSERT(!_n || (_to && _from));
		if (std::is_constant_evaluated()) {
			while (_n--) {
				std::construct_at(_to, std::move(*_from));
				if (_n) {
					++_to;
					++_from;
				}
			}
		}
		else if constexpr (std::is_trivially_copy_assignable_v<value_t>) {
			std::memcpy(_to, _from, _n * sizeof(value_t));
			_to += _n;
		}
		else {
			while (_n--) IRON_LIKELY
			{
				std::construct_at(_to++, std::move(*_from++));
			}
		}
		return _to;
	}

	[[nodiscard]] static constexpr error_handler_t uninitialized_fill(value_t* _to, size_t _n)noexcept
	requires(std::is_default_constructible_v<value_t>)
	{
		if (std::is_constant_evaluated()) {
			while (_n--) {
				if constexpr (std::is_trivially_constructible_v<value_t>) {
					std::construct_at(_to, value_t{});
				}
				else {
					std::construct_at(_to);
				}
				if (_n) {
					++_to;
				}
			}
		}
		else {
			if constexpr (std::is_nothrow_default_constructible_v<value_t>) {
				if constexpr (std::is_trivially_constructible_v<value_t>) {
					std::memset(_to, 0, _n * sizeof(value_t));
				}
				else {
					while (_n--) IRON_LIKELY
					{
						std::construct_at(_to++);
					}
				}
			}
			else {
				auto _p = _to;
				while (_n--) IRON_LIKELY
				{
					auto _h = construct_at(_p++);
					if (!_h) IRON_UNLIKELY
					{
						destroy_n(_to, _p - _to);
						return _h;
					}
				}
			}
		}
		return {};
	}


	[[nodiscard]] static constexpr error_handler_t uninitialized_fill(value_t* _to, size_t _n, value_t&& _from)noexcept
	requires ( iron::concepts::copyable_or_cloneable<value_t> && std::is_nothrow_move_constructible_v<value_t>)
	{
		DEBUG_ASSERT(!_n || _to);
		if (std::is_constant_evaluated()) {
			if (_n--) {
				auto _p = _to;
				std::construct_at(_p, std::move(_from));
				if (_n) {
					++_p;
				}
				while (_n--) {
					auto _res = iron::clone::try_clone_from(*_to);
					auto& _v = _res.unchecked_unwrap();
					std::construct_at(_p, std::move(_v));
					if (_n) {
						++_p;
					}
				}
			}
		}
		else {
			if constexpr (std::is_nothrow_copy_constructible_v<value_t>) {
				if constexpr(std::is_same_v<value_t,unsigned char>) {
					const int _i = _from;
					std::memset(_to, _i, _n);
				}
				else {
					while (_n--) IRON_LIKELY
					{
						std::construct_at(_to++, _from);
					}
				}
			}
			else {
				if (_n--) IRON_LIKELY
				{
					auto _p = _to;
					std::construct_at(_p++, std::move(_from));
					while (_n--) IRON_LIKELY
					{
						auto _r = iron::clone::try_clone_from(*_to);
						if (!_r)  IRON_UNLIKELY
						{
							destroy_n(_to, _p - _to);
							return std::move(_r.unchecked_unwrap_err());
						}
						std::construct_at(_p++, std::move(_r.unchecked_unwrap()));
					}
				}
			}
		}
		return {};
	}
	[[nodiscard]] static constexpr error_handler_t uninitialized_copy_n(value_t* _to, size_t _n, const value_t* _from)noexcept
		requires (iron::concepts::copyable_or_cloneable<value_t>&& std::is_nothrow_move_constructible_v<value_t>)
	{
		DEBUG_ASSERT(!_n || (_to && _from));
		if (std::is_constant_evaluated()) {
			while (_n--) {
				auto _res = iron::clone::try_clone_from(*_from);
				auto& _v = _res.unchecked_unwrap();
				std::construct_at(_to, std::move(_v));
				if (_n) {
					++_to;
					++_from;
				}
			}
		}
		else {
			if constexpr (std::is_trivially_copy_assignable_v<value_t>) {
				std::memcpy(_to, _from, _n * sizeof(value_t));
			}
			else {
				auto _p = _to;
				while (_n--) IRON_LIKELY
				{
					auto _r = iron::clone::try_clone_from(*_from++);
					if (!_r) IRON_UNLIKELY
					{
						destroy_n(_to, _p - _to);
						return std::move(_r.unchecked_unwrap_err());
					}
					std::construct_at(_p++, std::move(_r.unchecked_unwrap()));
				}
			}
		}
		return {};
	}

	template <typename... _ARGS>
	[[nodiscard]] static constexpr error_handler_t construct_at(value_t* _p, _ARGS&&... _args)noexcept
	requires (iron::concepts::constructible_at<value_t, _ARGS&&...>)
	{
		DEBUG_ASSERT(_p);
		if constexpr(iron::concepts::nothrow_constructible_at<value_t,_ARGS&&...>) {
			std::construct_at(_p, std::forward<_ARGS>(_args)...);
			return {};
		}
		else {
			try {
				std::construct_at(_p, std::forward<_ARGS>(_args)...);
				return {};
			}
			catch (...) {
				return ERROR_HANDLER(std::current_exception());
			}
		}
	}
	static constexpr void destroy_n(value_t* _p, size_t _n)noexcept
	{
		DEBUG_ASSERT(!_n || _p);
		if constexpr (!std::is_trivially_destructible_v<value_t>) {
			std::destroy_n(_p, _n);
		}
	}
	static constexpr void destroy_at(value_t* _p)noexcept
	{
		DEBUG_ASSERT(_p);
		if constexpr (!std::is_trivially_destructible_v<value_t>) {
			std::destroy_at(_p);
		}
	}
	static constexpr void swap_n(value_t* _v, size_t _n,value_t* _u)noexcept
	requires std::is_nothrow_swappable_v<value_t>
	{
		DEBUG_ASSERT(!_n || (_v && _u));
		while (_n--) IRON_LIKELY
		{
			if (std::is_constant_evaluated()) {
				std::swap(*_v, *_u);
				if (_n) {
					++_v;
					++_u;
				}
			}
			else {
				std::swap(*_v++, *_u++);
			}
		}
	}

	[[nodiscard]] static constexpr error_handler_t assign_fill(value_t* _d, size_t _n,value_t&& _v)noexcept
	requires(std::is_nothrow_move_assignable_v<value_t> && iron::concepts::copyable_or_cloneable<value_t>)
	{
		DEBUG_ASSERT(!_n || _d);
		if (std::is_constant_evaluated()) {
			if (_d) {
				*_d = std::move(_v);
				if (--_n) {
					auto _p = _d;
					++_p;
					while (_n--) {
						auto _r = iron::clone::clone_from(*_d);
						*_p = std::move(_r);
						if (_n) {
							++_p;
						}
					}
				}
			}
		}
		else {
			if constexpr (std::is_same_v<value_t,unsigned char>) {
				const int _i = _v;
				std::memset(_d, _i, _n);
			}
			else {
				if (_d) IRON_LIKELY
				{
					if constexpr (std::is_trivially_copy_assignable_v<value_t>) {
						while (_n--) IRON_LIKELY
						{
							*_d++ = _v;
						}
					}
					else {
						*_d = std::move(_v);
						while (--_n) IRON_LIKELY
						{
							auto _r = iron::clone::try_clone_from(*_d); RE(_r);
							*++_d = std::move(_r.unchecked_unwrap());
						}
					}
				}
			}
		}
		return {};
	}

	[[nodiscard]] static constexpr error_handler_t assign_n(value_t* _d,size_t _n,const value_t* _s)noexcept
	requires(std::is_nothrow_move_assignable_v<value_t> && iron::concepts::copyable_or_cloneable<value_t>)
	{
		DEBUG_ASSERT(!_n || (_d && _s));
		if (std::is_constant_evaluated()) {
			while (_n--) {
				auto _r = iron::clone::clone_from(*_s);
				*_d = std::move(_r);
				if (_n) {
					++_d;
					++_s;
				}
			}
		}
		else {
			if constexpr (std::is_trivially_copy_assignable_v<value_t>) {
				std::memcpy(_d, _s, _n * sizeof(value_t));
			}
			else {
				while (_n--) IRON_LIKELY
				{
					auto _r = iron::clone::try_clone_from(*_s++); RE(_r);
					*_d++ = std::move(_r.unchecked_unwrap());
				}
			}
		}
		return {};
	}
	
	template <typename _U>
	[[nodiscard]] static constexpr error_handler_t assign(value_t& _v, _U&& _element)noexcept
	requires(iron::concepts::constructible_at<value_t, _U&&> && std::is_nothrow_move_assignable_v<std::decay_t<_U>>)
	{
		if constexpr (std::is_rvalue_reference_v<_U&&> && std::is_nothrow_convertible_v<_U, value_t>) {
			_v = std::move(_element);
		}
		else if constexpr (std::is_nothrow_convertible_v<_U,value_t>) {
			_v = _element;
		}
		else {
			using _a_t = iron::unsafe::allocator<value_t>;
			_a_t _a{};
			value_t* _t{};
			auto _h = allocate(_t, 1, _a); RE(_h);
			_h = construct_at(_t, std::forward<_U>(_element));
			if (!_h) IRON_UNLIKELY
			{
				_a.deallocate(_t, 1);
				return _h;
			}
			_v = std::move(*_t);
			destroy_at(_t);
			_a.deallocate(_t, 1);
		}
		return {};
	}


private:
	[[nodiscard]] static constexpr bool _is_hbit(size_t _v)noexcept {
		constexpr unsigned int _n = (sizeof(size_t) * CHAR_BIT) - 1;
		return ((_v >> _n) & 1);
	}

	[[nodiscard]] static constexpr size_t _max()noexcept { return static_cast<size_t>(std::numeric_limits<ptrdiff_t>::max()); }
};

} //ns

#if defined(_MSC_VER)
#pragma warning(pop)
#endif
