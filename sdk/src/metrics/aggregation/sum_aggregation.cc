// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/aggregation/sum_aggregation.h"
#  include "opentelemetry/version.h"

#  include <mutex>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

LongSumAggregation::LongSumAggregation(bool is_monotonic)
    : InstrumentMonotonicityAwareAggregation(is_monotonic),
      start_epoch_nanos_(opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now())),
      sum_(0l)
{}

void LongSumAggregation::Aggregate(long value, const PointAttributes &attributes) noexcept
{
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  sum_ += value;
}

PointType LongSumAggregation::Collect() noexcept
{
  opentelemetry::common::SystemTimestamp current_ts(std::chrono::system_clock::now());
  SumPointData sum;
  {
    const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
    PopulateSumPointData<long>(sum, start_epoch_nanos_, current_ts, sum_, IsMonotonic());
    start_epoch_nanos_ = current_ts;
    sum_               = 0;
  }
  return sum;
}

DoubleSumAggregation::DoubleSumAggregation(bool is_monotonic)
    : InstrumentMonotonicityAwareAggregation(is_monotonic),
      start_epoch_nanos_(opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now())),
      sum_(0L)
{}

void DoubleSumAggregation::Aggregate(double value, const PointAttributes &attributes) noexcept
{
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  sum_ += value;
}

PointType DoubleSumAggregation::Collect() noexcept
{
  opentelemetry::common::SystemTimestamp current_ts(std::chrono::system_clock::now());
  SumPointData sum;
  {
    const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
    PopulateSumPointData<double>(sum, start_epoch_nanos_, current_ts, sum_, IsMonotonic());
    start_epoch_nanos_ = current_ts;
    sum_               = 0;
  }
  return sum;
}

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif