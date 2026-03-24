#ifndef _IRON_SHARED_BOX_H
#define _IRON_SHARED_BOX_H
#include "iron/macros.h"

#ifdef IRON_NO_USE_MODULE
#include "iron/box.h"
#include "iron/memory/spin_lock.h"
#include "iron/impl/shared_box_impl.h"
#else
#error cannot use this header with modules
#endif

#endif

