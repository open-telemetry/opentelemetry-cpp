// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <vector>

#include "opentelemetry/sdk/configuration/experimental_logger_config_configuration.h"
#include "opentelemetry/sdk/configuration/experimental_logger_matcher_and_config_configuration.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

// YAML-SCHEMA: schema/logger_provider.json
// YAML-NODE: ExperimentalLoggerConfigurator
class ExperimentalLoggerConfiguratorConfiguration
{
public:
  ExperimentalLoggerConfigConfiguration default_config;
  std::vector<ExperimentalLoggerMatcherAndConfigConfiguration> loggers;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
