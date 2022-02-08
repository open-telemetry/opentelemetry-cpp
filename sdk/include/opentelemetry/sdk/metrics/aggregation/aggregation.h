// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/nostd/string_view.h"
#  include "opentelemetry/sdk/metrics/data/metric_data.h"
#  include "opentelemetry/sdk/metrics/data/point_data.h"
OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
class InstrumentMonotonicityAwareAggregation
{
public:
  InstrumentMonotonicityAwareAggregation(bool is_monotonic) : is_monotonic_(is_monotonic) {}
  bool IsMonotonic() { return is_monotonic_; }

private:
  bool is_monotonic_;
};

class Aggregation
{
public:
  virtual void Aggregate(long value, const PointAttributes &attributes = {}) noexcept = 0;

  virtual void Aggregate(double value, const PointAttributes &attributes = {}) noexcept = 0;

  virtual PointType Collect() noexcept = 0;
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
