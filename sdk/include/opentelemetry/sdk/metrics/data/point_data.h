// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/common/timestamp.h"
#  include "opentelemetry/nostd/variant.h"
#  include "opentelemetry/sdk/metrics/instruments.h"
#  include "opentelemetry/version.h"

#  include <vector>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

using ValueType = nostd::variant<long, double>;
using ListType  = nostd::variant<std::vector<long>, std::vector<double>>;

class SumPointData
{
public:
  opentelemetry::common::SystemTimestamp start_epoch_nanos_;
  opentelemetry::common::SystemTimestamp end_epoch_nanos_;
  ValueType value_;
  AggregationTemporarily aggregation_temporarily_;
  bool is_monotonic_;
};

class LastValuePointData
{
public:
  opentelemetry::common::SystemTimestamp epoch_nanos_;
  bool is_lastvalue_valid_;
  ValueType value_;
};

class HistogramPointData
{
public:
  opentelemetry::common::SystemTimestamp epoch_nanos_;
  ListType boundaries_;
  ValueType sum_;
  std::vector<uint64_t> counts_;
  uint64_t count_;
};

class DropPointData
{};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif