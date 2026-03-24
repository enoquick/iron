
namespace iron::unsafe::errors {

	class posix final : public base_with_info {
	public:
		using code_t = std::errc;
		static constexpr code_t code_ok = code_t(0);
		[[nodiscard]] static constexpr bool make_code(code_t&, std::underlying_type_t<code_t>)noexcept;
		posix()noexcept= default;
		explicit posix(code_t _code, const char* _file, line_number_t _line, const char* _func)noexcept
		: base_with_info(_file, _line, _func)
		, _m_err_code(_code)
		{
			DEBUG_ASSERT(_code != code_ok);
		}
		~posix() = default;
		posix(const posix&) = delete;
		posix& operator=(const posix&) = delete;

		posix(posix&& _v)noexcept
		: base_with_info()
		{
			_swap(_v);
		}
		posix& operator=(posix&& _v)noexcept {
			_swap(_v);
			return *this;
		}
		void swap(posix& _v)noexcept {
			_swap(_v);
		}
		[[nodiscard]] code_t code()const noexcept {
			return _m_err_code;
		}
		[[nodiscard]] static constexpr auto numeric_code(code_t _code)noexcept {
			return static_cast<std::underlying_type_t<code_t>>(_code);
		}
		[[nodiscard]] auto numeric_code()const noexcept {
			return numeric_code(_m_err_code);
		}
	protected:
		[[nodiscard]] bool virtual_is()const noexcept override { return _m_err_code == code_ok; }
		[[nodiscard]] const char* virtual_code()const noexcept override { return _code(*this); }
		[[nodiscard]] const char8_t* virtual_description()const noexcept override { return _description(*this); }
		[[nodiscard]] base* const virtual_clone()const noexcept override { 
			auto _p = iron::unsafe::allocator<posix>{}.allocate(1);
			return _clone(_p,*this); 
		}
	private:
//		[[nodiscard]] _IRON_DLL_EXPORT static bool _is(const posix&)noexcept;
		[[nodiscard]] _IRON_DLL_EXPORT static const char* _code(const posix&)noexcept;
		[[nodiscard]] _IRON_DLL_EXPORT static const char8_t* _description(const posix&)noexcept;
		[[nodiscard]] _IRON_DLL_EXPORT static base* const _clone(posix*,const posix&)noexcept;
		void _swap(posix& _v)noexcept {
			base_with_info::swap(_v);
			_swap2(*this, _v);
		}

		_IRON_DLL_EXPORT static void _swap2(posix&, posix&)noexcept;
		/*
			std::swap(_m_err_code, _v._m_err_code);
			std::swap(_m_arr_code, _v._m_arr_code);
			std::swap(_m_description, _v._m_description);

		}
		*/
		[[nodiscard]] static constexpr bool _make_code(code_t& _out, code_t _in)noexcept {
			_out = _in;
			return true;
		}
		code_t _m_err_code{};
		mutable char _m_arr_code[max_code_sz]{};
		mutable char8_t _m_description[max_path_sz]{};
	};

