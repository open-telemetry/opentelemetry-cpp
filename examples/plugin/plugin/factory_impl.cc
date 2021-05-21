/*
 * Copyright The OpenTelemetry Authors
 * SPDX-License-Identifier: Apache-2.0
 */

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
