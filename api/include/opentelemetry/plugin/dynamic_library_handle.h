#pragma once

namespace opentelemetry
{
namespace plugin
{
class DynamicLibraryHandle
{
public:
  virtual ~DynamicLibraryHandle() = default;
};
}  // namespace plugin
}  // namespace opentelemetry
