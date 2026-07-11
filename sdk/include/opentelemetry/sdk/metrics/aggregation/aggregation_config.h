// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

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
  // Valid range for max_scale per the OTel spec.
  static constexpr std::int32_t kMinScale = -10;
  static constexpr std::int32_t kMaxScale = 20;
  // Valid range for max_buckets per the OTel spec (minimum defined; maximum is SDK-enforced).
  static constexpr std::size_t kMinBuckets     = 2;
  static constexpr std::size_t kMaxBuckets     = 32768;
  static constexpr std::size_t kDefaultBuckets = 160;

  Base2ExponentialHistogramAggregationConfig(
      size_t cardinality_limit = kAggregationCardinalityLimit)
      : AggregationConfig(cardinality_limit)
  {}

  AggregationType GetType() const noexcept override
  {
    return AggregationType::kBase2ExponentialHistogram;
  }

  std::size_t max_buckets_{kDefaultBuckets};
  std::int32_t max_scale_{kMaxScale};
  bool record_min_max_ = true;
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
