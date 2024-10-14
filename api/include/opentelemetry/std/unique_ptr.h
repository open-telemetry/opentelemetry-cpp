// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

// IWYU pragma: private, include "opentelemetry/nostd/unique_ptr.h"

#include <memory>  // IWYU pragma: export

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
// Standard Type aliases in nostd namespace
namespace nostd
{

// nostd::unique_ptr<T...>
template <class... _Types>
using unique_ptr = std::unique_ptr<_Types...>;

#if (defined(__cplusplus) && __cplusplus >= 201402L) || \
    (defined(_MSVC_LANG) && _MSVC_LANG >= 201402L)
using std::make_unique;
#else
template <class T, class... Args>
unique_ptr<T> make_unique(Args &&...args)
{
  return unique_ptr<T>(new T(std::forward<Args>(args)...));
}
#endif

}  // namespace nostd
OPENTELEMETRY_END_NAMESPACE
