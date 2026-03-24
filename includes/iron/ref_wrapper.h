#ifndef _IRON_REF_WRAPPER_H
#define _IRON_REF_WRAPPER_H
#include "iron/macros.h"
#ifdef IRON_NO_USE_MODULE
#include <memory>
#include "iron/unsafe/security_checker.h"
#include "iron/impl/ref_wrapper_impl.h"
#else
#error cannot use this header with modules
#endif
#endif

