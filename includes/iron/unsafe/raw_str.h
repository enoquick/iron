#ifndef _IRON_UNSAFE_RAW_STR_H
#define _IRON_UNSAFE_RAW_STR_H
#include "iron/macros.h"
#ifdef IRON_NO_USE_MODULE
#include <cctype>
#include "iron/clone.h"
#include "iron/chr.h"
#include "iron/unsafe/unicode_conversions.h"
#include "iron/unsafe/raw_vector.h"
#include "iron/unsafe/iterators.h"
#include "iron/impl/unsafe/str/common_impl.h"
#include "iron/impl/unsafe/str/raw_strview_impl.h"
#include "iron/impl/unsafe/str/raw_str_impl.h"
#else
#error cannot use this header with modules
#endif //IRON_NO_USE_MODULE
#endif
