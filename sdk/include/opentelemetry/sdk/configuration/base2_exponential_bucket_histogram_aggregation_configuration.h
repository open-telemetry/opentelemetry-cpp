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

class Base2ExponentialBucketHistogramAggregationConfiguration : public AggregationConfiguration
{
public:
  Base2ExponentialBucketHistogramAggregationConfiguration()           = default;
  ~Base2ExponentialBucketHistogramAggregationConfiguration() override = default;

  size_t max_scale;
  size_t max_size;
  bool record_min_max;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
