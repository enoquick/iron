#ifndef _IRON_UNSAFE_LIST_NODE_H
#define _IRON_UNSAFE_LIST_NODE_H
#include "iron/macros.h"
#ifdef IRON_NO_USE_MODULE
#include "iron/unsafe/raw_storage.h"
#include "iron/impl/unsafe/list_node_impl.h"
#else
#error cannot use this header with modules
#endif
#endif
