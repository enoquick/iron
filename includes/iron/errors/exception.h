#ifndef _IRON_ERRORS_EXCEPTION_H
#define _IRON_ERRORS_EXCEPTION_H
#include "iron/macros.h"
#ifdef IRON_NO_USE_MODULE
#include <exception>
#include "iron/errors/base_with_info.h"
//#include "iron/memory/raw_wrapper.h"
#include "iron/impl/errors/exception_impl.h"
#else
#error cannot use this header with modules
#endif
#endif

