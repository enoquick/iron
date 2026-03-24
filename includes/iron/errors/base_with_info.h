#ifndef _IRON_ERRORS_BASE_WITH_INFO_H
#define _IRON_ERRORS_BASE_WITH_INFO_H
#include "iron/macros.h"
#ifdef IRON_NO_USE_MODULE
#include "iron/errors/base.h"
#include "iron/impl/errors/base_with_info_impl.h"
#else
#error cannot use this header with modules
#endif
#endif

