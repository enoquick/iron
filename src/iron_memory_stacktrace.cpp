#ifdef IRON_NO_USE_MODULE
#include "iron/str.h"
#include "iron/memory/stacktrace.h"
#include "iron/vector.h"
#include "iron/box.h"
#include "iron/collector.h"
#else
#endif

//set _IRON_LIB_STACKTRACE if std::stacktrace is set
#undef _IRON_LIB_STACKTRACE
#ifdef __cpp_lib_stacktrace
#define _IRON_LIB_STACKTRACE
#else
#ifdef _MSC_VER
#if _MSC_VER >= 1936
#define _IRON_LIB_STACKTRACE
#endif
#endif
#endif

#ifdef IRON_NO_USE_MODULE
#ifdef _IRON_LIB_STACKTRACE
#include <stacktrace>
#elif defined(__linux__)
#include <execinfo.h>
#else
#endif
#endif

namespace iron::memory {

	struct stacktrace_entry::_struct_entry {
		str_t _file{};
		str_t _func{};
		line_number_t _line{};
		_struct_entry()noexcept = default;
		_struct_entry(const _struct_entry&) = delete;
		_struct_entry(_struct_entry&&)noexcept = default;
		_struct_entry& operator=(const _struct_entry&) = delete;
		_struct_entry& operator=(_struct_entry&&) = delete;
		~_struct_entry() = default;
		_struct_entry* pclone()const noexcept {
			if (auto _rfile = _file.try_clone()) IRON_LIKELY
			{
				if (auto _rfunc = _func.try_clone()) IRON_LIKELY
				{
					using _a_t = iron::unsafe::allocator<_struct_entry>;
					if (auto _p = _a_t{}.allocate(1)) IRON_LIKELY
					{
						std::construct_at(_p);
						_p->_file.swap(_rfile.unchecked_unwrap());
						_p->_func.swap(_rfunc.unchecked_unwrap());
						_p->_line = _line;
						return _p;
					}
				}
			}
			return {};
		}
	};

	namespace _stacktrace_impl {

		static const iron::memory::stacktrace::entries_t _sm_entries;
		static const iron::memory::stacktrace_entry::str_t _sm_empty;
#ifdef _IRON_LIB_STACKTRACE
		inline bool _copy(stacktrace_entry& _out, const std::stacktrace_entry& _in)noexcept {
			try {
				const auto _sf = _in.source_file();
				const char* _p = _sf.c_str();
				if (auto _r_file = stacktrace_entry::str_t::try_unchecked_from(_p,_p + _sf.size())) IRON_LIKELY
				{
					_out = stacktrace_entry(std::move(_r_file.unchecked_unwrap()), _in.source_line(), {});
					return true;
				}
			}
			catch (...) {}
			return {};
		}
#elif defined(__linux__)
		inline bool _copy(stacktrace_entry& _out, const char* _in)noexcept {
			if (_in) {
				if (auto _r_file = stacktrace_entry::str_t::try_unchecked_from(_in,_in + std::strlen(_in))) IRON_LIKELY
				{
					_out = stacktrace_entry(std::move(_r_file.unchecked_unwrap()), {}, {});
					return true;
				}
			}
			return {};
		}
#else
#endif
	} //ns

	void stacktrace_entry::_construct(str_t&& _file, line_number_t _line, str_t&& _func)noexcept {
		IRON_SECURITY_CHECK_TEMPORARY(_file);
		IRON_SECURITY_CHECK_TEMPORARY(_func);
		DEBUG_ASSERT(!_m_entry);
		using _a_t = iron::unsafe::allocator<_entry_t>;
		if (auto _p = _a_t{}.allocate(1)) IRON_LIKELY
		{
			std::construct_at(_p);
			_p->_file = std::move(_file);
			_p->_line = _line;
			_p->_func = std::move(_func);
			_m_entry = _p;
		}
	}
	stacktrace_entry stacktrace_entry::_clone()const noexcept {
		stacktrace_entry _t;
		if (_m_entry) IRON_LIKELY
		{
			_t._m_entry = _m_entry->pclone();
		}
		return _t;
	}
	void stacktrace_entry::_destroy()noexcept {
		DEBUG_ASSERT(_m_entry);
		using _a_t = iron::unsafe::allocator<_entry_t>;
		std::destroy_at(_m_entry);
		_a_t{}.deallocate(_m_entry, 1);
		_m_entry = {};
	}
	auto stacktrace_entry::_file()const noexcept -> iron::ref_wrapper<const str_t> {
		auto& _file = _m_entry ? _m_entry->_file : _stacktrace_impl::_sm_empty;
		return iron::make_cref(*this,_file);
	}
	auto stacktrace_entry::_line()const noexcept -> line_number_t {
		return _m_entry ? _m_entry->_line : line_number_t{};
	}
	auto  stacktrace_entry::_func()const noexcept -> iron::ref_wrapper<const str_t> {
		auto& _func = _m_entry ? _m_entry->_func : _stacktrace_impl::_sm_empty;
		return iron::make_cref(*this, _func);
	}

