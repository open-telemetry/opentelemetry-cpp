// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/common/spin_lock_mutex.h"
#  include "opentelemetry/sdk/metrics/aggregation/aggregation.h"

#  include <mutex>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
template <class T>
static inline void PopulateHistogramDataPoint(HistogramPointData &histogram,
                                              opentelemetry::common::SystemTimestamp epoch_nanos,
                                              T sum,
                                              uint64_t count,
                                              std::vector<uint64_t> &counts,
                                              std::vector<T> boundaries)
{
  histogram.epoch_nanos_ = epoch_nanos;
  histogram.boundaries_  = boundaries;
  histogram.sum_         = sum;
  histogram.counts_      = counts;
  histogram.count_       = count;
}

class LongHistogramAggregation : public Aggregation
{
public:
  LongHistogramAggregation();

  void Aggregate(long value, const PointAttributes &attributes = {}) noexcept override;

  void Aggregate(double value, const PointAttributes &attributes = {}) noexcept override {}

  PointType Collect() noexcept override;

private:
  opentelemetry::common::SpinLockMutex lock_;
  std::vector<long> boundaries_;
  long sum_;
  std::vector<uint64_t> counts_;
  uint64_t count_;
};

class DoubleHistogramAggregation : public Aggregation
{
public:
  DoubleHistogramAggregation();

  void Aggregate(long value, const PointAttributes &attributes = {}) noexcept override {}

  void Aggregate(double value, const PointAttributes &attributes = {}) noexcept override;

  PointType Collect() noexcept override;

private:
  opentelemetry::common::SpinLockMutex lock_;
  std::vector<double> boundaries_;
  double sum_;
  std::vector<uint64_t> counts_;
  uint64_t count_;
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif