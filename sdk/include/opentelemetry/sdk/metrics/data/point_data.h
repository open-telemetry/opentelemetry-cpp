// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/common/timestamp.h"
#  include "opentelemetry/nostd/variant.h"
#  include "opentelemetry/sdk/metrics/instruments.h"
#  include "opentelemetry/version.h"

#  include <list>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

using ValueType = nostd::variant<long, double>;
using ListType  = nostd::variant<std::list<long>, std::list<double>>;

class SumPointData
{
public:
  ValueType value_;
};

class LastValuePointData
{
public:
  ValueType value_;
  bool is_lastvalue_valid_;
  opentelemetry::common::SystemTimestamp sample_ts_;
};

class HistogramPointData
{
public:
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