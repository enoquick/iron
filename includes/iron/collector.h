#ifndef _IRON_COLLECTOR_H
#define _IRON_COLLECTOR_H
#include "iron/macros.h"
#ifdef IRON_NO_USE_MODULE
#include "iron/error_handler.h"
#include "iron/unsafe/raw_storage.h"
#include "iron/impl/collector_impl.h"
#else
#error cannot use this header with modules
#endif
#endif

