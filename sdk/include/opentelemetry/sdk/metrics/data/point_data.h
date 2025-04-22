// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <vector>

#include "opentelemetry/common/timestamp.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/sdk/metrics/data/circular_buffer.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

using ValueType = nostd::variant<int64_t, double>;

// TODO: remove ctors and initializers from below classes when GCC<5 stops
// shipping on Ubuntu

class SumPointData
{
public:
  // TODO: remove ctors and initializers when GCC<5 stops shipping on Ubuntu
  SumPointData(SumPointData &&)            = default;
  SumPointData(const SumPointData &)       = default;
  SumPointData &operator=(SumPointData &&) = default;
  SumPointData()                           = default;

  ValueType value_   = {};
  bool is_monotonic_ = true;
};

class LastValuePointData
{
public:
  // TODO: remove ctors and initializers when GCC<5 stops shipping on Ubuntu
  LastValuePointData(LastValuePointData &&)            = default;
  LastValuePointData(const LastValuePointData &)       = default;
  LastValuePointData &operator=(LastValuePointData &&) = default;
  LastValuePointData()                                 = default;

  ValueType value_                                  = {};
  bool is_lastvalue_valid_                          = {};
  opentelemetry::common::SystemTimestamp sample_ts_ = {};
};

class HistogramPointData
{
public:
  // TODO: remove ctors and initializers when GCC<5 stops shipping on Ubuntu
  HistogramPointData(HistogramPointData &&)            = default;
  HistogramPointData &operator=(HistogramPointData &&) = default;
  HistogramPointData(const HistogramPointData &)       = default;
  HistogramPointData()                                 = default;
  HistogramPointData(std::vector<double> &boundaries) : boundaries_(boundaries) {}
  std::vector<double> boundaries_ = {};
  ValueType sum_                  = {};
  ValueType min_                  = {};
  ValueType max_                  = {};
  std::vector<uint64_t> counts_   = {};
  uint64_t count_                 = {};
  bool record_min_max_            = true;
};

class Base2ExponentialHistogramPointData
{
public:
  // Delete copy constructor and copy assignment operator
  Base2ExponentialHistogramPointData(const Base2ExponentialHistogramPointData &) = delete;
  Base2ExponentialHistogramPointData &operator=(const Base2ExponentialHistogramPointData &) =
      delete;

  // Default move constructor and move assignment operator
  Base2ExponentialHistogramPointData(Base2ExponentialHistogramPointData &&) noexcept = default;
  Base2ExponentialHistogramPointData &operator=(Base2ExponentialHistogramPointData &&) noexcept =
      default;

  // Default constructor
  Base2ExponentialHistogramPointData() = default;

  double sum_            = {};
  double min_            = {};
  double max_            = {};
  double zero_threshold_ = {};
  uint64_t count_        = {};
  uint64_t zero_count_   = {};

  std::unique_ptr<AdaptingCircularBufferCounter> positive_buckets_ =
      std::make_unique<AdaptingCircularBufferCounter>(0);
  std::unique_ptr<AdaptingCircularBufferCounter> negative_buckets_ =
      std::make_unique<AdaptingCircularBufferCounter>(0);

  size_t max_buckets_  = {};
  int32_t scale_       = {};
  bool record_min_max_ = true;
};

class DropPointData
{
public:
  // TODO: remove ctors and initializers when GCC<5 stops shipping on Ubuntu
  DropPointData(DropPointData &&)            = default;
  DropPointData(const DropPointData &)       = default;
  DropPointData()                            = default;
  DropPointData &operator=(DropPointData &&) = default;
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
