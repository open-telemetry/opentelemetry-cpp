#pragma once

#include <memory>

namespace opentelemetry
{
namespace plugin
{
inline std::unique_ptr<Factory> LoadFactory(const char *plugin,
                                            std::unique_ptr<char[]> &error_message) noexcept
{
  (void)plugin;
  (void)error_message;
  return nullptr;
}
}  // namespace plugin
}  // namespace opentelemetry
