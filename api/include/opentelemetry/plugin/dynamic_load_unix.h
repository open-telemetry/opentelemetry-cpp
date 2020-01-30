#pragma once

#include <algorithm>
#include <memory>

#include <dlfcn.h>

#include "opentelemetry/plugin/factory.h"
#include "opentelemetry/plugin/hook.h"
#include "opentelemetry/version.h"

namespace opentelemetry
{
namespace plugin
{
namespace detail
{
inline void CopyErrorMessage(nostd::string_view source,
                             std::unique_ptr<char[]> destination) noexcept
{
  destination.reset(new char[source.size()]);
  if (destination == nullptr)
  {
    return;
  }
  std::copy(source.begin(), source.end(), destination.get());
}
}  // namespace detail

class DynamicLibraryHandleUnix final : public DynamicLibraryHandle
{
public:
  explicit DynamicLibraryHandleUnix(void *handle) noexcept : handle_{handle} {}

  ~DynamicLibraryHandle() override { ::dlclose(handle_); }

private:
  void *handle_;
};

inline std::unique_ptr<Factory> LoadFactory(nostd::string_view plugin,
                                            std::unique_ptr<char[]> &error_message) noexcept
{
  dlerror();  // Clear any existing error.

  auto handle = dlopen(plugin, RTLD_NOW | RTLD_LOCAL);
  if (handle == nullptr)
  {
    detail::CopyErrorMessage(dlerror(), error_message);
    return nullptr;
  }

  std::shared_ptr<DynamicLibraryHandle> library_handle{new (std::nothrow)
                                                           DynamicLibraryHandleUnix{handle}};
  if (library_handle == nullptr)
  {
    return nullptr;
  }

  auto make_factory_impl =
      reinterpret_cast<OpenTelemetryHook *>(::dlsym(handle, #OPENTELEMETRY_PLUGIN_HOOK));
  if (make_factory_impl == nullptr)
  {
    detail::CopyErrorMessage(dlerror(), error_message);
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
    error_message = std::move(plugin_error_message);
    return nullptr;
  }
  return std::unique_ptr<Factory>{new (std::nothrow)
                                      Factory{std::move(library_handle), std::move(factory_impl)}};
}
}  // namespace plugin
}  // namespace opentelemetry
