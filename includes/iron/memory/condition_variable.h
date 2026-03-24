#ifndef _IRON_MEMORY_CONDITION_VARIABLE_H
#define _IRON_MEMORY_CONDITION_VARIABLE_H
#include "iron/macros.h"
#ifdef IRON_NO_USE_MODULE
#include "iron/result.h"
#include <mutex> 
#include <condition_variable>
#include "iron/memory/mutex.h"
#include "iron/impl/stack_area_impl.h"
#include "iron/impl/memory/condition_variable_impl.h"
#else
#error cannot use this header with modules
#endif //IRON_NO_USE_MODULE
#endif //_IRON_MEMORY_CONDITION_VARIABLE_H
