#ifndef _IRON_CONCEPTS_H
#define _IRON_CONCEPTS_H

#include "iron/macros.h"
#ifdef IRON_NO_USE_MODULE
#include <concepts>
#include <utility>
#include <functional>
#include <memory>
#include "iron/impl/concepts_impl.h"
#else
#error cannot use this header with modules
#endif

#endif
