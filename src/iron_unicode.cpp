#ifdef IRON_NO_USE_MODULE
#include "iron/unicode.h"
#include "iron/unsafe/unicode_conversions.h"
#endif

namespace iron {
	namespace unicode_impl {
#include "iron/impl/unicode/structs.h"
#include "iron/impl/unicode/ranges.h"
#include "iron/impl/unicode/sections.h"
		static constexpr auto m_sections_size = sizeof(s_sections) / sizeof(s_sections[0]);

	}

	template <typename T>
	class base_unicode<T>::pimp final {
	public:
		pimp() = delete;
		using data_t = unicode_impl::struct_data;
		using section_t = unicode_impl::struct_section;
		using value_t = data_t::value_t;
		static const data_t*const search(value_t code)noexcept;
		static iter_t cases(const value_t*)noexcept;
	private:
		static const section_t* const binary_search(const section_t*,std::size_t sz,value_t code)noexcept;

	};

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:26481)
#pragma warning(disable:26485)
#pragma warning(disable:6011)
#endif

	template <typename T>
	auto base_unicode<T>::pimp::binary_search(const section_t* ss, std::size_t sz, value_t code)noexcept -> const section_t* const {
		DEBUG_ASSERT(sz > 0);
		std::size_t low{};
		auto high = sz - 1;
		while (low <= high) IRON_LIKELY
		{
			const auto mid = low + ((high - low) / 2);
			auto s = ss + mid;
			if (code >= s->from &&  code <= s->to) {
				return s;
			}
			if (s->to < code) {
				low = mid + 1;
			}
			else {
				high = mid - 1;
			}
		}
		return {};
	}

	template <typename T>
	inline auto base_unicode<T>::pimp::search(value_t code)noexcept-> const data_t*const {
		auto& ss = unicode_impl::s_sections;
		if (_p_is_ascii(code)) IRON_UNLIKELY //normally search is used for not ascii
		{
			auto s = ss + code;
			DEBUG_ASSERT(s->begin);
			return &*(s->begin);
		}
		constexpr auto sz = sizeof(ss) / sizeof(ss[0]);
		if (const auto s=binary_search(ss,sz,code)) IRON_LIKELY
		{
			DEBUG_ASSERT(s->begin);
			DEBUG_ASSERT(code >= s->from);
			DEBUG_ASSERT(code <= s->to);
			auto b = s->begin;
			return s->shared
				? &*b
				: &*(b + (code - s->from))
				;
		}
		return {};
	}

	template <typename T>
	bool base_unicode<T>::_unchecked_is(const u32_t& _u, attr_t _c)noexcept {
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

	class _master final {
	public:
		_master()noexcept=default;
		IRON_SECURITY_METHOD()
	private:
		IRON_SECURITY_DCL();
	};
	template <typename T>
	inline auto base_unicode<T>::pimp::cases(const value_t* _p)noexcept->iter_t {
		DEBUG_ASSERT(_p);
		auto _b = _p;
		static _master _m; //dummy obj for checker
		while (*_b) IRON_LIKELY
		{
			++_b;
		}
		return iter_t::unchecked_from(_m,_iterw_t(_p), _iterw_t(_b));
	}

	template <typename T>
	auto base_unicode<T>::_unchecked_uppers(const u32_t& _u)noexcept->iter_t {
		if (auto _d = pimp::search(_u)) IRON_LIKELY
		{
			return pimp::cases(_d->ucases);
		}
		return {};
	}

	template <typename T>
	auto base_unicode<T>::_unchecked_lowers(const u32_t& _u)noexcept->iter_t {
		if (auto _d = pimp::search(_u)) IRON_LIKELY
		{
			return pimp::cases(_d->lcases);
		}
		return {};
	}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

	template class base_unicode<u32::value_t>;
}

