#ifdef IRON_NO_USE_MODULE
#include "iron/unsafe/unicode.h"
#include "iron/unsafe/unicode_conversions.h"
#endif

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:26481)
#pragma warning(disable:26485)
#pragma warning(disable:6011)
#endif

namespace iron::unsafe {
	namespace unicode_impl {
#include "iron/impl/unsafe/unicode/structs.h"
#include "iron/impl/unsafe/unicode/ranges.h"
#include "iron/impl/unsafe/unicode/sections.h"
		static constexpr auto m_sections_size = sizeof(s_sections) / sizeof(s_sections[0]);

	}

	class unicode::pimp final {
	public:
		pimp() = delete;
		using data_t = unicode_impl::struct_data;
		using section_t = unicode_impl::struct_section;
		using value_t = data_t::value_t;
		static const data_t*const search(value_t)noexcept;
		static seq_t cases(const value_t*)noexcept;
	private:
		static const section_t* const _binary_search(const section_t*,std::size_t,value_t)noexcept;

	};

	auto unicode::pimp::_binary_search(const section_t* _ss, std::size_t _sz, value_t _code)noexcept -> const section_t* const {
		DEBUG_ASSERT(_sz > 0);
		std::size_t _low{};
		auto _high = _sz - 1;
		while (_low <= _high) IRON_LIKELY
		{
			const auto _mid = _low + ((_high - _low) / 2);
			auto _s = _ss + _mid;
			if (_code >= _s->from &&  _code <= _s->to) {
				return _s;
			}
			if (_s->to < _code) {
				_low = _mid + 1;
			}
			else {
				_high = _mid - 1;
			}
		}
		return {};
	}

	inline auto unicode::pimp::search(value_t _code)noexcept-> const data_t*const {
		auto& _ss = unicode_impl::s_sections;
		if (_is_ascii(_code)) IRON_UNLIKELY //normally search is used for not ascii
		{
			auto _s = _ss + _code;
			DEBUG_ASSERT(_s->begin);
			return &*(_s->begin);
		}
		constexpr auto _sz = sizeof(_ss) / sizeof(_ss[0]);
		if (const auto _s=_binary_search(_ss,_sz,_code)) IRON_LIKELY
		{
			DEBUG_ASSERT(_s->begin);
			DEBUG_ASSERT(_code >= _s->from);
			DEBUG_ASSERT(_code <= _s->to);
			auto _b = _s->begin;
			return _s->shared
				? &*_b
				: &*(_b + (_code - _s->from))
				;
		}
		return {};
	}

	bool unicode::_is(u32_t _u, attr_t _c)noexcept {
		if (auto _d = pimp::search(_u)) IRON_LIKELY
		{
			using _n_t = std::underlying_type_t<attr_t>;
			const auto _pow2 = _n_t{ 1 } << static_cast<_n_t>(_c); //pow(2,n)
			if (_d->cat & _pow2) {
				return true;
			}
		}
		return {};
	}

	inline auto unicode::pimp::cases(const value_t* _p)noexcept->seq_t {
		DEBUG_ASSERT(_p);
		auto _b = _p;
		while (*_b) IRON_LIKELY
		{
			++_b;
		}
		return std::make_pair(iter_t(_p), iter_t(_b));
	}
	auto unicode::_uppers(u32_t _u)noexcept->seq_t {
		if (auto _d = pimp::search(_u)) IRON_LIKELY
		{
			return pimp::cases(_d->ucases);
		}
		return {};
	}

	auto unicode::_lowers(u32_t _u)noexcept->seq_t {
		if (auto _d = pimp::search(_u)) IRON_LIKELY
		{
			return pimp::cases(_d->lcases);
		}
		return {};
	}

}

#ifdef _MSC_VER
#pragma warning(pop)
#endif


