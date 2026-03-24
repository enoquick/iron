export module iron.core;
#undef IRON_NO_USE_MODULE
#define IRON_USE_MODULES
#ifdef IRONMODULES_EXPORTS 
#ifdef _WIN32
#define _IRON_DLL_EXPORT   __declspec( dllexport )
#pragma comment( lib, "iron_modules")
#else 
#error  IRON_DLL_EXPORT is not set
#endif
#endif //IRONMODULES_EXPORTS
import <climits>;
import <cstdint>;
import <cerrno>;
import <system_error>;
import <cstring>;
import <iterator>;
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable:5050)
#endif 
import std.core;
#if defined(_MSC_VER)
#pragma warning(pop)
#endif 
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable:26402)
#pragma warning (disable:26409)
#pragma warning (disable:5063)
#endif
#include "iron/impl/version_impl.h"
#include "iron/impl/macros_impl.h"
#include "iron/impl/concepts_impl.h"
#include "iron/impl/fwd_impl.h"
#include "iron/impl/unsafe/asserts_impl.h"
#include "iron/impl/cfg_impl.h"
#include "iron/impl/unsafe/allocator_impl.h"
#include "iron/impl/unsafe/unicode_conversions_impl.h"
#include "iron/impl/unsafe/security_checker_impl.h"
#include "iron/impl/bit_set_impl.h"
#include "iron/impl/enum_set_impl.h"
#include "iron/impl/ref_wrapper_impl.h"
#include "iron/impl/unsafe/errors/base_impl.h"
#include "iron/impl/unsafe/errors/base_with_info_impl.h"
#include "iron/impl/unsafe/errors/posix_impl.h"
#include "iron/impl/unsafe/errors/os_impl.h"
#include "iron/impl/unsafe/raw_wrapper_impl.h"
#include "iron/impl/unsafe/errors/exception_impl.h"
#include "iron/impl/memory/stacktrace_impl.h"
#include "iron/impl/error_handler_impl.h"
#include "iron/impl/result_impl.h"
#include "iron/impl/clone_impl.h"
#include "iron/impl/option_impl.h"
#include "iron/impl/integrals_impl.h"
#include "iron/impl/unsafe/iterators/contiguous_impl.h"
#include "iron/impl/unsafe/unicode_impl.h"
#include "iron/impl/unsafe/raw_storage_impl.h"
#include "iron/impl/unsafe/raw_vector_impl.h"
#include "iron/impl/unsafe/list_node_impl.h"
#include "iron/impl/unsafe/raw_list_impl.h"
#include "iron/impl/adapter_box_impl.h"
#include "iron/impl/collector_impl.h"
#include "iron/impl/chr_impl.h"
#include "iron/impl/box_impl.h"
#include "iron/impl/memory/spin_lock_impl.h"
#include "iron/impl/memory/lock_guard_impl.h"
#include "iron/impl/shared_box_impl.h"
#include "iron/impl/stack_area_impl.h"
#include "iron/impl/memory/mutex_impl.h"
#include "iron/impl/memory/condition_variable_impl.h"
#include "iron/impl/unsafe/iterators/filter_impl.h"
#include "iron/impl/unsafe/iterators/step_impl.h"
#include "iron/impl/unsafe/iterators/take_impl.h"
#include "iron/impl/unsafe/iterators/enumerate_impl.h"
#include "iron/impl/unsafe/iterators/unicode_impl.h"
#include "iron/impl/unsafe/iterators/list_impl.h"
#include "iron/impl/sequence_impl.h"
#include "iron/impl/vector_impl.h"
#include "iron/impl/list_impl.h"
#include "iron/impl/array_impl.h"
#include "iron/impl/hasher_impl.h"
#include "iron/impl/comparables_impl.h"
#include "iron/impl/proxy_pair_impl.h"
#include "iron/impl/unsafe/iterators/hash_impl.h"
#include "iron/impl/unsafe/raw_hash_table_impl.h"
#include "iron/impl/hash_table_impl.h"
#include "iron/impl/unsafe/raw_array_list_impl.h"
#include "iron/impl/unsafe/btree/node_impl.h"
#include "iron/impl/unsafe/btree/common_impl.h"
#include "iron/impl/unsafe/btree/index_impl.h"
#include "iron/impl/unsafe/iterators/btree_impl.h"
#include "iron/impl/unsafe/btree/raw_table_impl.h"
#include "iron/impl/btree_table_impl.h"
#include "iron/impl/unsafe/str/common_impl.h"
#include "iron/impl/unsafe/str/raw_strview_impl.h"
#include "iron/impl/unsafe/str/raw_str_impl.h"
#include "iron/impl/strview_impl.h"
#include "iron/impl/str_impl.h"


#if defined(_MSC_VER)
#pragma warning(pop)
#endif

