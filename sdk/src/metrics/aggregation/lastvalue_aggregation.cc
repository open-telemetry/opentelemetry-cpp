// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/aggregation/lastvalue_aggregation.h"
#  include "opentelemetry/common/timestamp.h"
#  include "opentelemetry/version.h"

#  include <mutex>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

LongLastValueAggregation::LongLastValueAggregation()
{
  point_data_.is_lastvalue_valid_ = false;
  point_data_.value_              = 0l;
}
LongLastValueAggregation::LongLastValueAggregation(LastValuePointData &&data)
    : point_data_{std::move(data)}
{}

void LongLastValueAggregation::Aggregate(long value, const PointAttributes &attributes) noexcept
{
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  point_data_.is_lastvalue_valid_ = true;
  point_data_.value_              = value;
}

std::unique_ptr<Aggregation> LongLastValueAggregation::Merge(Aggregation &delta) noexcept
{
  Aggregation &agg_to_merge =
      nostd::get<LastValuePointData>(ToPoint()).sample_ts_.time_since_epoch() >
              nostd::get<LastValuePointData>(delta.ToPoint()).sample_ts_.time_since_epoch()
          ? *this
          : delta;
  LastValuePointData merge_data = std::move(nostd::get<LastValuePointData>(agg_to_merge.ToPoint()));
  return std::unique_ptr<Aggregation>(new LongLastValueAggregation(std::move(merge_data)));
}

std::unique_ptr<Aggregation> LongLastValueAggregation::Diff(Aggregation &next) noexcept
{
  Aggregation &agg_to_merge =
      nostd::get<LastValuePointData>(ToPoint()).sample_ts_.time_since_epoch() >
              nostd::get<LastValuePointData>(next.ToPoint()).sample_ts_.time_since_epoch()
          ? *this
          : next;
  LastValuePointData diff_data = std::move(nostd::get<LastValuePointData>(agg_to_merge.ToPoint()));
  return std::unique_ptr<Aggregation>(new LongLastValueAggregation(std::move(diff_data)));
}

PointType LongLastValueAggregation::ToPoint() noexcept
{
  return point_data_;
}

DoubleLastValueAggregation::DoubleLastValueAggregation()
{
  point_data_.is_lastvalue_valid_ = false;
  point_data_.value_              = 0.0;
}
DoubleLastValueAggregation::DoubleLastValueAggregation(LastValuePointData &&data)
    : point_data_{std::move(data)}
{}

void DoubleLastValueAggregation::Aggregate(double value, const PointAttributes &attributes) noexcept
{
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  point_data_.is_lastvalue_valid_ = true;
  point_data_.value_              = value;
}

std::unique_ptr<Aggregation> DoubleLastValueAggregation::Merge(Aggregation &delta) noexcept
{
  Aggregation &agg_to_merge =
      nostd::get<LastValuePointData>(ToPoint()).sample_ts_.time_since_epoch() >
              nostd::get<LastValuePointData>(delta.ToPoint()).sample_ts_.time_since_epoch()
          ? *this
          : delta;
  LastValuePointData merge_data = std::move(nostd::get<LastValuePointData>(agg_to_merge.ToPoint()));
  return std::unique_ptr<Aggregation>(new DoubleLastValueAggregation(std::move(merge_data)));
}

std::unique_ptr<Aggregation> DoubleLastValueAggregation::Diff(Aggregation &next) noexcept
{
  Aggregation &agg_to_merge =
      nostd::get<LastValuePointData>(ToPoint()).sample_ts_.time_since_epoch() >
              nostd::get<LastValuePointData>(next.ToPoint()).sample_ts_.time_since_epoch()
          ? *this
          : next;
  LastValuePointData diff_data = std::move(nostd::get<LastValuePointData>(agg_to_merge.ToPoint()));
  return std::unique_ptr<Aggregation>(new DoubleLastValueAggregation(std::move(diff_data)));
}

PointType DoubleLastValueAggregation::ToPoint() noexcept
{
  return point_data_;
}
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif