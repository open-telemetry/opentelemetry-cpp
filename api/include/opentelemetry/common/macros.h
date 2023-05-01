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

//
// Atomic wrappers based on compiler intrinsics for memory read/write.
// The tailing number is read/write length in bits.
//
// N.B. Compiler instrinsic is used because the usage of C++ standard library is restricted in the
// OpenTelemetry C++ API.
//
#if defined(__GNUC__)

#  define OPENTELEMETRY_ATOMIC_READ_8(ptr) __atomic_load_n(ptr, __ATOMIC_SEQ_CST)
#  define OPENTELEMETRY_ATOMIC_WRITE_8(ptr, value) __atomic_store_n(ptr, value, __ATOMIC_SEQ_CST)

#elif defined(_MSC_VER)

#  include <intrin.h>

#  define OPENTELEMETRY_ATOMIC_READ_8(ptr) \
    static_cast<uint8_t>(_InterlockedCompareExchange8(reinterpret_cast<char *>(ptr), 0, 0))
#  define OPENTELEMETRY_ATOMIC_WRITE_8(ptr, value) \
    _InterlockedExchange8(reinterpret_cast<char *>(ptr), static_cast<char>(value))

#else
#  error port atomics read/write for the current platform
#endif

/* clang-format on */
//
// The if/elif order matters here. If both OPENTELEMETRY_BUILD_IMPORT_DLL and
// OPENTELEMETRY_BUILD_EXPORT_DLL are defined, the former takes precedence.
//
// TODO: consider define OPENTELEMETRY_EXPORT for cygwin/gcc, see below link.
// https://gcc.gnu.org/wiki/Visibility#How_to_use_the_new_C.2B-.2B-_visibility_support
//
#if defined(_MSC_VER) && defined(OPENTELEMETRY_BUILD_IMPORT_DLL)

#  define OPENTELEMETRY_EXPORT __declspec(dllimport)

#elif defined(_MSC_VER) && defined(OPENTELEMETRY_BUILD_EXPORT_DLL)

#  define OPENTELEMETRY_EXPORT __declspec(dllexport)

#else

//
// build OpenTelemetry as static library or not on Windows.
//
#  define OPENTELEMETRY_EXPORT

#endif

// Bazel on MSVC defines COMPILER_MSVC. This is how we detect the combination.
#if defined(COMPILER_MSVC) && COMPILER_MSVC
// When compiling with --//:with_dll=true OPENTELEMETRY_DLL is set to 1
#  if defined(OPENTELEMETRY_DLL) && OPENTELEMETRY_DLL
#    undef OPENTELEMETRY_EXPORT
#    if !defined(ENABLE_LOGS_PREVIEW) || ENABLE_LOGS_PREVIEW==0
#      error OPENTELEMETRY_DLL: ENABLE_LOGS_PREVIEW must be 1
#    endif
#    if !defined(HAVE_CPP_STDLIB) || HAVE_CPP_STDLIB==0
#      error OPENTELEMETRY_DLL: HAVE_CPP_STDLIB must be 1
#    endif
#    if !defined(OPENTELEMETRY_DLL_EXPORT)
#      define OPENTELEMETRY_DLL_EXPORT __declspec(dllimport)
#    endif
#    define OPENTELEMETRY_EXPORT OPENTELEMETRY_DLL_EXPORT
#    define OPENTELEMETRY_API_EXPORT OPENTELEMETRY_DLL_EXPORT
#    define OPENTELEMETRY_SDK_COMMON_EXPORT OPENTELEMETRY_DLL_EXPORT
#    define OPENTELEMETRY_SDK_INSTRUMENTATIONSCOPE_EXPORT OPENTELEMETRY_DLL_EXPORT
#    define OPENTELEMETRY_SDK_LOGS_EXPORT OPENTELEMETRY_DLL_EXPORT
#    define OPENTELEMETRY_SDK_METRICS_EXPORT OPENTELEMETRY_DLL_EXPORT
#    define OPENTELEMETRY_SDK_RESOURCE_EXPORT OPENTELEMETRY_DLL_EXPORT
#    define OPENTELEMETRY_SDK_TRACE_EXPORT OPENTELEMETRY_DLL_EXPORT
#    define OPENTELEMETRY_EXPORTERS_ELASTICSEARCH_EXPORT OPENTELEMETRY_DLL_EXPORT
#    define OPENTELEMETRY_EXPORTERS_ETW_EXPORT OPENTELEMETRY_DLL_EXPORT
#    define OPENTELEMETRY_EXPORTERS_MEMORY_EXPORT OPENTELEMETRY_DLL_EXPORT
#    define OPENTELEMETRY_EXPORTERS_OSTREAM_EXPORT OPENTELEMETRY_DLL_EXPORT
#    define OPENTELEMETRY_EXPORTERS_OTLP_EXPORT OPENTELEMETRY_DLL_EXPORT
#    define OPENTELEMETRY_EXPORTERS_PROMETHEUS_EXPORT OPENTELEMETRY_DLL_EXPORT
#    define OPENTELEMETRY_EXPORTERS_ZIPKIN_EXPORT OPENTELEMETRY_DLL_EXPORT
#    define OPENTELEMETRY_EXT_HTTP_CLIENT_CURL_EXPORT OPENTELEMETRY_DLL_EXPORT
#    define OPENTELEMETRY_EXT_HTTP_CLIENT_EXPORT OPENTELEMETRY_DLL_EXPORT
#    define OPENTELEMETRY_EXT_ZPAGES_EXPORT OPENTELEMETRY_DLL_EXPORT
#  endif // if defined(OPENTELEMETRY_DLL) && OPENTELEMETRY_DLL
#endif // if defined(COMPILER_MSVC) && COMPILER_MSVC

