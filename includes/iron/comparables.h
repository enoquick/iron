#ifndef _IRON_COMPARABLES_H
#define _IRON_COMPARABLES_H
#include "iron/macros.h"
#ifdef IRON_NO_USE_MODULE
#include "iron/unsafe/security_checker.h"
#include <compare>
#include "iron/impl/comparables_impl.h"
#else
#error cannot use this header with modules
#endif
#endif
