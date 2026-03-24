#ifndef _IRON_ENUM_SET_H
#define _IRON_ENUM_SET_H
#include "iron/macros.h"
#ifdef IRON_NO_USE_MODULE
#include <climits>
#include <cstring>
#include "iron/unsafe/security_checker.h"
#include "iron/bit_set.h"
#include "iron/impl/enum_set_impl.h"
#else 
#error cannot use this header with modules
#endif //IRON_NO_USE_MODULE
#endif //_IRON_BIT_SET_H
