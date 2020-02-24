#pragma once

#include <memory>

#include "opentelemetry/plugin/detail/utility.h"
#include "opentelemetry/plugin/factory.h"
#include "opentelemetry/plugin/hook.h"
#include "opentelemetry/version.h"

#include <windows.h>

namespace opentelemetry
{
namespace plugin
{
class DynamicLibraryHandleWindows final : public DynamicLibraryHandle
{
public:
  explicit DynamicLibraryHandleWindows(HINSTANCE handle) : handle_{handle} {}

  ~DynamicLibraryHandleWindows() override { ::FreeLibrary(handle_); }

private:
  HINSTANCE handle_;
};

inline std::unique_ptr<Factory> LoadFactory(const char *plugin, std::string &error_message) noexcept
{
  auto handle = ::LoadLibrary(plugin);
  if (handle == nullptr)
  {
    // TODO: Set error_message
    detail::CopyErrorMessage("Failed to load plugin", error_message);
    return nullptr;
  }

  std::shared_ptr<DynamicLibraryHandle> library_handle{new (std::nothrow)
                                                           DynamicLibraryHandleWindows{handle}};
  if (library_handle == nullptr)
  {
    detail::CopyErrorMessage("Allocation failure", error_message);
    return nullptr;
  }

  auto make_factory_impl = reinterpret_cast<OpenTelemetryHook *>(
      ::GetProcAddress(handle, "OpenTelemetryMakeFactoryImpl"));
  if (make_factory_impl == nullptr)
  {
    // TODO: set error_message
    detail::CopyErrorMessage("Failed find hook", error_message);
    return nullptr;
  }
  if (*make_factory_impl == nullptr)
  {
    detail::CopyErrorMessage("Invalid plugin hook", error_message);
    return nullptr;
  }

  LoaderInfo loader_info;
  nostd::unique_ptr<char[]> plugin_error_message;
  auto factory_impl = (**make_factory_impl)(loader_info, plugin_error_message);
  if (factory_impl == nullptr)
  {
    detail::CopyErrorMessage(plugin_error_message.get(), error_message);
    return nullptr;
  }
  return std::unique_ptr<Factory>{new (std::nothrow)
                                      Factory{std::move(library_handle), std::move(factory_impl)}};
}
}  // namespace plugin
}  // namespace opentelemetry
