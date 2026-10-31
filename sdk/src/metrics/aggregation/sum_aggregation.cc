// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <cstdint>
#include <memory>
#include <mutex>
#include <ostream>

#include "opentelemetry/common/spin_lock_mutex.h"
#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/metrics/aggregation/aggregation.h"
#include "opentelemetry/sdk/metrics/aggregation/sum_aggregation.h"
#include "opentelemetry/sdk/metrics/data/metric_data.h"
#include "opentelemetry/sdk/metrics/data/point_data.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

LongSumAggregation::LongSumAggregation(bool is_monotonic)
{
  point_data_.value_        = static_cast<int64_t>(0);
  point_data_.is_monotonic_ = is_monotonic;
}

LongSumAggregation::LongSumAggregation(const LongSumPointData &data) : point_data_{data} {}

void LongSumAggregation::Aggregate(int64_t value, const PointAttributes & /* attributes */) noexcept
{
  if (point_data_.is_monotonic_ && value < 0)
  {
    OTEL_INTERNAL_LOG_WARN(
        " LongSumAggregation::Aggregate Negative value ignored for Monotonic increasing "
        "measurement. Value"
        << value);
    return;
  }
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  point_data_.value_ = point_data_.value_ + value;
}

std::unique_ptr<Aggregation> LongSumAggregation::Merge(const Aggregation &delta) const noexcept
{
  auto delta_sum = static_cast<const LongSumAggregation &>(delta).ToPointData();
  auto curr_sum  = ToPointData();

  const auto delta_val = delta_sum.value_;
  const auto curr_val  = curr_sum.value_;

  std::unique_ptr<Aggregation> aggr(new LongSumAggregation(point_data_.is_monotonic_));
  static_cast<LongSumAggregation *>(aggr.get())->point_data_.value_ = delta_val + curr_val;
  return aggr;
}

std::unique_ptr<Aggregation> LongSumAggregation::Diff(const Aggregation &next) const noexcept
{
  auto next_sum = static_cast<const LongSumAggregation &>(next).ToPointData();
  auto curr_sum = ToPointData();

  const auto next_val = next_sum.value_;
  const auto curr_val = curr_sum.value_;

  std::unique_ptr<Aggregation> aggr(new LongSumAggregation(point_data_.is_monotonic_));
  static_cast<LongSumAggregation *>(aggr.get())->point_data_.value_ = next_val - curr_val;
  return aggr;
}

PointType LongSumAggregation::ToPoint() const noexcept
{
  // Wrap in a PointType variant
  return ToPointData();
}

LongSumPointData LongSumAggregation::ToPointData() const noexcept
{
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  return point_data_;
}

DoubleSumAggregation::DoubleSumAggregation(bool is_monotonic)
{
  point_data_.value_        = 0.0;
  point_data_.is_monotonic_ = is_monotonic;
}

DoubleSumAggregation::DoubleSumAggregation(const DoubleSumPointData &data) : point_data_(data) {}

void DoubleSumAggregation::Aggregate(double value,
                                     const PointAttributes & /* attributes */) noexcept
{
  if (point_data_.is_monotonic_ && value < 0)
  {
    OTEL_INTERNAL_LOG_WARN(
        " DoubleSumAggregation::Aggregate Negative value ignored for Monotonic increasing "
        "measurement. Value"
        << value);
    return;
  }
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  point_data_.value_ = point_data_.value_ + value;
}

std::unique_ptr<Aggregation> DoubleSumAggregation::Merge(const Aggregation &delta) const noexcept
{
  auto delta_sum = static_cast<const DoubleSumAggregation &>(delta).ToPointData();
  auto curr_sum  = ToPointData();

  const auto delta_val = delta_sum.value_;
  const auto curr_val  = curr_sum.value_;

  std::unique_ptr<Aggregation> aggr(new DoubleSumAggregation(point_data_.is_monotonic_));
  static_cast<DoubleSumAggregation *>(aggr.get())->point_data_.value_ = delta_val + curr_val;
  return aggr;
}

std::unique_ptr<Aggregation> DoubleSumAggregation::Diff(const Aggregation &next) const noexcept
{
  auto next_sum = static_cast<const DoubleSumAggregation &>(next).ToPointData();
  auto curr_sum = ToPointData();

  const auto next_val = next_sum.value_;
  const auto curr_val = curr_sum.value_;

  std::unique_ptr<Aggregation> aggr(new DoubleSumAggregation(point_data_.is_monotonic_));
  static_cast<DoubleSumAggregation *>(aggr.get())->point_data_.value_ = next_val - curr_val;
  return aggr;
}

PointType DoubleSumAggregation::ToPoint() const noexcept
{
  // Wrap in a PointType variant
  return ToPointData();
}

DoubleSumPointData DoubleSumAggregation::ToPointData() const noexcept
{
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  return point_data_;
}

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
