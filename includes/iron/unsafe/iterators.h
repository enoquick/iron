#ifndef _IRON_UNSAFE_ITERATORS_H
#define _IRON_UNSAFE_ITERATORS_H

#include "iron/macros.h"
#ifdef IRON_NO_USE_MODULE
#include <cstdint>
#include <utility>
#include "iron/fwd.h"
#include "iron/unsafe/unicode_conversions.h"
#include "iron/proxy_pair.h"
#include "iron/unsafe/raw_list.h"
#include "iron/impl/unsafe/iterators/contiguous_impl.h"
#include "iron/impl/unsafe/iterators/enumerate_impl.h"
#include "iron/impl/unsafe/iterators/filter_impl.h"
#include "iron/impl/unsafe/iterators/hash_impl.h"
#include "iron/impl/unsafe/iterators/list_impl.h"
#include "iron/impl/unsafe/iterators/step_impl.h"
#include "iron/impl/unsafe/iterators/take_impl.h"
#include "iron/impl/unsafe/iterators/unicode_impl.h"
#else
#error cannot use this header with modules
#endif

#endif

