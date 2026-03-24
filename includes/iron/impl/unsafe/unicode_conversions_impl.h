
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable:26429)
#pragma warning (disable:26472)
#pragma warning (disable:26481)
#pragma warning (disable:26493)
#pragma warning (disable:26494)
#pragma warning (disable:26496)
#pragma warning (disable:26497)
#endif

namespace iron::unsafe {

	class unicode_conversions final {
		public:
			using u8_t = std::uint8_t;
			using u16_t = std::uint16_t;
			using u32_t = std::uint32_t;
			using size_t = std::size_t;
			static constexpr char32_t default_replacement_chr = 0xFFFD;
			static constexpr u32_t max=0x0010fffe;
			unicode_conversions()=delete;
			template <typename _T>
			[[nodiscard]] static constexpr bool is_basic_charset(_T _v)noexcept
			requires (iron::concepts::charset<_T>)
			{
#ifdef IRON_EBCDIC
#error iron::memory::unicode_conversions::is_basic_charset() not implement (for now) EBCDIC
#else
				return static_cast<std::uint32_t>(_v) < 0x80U; //NOTE: ascii
#endif
			}
			template <typename _T>
			[[nodiscard]] static constexpr bool is_utf32(_T _v)noexcept
			requires(iron::concepts::utf32_charset<_T>)
			{
				return static_cast<u32_t>(_v) <= max;
			}
			template <typename _T>
			[[nodiscard]] static constexpr size_t len(const _T* _p)noexcept
			requires (iron::concepts::charset<_T>)
			{
				DEBUG_ASSERT(_p);
				using _t_t = std::decay_t<_T>;
				if (std::is_constant_evaluated()) {
					size_t _sz{};
					while (*_p++ != _t_t{}) { ++_sz; }
					return _sz;
				}
				else {
					if constexpr (std::is_same_v<_t_t, char>) {
						return std::strlen(_p);
					}
					else {
						size_t _sz{};
						while (*_p++ != _t_t{}) { ++_sz; }
						return _sz;
					}
				}
			}

			template <typename _T>
			[[nodiscard]] static constexpr size_t size_utf8(_T _u)noexcept
			requires (
				iron::concepts::basic_charset<_T>
				|| iron::concepts::utf8_charset<_T>
				|| iron::concepts::utf32_charset<_T>
			)
			{
				if (is_basic_charset(_u)) IRON_LIKELY
				{
					return 1U;
				}
				if constexpr (iron::concepts::basic_charset<_T>) {
					return {};
				}
				else if constexpr (iron::concepts::utf8_charset<_T>) {
					if ((_u & 0xE0) == 0xC0) IRON_LIKELY
					{
						return 2U;
					}
					if ((_u & 0xF0) == 0xE0) IRON_LIKELY
					{
						return 3U;
					}
					if ((_u & 0xF8) == 0xF0) IRON_LIKELY
					{
						return 4U;
					}
					return {};
				}
				else if constexpr(iron::concepts::utf32_charset<_T>) {
					if (_u < 0x800) IRON_LIKELY
					{
						return 2U;
					}
					if (_u < 0x10000) IRON_LIKELY
					{
						if (_u >= 0xD800 && _u <= 0xDFFF) IRON_UNLIKELY
						{
							return {};
						}
						return 3U;
					}
					if (_u < 0x10FFFF) IRON_LIKELY
					{
						return 4U;
					}
					return {};
				}
				else {
					static_assert(iron::unsafe::asserts::always_false<_T>, "internal error - unknow charset");
				}
			}

			template <typename _T>
			[[nodiscard]] static constexpr size_t size_utf16(_T _v)noexcept
			requires(iron::concepts::utf16_charset<_T> || iron::concepts::utf32_charset<_T>)
			{
				if constexpr (iron::concepts::utf16_charset<_T>) {
					if (_v >= 0xD800 && _v <= 0xDBFF) IRON_UNLIKELY
					{
						return 2U;
					}
					if (_v >= 0xDC00 && _v <= 0xDFFF) IRON_UNLIKELY
					{
						return {};
					}
					return 1U;
				}
				else {
					if (!size_utf8(_v)) IRON_UNLIKELY
					{
						return {};
					}
					if (_v < 0x10000) IRON_LIKELY
					{
						return 1;
					}
					return 2;
				}
			}

