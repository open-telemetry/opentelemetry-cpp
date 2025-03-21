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

class StreamConfiguration
{
public:
  StreamConfiguration()  = default;
  ~StreamConfiguration() = default;

  std::string name;
  std::string description;
  std::unique_ptr<AggregationConfiguration> aggregation;
  std::vector<std::string> attribute_keys;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
