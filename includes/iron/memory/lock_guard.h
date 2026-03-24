#ifndef _IRON_MEMORY_LOCK_GUARD_H
#define _IRON_MEMORY_LOCK_GUARD_H
#include "iron/macros.h"
#ifdef IRON_NO_USE_MODULE
#include "iron/concepts.h"
#include "iron/fwd.h"
#include "iron/impl/memory/lock_guard_impl.h"
#else
#error cannot use this header with modules
#endif // IRON_NO_USE_MODULE
#endif //_IRON_MEMORY_LOCK_GUARD_H