			/*
				convert utf32 -> utf8
				return the size of bytes copies - for invalid utf32 return 0
			*/

			template <typename _U8,typename _U32, size_t _N>
			[[nodiscard]] static constexpr size_t utf32_to_utf8(_U8(&_buffer)[_N],_U32 _u)noexcept
			requires (
				iron::concepts::utf8_charset<_U8>
				&& iron::concepts::utf32_charset<_U32>
				&& _N > 4
			)
			{
				auto _b = &_buffer[0];
				if (is_basic_charset(_u)) IRON_LIKELY
				{
					*_b++ = static_cast<_U8>(_u);
				}
				else if (_u < 0x800)  IRON_LIKELY
				{
					*_b++ = 0b1100'0000 + static_cast<_U8>(_u >> 6);
					*_b++ = 0b1000'0000 + static_cast<_U8>(_u & 0b0011'1111);
				}
				else if (_u < 0x10000)   IRON_LIKELY
				{
					if (_u >= 0xD800 && _u <= 0xDFFF)  IRON_UNLIKELY
					{
						return {};
					}
					*_b++ = 0b1110'0000 + static_cast<_U8>(_u >> 12);
					*_b++ = 0b1000'0000 + static_cast<_U8>((_u >> 6) & 0b0011'1111);
					*_b++ = 0b1000'0000 + static_cast<_U8>(_u & 0b0011'1111);
				}
				else if (_u < 0x10FFFF)  IRON_LIKELY
				{
					*_b++ = 0b1111'0000 + static_cast<_U8>(_u >> 18);
					*_b++ = 0b1000'0000 + static_cast<_U8>((_u >> 12) & 0b0011'1111);
					*_b++ = 0b1000'0000 + static_cast<_U8>((_u >> 6) & 0b0011'1111);
					*_b++ = 0b1000'0000 + static_cast<_U8>(_u & 0b0011'1111);
				}
				else { 	//bad input
				}
				*_b = 0x00;
				return static_cast<size_t>(_b - &_buffer[0]);
			}

			template <typename _U16,typename _U32, size_t _N>
			[[nodiscard]] static constexpr size_t utf32_to_utf16(_U16(&_buffer)[_N],_U32 _u)noexcept
			requires (
				iron::concepts::utf16_charset<_U16>
				&& iron::concepts::utf32_charset<_U32>
				&& _N > 2
			)
			{
				auto _b = &_buffer[0];
				const auto _sz = size_utf16(_u);
				if (_sz == 1) IRON_LIKELY
				{
					*_b++ = static_cast<_U16>(_u);
					*_b = {};
					return _sz;
				}
				if (_sz == 2) IRON_LIKELY
				{
					const std::uint32_t _t = _u - 0x10000;
					const auto _ht = (((_t << 12) >> 22) + 0xD800);
					[[maybe_unused]] constexpr auto _max = static_cast<std::uint32_t>(std::numeric_limits<std::uint16_t>::max());
					DEBUG_ASSERT(_ht <= _max);
					*_b++ = static_cast<_U16>(_ht);
					const auto _lt = (((_t << 22) >> 22) + 0xDC00);
					DEBUG_ASSERT(_lt <= _max);
					*_b++ = static_cast<_U16>(_lt);
					*_b = {};
					return _sz;
				}
				DEBUG_ASSERT(!_sz);
				*_b = {};
				return _sz;
			}


			template <typename _U32,typename _ITER>
			[[nodiscard]] static constexpr size_t utf8_to_utf32(_U32& _cp,_ITER& _b,const _ITER& _e)noexcept
			requires (
				iron::concepts::utf32_charset<_U32>
				&& iron::concepts::charset_forward_iterator<_ITER>
				&& iron::concepts::utf8_charset<decltype(*_b)>
			)
			{
				if (_b == _e)  IRON_UNLIKELY
				{
					return {};
				}
				_cp = _mask(*_b);
				switch (const auto _sz = size_utf8(*_b++)) {
					case 0: {
						return _sz;
					}
					case 1: {
						break;
					}
					case 2: {
						if (_b == _e)  IRON_UNLIKELY
						{
							return {};
						}
						_cp = ((_cp << 6) & 0x07FF) + ((*_b++) & 0x3F);
						break;
					}
					case 3:{
						if (_b == _e)  IRON_UNLIKELY
						{
							return {};
						}
						_cp = ((_cp << 12) & 0xFFFF) + ((_mask(*_b++) << 6) & 0x0FFF);
						if (_b == _e) return {};
						_cp += (*_b++) & 0x3F;
						break;
					}
					case 4: {
						if (_b == _e)  IRON_UNLIKELY
						{
							return {};
						}
						_cp = ((_cp << 18) & 0x1FFFFF) + ((_mask(*_b++) << 12) & 0x3FFFF);
						if (_b == _e) IRON_UNLIKELY
						{
							return {};
						}
						_cp += (_mask(*_b++)  << 6) & 0x0FFF;
						if (_b == _e) IRON_UNLIKELY
						{
							return {};
						}
						_cp += *_b++ & 0x3F;
						break;
					}
					default: {
						DEBUG_ASSERT(false);
						return {};
					}
				}
				return 1;
			}

			/* convert utf16->utf32
			return 1 for conversion OK else return 0
			*/

			template <typename _U32,typename _ITER>
			[[nodiscard]] static constexpr size_t utf16_to_utf32(_U32& _c,_ITER& _b,const _ITER& _e)noexcept
				requires (
				iron::concepts::utf32_charset<_U32>
				&& iron::concepts::charset_forward_iterator<_ITER>
				&& iron::concepts::utf16_charset<decltype(*_b)>
				)
			{
				if (_b == _e) IRON_UNLIKELY
				{
					return {};
				}
				constexpr std::uint32_t _hstart = 0xD800UL;
				constexpr std::uint32_t _lstart = 0xDC00UL;
				constexpr std::uint32_t _lend = 0xDFFFUL;
				constexpr std::uint32_t _halfshift = 10UL;
				constexpr std::uint32_t _halfbase = 0x0010000UL;

				_c = static_cast<_U32>(*_b++);
				const auto _sz = size_utf16(static_cast<char16_t>(_c));
				if (_sz == 1) IRON_LIKELY
				{
					return _sz;
				}
				if (!_sz) IRON_UNLIKELY
				{
					return _sz;
				}
				DEBUG_ASSERT(_sz == 2);
				if (_b == _e) IRON_UNLIKELY
				{
					return {};
				}
				const std::uint32_t _c2 = *_b++;
				if (_c2 >= _lstart && _c2 <= _lend) IRON_LIKELY
				{
					//UTF32 = ((H - 0xD800) x 0x400) + (L - 0xDC00) + 0x10000
					_c = ((_c - _hstart) << _halfshift) + (_c2 - _lstart) + _halfbase;
					return 1;
				}
				return {};
			}

			template <typename _U1,typename _U2>
			[[nodiscard]] constexpr size_t utf32_to_utf32(_U1& _out, _U2 _in)noexcept
			requires (
				iron::concepts::utf32_charset<_U1>
				&& iron::concepts::utf32_charset<_U2>
			)
			{
				if constexpr (std::is_same_v<_U1, _U2>) {
					_out = _in;
				}
				else {
					_out = static_cast<_U1>(_in);
				}
				if (!is_utf32(_in)) IRON_UNLIKELY
				{
					return 0;
				}
				return 1;
			}

			template <bool _CHECK, typename _T, typename _ITER>
			[[nodiscard]] static constexpr bool copy(_T* _to, size_t& _to_size, _ITER& _b, const _ITER& _e)noexcept
			requires(iron::concepts::charset<_T> && iron::concepts::charset_forward_iterator<_ITER>)
			{
				using _t_t = std::decay_t<_T>;
				using _f_t = std::decay_t<decltype(*_b)>;
				if constexpr (iron::concepts::basic_charset<_t_t> && iron::concepts::basic_charset<_f_t>) {
					return _b_b<_CHECK>(_to, _to_size, _b, _e);
				}
				else if constexpr (iron::concepts::basic_charset<_t_t> && iron::concepts::utf8_charset<_f_t>) {
					return _b_8<_CHECK>(_to, _to_size, _b, _e);
				}
				else if constexpr (iron::concepts::basic_charset<_t_t> && iron::concepts::utf16_charset<_f_t>) {
					return _b_16<_CHECK>(_to, _to_size, _b, _e);
				}
				else if constexpr (iron::concepts::basic_charset<_t_t> && iron::concepts::utf32_charset<_f_t>) {
					return _b_32<_CHECK>(_to, _to_size, _b, _e);
				}
				//
				else if constexpr (iron::concepts::utf8_charset<_t_t> && iron::concepts::basic_charset<_f_t>) {
					return _8_b<_CHECK>(_to, _to_size, _b, _e);
				}
				else if constexpr (iron::concepts::utf8_charset<_t_t> && iron::concepts::utf8_charset<_f_t>) {
					return _8_8<_CHECK>(_to, _to_size, _b, _e);
				}
				else if constexpr (iron::concepts::utf8_charset<_t_t> && iron::concepts::utf16_charset<_f_t>) {
					return _8_16(_to, _to_size, _b, _e);
				}
				else if constexpr (iron::concepts::utf8_charset<_t_t> && iron::concepts::utf32_charset<_f_t>) {
					return _8_32(_to, _to_size, _b, _e);
				}
				//
				else if constexpr (iron::concepts::utf16_charset<_t_t> && iron::concepts::basic_charset<_f_t>) {
					return _16_b<_CHECK>(_to, _to_size, _b, _e);
				}
				else if constexpr (iron::concepts::utf16_charset<_t_t> && iron::concepts::utf8_charset<_f_t>) {
					return _16_8(_to, _to_size, _b, _e);
				}
				else if constexpr (iron::concepts::utf16_charset<_t_t> && iron::concepts::utf16_charset<_f_t>) {
					return _16_16<_CHECK>(_to, _to_size, _b, _e);
				}
				else if constexpr (iron::concepts::utf16_charset<_t_t> && iron::concepts::utf32_charset<_f_t>) {
					return _16_32(_to, _to_size, _b, _e);
				}
				//
				else if constexpr (iron::concepts::utf32_charset<_t_t> && iron::concepts::basic_charset<_f_t>) {
					return _32_b<_CHECK>(_to, _to_size, _b, _e);
				}
				else if constexpr (iron::concepts::utf32_charset<_t_t> && iron::concepts::utf8_charset<_f_t>) {
					return _32_8(_to, _to_size, _b, _e);
				}
				else if constexpr (iron::concepts::utf32_charset<_t_t> && iron::concepts::utf16_charset<_f_t>) {
					return _32_16(_to, _to_size, _b, _e);
				}
				else if constexpr (iron::concepts::utf32_charset<_t_t> && iron::concepts::utf32_charset<_f_t>) {
					return _32_32<_CHECK>(_to, _to_size, _b, _e);
				}
				else {
					PANIC("internal error - unknow charsets");
				}
			}
		private:
			template <bool _CHECK, typename _T, typename _ITER>
			[[nodiscard]] static constexpr bool _b_b(_T* _to, size_t& _to_size, _ITER& _b, const _ITER& _e)noexcept {
				return _b_simple<_CHECK>(_to, _to_size, _b, _e);
			}
			template <bool _CHECK, typename _T, typename _ITER>
			[[nodiscard]] static constexpr bool _b_8(_T* _to, size_t& _to_size, _ITER& _b, const _ITER& _e)noexcept {
				return _b_simple<_CHECK>(_to, _to_size, _b, _e);
			}
			template <bool _CHECK, typename _T, typename _ITER>
			[[nodiscard]] static constexpr bool _b_16(_T* _to, size_t& _to_size, _ITER& _b, const _ITER& _e)noexcept {
				return _b_simple<_CHECK>(_to, _to_size, _b, _e);
			}
			template <bool _CHECK, typename _T, typename _ITER>
			[[nodiscard]] static constexpr bool _b_32(_T* _to, size_t& _to_size, _ITER& _b, const _ITER& _e)noexcept {
				return _b_simple<_CHECK>(_to, _to_size, _b, _e);
			}
			template <bool _CHECK,typename _ITER>
			[[nodiscard]] static constexpr bool _8_b(char8_t* _to, size_t& _to_size, _ITER& _b, const _ITER& _e)noexcept {
				size_t _count{};
				bool _ret = true;
				char8_t _buff[5];
				while (_b != _e)  IRON_LIKELY
				{
					size_t _sz = 1;
					if constexpr (_CHECK) {
						const char _c = *_b++;
						if (is_basic_charset(_c)) IRON_LIKELY
						{
							_buff[0] = static_cast<char8_t>(_c);
						}
						else {
							_sz = utf32_to_utf8(_buff, default_replacement_chr);
							_ret = false;
						}
					}
					else {
						_buff[0] = static_cast<char8_t>(*_b++);
					}
					_count += _sz;
					_simple_copy(_to, _to_size, &_buff[0], _sz);
				}
				_to_size = _count;
				return _ret;
			}

			template <bool _CHECK,typename _ITER>
			[[nodiscard]] static constexpr bool _8_8(char8_t* _to, size_t& _to_size, _ITER& _b, const _ITER& _e)noexcept {
				size_t _count{};
				bool _ret = true;
				char8_t _buff[5];
				while (_b != _e)  IRON_LIKELY
				{
					size_t _sz = 1;
					if constexpr (_CHECK) {
						char32_t _c;
						if (!utf8_to_utf32(_c, _b, _e)) IRON_UNLIKELY
						{
							_c = default_replacement_chr;
							_ret = false;
						}
						_sz = utf32_to_utf8(_buff, _c);
						DEBUG_ASSERT(_sz);
					}
					else {
						_buff[0] = *_b++;
					}
					_count += _sz;
					_simple_copy(_to, _to_size, &_buff[0], _sz);
				}
				_to_size = _count;
				return _ret;
			}
			template <typename _ITER>
			[[nodiscard]] static constexpr bool _8_16(char8_t* _to, size_t& _to_size, _ITER& _b, const _ITER& _e)noexcept {
				size_t _count{};
				bool _ret = true;
				char8_t _buff[5];
				while (_b != _e)  IRON_LIKELY
				{
					const auto _w = *_b;
					size_t _sz = 1;
					if (is_basic_charset(_w)) IRON_LIKELY
					{
						_buff[0] = static_cast<char8_t>(_w);
						++_b;
					}
					else {
						char32_t _c32;
						_sz = utf16_to_utf32(_c32, _b, _e);
						if (!_sz) IRON_UNLIKELY
						{
							_c32 = default_replacement_chr;
							_ret = false;
						}
						_sz = utf32_to_utf8(_buff,_c32);
						DEBUG_ASSERT(_sz);
					}
					_count += _sz;
					_simple_copy(_to, _to_size, &_buff[0], _sz);
				}
				_to_size = _count;
				return _ret;
			}
			template <typename _ITER>
			[[nodiscard]] static constexpr bool _8_32(char8_t* _to, size_t& _to_size, _ITER& _b, const _ITER& _e)noexcept {
				size_t _count{};
				bool _ret = true;
				char8_t _buff[5];
				while (_b != _e) IRON_LIKELY
				{
					auto _sz=utf32_to_utf8(_buff, *_b++);
					if (!_sz) IRON_UNLIKELY //invalid
					{
						_sz = utf32_to_utf8(_buff,default_replacement_chr);
						DEBUG_ASSERT(_sz);
						_ret = false;
					}
					_count += _sz;
					_simple_copy(_to, _to_size, &_buff[0], _sz);
				}
				_to_size = _count;
				return _ret;
			}
			//
			template <bool _CHECK,typename _T, typename _ITER>
			[[nodiscard]] static constexpr bool _16_b(_T* _to, size_t& _to_size, _ITER& _b, const _ITER& _e)noexcept {
#ifdef IRON_EBCDIC
#error unicode conversion not implement (for now) EBCDIC
#else
				size_t _count{};
				bool _ret = true;
				_T _buff[3];
				while (_b != _e) IRON_LIKELY
				{
					size_t _sz = 1;
					if constexpr (_CHECK) {
						const auto _c = *_b++;
						if (is_basic_charset(_c)) IRON_LIKELY
						{
							_buff[0] = static_cast<_T>(_c);
						}
						else { //invalid
							_sz = utf32_to_utf16(_buff, default_replacement_chr);
							DEBUG_ASSERT(_sz);
							_ret = false;
						}
					}
					else {
						_buff[0] = static_cast<_T>(*_b++);
					}
					_count += _sz;
					_simple_copy(_to, _to_size, &_buff[0], _sz);
				}
				_to_size = _count;
				return _ret;
#endif
			}
			template <typename _T, typename _ITER>
			[[nodiscard]] static constexpr bool _16_8(_T* _to, size_t& _to_size, _ITER& _b, const _ITER& _e)noexcept {
				size_t _count{};
				bool _ret = true;
				_T _buff[3];
				while (_b != _e)  IRON_LIKELY
				{
					char32_t _c;
					if (!utf8_to_utf32(_c, _b, _e)) IRON_UNLIKELY
					{
						_c = default_replacement_chr;
						_ret = false;
					}
					auto _sz = utf32_to_utf16(_buff, _c);
					DEBUG_ASSERT(_sz);
					_count += _sz;
					_simple_copy(_to, _to_size, &_buff[0], _sz);
				}
				_to_size = _count;
				return _ret;
			}
			template <bool _CHECK, typename _T, typename _ITER>
			[[nodiscard]] static constexpr bool _16_16(_T* _to, size_t& _to_size, _ITER& _b, const _ITER& _e)noexcept {
				size_t _count{};
				bool _ret = true;
				_T _buff[3];
				while (_b != _e)  IRON_LIKELY
				{
					size_t _sz = 1;
					if constexpr (_CHECK) {
						char32_t _c;
						if (!utf16_to_utf32(_c, _b, _e)) IRON_UNLIKELY
						{
							_c = default_replacement_chr;
							_ret = false;
						}
						_sz = utf32_to_utf16(_buff, _c);
						DEBUG_ASSERT(_sz);
					}
					else {
						if constexpr (std::is_same_v<std::decay_t<_T>, std::decay_t<decltype(*_b)>>) {
							_buff[0] = *_b++;
						}
						else {
							_buff[0] = static_cast<_T>(*_b++);
						}
					}
					_count += _sz;
					_simple_copy(_to, _to_size, &_buff[0], _sz);
				}
				_to_size = _count;
				return _ret;
			}
			template <typename _T, typename _ITER>
			[[nodiscard]] static constexpr bool _16_32(_T* _to, size_t& _to_size, _ITER& _b, const _ITER& _e)noexcept {
				size_t _count{};
				bool _ret = true;
				_T _buff[3];
				while (_b != _e) IRON_LIKELY
				{
					auto _sz = utf32_to_utf16(_buff,*_b++);
					if (!_sz) IRON_UNLIKELY
					{
						_sz = utf32_to_utf16(_buff,default_replacement_chr);
						_ret = false;
						DEBUG_ASSERT(_sz);
					}
					_count += _sz;
					_simple_copy(_to, _to_size, &_buff[0], _sz);
				}
				_to_size = _count;
				return _ret;
			}
			//
			template <bool _CHECK, typename _T, typename _ITER>
			[[nodiscard]] static constexpr bool _32_b(_T* _to, size_t& _to_size, _ITER& _b, const _ITER& _e)noexcept {
#ifdef IRON_EBCDIC
#error unicode conversion not implement (for now) EBCDIC
#else
				size_t _count{};
				bool _ret = true;
				_T _buff;
				while (_b != _e) IRON_LIKELY
				{
					_buff = static_cast<_T>(*_b++);
					size_t _sz = 1;
					if constexpr (_CHECK) {
						if (!is_basic_charset(_buff)) IRON_UNLIKELY
						{
							_buff = default_replacement_chr;
							_ret = false;
						}
					}
					_count += _sz;
					_simple_copy(_to, _to_size, &_buff, _sz);
				}
				_to_size = _count;
				return _ret;
#endif
			}
			template <typename _T, typename _ITER>
			[[nodiscard]] static constexpr bool _32_8(_T* _to, size_t& _to_size, _ITER& _b, const _ITER& _e)noexcept {
				size_t _count{};
				bool _ret = true;
				_T _buff;
				while (_b != _e) IRON_LIKELY
				{
					auto _sz = utf8_to_utf32(_buff,_b,_e);
					if (!_sz) IRON_UNLIKELY
					{
						_buff = default_replacement_chr;
						_ret = false;
						_sz = 1;
					}
					_count += _sz;
					_simple_copy(_to, _to_size, &_buff,_sz);
				}
				_to_size = _count;
				return _ret;
			}
			template <typename _T, typename _ITER>
			[[nodiscard]] static constexpr bool _32_16(_T* _to, size_t& _to_size, _ITER& _b, const _ITER& _e)noexcept {
				size_t _count{};
				bool _ret = true;
				_T _buff;
				while (_b != _e) IRON_LIKELY
				{
					auto _sz = utf16_to_utf32(_buff,_b,_e);
					if (!_sz) IRON_UNLIKELY
					{
						_buff = default_replacement_chr;
						_ret = false;
						_sz = 1;
					}
					_count += _sz;
					_simple_copy(_to, _to_size, &_buff,_sz);
				}
				_to_size = _count;
				return _ret;
			}
			template <bool _CHECK, typename _T, typename _ITER>
			[[nodiscard]] static constexpr bool _32_32(_T* _to, size_t& _to_size, _ITER& _b, const _ITER& _e)noexcept {
				size_t _count{};
				bool _ret = true;
				_T _buff;
				while (_b != _e)  IRON_LIKELY
				{
					size_t _sz = 1;
					if constexpr (std::is_same_v<std::decay_t<_T>, std::decay_t<decltype(*_b)>>) {
						_buff = *_b++;
					}
					else {
						_buff = static_cast<_T>(*_b++);
					}
					if constexpr (_CHECK) {
						if (!is_utf32(_buff)) IRON_UNLIKELY
						{
							_buff = default_replacement_chr;
							_ret = false;
						}
					}
					_count += _sz;
					_simple_copy(_to, _to_size, &_buff, _sz);
				}
				_to_size = _count;
				return _ret;
			}

			template <typename _T>
			static constexpr void _simple_copy(_T*& _to, size_t& _to_size, const _T* _p, size_t _sz)noexcept {
				while (_to_size && _sz--) IRON_LIKELY
				{
					*_to++ = *_p++;
					--_to_size;
				}
			}
			//
			template <bool _CHECK,typename _ITER>
			[[nodiscard]] static constexpr bool _b_simple(char* _to, size_t& _to_size, _ITER& _b, const _ITER& _e)noexcept {
				size_t _count{};
				bool _ret = true;
#ifdef IRON_EBCDIC
#error iron::memory::unicode_conversions::is_basic_charset() not implement (for now) EBCDIC
#else
				while (_b != _e) IRON_LIKELY
				{
					if constexpr (_CHECK) {
						if (!is_basic_charset(*_b)) IRON_UNLIKELY
						{
							_ret = false;
						}
					}
					if (_to && _to_size) IRON_LIKELY
					{
						if constexpr (std::is_same_v<char, std::decay_t<decltype(*_b)>>) {
							*_to++ = *_b;
						}
						else {
							*_to++ = static_cast<char>(*_b);
						}
						--_to_size;
					}
					++_count;
					++_b;
				}
				_to_size = _count;
				return _ret;
#endif
			}
			template <typename _U>
			[[nodiscard]] static constexpr _U _mask(_U _v)noexcept { return _v & 0xFF; }
	};

} //iron

#undef _IRON_CHECK

#if defined(_MSC_VER)
#pragma warning(pop)
#endif
