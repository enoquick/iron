#ifndef _IRON_FWD_H
#define _IRON_FWD_H
#include "iron/macros.h"
#ifdef IRON_NO_USE_MODULE
#include <cstddef>
#include <memory>
#include "iron/concepts.h"
#include "iron/impl/fwd_impl.h"
#else
#error cannot use this header with modules
#endif // IRON_NO_USE_MODULE

#endif // _IRON_FWD_H

