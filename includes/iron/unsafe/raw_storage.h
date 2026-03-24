#ifndef _IRON_UNSAFE_RAW_STORAGE_H
#define _IRON_UNSAFE_RAW_STORAGE_H
#include "iron/macros.h"
#ifdef IRON_NO_USE_MODULE
#include <limits>
#include "iron/clone.h"
#include "iron/impl/unsafe/raw_storage_impl.h"
#else
#error cannot use this header with modules
#endif
#endif
