// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "opentelemetry/context/propagation/text_map_propagator.h"
#include "opentelemetry/sdk/configuration/configuration.h"
#include "opentelemetry/sdk/configuration/registry.h"
#include "opentelemetry/sdk/logs/logger_provider.h"
#include "opentelemetry/sdk/metrics/meter_provider.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

class ConfiguredSdk
{
public:
  static std::unique_ptr<ConfiguredSdk> Create(
      std::shared_ptr<Registry> registry,
      const std::unique_ptr<opentelemetry::sdk::configuration::Configuration> &model);

  ConfiguredSdk(
      opentelemetry::sdk::resource::Resource &&resource,
      const std::shared_ptr<opentelemetry::sdk::trace::TracerProvider> &tracer_provider,
      const std::shared_ptr<opentelemetry::context::propagation::TextMapPropagator> &propagator,
      const std::shared_ptr<opentelemetry::sdk::metrics::MeterProvider> &meter_provider,
      const std::shared_ptr<opentelemetry::sdk::logs::LoggerProvider> &logger_provider)
      : resource_(std::move(resource)),
        tracer_provider_(tracer_provider),
        propagator_(propagator),
        meter_provider_(meter_provider),
        logger_provider_(logger_provider)
  {}

  void Init();
  void Cleanup();

private:
  opentelemetry::sdk::resource::Resource resource_;
  std::shared_ptr<opentelemetry::sdk::trace::TracerProvider> tracer_provider_;
  std::shared_ptr<opentelemetry::context::propagation::TextMapPropagator> propagator_;
  std::shared_ptr<opentelemetry::sdk::metrics::MeterProvider> meter_provider_;
  std::shared_ptr<opentelemetry::sdk::logs::LoggerProvider> logger_provider_;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
