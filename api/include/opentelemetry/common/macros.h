// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstdint>

#include "opentelemetry/version.h"

/// \brief Declare class, variable or functions as deprecated
/// usage:
///   OPENTELEMETRY_DEPRECATED int a;
///   class OPENTELEMETRY_DEPRECATED a;
///   OPENTELEMETRY_DEPRECATED int a();
/// usage:
///   OPENTELEMETRY_DEPRECATED_MESSAGE("there is better choose") int a;
///   class DEPRECATED_MSG("there is better choose") a;
///   OPENTELEMETRY_DEPRECATED_MESSAGE("there is better choose") int a();
///

#if defined(__cplusplus) && __cplusplus >= 201402L
#  define OPENTELEMETRY_DEPRECATED [[deprecated]]
#elif defined(__clang__)
#  define OPENTELEMETRY_DEPRECATED __attribute__((deprecated))
#elif defined(__GNUC__) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1)))
#  define OPENTELEMETRY_DEPRECATED __attribute__((deprecated))
#elif defined(_MSC_VER) && _MSC_VER >= 1400  // vs 2005 or higher
#  if _MSC_VER >= 1910 && defined(_MSVC_LANG) && _MSVC_LANG >= 201703L
#    define OPENTELEMETRY_DEPRECATED [[deprecated]]
#  else
#    define OPENTELEMETRY_DEPRECATED __declspec(deprecated)
#  endif
#else
#  define OPENTELEMETRY_DEPRECATED
#endif

#if defined(__cplusplus) && __cplusplus >= 201402L
#  define OPENTELEMETRY_DEPRECATED_MESSAGE(msg) [[deprecated(msg)]]
#elif defined(__clang__)
#  define OPENTELEMETRY_DEPRECATED_MESSAGE(msg) __attribute__((deprecated(msg)))
#elif defined(__GNUC__) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1)))
#  define OPENTELEMETRY_DEPRECATED_MESSAGE(msg) __attribute__((deprecated(msg)))
#elif defined(_MSC_VER) && _MSC_VER >= 1400  // vs 2005 or higher
#  if _MSC_VER >= 1910 && defined(_MSVC_LANG) && _MSVC_LANG >= 201703L
#    define OPENTELEMETRY_DEPRECATED_MESSAGE(msg) [[deprecated(msg)]]
#  else
#    define OPENTELEMETRY_DEPRECATED_MESSAGE(msg) __declspec(deprecated(msg))
#  endif
#else
#  define OPENTELEMETRY_DEPRECATED_MESSAGE(msg)
#endif

/// \brief Declare variable as maybe unused
/// usage:
///   OPENTELEMETRY_MAYBE_UNUSED int a;
///   class OPENTELEMETRY_MAYBE_UNUSED a;
///   OPENTELEMETRY_MAYBE_UNUSED int a();
///
#if defined(__cplusplus) && __cplusplus >= 201703L
#  define OPENTELEMETRY_MAYBE_UNUSED [[maybe_unused]]
#elif defined(__clang__)
#  define OPENTELEMETRY_MAYBE_UNUSED __attribute__((unused))
#elif defined(__GNUC__) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1)))
#  define OPENTELEMETRY_MAYBE_UNUSED __attribute__((unused))
#elif defined(_MSC_VER) && _MSC_VER >= 1700  // vs 2012 or higher
#  if _MSC_VER >= 1910 && defined(_MSVC_LANG) && _MSVC_LANG >= 201703L
#    define OPENTELEMETRY_MAYBE_UNUSED [[maybe_unused]]
#  else
#    define OPENTELEMETRY_MAYBE_UNUSED
#  endif
#else
#  define OPENTELEMETRY_MAYBE_UNUSED
#endif

/// \brief Allow fallthrough of case in switch
/// usage:
///   OPENTELEMETRY_FALLTHROUGH int a;
///   switch (xxx) {
///      case XXX:
///      OPENTELEMETRY_FALLTHROUGH
///
#if defined(__cplusplus) && __cplusplus >= 201703L
#  define OPENTELEMETRY_FALLTHROUGH [[fallthrough]];
#elif defined(__clang__) && ((__clang_major__ * 100) + __clang_minor__) >= 309
#  if defined(__apple_build_version__)
#    define OPENTELEMETRY_FALLTHROUGH
#  elif defined(__has_warning) && __has_feature(cxx_attributes) && \
      __has_warning("-Wimplicit-fallthrough")
#    define OPENTELEMETRY_FALLTHROUGH [[clang::fallthrough]];
#  else
#    define OPENTELEMETRY_FALLTHROUGH
#  endif
#elif defined(__GNUC__) && (__GNUC__ >= 7)
#  define OPENTELEMETRY_FALLTHROUGH [[gnu::fallthrough]];
#elif defined(_MSC_VER) && _MSC_VER >= 1700  // vs 2012 or higher
#  if _MSC_VER >= 1910 && defined(_MSVC_LANG) && _MSVC_LANG >= 201703L
#    define OPENTELEMETRY_FALLTHROUGH [[fallthrough]];
#  else
#    define OPENTELEMETRY_FALLTHROUGH
#  endif
#else
#  define OPENTELEMETRY_FALLTHROUGH
#endif
