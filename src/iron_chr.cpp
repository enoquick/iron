#ifdef IRON_NO_USE_MODULE
#include "iron/chr.h"
#include <charconv>
#else
#endif

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable:26446)
#pragma warning (disable:26481)
#pragma warning (disable:26482)
#pragma warning (disable:26497)
#endif

namespace iron {

	auto  chr::_to_digit_nc(char32_t _ch, radix_t _r)noexcept-> iron::option<u32_t> {
		return _to_digit_c(_ch, _r);
	}

	auto chr::_size_utf8_nc(char32_t _ch)noexcept -> size_t {
		return _size_utf8_c(_ch);
	}

	auto chr::_encode_utf8_nc(char8_t* _p, char32_t _ch)noexcept -> size_t {
		char8_t _buff[5];
		const auto _sz = _encode_utf8_c(_buff, _ch);
		std::memcpy(_p, &_buff[0], (_sz + 1) * sizeof(char8_t));
		return _sz;
	}

	auto  chr::_size_utf16_nc(char32_t _ch)noexcept -> size_t {
		return _size_utf16_c(_ch);
	}

	auto chr::_to_lowercase(char32_t _ch)noexcept -> seq_t {
		const auto _iters = _uc_t::lowers(static_cast<u32_t>(_ch));
		return seq_t(_iters.first, _iters.second);
	}

	auto chr::_to_uppercase(char32_t _ch)noexcept -> seq_t {
		const auto _iters = _uc_t::uppers(static_cast<u32_t>(_ch));
		return seq_t(_iters.first, _iters.second);
	}
	auto chr::_from_digit_nc(radix_t _n)noexcept -> iron::option<chr> {
		return _from_digit_c(_n);
	}

	class chr::pimp final {
	public:
		pimp() = delete;
		template <std::size_t _N>
		[[nodiscard]] static const chr* init_escape(chr(&buffer)[_N], const chr&,chr::seq_t::escape_t)noexcept;
		template <std::size_t _N>
		[[nodiscard]] static const chr* init_escape_unicode(chr(&buffer)[_N], const chr&)noexcept;
		[[nodiscard]] static bool is_equal_case_insensitive(const chr&, chr::seq_t&)noexcept;
		[[nodiscard]] static bool is_less_case_insensitive(const chr&, chr::seq_t&)noexcept;
		template <typename _T>
		[[nodiscard]] static size_t encode_utf16(_T* _p, char32_t _ch)noexcept {
			_T _buff[3];
			const auto _sz=_encode_utf16_c(_buff, _ch);
			std::memcpy(_p, &_buff[0], (_sz + 1) * sizeof(_T));
			return _sz;
		}
	private:
		template <std::size_t _N>
		[[nodiscard]] static const chr* _init(chr(&_buffer)[_N], const char32_t*)noexcept;
	};

	template <> auto  chr::_encode_utf16_nc(char16_t* _p, char32_t _ch)noexcept -> size_t {
		return pimp::encode_utf16(_p, _ch);
	}
#ifdef _WIN32 
	template <> auto  chr::_encode_utf16_nc(wchar_t* _p, char32_t _ch)noexcept -> size_t {
		return pimp::encode_utf16(_p, _ch);
	}
#endif 

	template <std::size_t _N>
	inline  const chr* chr::pimp::_init(chr(&_buffer)[_N], const char32_t* _p)noexcept {
		DEBUG_ASSERT(_p);
		auto _b = std::begin(_buffer);
		DEBUG_ASSERT(_b);
		[[maybe_unused]] const auto _e = std::end(_buffer);
		while (*_p) IRON_LIKELY
		{
			DEBUG_ASSERT(_b != _e);
			std::construct_at(_b++, chr::unchecked_from(*_p++));
		}
#ifndef NDEBUG
		auto _br = _b;
		while (_br != _e) IRON_LIKELY
		{
			std::construct_at(_br++);
		}
#endif
		return _b;
	}

	template <std::size_t _N>
	const chr* chr::pimp::init_escape_unicode(chr(&_buffer)[_N], const chr& _c)noexcept {
		static_assert(_N > 10);
		constexpr int _base = 16;
		auto _p = std::begin(_buffer);
		DEBUG_ASSERT(_p);
		std::construct_at(_p++, chr(U'0'));
		std::construct_at(_p++, chr(U'X'));
		char _v[9]{};
		auto _b = std::begin(_v);
		DEBUG_ASSERT(_b);
		const auto _r = std::to_chars(_b, std::end(_v), _c._m_c, _base);
		DEBUG_ASSERT(_r.ec == std::errc());
		while (_b != _r.ptr) IRON_LIKELY
		{
			DEBUG_ASSERT(_p != std::end(_buffer));
			std::construct_at(_p++, chr::unchecked_from_u32(static_cast<u32::value_t>(*_b++)));
		}
		return _p;
	}

