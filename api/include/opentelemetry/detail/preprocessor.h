// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

// NOTE - code within detail namespace implements internal details, and not part
// of the public interface.

#pragma once

#define OPENTELEMETRY_STRINGIFY(S) OPENTELEMETRY_STRINGIFY_(S)
#define OPENTELEMETRY_STRINGIFY_(S) #S

#define OPENTELEMETRY_CONCAT(A, B) OPENTELEMETRY_CONCAT_(A, B)
#define OPENTELEMETRY_CONCAT_(A, B) A##B

// Import the C++20 feature-test macros
#ifdef __has_include
#  if __has_include(<version>)
#    include <version>
#  endif
#elif defined(_MSC_VER) && ((defined(__cplusplus) && __cplusplus >= 202002L) || \
                            (defined(_MSVC_LANG) && _MSVC_LANG >= 202002L))
#  if _MSC_VER >= 1922
#    include <version>
#  endif
#endif
