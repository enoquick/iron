#ifndef _IRON_CFG_H
#define _IRON_CFG_H
#include "iron/macros.h"
#ifdef IRON_NO_USE_MODULE
#include <climits>
#include <cstdint>
#include <bit>
#include "iron/concepts.h"
#include "iron/version.h"
#include "iron/impl/cfg_impl.h"
#else 
#error cannot use this header with modules
#endif
#endif

