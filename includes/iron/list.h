#ifndef _IRON_LIST_H
#define _IRON_LIST_H
#include "iron/macros.h"
#ifdef IRON_NO_USE_MODULE
#include "iron/sequence.h"
#include "iron/adapter_box.h"
#include "iron/unsafe/list_node.h"
#include "iron/unsafe/raw_list.h"
#include "iron/unsafe/iterators.h"
#include "iron/impl/list_impl.h"
#else
#error cannot use this header with modules
#endif
#endif