	template <std::size_t _N>
	const chr* chr::pimp::init_escape(chr(&_buffer)[_N], const chr& _c,chr::seq_t::escape_t _e)noexcept {
		static_assert(_N > 10);
		using e_t = chr::seq_t::escape_t;
		DEBUG_ASSERT(_e == e_t::debug || _e == e_t::def);
		switch (_c._m_c) {
			case 0x00: return _e == e_t::debug ? _init(_buffer, U"\\0") : init_escape_unicode(_buffer, _c);
			case U'\t': return _init(_buffer, U"\\t");
			case U'\n': return _init(_buffer, U"\\n");
			case U'\r': return _init(_buffer, U"\\r");
			case U'\\': return _init(_buffer, U"\\\\");
			case U'"': return _init(_buffer, U"\\\"");
			case U'\'': return _init(_buffer, U"\\'");
			default: {}
		}
		if (_c.is_ascii_graphic() || _c.is_ascii_whitespace() || (_e == e_t::debug && _c.is_alphanumeric())) IRON_LIKELY
		{
			auto _b = std::begin(_buffer);
			DEBUG_ASSERT(_b);
			std::construct_at(_b++, _c);
#ifndef NDEBUG
			auto _bb = _b;
			while (_bb != std::end(_buffer)) IRON_LIKELY
			{
				std::construct_at(_bb++, _c);
			}
#endif
			return _b;
		}
		return init_escape_unicode(_buffer, _c);
	}



	chr::sequence::sequence(_uc_t::iter_t _bs,_uc_t::iter_t _es)noexcept
		: _m_current()
		, _m_sz()
	{ //copy [_bs,_es[ in m_v
		const auto _start = _bs;
		auto _bd = std::begin(_m_v);
		while (_bs != _es) IRON_LIKELY
		{
			DEBUG_ASSERT(_bd != std::end(_m_v));
			*_bd++ = unchecked_from_u32(*_bs++);
		}
		*_bd = {};
		_m_sz = static_cast<size_t>(std::distance(_start, _bs));
	}


	chr::sequence::sequence(const value_t& _c, escape_t _e)noexcept
		: _m_current()
		, _m_sz()
	{
		switch (_e) {
			case escape_t::debug: { _m_sz = pimp::init_escape(_m_v, _c,_e) - std::begin(_m_v); return; }
			case escape_t::def: { _m_sz = pimp::init_escape(_m_v, _c,_e) - std::begin(_m_v); return; }
			case escape_t::unicode: { _m_sz = pimp::init_escape_unicode(_m_v, _c) - std::begin(_m_v); return; }
		}
		DEBUG_ASSERT(false);
	}

	inline bool chr::pimp::is_equal_case_insensitive(const chr& _ch,chr::seq_t& _i)noexcept {
		while (auto _opt = _i.next()) IRON_LIKELY
		{
			if (_opt.unchecked_unwrap() == _ch) IRON_UNLIKELY
			{
				return true;
			}
		}
		return {};
	}

	inline bool chr::pimp::is_less_case_insensitive(const chr& _ch, chr::seq_t& _i)noexcept {
		while (auto _opt = _i.next()) IRON_LIKELY
		{
			if (_ch < _opt.unchecked_unwrap()) IRON_LIKELY
			{
				return true;
			}
		}
		return {};
	}

	bool chr::_equal_case_insensitive(const chr& _ch1, const chr& _ch2)noexcept {
		DEBUG_ASSERT(_ch1.to_u32() != _ch2.to_u32());
		if (_ch1.is_unicode() && _ch2.is_unicode()) IRON_LIKELY
		{
			if (_ch1.is_lowercase()) IRON_LIKELY
			{
				auto _iter = _ch2.to_lowercase();
				return pimp::is_equal_case_insensitive(_ch1, _iter);
			}
			if (_ch1.is_uppercase()) IRON_LIKELY
			{
				auto _iter = _ch2.to_uppercase();
				return pimp::is_equal_case_insensitive(_ch1, _iter);
			}
		}
		return {};
	}

	bool chr::_less_case_insensitive(const chr& _ch1, const chr& _ch2)noexcept {
		DEBUG_ASSERT(_ch1.to_u32() != _ch2.to_u32());
		if (_ch1.is_unicode() && _ch2.is_unicode()) IRON_LIKELY
		{
			if (_ch1.is_lowercase()) IRON_LIKELY
			{
				auto _iter = _ch2.to_lowercase();
				return pimp::is_less_case_insensitive(_ch1, _iter);
			}
			if (_ch1.is_uppercase()) IRON_LIKELY
			{
				auto _iter = _ch2.to_uppercase();
				return pimp::is_less_case_insensitive(_ch1, _iter);
			}
		}
		return _ch1 < _ch2;
	}

};

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