	inline constexpr bool posix::make_code(code_t& _out, std::underlying_type_t<code_t> _c)noexcept {
		switch (_c) {
			case 0: return {};
			case EAFNOSUPPORT: return _make_code(_out, code_t::address_family_not_supported);
			case EADDRINUSE: return _make_code(_out, code_t::address_in_use);
			case EADDRNOTAVAIL: return _make_code(_out, code_t::address_not_available);
			case EISCONN: return _make_code(_out, code_t::already_connected);
			case E2BIG: return _make_code(_out, code_t::argument_list_too_long);
			case EDOM: return _make_code(_out, code_t::argument_out_of_domain);
			case EFAULT: return _make_code(_out, code_t::bad_address);
			case EBADF: return _make_code(_out, code_t::bad_file_descriptor);
			case EBADMSG: return _make_code(_out, code_t::bad_message);
			case EPIPE: return _make_code(_out, code_t::broken_pipe);
			case ECONNABORTED: return _make_code(_out, code_t::connection_aborted);
			case EALREADY: return _make_code(_out, code_t::connection_already_in_progress);
			case ECONNREFUSED: return _make_code(_out, code_t::connection_refused);
			case ECONNRESET: return _make_code(_out, code_t::connection_reset);
			case EXDEV: return _make_code(_out, code_t::cross_device_link);
			case EDESTADDRREQ: return _make_code(_out, code_t::destination_address_required);
			case EBUSY: return _make_code(_out, code_t::device_or_resource_busy);
			case ENOTEMPTY: return _make_code(_out, code_t::directory_not_empty);
			case ENOEXEC: return _make_code(_out, code_t::executable_format_error);
			case EEXIST: return _make_code(_out, code_t::file_exists);
			case EFBIG: return _make_code(_out, code_t::file_too_large);
			case ENAMETOOLONG: return _make_code(_out, code_t::filename_too_long);
			case ENOSYS: return _make_code(_out, code_t::function_not_supported);
			case EHOSTUNREACH: return _make_code(_out, code_t::host_unreachable);
			case EIDRM: return _make_code(_out, code_t::identifier_removed);
			case EILSEQ: return _make_code(_out, code_t::illegal_byte_sequence);
			case ENOTTY: return _make_code(_out, code_t::inappropriate_io_control_operation);
			case EINTR: return _make_code(_out, code_t::interrupted);
			case EINVAL: return _make_code(_out, code_t::invalid_argument);
			case ESPIPE: return _make_code(_out, code_t::invalid_seek);
			case EIO: return _make_code(_out, code_t::io_error);
			case EISDIR: return _make_code(_out, code_t::is_a_directory);
			case EMSGSIZE: return _make_code(_out, code_t::message_size);
			case ENETDOWN: return _make_code(_out, code_t::network_down);
			case ENETRESET: return _make_code(_out, code_t::network_reset);
			case ENETUNREACH: return _make_code(_out, code_t::network_unreachable);
			case ENOBUFS: return _make_code(_out, code_t::no_buffer_space);
			case ECHILD: return _make_code(_out, code_t::no_child_process);
			case ENOLINK: return _make_code(_out, code_t::no_link);
			case ENOLCK: return _make_code(_out, code_t::no_lock_available);
				//case ENODATA: return _make_code(_out,code_t::no_message_available); //deprecated in c++23
			case ENOMSG: return _make_code(_out, code_t::no_message);
			case ENOPROTOOPT: return _make_code(_out, code_t::no_protocol_option);
			case ENOSPC: return _make_code(_out, code_t::no_space_on_device);
				//case ENOSR: return _make_code(_out,code_t::no_stream_resources); //deprecated in c++23
			case ENXIO: return _make_code(_out, code_t::no_such_device_or_address);
			case ENODEV: return _make_code(_out, code_t::no_such_device);
			case ENOENT: return _make_code(_out, code_t::no_such_file_or_directory);
			case ESRCH: return _make_code(_out, code_t::no_such_process);
			case ENOTDIR: return _make_code(_out, code_t::not_a_directory);
			case ENOTSOCK: return _make_code(_out, code_t::not_a_socket);
				//case ENOSTR: return _make_code(_out,code_t::not_a_stream); //deprecated in c++23
			case ENOTCONN: return _make_code(_out, code_t::not_connected);
			case ENOMEM: return _make_code(_out, code_t::not_enough_memory);
			case ENOTSUP: return _make_code(_out, code_t::not_supported);
			case ECANCELED: return _make_code(_out, code_t::operation_canceled);
			case EINPROGRESS: return _make_code(_out, code_t::operation_in_progress);
			case EPERM: return _make_code(_out, code_t::operation_not_permitted);
#ifdef _WIN32
			case EOPNOTSUPP: return _make_code(_out, code_t::operation_not_supported);
#elif defined(__unix__)
				//same value of ENOTSUP
#else
#error unknow platform
#endif
			case EWOULDBLOCK: return _make_code(_out, code_t::operation_would_block);
			case EOWNERDEAD: return _make_code(_out, code_t::owner_dead);
			case EACCES: return _make_code(_out, code_t::permission_denied);
			case EPROTO: return _make_code(_out, code_t::protocol_error);
			case EPROTONOSUPPORT: return _make_code(_out, code_t::protocol_not_supported);
			case EROFS: return _make_code(_out, code_t::read_only_file_system);
			case EDEADLK: return _make_code(_out, code_t::resource_deadlock_would_occur);
#ifdef _WIN32
			case EAGAIN: return _make_code(_out, code_t::resource_unavailable_try_again);
#elif defined(__unix__)
				//same value of EWOULDBLOCK
#else
#error unknow platform
#endif
			case ERANGE: return _make_code(_out, code_t::result_out_of_range);
			case ENOTRECOVERABLE: return _make_code(_out, code_t::state_not_recoverable);
				//			case ETIME: return _make_code(_out,code_t::stream_timeout); //deprecated in c++23
			case ETXTBSY: return _make_code(_out, code_t::text_file_busy);
			case ETIMEDOUT: return _make_code(_out, code_t::timed_out);
			case ENFILE: return _make_code(_out, code_t::too_many_files_open_in_system);
			case EMFILE: return _make_code(_out, code_t::too_many_files_open);
			case EMLINK: return _make_code(_out, code_t::too_many_links);
			case ELOOP: return _make_code(_out, code_t::too_many_symbolic_link_levels);
			case EOVERFLOW: return _make_code(_out, code_t::value_too_large);
			case EPROTOTYPE: return _make_code(_out, code_t::wrong_protocol_type);
			default: break;
		}
		return {};
	}

}
