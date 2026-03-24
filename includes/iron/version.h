#ifndef _IRON_VERSION_H
#define _IRON_VERSION_H
#include "iron/macros.h"
#ifdef IRON_NO_USE_MODULE
#include "iron/impl/version_impl.h"
#else
#error cannot use this header with modules
#endif
#endif
