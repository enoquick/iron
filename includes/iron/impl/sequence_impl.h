
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable:26472)
#endif

namespace iron {

	template <typename _ITER>
	class sequence {
	public:
		using iter_type=_ITER;
		using iter_t = iter_type;
		static constexpr bool is_forward_iterator=std::forward_iterator<iter_type>;
		static constexpr bool is_bidirectional_iterator = std::bidirectional_iterator<iter_type>;
		static constexpr bool is_random_iterator = std::random_access_iterator<iter_type>;
		static constexpr bool is_contiguous_iterator = std::contiguous_iterator<iter_type>;
		static_assert(is_forward_iterator, "ITER is not a fwd iterator");
		static constexpr bool is_ref = std::is_lvalue_reference_v<decltype(*std::declval<iter_type>())>;
	private:
		using _iter_value_t = std::remove_reference_t<typename std::iterator_traits<iter_type>::reference>;
	public:
		using value_t = std::conditional_t<is_ref, typename std::iterator_traits<iter_type>::value_type, std::decay_t<_iter_value_t>>;
		using value_type = value_t;
		using ref_t = std::conditional_t<is_ref, iron::ref_wrapper<_iter_value_t>,_iter_value_t>;
		using reference = ref_t;
		struct sequence_tag { sequence_tag() = delete; };
		static constexpr bool is_const = []()constexpr noexcept->bool {
			if constexpr (std::is_lvalue_reference_v<decltype(*std::declval<iter_type>())>) {
				return std::is_const_v<typename ref_t::value_t>;
			}
			else {
				return true;
			}
		}();
		using difference_type = typename iter_type::difference_type;
		using difference_t = difference_type;
		using size_type = std::size_t;
		using size_t = size_type;
		using error_handler_t = iron::error_handler;
		using error_handler_type = error_handler_t;
		template<typename _F>
		[[nodiscard]] static consteval bool is_bool_predicate()noexcept { return std::is_nothrow_invocable_r_v<bool, _F&, const value_t&>; }
		template <typename _F,typename _OTHVALUE>
		[[nodiscard]] static consteval bool is_cmp_predicate()noexcept { return std::is_nothrow_invocable_r_v<bool, _F&, const value_t&,const _OTHVALUE&>; }
		template<typename _F>
		[[nodiscard]] static consteval bool is_transform_predicate()noexcept { return std::is_nothrow_invocable_r_v<iron::result<bool,iron::error_handler>, _F&,value_t&>; }
		constexpr sequence()noexcept = default;
		template <typename _MASTER>
		[[nodiscard]] static constexpr sequence unchecked_from(const _MASTER& _master, const iter_type& _b, const iter_type& _e)noexcept 
		IRON_SECURITY_REQ()
		{
			return sequence(_master,_b, _e);
		}
		constexpr sequence(const sequence& _v)noexcept 
			: _m_b(_v._m_b)
			, _m_e(_v._m_e)
		{
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			IRON_SECURITY_CHECK_MASTER_LIVE(_v);
			IRON_SECURITY_ASSIGN(_v);
		}
		constexpr sequence(sequence&& _v)noexcept
			: _m_b(std::move(_v._m_b))
			, _m_e(std::move(_v._m_e))
		{
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			IRON_SECURITY_CHECK_MASTER_LIVE(_v);
			IRON_SECURITY_SWAP(_v);
			IRON_SECURITY_MARK_TEMPORARY(_v);
		}
		constexpr sequence& operator=(const sequence& _v)noexcept {
			auto _t=_v;
			_swap(_t);
			return *this;
		}
		constexpr sequence& operator=(sequence&& _v)noexcept {
#if IRON_SECURITY_CHECKER
			auto _t=std::move(_v);
			_swap(_t);
#else
			_swap(_v);
#endif
			return *this;
		}
		constexpr ~sequence() = default;
		constexpr sequence& swap(sequence& _v)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE(_v);
			_swap(_v);
			return *this;
		}
		[[nodiscard]] constexpr sequence clone()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			return *this;
		}
		[[nodiscard]] constexpr bool empty()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			return _empty();
		}
		[[nodiscard]] constexpr iron::option<size_t> size()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			if constexpr (is_contiguous_iterator) {
				return iron::some(_size());
			}
			else {
				return {};
			}
		}
#if IRON_SECURITY_CHECKER
		template <typename _S>
		[[nodiscard]] constexpr bool is_same_container(const _S& _s)const noexcept
		requires iron::concepts::sequence<_S>
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_s);
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE(_s);
			return _m_ck.same_data(_s.checker());
		}
