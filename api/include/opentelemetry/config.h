// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <type_traits>  // IWYU pragma: keep

#if !defined(__GLIBCXX__) || (defined(_GLIBCXX_RELEASE) && _GLIBCXX_RELEASE >= 7) || \
    (defined(__GLIBCXX__) && __GLIBCXX__ >= 20150422)  // >= libstdc++-5
#  define OPENTELEMETRY_TRIVIALITY_TYPE_TRAITS
#endif
