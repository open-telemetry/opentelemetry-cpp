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
static inline void PopulateSumPointData(SumPointData &sum,
                                        opentelemetry::common::SystemTimestamp start_ts,
                                        opentelemetry::common::SystemTimestamp end_ts,
                                        T value,
                                        bool is_monotonic)
{
  sum.start_epoch_nanos_       = start_ts;
  sum.end_epoch_nanos_         = end_ts;
  sum.value_                   = value;
  sum.is_monotonic_            = is_monotonic;
  sum.aggregation_temporarily_ = AggregationTemporarily::kDelta;
}

class LongSumAggregation : public Aggregation, InstrumentMonotonicityAwareAggregation
{
public:
  LongSumAggregation(bool is_monotonic);

  void Aggregate(long value, const PointAttributes &attributes = {}) noexcept override;

  void Aggregate(double value, const PointAttributes &attributes = {}) noexcept override {}

  PointType Collect() noexcept override;

private:
  opentelemetry::common::SpinLockMutex lock_;
  opentelemetry::common::SystemTimestamp start_epoch_nanos_;
  long sum_;
};

class DoubleSumAggregation : public Aggregation, InstrumentMonotonicityAwareAggregation
{
public:
  DoubleSumAggregation(bool is_monotonic);

  void Aggregate(long value, const PointAttributes &attributes = {}) noexcept override {}

  void Aggregate(double value, const PointAttributes &attributes = {}) noexcept override;

  PointType Collect() noexcept override;

private:
  opentelemetry::common::SpinLockMutex lock_;
  opentelemetry::common::SystemTimestamp start_epoch_nanos_;
  double sum_;
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif