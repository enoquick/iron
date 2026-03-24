#ifndef _IRON_ERRORS_OS_H
#define _IRON_ERRORS_OS_H
#include "iron/macros.h"
#ifdef IRON_NO_USE_MODULE
#include <cerrno>
#include "iron/errors/base_with_info.h"
#include "iron/impl/errors/os_impl.h"
#else
#error cannot use this header with modules
#endif
#endif

