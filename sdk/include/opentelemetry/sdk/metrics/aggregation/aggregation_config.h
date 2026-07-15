// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstdint>
#include <vector>

#include "opentelemetry/sdk/metrics/instruments.h"
#include "opentelemetry/sdk/metrics/state/attributes_hashmap.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

class AggregationConfig
{
public:
  AggregationConfig(size_t cardinality_limit = kAggregationCardinalityLimit)
      : cardinality_limit_(cardinality_limit)
  {}

  AggregationConfig(const AggregationConfig &)            = default;
  AggregationConfig(AggregationConfig &&)                 = default;
  AggregationConfig &operator=(const AggregationConfig &) = default;
  AggregationConfig &operator=(AggregationConfig &&)      = default;

  virtual AggregationType GetType() const noexcept { return AggregationType::kDefault; }

  static const AggregationConfig *GetOrDefault(const AggregationConfig *config)
  {
    if (config)
    {
      return config;
    }
    static const AggregationConfig default_config{};
    return &default_config;
  }

  size_t cardinality_limit_;
  virtual ~AggregationConfig() = default;
};

static constexpr size_t kBase2ExponentialHistogramDefaultMaxSize = 160;
static constexpr size_t kBase2ExponentialHistogramMinSize = 2;
static constexpr size_t kBase2ExponentialHistogramMaxSize = 1024;
static constexpr int32_t kBase2ExponentialHistogramMinScale = -10;
static constexpr int32_t kBase2ExponentialHistogramMaxScale = 20;

class HistogramAggregationConfig : public AggregationConfig
{
public:
  HistogramAggregationConfig(size_t cardinality_limit = kAggregationCardinalityLimit)
      : AggregationConfig(cardinality_limit)
  {}

  AggregationType GetType() const noexcept override { return AggregationType::kHistogram; }

  std::vector<double> boundaries_;
  bool record_min_max_ = true;
};

class Base2ExponentialHistogramAggregationConfig : public AggregationConfig
{
public:
  Base2ExponentialHistogramAggregationConfig(
      size_t cardinality_limit = kAggregationCardinalityLimit)
      : AggregationConfig(cardinality_limit)
  {}

  AggregationType GetType() const noexcept override
  {
    return AggregationType::kBase2ExponentialHistogram;
  }

  size_t max_size_     = kBase2ExponentialHistogramDefaultMaxSize;
  int32_t max_scale_   = kBase2ExponentialHistogramMaxScale;
  bool record_min_max_ = true;
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
