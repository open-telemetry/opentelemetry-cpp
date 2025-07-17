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

  ConfiguredSdk() : resource(opentelemetry::sdk::resource::Resource::GetEmpty()) {}

  void Install();
  void UnInstall();

  opentelemetry::sdk::resource::Resource resource;
  std::shared_ptr<opentelemetry::sdk::trace::TracerProvider> tracer_provider;
  std::shared_ptr<opentelemetry::context::propagation::TextMapPropagator> propagator;
  std::shared_ptr<opentelemetry::sdk::metrics::MeterProvider> meter_provider;
  std::shared_ptr<opentelemetry::sdk::logs::LoggerProvider> logger_provider;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
