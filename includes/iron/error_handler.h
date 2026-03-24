#ifndef _IRON_HANDLER_H
#define _IRON_HANDLER_H
#include "iron/macros.h"
#ifdef IRON_NO_USE_MODULE
#include <limits>
#include "iron/unsafe/allocator.h"
#include "iron/memory/stacktrace.h"
#include "iron/unsafe/errors.h"
#include "iron/impl/error_handler_impl.h"
#else
#error cannot use this header with modules
#endif // IRON_NO_USE_MODULE
#endif //_IRON_HANDLER_H
