// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>
#include <limits>
#include <vector>

#include "opentelemetry/sdk/metrics/instruments.h"
#include "opentelemetry/sdk/metrics/state/attributes_hashmap.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

// Sentinel passed to AggregationConfig's cardinality_limit constructor argument to mean "the
// caller did not specify a limit", distinct from any real limit value (which is always a small
// positive count). Lets the constructor tell apart e.g. HistogramAggregationConfig() built only
// to carry boundaries_ from HistogramAggregationConfig(500) built to set a real limit of 500,
// without needing <optional> (avoided elsewhere in the SDK for ABI reasons).
constexpr size_t kCardinalityLimitUnspecified = (std::numeric_limits<size_t>::max)();

class AggregationConfig
{
public:
  AggregationConfig(size_t cardinality_limit = kCardinalityLimitUnspecified)
      : cardinality_limit_(cardinality_limit == kCardinalityLimitUnspecified
                               ? kAggregationCardinalityLimit
                               : cardinality_limit),
        cardinality_limit_explicit_(cardinality_limit != kCardinalityLimitUnspecified)
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
  // Whether cardinality_limit_ reflects an intentionally-configured value, as opposed to just
  // the compiled-in default it was left at because this config was constructed for some other
  // reason (e.g. histogram boundaries) without a cardinality_limit argument. Derived from
  // whether the constructor's cardinality_limit argument was kCardinalityLimitUnspecified, so
  // this is correct for every construction path (tests, programmatic API, SdkBuilder) without
  // each caller having to set it manually. A MetricReader-level fallback applies whenever this
  // is false.
  bool cardinality_limit_explicit_;
  virtual ~AggregationConfig() = default;
};

class HistogramAggregationConfig : public AggregationConfig
{
public:
  HistogramAggregationConfig(size_t cardinality_limit = kCardinalityLimitUnspecified)
      : AggregationConfig(cardinality_limit)
  {}

  AggregationType GetType() const noexcept override { return AggregationType::kHistogram; }

  // The SDK-specified default bucket boundaries, used when no boundaries are configured.
  static const std::vector<double> &DefaultBoundaries()
  {
    static const std::vector<double> boundaries = {0.0,    5.0,    10.0,   25.0,   50.0,
                                                   75.0,   100.0,  250.0,  500.0,  750.0,
                                                   1000.0, 2500.0, 5000.0, 7500.0, 10000.0};
    return boundaries;
  }

  std::vector<double> boundaries_;
  bool record_min_max_ = true;
};

// Valid ranges per the declarative configuration schema; the schema defines no maximum for
// max_size.
// https://github.com/open-telemetry/opentelemetry-configuration/blob/main/schema/meter_provider.yaml
constexpr std::int32_t kMaxScaleMin = -10;
constexpr std::int32_t kMaxScaleMax = 20;
constexpr std::size_t kMaxSizeMin   = 2;

class Base2ExponentialHistogramAggregationConfig : public AggregationConfig
{
public:
  Base2ExponentialHistogramAggregationConfig(
      size_t cardinality_limit = kCardinalityLimitUnspecified)
      : AggregationConfig(cardinality_limit)
  {}

  AggregationType GetType() const noexcept override
  {
    return AggregationType::kBase2ExponentialHistogram;
  }

  size_t max_size_     = 160;
  int32_t max_scale_   = 20;
  bool record_min_max_ = true;
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
