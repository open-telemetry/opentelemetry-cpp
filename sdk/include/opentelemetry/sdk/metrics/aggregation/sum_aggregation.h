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

class LongSumAggregation : public Aggregation
{
public:
  LongSumAggregation();
  LongSumAggregation(SumPointData &&);

  void Aggregate(long value, const PointAttributes &attributes = {}) noexcept override;

  void Aggregate(double value, const PointAttributes &attributes = {}) noexcept override {}

  virtual std::unique_ptr<Aggregation> Merge(Aggregation &delta) noexcept override;

  virtual std::unique_ptr<Aggregation> Diff(Aggregation &next) noexcept override;

  PointType ToPoint() noexcept override;

private:
  opentelemetry::common::SpinLockMutex lock_;
  SumPointData point_data_;
};

class DoubleSumAggregation : public Aggregation
{
public:
  DoubleSumAggregation();
  DoubleSumAggregation(SumPointData &&);

  void Aggregate(long value, const PointAttributes &attributes = {}) noexcept override {}

  void Aggregate(double value, const PointAttributes &attributes = {}) noexcept override;

  virtual std::unique_ptr<Aggregation> Merge(Aggregation &delta) noexcept override;

  virtual std::unique_ptr<Aggregation> Diff(Aggregation &next) noexcept override;

  PointType ToPoint() noexcept override;

private:
  opentelemetry::common::SpinLockMutex lock_;
  SumPointData point_data_;
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif