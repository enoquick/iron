#ifndef _IRON_MEMORY_SPIN_LOCK_H
#define _IRON_MEMORY_SPIN_LOCK_H
#include "iron/macros.h"
#ifdef IRON_NO_USE_MODULE
#include <atomic>
#include <thread>
#include "iron/memory/lock_guard.h"
#include "iron/impl/memory/spin_lock_impl.h"
#else
#error cannot use this header with modules
#endif // IRON_NO_USE_MODULE
#endif //_IRON_MEMORY_SPIN_LOCK_H
