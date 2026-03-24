#ifndef _IRON_UNSAFE_ERRORS_H
#define _IRON_UNSAFE_ERRORS_H
#include "iron/macros.h"
#ifdef IRON_NO_USE_MODULE

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable:26481)
#endif
#include <cstdint>
#include <system_error>
#include "iron/unsafe/allocator.h"
#include "iron/impl/unsafe/errors/base_impl.h"
#include "iron/impl/unsafe/errors/base_with_info_impl.h"
#include "iron/impl/unsafe/errors/posix_impl.h"
#include "iron/impl/unsafe/errors/os_impl.h"
#include "iron/impl/unsafe/errors/exception_impl.h"

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#else
#error cannot use this header with modules
#endif
#endif
