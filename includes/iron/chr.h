#ifndef _IRON_CHR_H
#define _IRON_CHR_H
#include "iron/macros.h"
#ifdef IRON_NO_USE_MODULE
#include "iron/unsafe/unicode.h"
#include "iron/unsafe/unicode_conversions.h"
#include "iron/impl/chr_impl.h"
#else
#error cannot use this header with modules
#endif
#endif 
