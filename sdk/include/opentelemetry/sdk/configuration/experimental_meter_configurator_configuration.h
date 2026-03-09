// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <vector>

#include "opentelemetry/sdk/configuration/experimental_meter_config_configuration.h"
#include "opentelemetry/sdk/configuration/experimental_meter_matcher_and_config_configuration.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

// YAML-SCHEMA: schema/meter_provider.yaml
// YAML-NODE: ExperimentalMeterConfigurator
class ExperimentalMeterConfiguratorConfiguration
{
public:
  ExperimentalMeterConfigConfiguration default_config;
  std::vector<ExperimentalMeterMatcherAndConfigConfiguration> meters;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
