#ifndef _IRON_BTREE_TABLE_H
#define _IRON_BTREE_TABLE_H
#include "iron/macros.h"
#ifdef IRON_NO_USE_MODULE
#include "iron/sequence.h"
#include "iron/adapter_box.h"
#include "iron/unsafe/raw_btree_table.h"
#include "iron/impl/btree_table_impl.h"
#else
#error cannot use this header with modules
#endif //IRON_NO_USE_MODULE
#endif //_IRON_BTREE_TABLE_H
