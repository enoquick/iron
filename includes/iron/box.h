#ifndef _IRON_BOX_H
#define _IRON_BOX_H
#include "iron/macros.h"
#ifdef IRON_NO_USE_MODULE
#include "iron/error_handler.h"
#include "iron/option.h"
#include "iron/result.h"
#include "iron/unsafe/raw_storage.h"
#include "iron/impl/box_impl.h"
#else
#error cannot use this header with modules
#endif
#endif

