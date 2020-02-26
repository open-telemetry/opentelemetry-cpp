#pragma once

namespace opentelemetry
{
namespace plugin
{
/**
 * Manage the ownership of a dynamically loaded library.
 */
class DynamicLibraryHandle
{
public:
  virtual ~DynamicLibraryHandle() = default;
};
}  // namespace plugin
}  // namespace opentelemetry
