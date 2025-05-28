// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "opentelemetry/sdk/configuration/aggregation_configuration.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

// YAML-SCHEMA: schema/meter_provider.json
// YAML-NODE: ViewStream
class ViewStreamConfiguration
{
public:
  ViewStreamConfiguration()  = default;
  ~ViewStreamConfiguration() = default;

  std::string name;
  std::string description;
  std::unique_ptr<AggregationConfiguration> aggregation;
  size_t aggregation_cardinality_limit;
  std::vector<std::string> attribute_keys;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
