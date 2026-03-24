#ifndef _IRON_OPTION_H
#define _IRON_OPTION_H

#include "iron/macros.h"
#ifdef IRON_NO_USE_MODULE
#include <cstring>
#include "iron/unsafe/raw_storage.h"
#include "iron/unsafe/raw_wrapper.h"
#include "iron/clone.h"
#include "iron/ref_wrapper.h"
#include "iron/impl/option_impl.h"
#else 
#error cannot use this header with modules
#endif

#endif 

