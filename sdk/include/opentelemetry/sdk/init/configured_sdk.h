// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/context/propagation/text_map_propagator.h"
#include "opentelemetry/sdk/configuration/configuration.h"
#include "opentelemetry/sdk/init/registry.h"
#include "opentelemetry/trace/tracer_provider.h"
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

  // FIXME: should be sdk::trace::TracerProvider
  std::shared_ptr<opentelemetry::trace::TracerProvider> m_tracer_provider;
  std::shared_ptr<opentelemetry::context::propagation::TextMapPropagator> m_propagator;
};

}  // namespace init
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
