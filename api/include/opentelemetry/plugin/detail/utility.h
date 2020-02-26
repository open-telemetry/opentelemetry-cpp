#pragma once

#include <memory>
#include <string>

#include "opentelemetry/nostd/string_view.h"

namespace opentelemetry
{
namespace plugin
{
namespace detail
{
inline void CopyErrorMessage(const char *source, std::string &destination) noexcept
try
{
  if (source == nullptr)
  {
    return;
  }
  destination.assign(source);
}
catch (const std::bad_alloc &)
{}
}  // namespace detail
}  // namespace plugin
}  // namespace opentelemetry
