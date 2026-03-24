#ifndef _IRON_MEMORY_STACKTRACE_H
#define _IRON_MEMORY_STACKTRACE_H
#include "iron/macros.h"
#ifdef IRON_NO_USE_MODULE
#include "iron/ref_wrapper.h"
#include "iron/impl/memory/stacktrace_impl.h"
#else
#error cannot use this header with modules
#endif
#endif
