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
// YAML-NODE: explicit_bucket_histogram
class ExplicitBucketHistogramAggregationConfiguration : public AggregationConfiguration
{
public:
  ExplicitBucketHistogramAggregationConfiguration()           = default;
  ~ExplicitBucketHistogramAggregationConfiguration() override = default;

  std::vector<double> boundaries;
  bool record_min_max{false};
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
