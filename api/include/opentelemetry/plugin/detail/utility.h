// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/version.h"

#include <string>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace plugin
{
namespace detail
{
inline void CopyErrorMessage(const char *source, std::string &destination) noexcept
{
  OPENTELEMETRY_TRY
  {
    if (source == nullptr)
    {
      return;
    }
    destination.assign(source);
  }
  OPENTELEMETRY_CATCH_ALL
  {
    return;
  }
}
}  // namespace detail
}  // namespace plugin
OPENTELEMETRY_END_NAMESPACE
