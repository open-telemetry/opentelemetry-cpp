// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#if !defined(OPENTELEMETRY_LIKELY_IF) && defined(__cplusplus)
// GCC 9 has likely attribute but do not support declare it at the beginning of statement
#  if defined(__has_cpp_attribute) && (defined(__clang__) || !defined(__GNUC__) || __GNUC__ > 9)
#    if __has_cpp_attribute(likely)
#      define OPENTELEMETRY_LIKELY_IF(...) \
        if (__VA_ARGS__)                   \
        [[likely]]
#    endif
#  endif
#endif
#if !defined(OPENTELEMETRY_LIKELY_IF) && (defined(__clang__) || defined(__GNUC__))
#  define OPENTELEMETRY_LIKELY_IF(...) if (__builtin_expect(!!(__VA_ARGS__), true))
#endif
#ifndef OPENTELEMETRY_LIKELY_IF
#  define OPENTELEMETRY_LIKELY_IF(...) if (__VA_ARGS__)
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
#elif (defined(_MSC_VER) && _MSC_VER >= 1910) && (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
#  define OPENTELEMETRY_MAYBE_UNUSED [[maybe_unused]]
#else
#  define OPENTELEMETRY_MAYBE_UNUSED
#endif

#ifndef OPENTELEMETRY_RTTI_ENABLED
#  if defined(__clang__)
#    if __has_feature(cxx_rtti)
#      define OPENTELEMETRY_RTTI_ENABLED
#    endif
#  elif defined(__GNUG__)
#    if defined(__GXX_RTTI)
#      define OPENTELEMETRY_RTTI_ENABLED
#    endif
#  elif defined(_MSC_VER)
#    if defined(_CPPRTTI)
#      define OPENTELEMETRY_RTTI_ENABLED
#    endif
#  endif
#endif

#if defined(__cplusplus) && __cplusplus >= 201402L
#  define OPENTELEMETRY_DEPRECATED [[deprecated]]
#elif defined(__clang__)
#  define OPENTELEMETRY_DEPRECATED __attribute__((deprecated))
#elif defined(__GNUC__)
#  define OPENTELEMETRY_DEPRECATED __attribute__((deprecated))
#elif defined(_MSC_VER)
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
#elif defined(__GNUC__)
#  define OPENTELEMETRY_DEPRECATED_MESSAGE(msg) __attribute__((deprecated(msg)))
#elif defined(_MSC_VER)
#  if _MSC_VER >= 1910 && defined(_MSVC_LANG) && _MSVC_LANG >= 201703L
#    define OPENTELEMETRY_DEPRECATED_MESSAGE(msg) [[deprecated(msg)]]
#  else
#    define OPENTELEMETRY_DEPRECATED_MESSAGE(msg) __declspec(deprecated(msg))
#  endif
#else
#  define OPENTELEMETRY_DEPRECATED_MESSAGE(msg)
#endif

// Regex support
#if (__GNUC__ == 4 && (__GNUC_MINOR__ == 8 || __GNUC_MINOR__ == 9))
#  define OPENTELEMETRY_HAVE_WORKING_REGEX 0
#else
#  define OPENTELEMETRY_HAVE_WORKING_REGEX 1
#endif

/* clang-format off */

/**
  @page HEADER_ONLY_SINGLETON Header only singleton.

  @section ELF_SINGLETON

  For clang and gcc, the desired coding pattern is as follows.

  @verbatim
  class Foo
  {
    // (a)
    __attribute__((visibility("default")))
    // (b)
    T& get_singleton()
    {
      // (c)
      static T singleton;
      return singleton;
    }
  };
  @endverbatim

  (a) is needed when the code is build with
  @code -fvisibility="hidden" @endcode
  to ensure that all instances of (b) are visible to the linker.

  What is duplicated in the binary is @em code, in (b).

  The linker will make sure only one instance
  of all the (b) methods is used.

  (c) is a singleton implemented inside a method.

  This is very desirable, because:

  - the C++ compiler guarantees that construction
    of the variable (c) is thread safe.

  - constructors for (c) singletons are executed in code path order,
    or not at all if the singleton is never used.

  @section OTHER_SINGLETON

  For other platforms, header only singletons are not supported at this
point.

  @section CODING_PATTERN

  The coding pattern to use in the source code is as follows

  @verbatim
  class Foo
  {
    OPENTELEMETRY_API_SINGLETON
    T& get_singleton()
    {
      static T singleton;
      return singleton;
    }
  };
  @endverbatim
*/

/* clang-format on */

#if defined(__clang__)

#  define OPENTELEMETRY_API_SINGLETON __attribute__((visibility("default")))

#elif defined(__GNUC__)

#  define OPENTELEMETRY_API_SINGLETON __attribute__((visibility("default")))

#else

/* Add support for other compilers here. */

#  define OPENTELEMETRY_API_SINGLETON

#endif
