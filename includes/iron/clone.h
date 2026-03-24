#ifndef _IRON_CLONE_H
#define _IRON_CLONE_H
#include "iron/macros.h"
#ifdef IRON_NO_USE_MODULE
#include "iron/error_handler.h"
#include "iron/impl/clone_impl.h"
#else 
#error cannot use this header with modules
#endif
#endif 

