// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/common/macros.h"
#include "opentelemetry/detail/preprocessor.h"

#if defined(OPENTELEMETRY_STL_VERSION)
#  if (!defined(_MSVC_LANG) && OPENTELEMETRY_STL_VERSION > (__cplusplus / 100)) || \
      (defined(_MSVC_LANG) && OPENTELEMETRY_STL_VERSION > (_MSVC_LANG / 100))
#    if defined(_MSVC_LANG)
#      pragma message OPENTELEMETRY_STRINGIFY( \
          OPENTELEMETRY_STL_VERSION) " vs. " OPENTELEMETRY_STRINGIFY(_MSVC_LANG)
#    else
#      pragma message OPENTELEMETRY_STRINGIFY( \
          OPENTELEMETRY_STL_VERSION) " vs. " OPENTELEMETRY_STRINGIFY(__cplusplus)
#    endif
#    error "OPENTELEMETRY_STL_VERSION is set to a version newer than the curent C++ version."
#  endif
#endif

#ifndef OPENTELEMETRY_ABI_VERSION_NO
#  define OPENTELEMETRY_ABI_VERSION_NO 1
#endif

#define OPENTELEMETRY_VERSION "1.13.0"
#define OPENTELEMETRY_VERSION_MAJOR 1
#define OPENTELEMETRY_VERSION_MINOR 13
#define OPENTELEMETRY_VERSION_PATCH 0

#define OPENTELEMETRY_ABI_VERSION OPENTELEMETRY_STRINGIFY(OPENTELEMETRY_ABI_VERSION_NO)

// clang-format off
#define OPENTELEMETRY_BEGIN_NAMESPACE \
  namespace opentelemetry { inline namespace OPENTELEMETRY_CONCAT(v, OPENTELEMETRY_ABI_VERSION_NO) {

#define OPENTELEMETRY_END_NAMESPACE \
  }}

#define OPENTELEMETRY_NAMESPACE opentelemetry :: OPENTELEMETRY_CONCAT(v, OPENTELEMETRY_ABI_VERSION_NO)

// clang-format on
