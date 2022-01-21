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
class LongLastValueAggregation : public Aggregation
{
public:
  LongLastValueAggregation();

  void Aggregate(long value, const PointAttributes &attributes = {}) noexcept override;

  void Aggregate(double value, const PointAttributes &attributes = {}) noexcept override {}

  PointType Collect() noexcept override;

private:
  opentelemetry::common::SpinLockMutex lock_;
  long value_;
  bool is_lastvalue_valid_;
};

class DoubleLastValueAggregation : public Aggregation
{
public:
  DoubleLastValueAggregation();

  void Aggregate(long value, const PointAttributes &attributes = {}) noexcept override {}

  void Aggregate(double value, const PointAttributes &attributes = {}) noexcept override;

  PointType Collect() noexcept override;

private:
  opentelemetry::common::SpinLockMutex lock_;
  double value_;
  bool is_lastvalue_valid_;
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif