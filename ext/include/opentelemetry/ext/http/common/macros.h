#pragma once

#include <mutex>

#ifndef TOKENPASTE
#  define TOKENPASTE(x, y) x##y
#endif

#ifndef TOKENPASTE2
#  define TOKENPASTE2(x, y) TOKENPASTE(x, y)
#endif

#ifndef LOCKGUARD
#  define LOCKGUARD(macro_mutex) \
    std::lock_guard<decltype(macro_mutex)> TOKENPASTE2(__guard_, __LINE__)(macro_mutex)
#endif

#if defined(HAVE_CONSOLE_LOG) && !defined(LOG_DEBUG)
// Log to console if there's no standard log facility defined
#  include <cstdio>
#  ifndef LOG_DEBUG
#    define LOG_DEBUG(fmt_, ...) printf(" " fmt_ "\n", ##__VA_ARGS__)
#    define LOG_TRACE(fmt_, ...) printf(" " fmt_ "\n", ##__VA_ARGS__)
#    define LOG_INFO(fmt_, ...) printf(" " fmt_ "\n", ##__VA_ARGS__)
#    define LOG_WARN(fmt_, ...) printf(" " fmt_ "\n", ##__VA_ARGS__)
#    define LOG_ERROR(fmt_, ...) printf(" " fmt_ "\n", ##__VA_ARGS__)
#  endif
#endif

#ifndef LOG_DEBUG
// Don't log anything if there's no standard log facility defined
#  define LOG_DEBUG(fmt_, ...)
#  define LOG_TRACE(fmt_, ...)
#  define LOG_INFO(fmt_, ...)
#  define LOG_WARN(fmt_, ...)
#  define LOG_ERROR(fmt_, ...)
#endif

// Annex K macros
#if !defined(_MSC_VER)
#  ifndef strncpy_s
#    define strncpy_s(dest, destsz, src, count) \
      strncpy(dest, src, (destsz <= count) ? destsz : count)
#  endif
#endif

// SAL macro
#ifndef _Out_cap_
#  define _Out_cap_(size)
#endif
