#ifndef _IRON_UNSAFE_ASSERTS_H
#define _IRON_UNSAFE_ASSERTS_H

#include "iron/macros.h"
#ifdef IRON_NO_USE_MODULE
#include <cstdint>
#include <utility>
#include "iron/fwd.h"
#include "iron/impl/unsafe/asserts_impl.h"
#else
#error cannot use this header with modules
#endif

#endif

