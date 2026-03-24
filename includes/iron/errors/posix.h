#ifndef _IRON_ERRORS_POSIX_H
#define _IRON_ERRORS_POSIX_H
#include "iron/macros.h"
#ifdef IRON_NO_USE_MODULE
#include <system_error>
#include "iron/errors/base_with_info.h"
#include "iron/impl/errors/posix_impl.h"
#else
#error cannot use this header with modules
#endif
#endif