	void stacktrace::_construct(size_t _n)noexcept {
		DEBUG_ASSERT(_n);
		if (!_disable_env()) {
			return;
		}
		auto _r = iron::try_make_box<entries_t>();
		if (_r) IRON_LIKELY
		{
			auto& _box = _r.unchecked_unwrap();
			auto& _entries = _box.unchecked_value();
#ifdef _IRON_LIB_STACKTRACE
			const auto _sk = std::stacktrace::current(0, _n); //noexcept
			if (_entries.try_reserve_exact(_sk.size())) IRON_LIKELY
			{
				for (auto& _stde : _sk) IRON_LIKELY
				{
					entry_t _e;
					if (!_stacktrace_impl::_copy(_e, _stde) || !_e) IRON_UNLIKELY
					{
						break;
					}
					if (!_entries.try_push_back(std::move(_e))) IRON_UNLIKELY
					{
						break;
					}
				}
				if (_entries.size()) IRON_LIKELY
				{
					_m_entries = _box.unsafe_release();
				}
			}
#elif defined(__linux__)
			static constexpr std::size_t _max_sz = 127;
			void* _buffer[_max_sz];
			if (const auto _nptrs = ::backtrace(_buffer, _max_sz) > 0) IRON_LIKELY
			{
				const auto _unptrs=static_cast<std::size_t>(_nptrs);
				auto _sk_size = _n > _unptrs ? _unptrs : _n;
				if (char** _strings = ::backtrace_symbols(_buffer, _sk_size)) IRON_LIKELY
				{
					if (_entries.try_reserve_exact(_sk_size)) IRON_LIKELY
					{
						auto _p = _strings;
						while (_sk_size--) IRON_LIKELY
						{
							entry_t _e;
							if (!_stacktrace_impl::_copy(_e, *_p++) || !_e) IRON_UNLIKELY
							{
								break;
							}
							if (!_entries.try_push_back(std::move(_e))) IRON_UNLIKELY
							{
								break;
							}
						}
						if (_entries.size()) IRON_LIKELY
						{
							_m_entries = _box.unsafe_release();
						}
					}
					::free(_strings);
				}
			}
#else
#endif

		} //if
		if (_m_disable_env) IRON_LIKELY
		{
			if (!_enable_env()) IRON_LIKELY
			{
				_destroy();
			}
			_m_disable_env = {};
		}
	}

	void stacktrace::_destroy()noexcept {
		DEBUG_ASSERT(_m_entries);
		using _a_t = iron::unsafe::allocator<entries_t>;
		std::destroy_at(_m_entries);
		_a_t{}.deallocate(_m_entries,1);
		_m_entries = {};
	}

	stacktrace stacktrace::_clone()const noexcept {
		stacktrace _t;
		if (_m_entries && _m_entries->size()) IRON_LIKELY
		{
			if (!_t._disable_env()) IRON_UNLIKELY
			{
				return _t;
			}
			entries_t _entries;
			if (_entries.try_reserve_exact(_m_entries->size())) IRON_LIKELY
			{
				for (auto& _e : _m_entries->seq()) IRON_LIKELY
				{
					if (_e) IRON_LIKELY
					{
						auto _ec = _e.clone();
						if (!_ec) IRON_UNLIKELY //error
						{
							return _t;
						}
						[[maybe_unused]] const auto _r=_entries.try_push_back(std::move(_ec)); //not in error
					}
					else {
						[[maybe_unused]] const auto _r=_entries.try_push_back(entry_t{}); //not in error
					}
				}
			}
			if (_entries.size()) IRON_LIKELY
			{
				using _a_t = iron::unsafe::allocator<entries_t>;
				if (auto _p = _a_t{}.allocate(1)) IRON_LIKELY
				{
					std::construct_at(_p, std::move(_entries));
					_t._m_entries = _p;
				}
			}
		}
		if (!_t._enable_env()) IRON_UNLIKELY
		{
			_t._destroy();
		}
		return _t;
	}
	auto stacktrace::_entries()const noexcept -> iron::ref_wrapper<const entries_t> {
		auto& _es = _m_entries ? *_m_entries : _stacktrace_impl::_sm_entries;
		return iron::make_cref(*this, _es);
	}

	inline bool stacktrace::_disable_env()noexcept {
		if (iron::unsafe::errors::_pimp::_is_stacktrace_enable()) IRON_UNLIKELY
		{
			if (iron::unsafe::errors::_pimp::_stacktrace_set(false)) IRON_LIKELY  //disable env stacktrace
			{
				_m_disable_env = true;
				return true;
			}
			return {}; //error
		}
		return true; //stacktrace env is not enable - ok
	}

	inline bool stacktrace::_enable_env()noexcept {
		if (_m_disable_env) IRON_LIKELY
		{
			const auto _b=iron::unsafe::errors::_pimp::_stacktrace_set(true);
			_m_disable_env = {};
			return _b;
		}
		return true;
	}
}


#undef _IRON_LIB_STACKTRACE
