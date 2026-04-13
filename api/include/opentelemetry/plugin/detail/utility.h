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
#if OPENTELEMETRY_HAVE_EXCEPTIONS
  try
#endif
  {
    if (source == nullptr)
    {
      return;
    }
    destination.assign(source);
  }
#if OPENTELEMETRY_HAVE_EXCEPTIONS
  catch (...)
  {
    return;
  }
#endif
}
}  // namespace detail
}  // namespace plugin
OPENTELEMETRY_END_NAMESPACE
