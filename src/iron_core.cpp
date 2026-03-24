#ifdef USE_MODULES
module;
#endif
#include "iron/macros.h"

#ifdef USE_MODULES
#undef IRON_NO_USE_MODULE
#else
#define IRON_NO_USE_MODULE
#endif

#ifdef IRON_NO_USE_MODULE
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "corecrt_share.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <shlwapi.h>
#undef max
#undef min
#elif defined(__unix__)
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/select.h>
#else
#error unknow platform
#endif
#else /*!IRON_NO_USE_MODULE*/
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable:5105)
#pragma warning (disable:26429)
#pragma warning (disable:26473)
#endif
//#include "iron/module_support.h"
#include <cerrno>
#include <cstdio>
#ifdef _WIN32
#ifndef _CORECRT_SHARE_H
#define _CORECRT_SHARE_H
#include "corecrt_share.h"
#endif //_CORECRT_SHARE_H
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <shlwapi.h>
#undef min
#undef max
#endif // WIN32_LEAN_AND_MEAN
#if defined(_MSC_VER)
#pragma warning(pop)
#endif
#endif /*_WIN32*/
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable:5050)
#endif
module iron.core;
#if defined(_MSC_VER)
#pragma warning(pop)
#endif
#endif //!IRON_NO_USE_MODULE

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning (disable:5105)
#pragma warning (disable:26408)
#pragma warning (disable:26409)
#pragma warning (disable:26415)
#pragma warning (disable:26418)
#pragma warning (disable:26432)
#pragma warning (disable:26446)
#pragma warning (disable:26457)
#pragma warning (disable:26471)
#pragma warning (disable:26472)
#pragma warning (disable:26473)
#pragma warning (disable:26481)
#pragma warning (disable:26482)
#pragma warning (disable:26485)
#pragma warning (disable:26490)
#pragma warning (disable:26491)
#pragma warning (disable:26492)
#pragma warning (disable:26496)
#pragma warning (disable:26830)
#pragma warning (disable:26429)
#pragma warning (disable:26430)
#endif //_MSC_VER

#include "../src/iron_unsafe_asserts.cpp"
#include "../src/iron_cfg.cpp"
#include "../src/iron_unsafe_unicode.cpp"
#include "../src/iron_chr.cpp"
#include "../src/iron_unsafe_security_checker.cpp"
#include "../src/iron_error_handler.cpp"
#include "../src/iron_memory_stacktrace.cpp"
#include "../src/iron_memory_mutex.cpp"
#include "../src/iron_memory_condition_variable.cpp" //after mutex
#include "../src/iron_unsafe_raw_str.cpp"
#include "../src/iron_unsafe_str_common.cpp"

#ifdef _MSC_VER
#pragma warning(pop)
#endif

