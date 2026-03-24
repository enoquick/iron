#ifndef _IRON_HASH_TABLE_H
#define _IRON_HASH_TABLE_H
#include "iron/macros.h"
#ifdef IRON_NO_USE_MODULE
#include "iron/sequence.h"
#include "iron/adapter_box.h"
#include "iron/proxy_pair.h"
#include "iron/unsafe/list_node.h"
#include "iron/unsafe/raw_list.h"
#include "iron/unsafe/iterators.h"
#include "iron/unsafe/raw_hash_table.h"
#include "iron/impl/hash_table_impl.h"
#else
#error cannot use this header with modules
#endif //IRON_NO_USE_MODULE
#endif //_IRON_HASH_TABLE_H
