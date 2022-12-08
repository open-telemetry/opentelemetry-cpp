// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/common/spin_lock_mutex.h"
#include "opentelemetry/sdk/metrics/aggregation/aggregation.h"

#include <mutex>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

class LongSumAggregation : public Aggregation
{
public:
  LongSumAggregation(bool is_monotonic);
  LongSumAggregation(SumPointData &&);
  LongSumAggregation(const SumPointData &);

  void Aggregate(int64_t value, const PointAttributes &attributes = {}) noexcept override;

  void Aggregate(double /* value */, const PointAttributes & /* attributes */) noexcept override {}

  std::unique_ptr<Aggregation> Merge(const Aggregation &delta) const noexcept override;

  std::unique_ptr<Aggregation> Diff(const Aggregation &next) const noexcept override;

  PointType ToPoint() const noexcept override;

private:
  mutable opentelemetry::common::SpinLockMutex lock_;
  SumPointData point_data_;
};

class DoubleSumAggregation : public Aggregation
{
public:
  DoubleSumAggregation(bool is_monotonic);
  DoubleSumAggregation(SumPointData &&);
  DoubleSumAggregation(const SumPointData &);

  void Aggregate(int64_t /* value */, const PointAttributes & /* attributes */) noexcept override {}

  void Aggregate(double value, const PointAttributes &attributes = {}) noexcept override;

  std::unique_ptr<Aggregation> Merge(const Aggregation &delta) const noexcept override;

  std::unique_ptr<Aggregation> Diff(const Aggregation &next) const noexcept override;

  PointType ToPoint() const noexcept override;

private:
  mutable opentelemetry::common::SpinLockMutex lock_;
  SumPointData point_data_;
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
