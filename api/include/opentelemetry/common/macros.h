// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

/*
   Expected usage pattern:

   if OPENTELEMETRY_LIKELY_CONDITION (ptr != nullptr)
   {
     do_something_likely();
   } else {
     do_something_unlikely();
   }

   This pattern works with gcc/clang and __builtin_expect(),
   as well as with C++20.
   It is unclear if __builtin_expect() will be deprecated
   in favor of C++20 [[likely]] or not.

   OPENTELEMETRY_LIKELY_CONDITION is preferred over OPENTELEMETRY_LIKELY,
   to be revisited when C++20 is required.
*/

#if !defined(OPENTELEMETRY_LIKELY_CONDITION) && defined(__cplusplus)
// Only use likely with C++20
#  if __cplusplus >= 202002L
// GCC 9 has likely attribute but do not support declare it at the beginning of statement
#    if defined(__has_cpp_attribute) && (defined(__clang__) || !defined(__GNUC__) || __GNUC__ > 9)
#      if __has_cpp_attribute(likely)
#        define OPENTELEMETRY_LIKELY_CONDITION(C) (C) [[likely]]
#      endif
#    endif
#  endif
#endif
#if !defined(OPENTELEMETRY_LIKELY_CONDITION) && (defined(__clang__) || defined(__GNUC__))
// Only use if supported by the compiler
#  define OPENTELEMETRY_LIKELY_CONDITION(C) (__builtin_expect(!!(C), true))
#endif
#ifndef OPENTELEMETRY_LIKELY_CONDITION
// Do not use likely annotations
#  define OPENTELEMETRY_LIKELY_CONDITION(C) (C)
#endif

#if !defined(OPENTELEMETRY_UNLIKELY_CONDITION) && defined(__cplusplus)
// Only use unlikely with C++20
#  if __cplusplus >= 202002L
// GCC 9 has unlikely attribute but do not support declare it at the beginning of statement
#    if defined(__has_cpp_attribute) && (defined(__clang__) || !defined(__GNUC__) || __GNUC__ > 9)
#      if __has_cpp_attribute(unlikely)
#        define OPENTELEMETRY_UNLIKELY_CONDITION(C) (C) [[unlikely]]
#      endif
#    endif
#  endif
#endif
#if !defined(OPENTELEMETRY_UNLIKELY_CONDITION) && (defined(__clang__) || defined(__GNUC__))
// Only use if supported by the compiler
#  define OPENTELEMETRY_UNLIKELY_CONDITION(C) (__builtin_expect(!!(C), false))
#endif
#ifndef OPENTELEMETRY_UNLIKELY_CONDITION
// Do not use unlikely annotations
#  define OPENTELEMETRY_UNLIKELY_CONDITION(C) (C)
#endif

/*
   Expected usage pattern:

   if (ptr != nullptr)
   OPENTELEMETRY_LIKELY
   {
     do_something_likely();
   } else {
     do_something_unlikely();
   }

   This pattern works starting with C++20.
   See https://en.cppreference.com/w/cpp/language/attributes/likely

   Please use OPENTELEMETRY_LIKELY_CONDITION instead for now.
*/

#if !defined(OPENTELEMETRY_LIKELY) && defined(__cplusplus)
// Only use likely with C++20
#  if __cplusplus >= 202002L
// GCC 9 has likely attribute but do not support declare it at the beginning of statement
#    if defined(__has_cpp_attribute) && (defined(__clang__) || !defined(__GNUC__) || __GNUC__ > 9)
#      if __has_cpp_attribute(likely)
#        define OPENTELEMETRY_LIKELY [[likely]]
#      endif
#    endif
#  endif
#endif

#ifndef OPENTELEMETRY_LIKELY
#  define OPENTELEMETRY_LIKELY
#endif

#if !defined(OPENTELEMETRY_UNLIKELY) && defined(__cplusplus)
// Only use unlikely with C++20
#  if __cplusplus >= 202002L
// GCC 9 has unlikely attribute but do not support declare it at the beginning of statement
#    if defined(__has_cpp_attribute) && (defined(__clang__) || !defined(__GNUC__) || __GNUC__ > 9)
#      if __has_cpp_attribute(unlikely)
#        define OPENTELEMETRY_UNLIKELY [[unlikely]]
#      endif
#    endif
#  endif
#endif

#ifndef OPENTELEMETRY_UNLIKELY
#  define OPENTELEMETRY_UNLIKELY
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
#  define OPENTELEMETRY_LOCAL_SYMBOL __attribute__((visibility("hidden")))

#elif defined(__GNUC__)

#  define OPENTELEMETRY_API_SINGLETON __attribute__((visibility("default")))
#  define OPENTELEMETRY_LOCAL_SYMBOL __attribute__((visibility("hidden")))

#else

/* Add support for other compilers here. */

#  define OPENTELEMETRY_API_SINGLETON
#  define OPENTELEMETRY_LOCAL_SYMBOL

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

// What follows is specific to the https://github.com/malkia/opentelemetry-cpp windows-dll branch.
// Users should not predefine OPENTELEMETRY_DLL, and it would get set to 1 ("dllimport").
// When building, using bazel --//:with_dll=true, OPENTELEMETRY_DLL is set to -1, except for tests.
// In all other cases (CMake, or bazel with_dll=false), then OPENTELEMETRY is set explicitly to 0.
#if !defined(OPENTELEMETRY_DLL)
#define OPENTELEMETRY_DLL 1
#elif OPENTELEMETRY_DLL == 1
#error OPENTELEMETRY_DLL: Users should not pre-define OPENTELEMETRY_DLL
#endif