#ifndef OPENTELEMETRY_EXPORT
#define OPENTELEMETRY_EXPORT
#endif

#ifndef OPENTELEMETRY_API_EXPORT
#define OPENTELEMETRY_API_EXPORT
#endif

#ifndef OPENTELEMETRY_SDK_COMMON_EXPORT
#define OPENTELEMETRY_SDK_COMMON_EXPORT
#endif

#ifndef OPENTELEMETRY_SDK_INSTRUMENTATIONSCOPE_EXPORT
#define OPENTELEMETRY_SDK_INSTRUMENTATIONSCOPE_EXPORT
#endif

#ifndef OPENTELEMETRY_SDK_LOGS_EXPORT
#define OPENTELEMETRY_SDK_LOGS_EXPORT
#endif

#ifndef OPENTELEMETRY_SDK_METRICS_EXPORT
#define OPENTELEMETRY_SDK_METRICS_EXPORT
#endif

#ifndef OPENTELEMETRY_SDK_RESOURCE_EXPORT
#define OPENTELEMETRY_SDK_RESOURCE_EXPORT
#endif

#ifndef OPENTELEMETRY_SDK_TRACE_EXPORT
#define OPENTELEMETRY_SDK_TRACE_EXPORT
#endif

#ifndef OPENTELEMETRY_EXPORTERS_ELASTICSEARCH_EXPORT
#define OPENTELEMETRY_EXPORTERS_ELASTICSEARCH_EXPORT
#endif

#ifndef OPENTELEMETRY_EXPORTERS_ETW_EXPORT
#define OPENTELEMETRY_EXPORTERS_ETW_EXPORT
#endif

#ifndef OPENTELEMETRY_EXPORTERS_MEMORY_EXPORT
#define OPENTELEMETRY_EXPORTERS_MEMORY_EXPORT
#endif

#ifndef OPENTELEMETRY_EXPORTERS_OSTREAM_EXPORT
#define OPENTELEMETRY_EXPORTERS_OSTREAM_EXPORT
#endif

#ifndef OPENTELEMETRY_EXPORTERS_OTLP_EXPORT
#define OPENTELEMETRY_EXPORTERS_OTLP_EXPORT
#endif

#ifndef OPENTELEMETRY_EXPORTERS_PROMETHEUS_EXPORT
#define OPENTELEMETRY_EXPORTERS_PROMETHEUS_EXPORT
#endif

#ifndef OPENTELEMETRY_EXPORTERS_ZIPKIN_EXPORT
#define OPENTELEMETRY_EXPORTERS_ZIPKIN_EXPORT
#endif

#ifndef OPENTELEMETRY_EXT_HTTP_CLIENT_CURL_EXPORT
#define OPENTELEMETRY_EXT_HTTP_CLIENT_CURL_EXPORT
#endif

#ifndef OPENTELEMETRY_EXT_HTTP_CLIENT_EXPORT
#define OPENTELEMETRY_EXT_HTTP_CLIENT_EXPORT
#endif

#ifndef OPENTELEMETRY_EXT_ZPAGES_EXPORT
#define OPENTELEMETRY_EXT_ZPAGES_EXPORT
#endif
