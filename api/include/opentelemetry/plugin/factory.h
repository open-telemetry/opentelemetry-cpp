// Copyright 2021, OpenTelemetry Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once
#include <memory>
#include <string>

#include "opentelemetry/plugin/detail/utility.h"
#include "opentelemetry/plugin/tracer.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace plugin
{
/**
 * Factory creates OpenTelemetry objects from configuration strings.
 */
class Factory final
{
public:
  class FactoryImpl
  {
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

  /**
   * Construct a tracer from a configuration string.
   * @param tracer_config a representation of the tracer's config as a string.
   * @param error_message on failure this will contain an error message.
   * @return a Tracer on success or nullptr on failure.
   */
  std::shared_ptr<opentelemetry::trace::Tracer> MakeTracer(
      nostd::string_view tracer_config,
      std::string &error_message) const noexcept
  {
    nostd::unique_ptr<char[]> plugin_error_message;
    auto tracer_handle = factory_impl_->MakeTracerHandle(tracer_config, plugin_error_message);
    if (tracer_handle == nullptr)
    {
      detail::CopyErrorMessage(plugin_error_message.get(), error_message);
      return nullptr;
    }
    return std::shared_ptr<opentelemetry::trace::Tracer>{
        new (std::nothrow) Tracer{library_handle_, std::move(tracer_handle)}};
  }

private:
  // Note: The order is important here.
  //
  // It's undefined behavior to close the library while a loaded FactoryImpl is still active.
  std::shared_ptr<DynamicLibraryHandle> library_handle_;
  std::unique_ptr<FactoryImpl> factory_impl_;
};
}  // namespace plugin
OPENTELEMETRY_END_NAMESPACE