// bazel --//:with_dll=true build only cares if OPENTELEMETRY_DLL is 1 (dllimport) or -1 (dllexport)
#if OPENTELEMETRY_DLL != 0
//  Build settings are hard-coded here, instead of the build files, in order to minimize ODR violations.
#   define OPENTELEMETRY_STL_VERSION 2017
#   define OPENTELEMETRY_OPTION_USE_STD_SPAN 0 // Use the nostd version, we don't want surprises.
#   define OPENTELEMETRY_ABI_VERSION_NO 2 // Use the new api
#   define ENABLE_METRICS_EXEMPLAR_PREVIEW 1
#   define ENABLE_ASYNC_EXPORT 1
#   define ENABLE_OTLP_GRPC_SSL_MTLS_PREVIEW 1
#   if defined(OPENTELEMETRY_BUILD_IMPORT_DLL) || defined(OPENTELEMETRY_BUILD_EXPORT_DLL)
#      error OPENTELEMETRY_DLL: Somehow CMake specific defines OPENTELEMETRY_BUILD_IMPORT_DLL and/or OPENTELEMETRY_BUILD_EXPORT_DLL got in. This is not expected!
#   endif
#   if !defined(_MSC_VER)
#      error OPENTELEMETRY_DLL: Only MSVC compiler is supported.
#   endif
#   if _MSVC_LANG < 201703L
#      error OPENTELEMETRY_DLL: Enable at least c++17 using /std:c++17 or larger
#   endif
#   if !defined(OPENTELEMETRY_RTTI_ENABLED)
#      error OPENTELEMETRY_DLL: RTTI must be enabled (/GR)
#   endif
//  We ensure that this is defined to a value of, as its gets encoded down in the detect_mismatch
#   undef OPENTELEMETRY_RTTI_ENABLED
#   define OPENTELEMETRY_RTTI_ENABLED 1
#   if defined(OPENTELEMETRY_STL_VERSION)
#      if OPENTELEMETRY_STL_VERSION != 2017
#         error OPENTELEMETRY_DLL: OPENTELEMETRY_STL_VERSION must be 2017
#      endif
#   else
#      define OPENTELEMETRY_STL_VERSION 2017
#   endif
#   if defined(OPENTELEMETRY_ABI_VERSION_NO)
#      if OPENTELEMETRY_ABI_VERSION_NO != 2
#         error OPENTELEMETRY_DLL: OPENTELEMETRY_ABI_VERSION_NO must be 2
#      endif
#   else
#      define OPENTELEMETRY_ABI_VERSION_NO 2
#   endif
#   undef OPENTELEMETRY_EXPORT
#   if OPENTELEMETRY_DLL==1
#      define OPENTELEMETRY_EXPORT __declspec(dllimport)
#   elif OPENTELEMETRY_DLL==-1 // Only used during build
#      undef OPENTELEMETRY_DLL
#      define OPENTELEMETRY_DLL 1 // this is for the detect_mismatch down below
#      define OPENTELEMETRY_EXPORT __declspec(dllexport)
#   else
#      error OPENTELEMETRY_DLL: OPENTELEMETRY_DLL must be 1 before including opentelemetry header files
#   endif
// The rule is that if there is struct/class with one or more OPENTELEMETRY_API_SINGLETON function members,
// then itself can't be defined OPENTELEMETRY_EXPORT 
#  undef OPENTELEMETRY_API_SINGLETON
#  define OPENTELEMETRY_API_SINGLETON OPENTELEMETRY_EXPORT
#  define OPENTELEMETRY_DLL_STRX(x) #x
#  define OPENTELEMETRY_DLL_STR(x) OPENTELEMETRY_DLL_STRX(x)
#  if defined(_MSC_VER)
// TODO: Revisit what's broken here
#     pragma detect_mismatch("otel_sdk_detect_mismatch", \
        "+dll:" OPENTELEMETRY_DLL_STR(OPENTELEMETRY_DLL) \
        "+stl:" OPENTELEMETRY_DLL_STR(OPENTELEMETRY_STL_VERSION) \
        "+rtti:" OPENTELEMETRY_DLL_STR(OPENTELEMETRY_RTTI_ENABLED) \
        "+std_span:" OPENTELEMETRY_DLL_STR(OPENTELEMETRY_OPTION_USE_STD_SPAN) \
        "+abi:" OPENTELEMETRY_DLL_STR(OPENTELEMETRY_ABI_VERSION_NO) \
        "+exemplar:" OPENTELEMETRY_DLL_STR(ENABLE_METRICS_EXEMPLAR_PREVIEW) \
        "+async:" OPENTELEMETRY_DLL_STR(ENABLE_ASYNC_EXPORT) \
        "+mtls:" OPENTELEMETRY_DLL_STR(ENABLE_OTLP_GRPC_SSL_MTLS_PREVIEW) \
      )
#  endif
#  undef OPENTELEMETRY_DLL_STRX
#  undef OPENTELEMETRY_DLL_STR
#endif // if OPENTELEMETRY_DLL != 0
