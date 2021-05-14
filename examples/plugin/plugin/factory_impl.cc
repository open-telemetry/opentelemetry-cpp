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

#include "opentelemetry/plugin/factory.h"
#include "opentelemetry/plugin/hook.h"
#include "tracer.h"

class TracerHandle final : public opentelemetry::plugin::TracerHandle
{
public:
  explicit TracerHandle(std::shared_ptr<Tracer> &&tracer) noexcept : tracer_{std::move(tracer)} {}

  // opentelemetry::plugin::TracerHandle
  Tracer &tracer() const noexcept override { return *tracer_; }

private:
  std::shared_ptr<Tracer> tracer_;
};

class FactoryImpl final : public opentelemetry::plugin::Factory::FactoryImpl
{
public:
  // opentelemetry::plugin::Factory::FactoryImpl
  opentelemetry::nostd::unique_ptr<opentelemetry::plugin::TracerHandle> MakeTracerHandle(
      opentelemetry::nostd::string_view tracer_config,
      opentelemetry::nostd::unique_ptr<char[]> &error_message) const noexcept override
  {
    std::shared_ptr<Tracer> tracer{new (std::nothrow) Tracer{""}};
    if (tracer == nullptr)
    {
      return nullptr;
    }
    return opentelemetry::nostd::unique_ptr<TracerHandle>{new (std::nothrow)
                                                              TracerHandle{std::move(tracer)}};
  }
};

static opentelemetry::nostd::unique_ptr<opentelemetry::plugin::Factory::FactoryImpl>
MakeFactoryImpl(const opentelemetry::plugin::LoaderInfo &loader_info,
                opentelemetry::nostd::unique_ptr<char[]> &error_message) noexcept
{
  (void)loader_info;
  (void)error_message;
  return opentelemetry::nostd::unique_ptr<opentelemetry::plugin::Factory::FactoryImpl>{
      new (std::nothrow) FactoryImpl{}};
}

OPENTELEMETRY_DEFINE_PLUGIN_HOOK(MakeFactoryImpl);
