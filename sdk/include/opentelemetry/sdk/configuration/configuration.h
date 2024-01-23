// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>

#include "opentelemetry/sdk/configuration/attribute_limit_configuration.h"
#include "opentelemetry/sdk/configuration/logger_provider_configuration.h"
#include "opentelemetry/sdk/configuration/meter_provider_configuration.h"
#include "opentelemetry/sdk/configuration/propagator_configuration.h"
#include "opentelemetry/sdk/configuration/resource_configuration.h"
#include "opentelemetry/sdk/configuration/tracer_provider_configuration.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

class Configuration
{
public:
  std::string file_format;
  bool disabled{false};

  std::unique_ptr<AttributeLimitConfiguration> attribute_limits;
  std::unique_ptr<LoggerProviderConfiguration> logger_provider;
  std::unique_ptr<MeterProviderConfiguration> meter_provider;
  std::unique_ptr<PropagatorConfiguration> propagator;
  std::unique_ptr<TracerProviderConfiguration> tracer_provider;
  std::unique_ptr<ResourceConfiguration> resource;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
