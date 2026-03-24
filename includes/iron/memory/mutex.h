#ifndef _IRON_MEMORY_MUTEX_H
#define _IRON_MEMORY_MUTEX_H
#include "iron/macros.h"
#ifdef IRON_NO_USE_MODULE
#include "iron/error_handler.h"
#include "iron/result.h"
#include "iron/impl/stack_area_impl.h"
#include "iron/memory/lock_guard.h"
#include "iron/impl/memory/mutex_impl.h"
#else
#error cannot use this header with modules
#endif //IRON_NO_USE_MODULE
#endif //_IRON_MEMORY_MUTEX_H