#endif
		[[nodiscard]] constexpr bool operator==(const sequence& _v)const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_v);
			IRON_SECURITY_CHECK_MASTER_LIVE(_v);
#if IRON_SECURITY_CHECKER
			PANIC_UNLESS(is_same_container(_v), "the sequence not share the same container");
#endif
			return _m_b == _v._m_b;
		}

		[[nodiscard]] constexpr size_t count()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			if constexpr (is_contiguous_iterator) {
				const auto _c= _size();
				_reset();
				return _c;
			}
			else {
				return _count();
			}
		}
		template <typename _F>
		[[nodiscard]] constexpr size_t count_if(_F _predicate)noexcept
		requires(is_bool_predicate<_F>())
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			return _count_if(_predicate);
		}

		[[nodiscard]] constexpr explicit operator bool()const noexcept { return !empty(); }

		[[nodiscard]] constexpr iter_t begin()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			return _m_b;
		}
		[[nodiscard]] constexpr iter_t& begin()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			return _m_b;
		}
		[[nodiscard]] constexpr iter_t end()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			return _m_e;
		}
		constexpr sequence& operator++()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			_advance();
			return *this;
		}

		[[nodiscard]] constexpr sequence operator++(int)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			auto _t = *this;
			_advance();
			return _t;
		}
		constexpr sequence& operator+=(size_t _n)noexcept {
			return skip(_n);
		}
		[[nodiscard]] constexpr auto& unsafe_peek()const noexcept
		requires(is_ref)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			DEBUG_ASSERT(!_empty());
			auto _r=_peek(_m_b);
			return _r.value();
		}
		[[nodiscard]] constexpr auto unsafe_peek()const noexcept
		requires(!is_ref)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			DEBUG_ASSERT(!_empty());
			return _peek(_m_b);
		}
		[[nodiscard]] constexpr iron::option<ref_t> peek()const noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			if (_empty()) IRON_UNLIKELY
			{
				return {};
			}
			return iron::some(_peek(_m_b));
		}
		[[nodiscard]] constexpr ref_t operator*()const noexcept {
			return peek().unwrap();
		}
		constexpr sequence& reset()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			_reset();
			return *this;
		}
		[[nodiscard]] constexpr auto& unchecked_next()noexcept 
		requires(is_ref)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			DEBUG_ASSERT(!_empty());
			auto& _v=*_m_b++;
			_reset_if_empty();
			return _v;
		}
		[[nodiscard]] constexpr auto unchecked_next()noexcept
		requires(!is_ref)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			DEBUG_ASSERT(!_empty());
			auto _v = *_m_b++;
			_reset_if_empty();
			return _v;
		}

		[[nodiscard]] constexpr iron::option<ref_t> next()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			if (_empty()) IRON_UNLIKELY
			{
				return {};
			}
			auto _opt=iron::some(_peek(_m_b++));
			_reset_if_empty();
			return _opt;
		}
		[[nodiscard]] constexpr iron::option<ref_t> last()noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			if (_empty()) IRON_UNLIKELY
			{
				return {};
			}
			return iron::some(_last());
		}
		[[nodiscard]] constexpr auto reverse()noexcept
		requires(is_bidirectional_iterator)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			using _riter_t = std::reverse_iterator<iter_type>;
			using _seq_t = sequence<_riter_t>;
			const auto _b = _riter_t(_m_e);
			const auto _e = _riter_t(_m_b);
			auto _seq = _seq_t::unchecked_from(*this, _b, _e);
			_reset();
			return _seq;
		}
		template <typename _F>
		[[nodiscard]] constexpr iron::option<ref_t> for_each_condition(_F _f)noexcept 
		requires(is_cmp_predicate<_F,value_t>())
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			const auto _iter = _for_each_condition(_f);
			if (_iter == _m_e) {
				DEBUG_ASSERT(empty());
				_reset();
				return {};
			}
			auto _opt=iron::some(_peek(_iter));
			DEBUG_ASSERT(empty());
			_reset_if_empty();
			return _opt;
		}
		template <typename _F>
		constexpr sequence& all(_F _predicate)noexcept
		requires(is_bool_predicate<_F>())
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			_all(_predicate);
			return *this;
		}
		template <typename _F>
		constexpr sequence& any(_F _predicate)noexcept
		requires(is_bool_predicate<_F>())
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			_any(_predicate);
			return *this;
		}
		template <typename _ENUMERATE>
		[[nodiscard]] constexpr auto enumerate(_ENUMERATE _start)noexcept
		requires (iron::concepts::numeric_integral<_ENUMERATE>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			using _ret_t = std::pair<_ENUMERATE,ref_t>;
			using _iter_t = iron::unsafe::iterators::enumerate_iterator<iter_t,_ENUMERATE,_ret_t>;
			const _iter_t _b(*this,_m_b, _start);
			const _iter_t _e(_m_e);
			using _seq_t = sequence<_iter_t>;
			auto _seq = _seq_t::unchecked_from(*this, _b, _e);
			_reset();
			return _seq;
		}
		[[nodiscard]] constexpr auto step_by(size_t _step)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			PANIC_UNLESS(_step, "step_by: argument must be > 0");
			using _iter_t = iron::unsafe::iterators::step_iterator<iter_t>;
			const _iter_t _b(_m_b, _m_e, _step);
			const _iter_t _e(_m_e);
			using _seq_t = sequence<_iter_t>;
			auto _seq = _seq_t::unchecked_from(*this, _b, _e);
			_reset();
			return _seq;
		}
		[[nodiscard]] constexpr auto slice(size_t _from,size_t _n=std::numeric_limits<size_t>::max())noexcept {
			return skip(_from).take(_n);
		}

		template <typename _S>
		[[nodiscard]] constexpr bool eq(_S& _oth)noexcept 
		requires(
			iron::concepts::sequence<_S>
			&& iron::concepts::nothrow_equality_comparable_with<value_t, typename _S::value_t>
			&& !std::is_const_v<_S>
		)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_oth);
			IRON_SECURITY_CHECK_MASTER_LIVE(_oth);
			const auto _b=_eq(*this, _oth);
			_reset();
			_oth._reset();
			return _b;
		}
		template <typename _S>
		[[nodiscard]] constexpr bool ne(_S& _oth)noexcept 
			requires(
			iron::concepts::sequence<_S>
			&& iron::concepts::nothrow_equality_comparable_with<value_t, typename _S::value_t>
			&& !std::is_const_v<_S>
			)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_oth);
			IRON_SECURITY_CHECK_MASTER_LIVE(_oth);
			const auto _b=_ne(*this, _oth);
			_reset();
			_oth._reset();
			return _b;
		}
		template <typename _S,typename _F>
		[[nodiscard]] constexpr sequence& cmp(_S& _oth, _F _cmp_function)noexcept 
			requires(
			iron::concepts::sequence<_S>
			&& !std::is_const_v<_S>
			&& is_cmp_predicate<_F, typename _S::value_t>()
			)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			IRON_SECURITY_CHECK_TEMPORARY(_oth);
			IRON_SECURITY_CHECK_MASTER_LIVE(_oth);
			_cmp(*this, _oth, _cmp_function);
			return *this;
		}
		template <typename _F>
		[[nodiscard]] constexpr auto filter(_F _predicate)noexcept 
		requires(is_bool_predicate<_F>())
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			using _iter_t = iron::unsafe::iterators::filter_iterator<iter_t, _F>;
			const _iter_t _b(_m_b, _m_e, _predicate);
			const _iter_t _e(_m_e, _predicate);
			using _seq_t = sequence<_iter_t>;
			auto _seq = _seq_t::unchecked_from(*this, _b, _e);
			_reset();
			return _seq;
		}
		template <typename _F>
		constexpr sequence& for_each(_F _function)noexcept 
		requires(is_bool_predicate<_F>())
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			_for_each(_function);
			return *this;
		}
		template <typename _F>
		[[nodiscard]] constexpr error_handler_t try_transform(_F _transform_function)noexcept 
		requires(is_transform_predicate<_F>() && !is_const)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			return _transform(_transform_function);
		}
		template <typename _F>
		constexpr sequence& transform(_F _transform_function)noexcept 
		requires(is_transform_predicate<_F>() && !is_const)
		{
			const auto _h = try_transform(_transform_function); RVP(_h, *this);
		}

		template <typename _F>
		constexpr sequence& find(_F _predicate, size_t _n = {})noexcept
		requires (is_bool_predicate<_F>())
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			_find(_predicate,_n);
			return *this;
		}
		constexpr sequence& find(const value_t& _v, size_t _n = {})noexcept 
		requires(iron::concepts::nothrow_equality_comparable<value_t>)
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			_find([&_v](const value_t& _c)noexcept { return _v == _c; },_n);
			return *this;
		}
		template <typename _F>
		[[nodiscard]] constexpr iron::option<size_t> position(_F _predicate, size_t _n = {})noexcept
		requires (is_bool_predicate<_F>())
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			return _position(_predicate,_n);
		}
		constexpr sequence& skip(size_t _n=1)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			if constexpr (is_contiguous_iterator) {
				_advance_contiguos(_n);
			}
			else {
				_advance(_n);
			}
			return *this;
		}
		constexpr auto take(size_t _n)noexcept {
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			using _iter_t = iron::unsafe::iterators::take_iterator<iter_t>;
			const _iter_t _b(_m_b, _m_e, _n);
			const _iter_t _e(_m_e);
			using _seq_t = sequence<_iter_t>;
			auto _seq = _seq_t::unchecked_from(*this, _b, _e);
			_reset();
			return _seq;
		}
		template <typename _F>
		constexpr sequence& skip_if(_F _predicate)noexcept
		requires (is_bool_predicate<_F>())
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			_skip<true>(_predicate);
			return *this;
		}
		template <typename _F>
		constexpr  sequence& take_if(_F _predicate)noexcept
		requires (is_bool_predicate<_F>())
		{
			IRON_SECURITY_CHECK_TEMPORARY_THIS();
			IRON_SECURITY_CHECK_MASTER_LIVE_THIS();
			_skip<false>(_predicate);
			return *this;
		}
		IRON_SECURITY_METHOD();
	private:
		template <typename _MASTER>
		constexpr sequence(const _MASTER& _master,const iter_type& _b, const iter_type& _e)noexcept 
		: _m_b(_b)
		, _m_e(_e)
		{
			IRON_SECURITY_MAKE_REF(_master);
			_reset_if_empty();
		}
		constexpr void _swap(sequence& _v)noexcept {
			std::swap(_m_b, _v._m_b);
			std::swap(_m_e, _v._m_e);
			IRON_SECURITY_SWAP(_v);
		}
		[[nodiscard]] constexpr bool _empty()const noexcept { 
			return _m_b == _m_e; 
		}
		constexpr void _reset()noexcept {
			_m_b = _m_e;
			IRON_SECURITY_RESET_THIS();
		}
		constexpr void _reset_if_empty()noexcept {
#if IRON_SECURITY_CHECKER
			if (_empty()) {
				IRON_SECURITY_RESET_THIS();
			}
#endif
		}
		constexpr void _advance()noexcept {
			if (!_empty()) IRON_LIKELY
			{
				++_m_b;
				_reset_if_empty();
			}
		}
		constexpr void _advance(size_t _n)noexcept 
		requires(!is_contiguous_iterator)
		{
			while (_n-- && !_empty()) IRON_LIKELY
			{
				++_m_b;
			}
			_reset_if_empty();
		}
		constexpr void _advance_contiguos(size_t _n)noexcept 
		requires(is_contiguous_iterator)
		{
			if (_size() < _n) IRON_UNLIKELY
			{
				_reset();
			}
			else {
				_m_b += static_cast<difference_t>(_n);
				_reset_if_empty();
			}
		}
		[[nodiscard]] constexpr size_t _size()const noexcept 
		requires(is_contiguous_iterator)
		{
			return static_cast<size_t>(std::distance(_m_b, _m_e));
		}
		[[nodiscard]] constexpr size_t _count()noexcept 
		requires(!is_contiguous_iterator)
		{
			size_t _c{};
			while (!_empty()) IRON_LIKELY
			{
				++_c;
				++_m_b;
			}
			_reset_if_empty();
			return _c;
		}
		template <typename _F>
		[[nodiscard]] constexpr size_t _count_if(_F _f)noexcept {
			size_t _c{};
			while (!_empty()) IRON_LIKELY
			{
				if (_f(*_m_b)) {
					++_c;
				}
				++_m_b;
			}
			_reset_if_empty();
			return _c;
		}
		[[nodiscard]] constexpr ref_t _peek(iter_t _iter)const noexcept {
			DEBUG_ASSERT(_iter != _m_e);
			if constexpr (is_ref) {
				return iron::make_ref(*this, *_iter);
			}
			else if constexpr (iron::concepts::proxy_pair<decltype(*std::declval<iter_type>())>) {
#if IRON_SECURITY_CHECKER
				using _proxy_pair_t = std::decay_t<decltype(*std::declval<iter_type>())>;
				auto _v = *_iter;
				return _proxy_pair_t::unchecked_from(*this,_v.first,_v.second);
#else
				return *_iter;
#endif
			}
			else {
				return *_iter;
			}
		}
		[[nodiscard]] constexpr ref_t _last()noexcept {
			DEBUG_ASSERT(!_empty());
			if constexpr (is_contiguous_iterator) {
				_m_b += (std::distance(_m_b, _m_e) - 1);
				DEBUG_ASSERT(!empty());
				auto _r=_peek(_m_b++);
				DEBUG_ASSERT(_empty());
				_reset_if_empty();
				return _r;
			}
			else {
				while (true) {
					const auto _b = _m_b++;
					if (_empty()) IRON_UNLIKELY
					{
						auto _r= _peek(_b);
						DEBUG_ASSERT(_empty());
						_reset_if_empty();
						return _r;
					}
				}
				DEBUG_ASSERT(0);
			}
		}
		template <typename _F>
		constexpr void _all(_F _f)noexcept {
			while (!_empty() && _f(*_m_b)) 
			{
				++_m_b;
			}
			_reset_if_empty();
		}
		template <typename _F>
		constexpr void _any(_F _f)noexcept {
			while (!_empty() && !_f(*_m_b)) 
			{
				++_m_b;
			}
			_reset_if_empty();
		}
		[[nodiscard]] static constexpr bool _equal(const iter_t& _b1, const iter_t& _b2)noexcept {
			return *_b1 == *_b2;
		}
		template <typename _S>
		[[nodiscard]] static constexpr bool _eq(sequence& _s1,_S& _s2)noexcept 
		{
			auto& _b1 = _s1._m_b;
			auto& _b2 = _s2._m_b;
			while (!_s1._empty() && !_s2._empty()) IRON_LIKELY
			{
				if (!_equal(_b1,_b2)) {
					return {};
				}
				++_b1;
				++_b2;
			}
			return _s1._empty() == _s2._empty();
		}
		template <typename _S>
		[[nodiscard]] static constexpr bool _ne(sequence& _s1,_S& _s2)noexcept {
			auto& _b1 = _s1._m_b;
			auto& _b2 = _s2._m_b;
			while (!_s1._empty() && !_s2._empty()) IRON_LIKELY 
			{
				if (!_equal(_b1,_b2)) {
					return true;
				}
				++_b1;
				++_b2;
			}
			return _s1._empty() != _s2._empty();
		}
		template <typename _S,typename _F>
		static constexpr void _cmp(sequence& _s1,_S& _s2,_F _f)noexcept {
			auto& _b1 = _s1._m_b;
			auto& _b2 = _s2._m_b;
			while (!_s1._empty() && !_s2._empty()) IRON_LIKELY
			{
				if (!_f(*_b1,*_b2)) {
					break;
				}
				++_b1;
				++_b2;
			}
			_s1._reset_if_empty();
			_s2._reset_if_empty();
		}
		template <typename _F>
		constexpr void _for_each(_F _f)noexcept {
			while (!_empty()) IRON_LIKELY
			{
				if (!_f(*_m_b)) {
					break;
				}
				++_m_b;
			}
			_reset_if_empty();
		}
		template <typename _F>
		[[nodiscard]] constexpr iron::error_handler _transform(_F _f)noexcept {
			while (!_empty()) IRON_LIKELY
			{
				auto _r = _f(*_m_b); RE(_r);
				if (!_r.unchecked_unwrap()) {
					break;
				}
				++_m_b;
			}
			_reset_if_empty();
			return {};
		}
		template <typename _F>
		constexpr void _find(_F _f,size_t _n)noexcept {
			size_t _count{};
			while (!_empty()) IRON_LIKELY
			{
				if (_f(*_m_b)) {
					if (_count == _n) {
						break;
					}
					++_count;
				}
				++_m_b;
			}
			_reset_if_empty();
		}
		template <typename _F>
		[[nodiscard]] constexpr iron::option<size_t> _position(_F _f,size_t _n)noexcept {
			size_t _c{}, _count{};
			while (!_empty()) IRON_LIKELY
			{
				if (_f(*_m_b)) {
					if (_count == _n) {
						break;
					}
					++_count;
				}
				++_m_b;
				++_c;
			}
			if (_empty()) IRON_UNLIKELY
			{
				_reset();
				return {};
			}
			return iron::some(_c);
		}
		template <bool _B,typename _F>
		constexpr void _skip(_F _f)noexcept {
			if (!_empty()) IRON_LIKELY
			{
				const auto _b = _f(*_m_b);
				if constexpr (_B) { //skip
					if (_b) 
					{
						++_m_b;
					}
				}
				else { //take
					if (!_b)
					{
						++_m_b;
					}
				}
				_reset_if_empty();
			}
		}
		template <typename _F>
		[[nodiscard]] constexpr iter_t _for_each_condition(_F _f)noexcept {
			auto _iter = _empty() ? _m_e : _m_b++;
			while (!empty()) IRON_LIKELY
			{
				if (_f(*_iter, *_m_b)) {
					_iter = _m_b;
				}
				++_m_b;
			}
			return _iter;
		}
		iter_t _m_b{},_m_e{};
		IRON_SECURITY_DCL();
	};

}


#if defined(_MSC_VER)
#pragma warning(pop)
#endif
