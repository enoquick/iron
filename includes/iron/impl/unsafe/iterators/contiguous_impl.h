

namespace iron::unsafe::iterators {


#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable:26481)
#endif

template <typename _T>
class contiguous_iterator final {
public:
	static_assert(!std::is_reference_v<_T>, "iterator template parameter must not be a reference type");
	static_assert(!std::is_array_v<_T>, "iterator template parameter must not be an array type");
	using value_type = std::decay_t<_T>;
	using value_t = value_type;
	using iterator_category = std::contiguous_iterator_tag;
	using difference_type = std::ptrdiff_t;
	using diff_t = difference_type;
	using iterator_concept = std::contiguous_iterator_tag;
	using pointer = _T*;
	using reference = _T&;
	using size_t = std::size_t;
	using size_type = size_t;
	struct contiguous_iterator_tag { contiguous_iterator_tag() = delete; };
	constexpr contiguous_iterator() noexcept = default;
	constexpr explicit contiguous_iterator(pointer _p) noexcept : _m_ptr(_p) {  }
	constexpr contiguous_iterator(const contiguous_iterator&) noexcept = default;
	constexpr contiguous_iterator(contiguous_iterator&&)noexcept = default;
	constexpr contiguous_iterator& operator=(const contiguous_iterator& _oth)noexcept {
		_m_ptr = _oth._m_ptr;
		return *this;
	}
	constexpr contiguous_iterator& operator=(contiguous_iterator&&)noexcept = default;
	constexpr ~contiguous_iterator() = default;
	template<typename _U>
	constexpr contiguous_iterator(const contiguous_iterator<_U>& _other) noexcept
	requires std::convertible_to<_U*, _T*>
	: _m_ptr(_other._m_ptr) {}

	[[nodiscard]] constexpr pointer operator->()const noexcept {
		return _m_ptr;
	}
	[[nodiscard]] constexpr reference operator*()const noexcept {
		DEBUG_ASSERT(_m_ptr);
		return *_m_ptr;
	}
	[[nodiscard]] constexpr reference operator[](std::size_t _idx) const noexcept {
		DEBUG_ASSERT(_m_ptr);
		return _m_ptr[_idx];
	}
	constexpr contiguous_iterator& operator++()noexcept {
		DEBUG_ASSERT(_m_ptr);
		++_m_ptr;
		return *this;
	}
	constexpr contiguous_iterator operator++(int)noexcept {
		DEBUG_ASSERT(_m_ptr);
		auto _temp = *this;
		++_m_ptr;
		return _temp;
	}
	constexpr contiguous_iterator& operator--()noexcept {
		DEBUG_ASSERT(_m_ptr);
		--_m_ptr;
		return *this;
	}
	constexpr contiguous_iterator operator--(int)noexcept {
		DEBUG_ASSERT(_m_ptr);
		auto _temp = *this;
		--_m_ptr;
		return _temp;
	}
	constexpr contiguous_iterator& operator+=(difference_type _n)noexcept {
		DEBUG_ASSERT(_m_ptr);
		_m_ptr += _n;
		return *this;
	}
	constexpr contiguous_iterator& operator-=(difference_type _n)noexcept {
		DEBUG_ASSERT(_m_ptr);
		_m_ptr -= _n;
		return *this;
	}
	constexpr difference_type operator-(const contiguous_iterator& _other) const noexcept {
		DEBUG_ASSERT((_m_ptr && _other._m_ptr) || (!_m_ptr && !_other._m_ptr));
		return _m_ptr - _other._m_ptr;
	}
	constexpr contiguous_iterator operator+(difference_type _n) noexcept {
		DEBUG_ASSERT(_m_ptr);
		return contiguous_iterator(_m_ptr + _n);
	}
	[[nodiscard]] constexpr bool operator==(const contiguous_iterator& _other) const noexcept {
		return _m_ptr == _other._m_ptr;
	}
	[[nodiscard]] constexpr bool operator!=(const contiguous_iterator& _other) const noexcept {
		return _m_ptr != _other._m_ptr;
	}
	[[nodiscard]] constexpr bool operator<(const contiguous_iterator& _other) const noexcept {
		return _m_ptr < _other._m_ptr;
	}
	[[nodiscard]] constexpr bool operator<=(const contiguous_iterator& _other) const noexcept {
		return _m_ptr <= _other._m_ptr;
	}
	[[nodiscard]] constexpr bool operator>(const contiguous_iterator& _other) const noexcept {
		return _m_ptr > _other._m_ptr;
	}
	[[nodiscard]] constexpr bool operator>=(const contiguous_iterator& _other) const noexcept {
		return _m_ptr >= _other._m_ptr;
	}
	[[nodiscard]] constexpr auto operator<=>(const contiguous_iterator& _other) const noexcept {
		return _m_ptr <=> _other._m_ptr;
	}
private:
	template<typename _U> friend class contiguous_iterator;
	pointer _m_ptr{};
};

template <typename _T>
[[nodiscard]] inline constexpr contiguous_iterator<_T> operator+(const contiguous_iterator<_T>& _v, typename contiguous_iterator<_T>::difference_type _n)noexcept {
	auto _t = _v;
	_t += _n;
	return _t;
}

template <typename _T>
[[nodiscard]]  inline constexpr contiguous_iterator<_T> operator+(typename contiguous_iterator<_T>::difference_type _n, const contiguous_iterator<_T>& _v)noexcept {
	return _v + _n;
}

template <typename _T>
[[nodiscard]] inline constexpr contiguous_iterator<_T> operator-(const contiguous_iterator<_T>& _v, typename contiguous_iterator<_T>::difference_type _n)noexcept {
	auto _t = _v;
	_t -= _n;
	return _t;
}

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

}

