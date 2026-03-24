#ifndef _IRON_UNSAFE_ALLOCATOR_H
#define _IRON_UNSAFE_ALLOCATOR_H

#include "iron/macros.h"
#ifdef IRON_NO_USE_MODULE
#include <memory>
#include <limits>
#include <cstring>
#include "iron/unsafe/asserts.h"
#include "iron/impl/unsafe/allocator_impl.h"
#else
#error cannot use this header with modules
#endif //IRON_NO_USE_MODULE
#endif //_IRON_MEMORY_ALLOCATOR_H
