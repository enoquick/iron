#ifndef _IRON_RESULT_H
#define _IRON_RESULT_H
#include "iron/macros.h"
#ifdef IRON_NO_USE_MODULE
#include <limits>
#include "iron/unsafe/security_checker.h"
#include "iron/unsafe/raw_wrapper.h" 
#include "iron/ref_wrapper.h"
#include "iron/impl/result_impl.h"
#include "iron/clone.h"
#else 
#error cannot use this header with modules
#endif
#endif

