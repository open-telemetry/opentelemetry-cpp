// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>
#include <string>

#include "opentelemetry/sdk/configuration/attribute_limits_configuration.h"
#include "opentelemetry/sdk/configuration/document.h"
#include "opentelemetry/sdk/configuration/logger_provider_configuration.h"
#include "opentelemetry/sdk/configuration/meter_provider_configuration.h"
#include "opentelemetry/sdk/configuration/propagator_configuration.h"
#include "opentelemetry/sdk/configuration/resource_configuration.h"
#include "opentelemetry/sdk/configuration/tracer_provider_configuration.h"
#include "opentelemetry/version.h"

/*
 * General notes about configuration classes.
 *
 * Each Yaml node that exists in the yaml schema,
 * as defined by https://github.com/open-telemetry/opentelemetry-configuration
 * is represented by a C++ class.
 * Special comments are used to relate the C++ class to
 * the Yaml node it represents.
 *
 * YAML-SCHEMA: points to the relevant file within the
 * opentelemetry-configuration repository
 * YAML-NODE: points to the relevant node within the file.
 *
 * For example,
 *   C++ class opentelemetry::sdk::configuration::Configuration
 * corresponds to
 *   Yaml node OpenTelemetryConfiguration,
 * in file
 * https://github.com/open-telemetry/opentelemetry-configuration/blob/main/schema/opentelemetry_configuration.json
 */

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

// YAML-SCHEMA: schema/opentelemetry_configuration.json
// YAML-NODE: OpenTelemetryConfiguration
class Configuration
{
public:
  Configuration(std::unique_ptr<Document> doc) : doc_(std::move(doc)) {}
  ~Configuration() = default;

  std::string file_format;
  bool disabled{false};
  std::string log_level;

  std::unique_ptr<AttributeLimitsConfiguration> attribute_limits;
  std::unique_ptr<LoggerProviderConfiguration> logger_provider;
  std::unique_ptr<MeterProviderConfiguration> meter_provider;
  std::unique_ptr<PropagatorConfiguration> propagator;
  std::unique_ptr<TracerProviderConfiguration> tracer_provider;
  std::unique_ptr<ResourceConfiguration> resource;
  // Ignored: instrumentation

private:
  std::unique_ptr<Document> doc_;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
