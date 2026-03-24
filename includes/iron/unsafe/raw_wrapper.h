#ifndef _IRON_UNSAFE_RAW_WRAPPER_H
#define _IRON_UNSAFE_RAW_WRAPPER_H
#include "iron/macros.h"
#ifdef IRON_NO_USE_MODULE
#include "iron/unsafe/asserts.h"
#include <cstring>
#include "iron/concepts.h"
#include "iron/impl/unsafe/raw_wrapper_impl.h"
#else
#error cannot use this header with modules
#endif
#endif
