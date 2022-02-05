// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/aggregation/histogram_aggregation.h"
#  include "opentelemetry/version.h"

#  include <mutex>
OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

LongHistogramAggregation::LongHistogramAggregation()
    : boundaries_{0l, 5l, 10l, 25l, 50l, 75l, 100l, 250l, 500l, 1000l},
      counts_(boundaries_.size() + 1, 0),
      sum_(0l),
      count_(0)
{}

void LongHistogramAggregation::Aggregate(long value, const PointAttributes &attributes) noexcept
{
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  count_ += 1;
  sum_ += value;
  for (auto it = boundaries_.begin(); it != boundaries_.end(); ++it)
  {
    if (value < *it)
    {
      counts_[std::distance(boundaries_.begin(), it)] += 1;
      return;
    }
  }
}

PointType LongHistogramAggregation::Collect() noexcept
{
  HistogramPointData point_data;
  auto epoch_nanos = std::chrono::system_clock::now();
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  PopulateHistogramDataPoint<long>(point_data, epoch_nanos, sum_, count_, counts_, boundaries_);
  return point_data;
}

DoubleHistogramAggregation::DoubleHistogramAggregation()
    : boundaries_{0.0, 5.0, 10.0, 25.0, 50.0, 75.0, 100.0, 250.0, 500.0, 1000.0},
      counts_(boundaries_.size() + 1, 0),
      sum_(0.0),
      count_(0)
{}

void DoubleHistogramAggregation::Aggregate(double value, const PointAttributes &attributes) noexcept
{
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  count_ += 1;
  sum_ += value;
  for (auto it = boundaries_.begin(); it != boundaries_.end(); ++it)
  {
    if (value < *it)
    {
      counts_[std::distance(boundaries_.begin(), it)] += 1;
      return;
    }
  }
}

PointType DoubleHistogramAggregation::Collect() noexcept
{
  HistogramPointData point_data;
  auto epoch_nanos = std::chrono::system_clock::now();
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  PopulateHistogramDataPoint<double>(point_data, epoch_nanos, sum_, count_, counts_, boundaries_);
  return point_data;
}

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif