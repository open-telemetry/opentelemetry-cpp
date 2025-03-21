// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <vector>

#include "opentelemetry/version.h"
#include "opentelemetry/sdk/metrics/state/attributes_hashmap.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
class AggregationConfig
{
public:
  AggregationConfig(size_t cardinality_limit = kAggregationCardinalityLimit) : cardinality_limit_(cardinality_limit) {}
  size_t cardinality_limit_;
  virtual ~AggregationConfig() = default;
};

class HistogramAggregationConfig : public AggregationConfig
{
public:
  std::vector<double> boundaries_;
  bool record_min_max_ = true;
};
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
