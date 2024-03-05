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

class ExplicitBucketHistogramAggregationConfiguration : public AggregationConfiguration
{
public:
  ExplicitBucketHistogramAggregationConfiguration()           = default;
  ~ExplicitBucketHistogramAggregationConfiguration() override = default;

  std::vector<double> boundaries;
  bool record_min_max;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
