#pragma once

#include <memory>

#include "opentelemetry/nostd/string_view.h"

namespace opentelemetry
{
namespace plugin
{
namespace detail
{
inline void CopyErrorMessage(nostd::string_view source,
                             std::unique_ptr<char[]> &destination) noexcept
{
  destination.reset(new char[source.size() + 1]);
  if (destination == nullptr)
  {
    return;
  }
  auto iter = std::copy(source.begin(), source.end(), destination.get());
  *iter     = '\0';
}
}  // namespace detail
}  // namespace plugin
}  // namespace opentelemetry
