// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/context/propagation/text_map_propagator.h"
#include "opentelemetry/sdk/configuration/configuration.h"
#include "opentelemetry/sdk/init/registry.h"
#include "opentelemetry/sdk/logs/logger_provider.h"
#include "opentelemetry/sdk/metrics/meter_provider.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace init
{

class ConfiguredSdk
{
public:
  static std::unique_ptr<ConfiguredSdk> Create(
      std::shared_ptr<Registry> registry,
      const std::unique_ptr<opentelemetry::sdk::configuration::Configuration> &model);

  void Install();
  void UnInstall();

  std::shared_ptr<opentelemetry::sdk::trace::TracerProvider> m_tracer_provider;
  std::shared_ptr<opentelemetry::context::propagation::TextMapPropagator> m_propagator;
  std::shared_ptr<opentelemetry::sdk::metrics::MeterProvider> m_meter_provider;
  std::shared_ptr<opentelemetry::sdk::logs::LoggerProvider> m_logger_provider;
};

}  // namespace init
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
