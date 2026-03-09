// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <vector>

#include "opentelemetry/sdk/configuration/experimental_tracer_config_configuration.h"
#include "opentelemetry/sdk/configuration/experimental_tracer_matcher_and_config_configuration.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

// YAML-SCHEMA: schema/tracer_provider.json
// YAML-NODE: ExperimentalTracerConfigurator
class ExperimentalTracerConfiguratorConfiguration
{
public:
  ExperimentalTracerConfigConfiguration default_config;
  std::vector<ExperimentalTracerMatcherAndConfigConfiguration> tracers;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
