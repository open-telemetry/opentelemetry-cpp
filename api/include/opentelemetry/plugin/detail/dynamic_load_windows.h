#pragma once

#include <memory>

#include "opentelemetry/plugin/detail/utility.h"
#include "opentelemetry/plugin/factory.h"
#include "opentelemetry/plugin/hook.h"
#include "opentelemetry/version.h"

namespace opentelemetry
{
namespace plugin
{
inline std::unique_ptr<Factory> LoadFactory(const char *plugin,
                                            std::unique_ptr<char[]> &error_message) noexcept
{
  detail::CopyErrorMessage("not supported yet", error_message);
  return nullptr;
}
}  // namespace plugin
}  // namespace opentelemetry
