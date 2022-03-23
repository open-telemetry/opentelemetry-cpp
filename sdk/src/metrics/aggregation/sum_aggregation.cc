// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/aggregation/sum_aggregation.h"
#  include "opentelemetry/sdk/metrics/data/point_data.h"
#  include "opentelemetry/version.h"

#  include <iostream>
#  include <mutex>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

LongSumAggregation::LongSumAggregation()
{
  point_data_.value_ = 0l;
}

LongSumAggregation::LongSumAggregation(SumPointData &&data) : point_data_{std::move(data)} {}

void LongSumAggregation::Aggregate(long value, const PointAttributes &attributes) noexcept
{
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  point_data_.value_ = nostd::get<long>(point_data_.value_) + value;
}

std::unique_ptr<Aggregation> LongSumAggregation::Merge(Aggregation &delta) noexcept
{
  long merge_value =
      nostd::get<long>(
          nostd::get<SumPointData>((static_cast<LongSumAggregation &>(delta).ToPoint())).value_) +
      nostd::get<long>(nostd::get<SumPointData>(ToPoint()).value_);
  std::unique_ptr<Aggregation> aggr(new LongSumAggregation());
  static_cast<LongSumAggregation *>(aggr.get())->point_data_.value_ = merge_value;
  return aggr;
}

std::unique_ptr<Aggregation> LongSumAggregation::Diff(Aggregation &next) noexcept
{

  long diff_value =
      nostd::get<long>(
          nostd::get<SumPointData>((static_cast<LongSumAggregation &>(next).ToPoint())).value_) -
      nostd::get<long>(nostd::get<SumPointData>(ToPoint()).value_);
  std::unique_ptr<Aggregation> aggr(new LongSumAggregation());
  static_cast<LongSumAggregation *>(aggr.get())->point_data_.value_ = diff_value;
  return aggr;
}

PointType LongSumAggregation::ToPoint() noexcept
{
  return point_data_;
}

DoubleSumAggregation::DoubleSumAggregation()
{
  point_data_.value_ = 0.0;
}

DoubleSumAggregation::DoubleSumAggregation(SumPointData &&data) : point_data_(std::move(data)) {}

void DoubleSumAggregation::Aggregate(double value, const PointAttributes &attributes) noexcept
{
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  point_data_.value_ = nostd::get<double>(point_data_.value_) + value;
}

std::unique_ptr<Aggregation> DoubleSumAggregation::Merge(Aggregation &delta) noexcept
{
  double merge_value =
      nostd::get<double>(
          nostd::get<SumPointData>((static_cast<DoubleSumAggregation &>(delta).ToPoint())).value_) +
      nostd::get<double>(nostd::get<SumPointData>(ToPoint()).value_);
  std::unique_ptr<Aggregation> aggr(new DoubleSumAggregation());
  static_cast<DoubleSumAggregation *>(aggr.get())->point_data_.value_ = merge_value;
  return aggr;
}

std::unique_ptr<Aggregation> DoubleSumAggregation::Diff(Aggregation &next) noexcept
{

  double diff_value =
      nostd::get<double>(
          nostd::get<SumPointData>((static_cast<DoubleSumAggregation &>(next).ToPoint())).value_) -
      nostd::get<double>(nostd::get<SumPointData>(ToPoint()).value_);
  std::unique_ptr<Aggregation> aggr(new DoubleSumAggregation());
  static_cast<DoubleSumAggregation *>(aggr.get())->point_data_.value_ = diff_value;
  return aggr;
}

PointType DoubleSumAggregation::ToPoint() noexcept
{
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  return point_data_;
}

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif