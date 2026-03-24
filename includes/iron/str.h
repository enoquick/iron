#ifndef _IRON_STR_H
#define _IRON_STR_H
#include "iron/macros.h"
#ifdef IRON_NO_USE_MODULE
#include "iron/error_handler.h"
#include "iron/sequence.h"
#include "iron/adapter_box.h"
#include "iron/unsafe/raw_str.h"
#include "iron/impl/strview_impl.h"
#include "iron/impl/str_impl.h"
#else
#error cannot use this header with modules
#endif // IRON_NO_USE_MODULE
#endif // _IRON_STR_H

