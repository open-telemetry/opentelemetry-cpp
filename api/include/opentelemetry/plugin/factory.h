#pragma once

#include <memory>
#include <string>

#include "opentelemetry/plugin/tracer.h"

namespace opentelemetry {
namespace plugin {
class Factory final {
 public:
   class FactoryImpl {
     public:
       virtual ~FactoryImpl() {}

       virtual nostd::unique_ptr<TracerHandle> MakeTracerHandle(
           nostd::string_view tracer_config,
           nostd::unique_ptr<char[]> &error_message) const noexcept = 0;
   };

   Factory(std::shared_ptr<DynamicLibraryHandle> library_handle,
           std::unique_ptr<FactoryImpl> &&factory_impl) noexcept
       : library_handle_{std::move(library_handle)}, factory_impl_{std::move(factory_impl)}
   {}

   std::shared_ptr<Tracer> MakeTracer(nostd::string_view tracer_config,
                                      std::unique_ptr<char[]> &error_message) const noexcept
   {
     nostd::unique_ptr<char[]> plugin_error_message;
     auto tracer_handle = factory_impl_->MakeTracerHandle(tracer_config, plugin_error_message);
     error_message = std::move(plugin_error_message);
     if (tracer_handle == nullptr)
     {
       return nullptr;
     }
     return std::shared_ptr<Tracer>{new (std::nothrow)
                                        Tracer{library_handle_, std::move(tracer_handle)}};
   }

 private:
  // Note: The order is important here.
  //
  // It's undefined behavior to close the library while a loaded FactoryImpl is still active.
   std::shared_ptr<DynamicLibraryHandle> library_handle_;
   std::unique_ptr<FactoryImpl> factory_impl_;
};
} // namespace plugin
} // namespace opentelemetry
