#ifndef _IRON_UNSAFE_RAW_VECTOR_H
#define _IRON_UNSAFE_RAW_VECTOR_H
#include "iron/macros.h"
#ifdef IRON_NO_USE_MODULE
#include "iron/unsafe/raw_storage.h"
#include "iron/integrals.h"
#include "iron/clone.h"
#include "iron/impl/unsafe/raw_vector_impl.h"
#else
#error cannot use this header with modules
#endif
#endif
