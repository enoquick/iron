#ifndef _IRON_ERRORS_BASE_H
#define _IRON_ERRORS_BASE_H
#include "iron/macros.h"
#ifdef IRON_NO_USE_MODULE
#include "iron/unsafe/security_checker.h"
#include "iron/impl/errors/base_impl.h"
#else 
#error cannot use this header with modules
#endif
#endif
