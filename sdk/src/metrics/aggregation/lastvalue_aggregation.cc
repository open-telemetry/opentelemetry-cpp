// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <chrono>
#include <cstdint>
#include <memory>
#include <mutex>

#include "opentelemetry/common/spin_lock_mutex.h"
#include "opentelemetry/common/timestamp.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/metrics/aggregation/aggregation.h"
#include "opentelemetry/sdk/metrics/aggregation/lastvalue_aggregation.h"
#include "opentelemetry/sdk/metrics/data/metric_data.h"
#include "opentelemetry/sdk/metrics/data/point_data.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

LongLastValueAggregation::LongLastValueAggregation()
{
  point_data_.is_lastvalue_valid_ = false;
  point_data_.value_              = static_cast<int64_t>(0);
}

LongLastValueAggregation::LongLastValueAggregation(const LastValuePointData &data)
    : point_data_{data}
{}

void LongLastValueAggregation::Aggregate(int64_t value,
                                         const PointAttributes & /* attributes */) noexcept
{
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  point_data_.is_lastvalue_valid_ = true;
  point_data_.value_              = value;
  point_data_.sample_ts_          = std::chrono::system_clock::now();
}

std::unique_ptr<Aggregation> LongLastValueAggregation::Merge(
    const Aggregation &delta) const noexcept
{
  auto curr_point  = ToPoint();
  auto delta_point = delta.ToPoint();

  const auto *curr_data  = nostd::get_if<LastValuePointData>(&curr_point);
  const auto *delta_data = nostd::get_if<LastValuePointData>(&delta_point);
  if (curr_data == nullptr || delta_data == nullptr)
  {
    OTEL_INTERNAL_LOG_ERROR("LongLastValueAggregation::Merge - Loss of type");
    return std::unique_ptr<Aggregation>(new LongLastValueAggregation());
  }

  if (curr_data->sample_ts_.time_since_epoch() > delta_data->sample_ts_.time_since_epoch())
  {
    return std::unique_ptr<Aggregation>(new LongLastValueAggregation(*curr_data));
  }
  else
  {
    return std::unique_ptr<Aggregation>(new LongLastValueAggregation(*delta_data));
  }
}

std::unique_ptr<Aggregation> LongLastValueAggregation::Diff(const Aggregation &next) const noexcept
{
  auto curr_point = ToPoint();
  auto next_point = next.ToPoint();

  const auto *curr_data = nostd::get_if<LastValuePointData>(&curr_point);
  const auto *next_data = nostd::get_if<LastValuePointData>(&next_point);
  if (curr_data == nullptr || next_data == nullptr)
  {
    OTEL_INTERNAL_LOG_ERROR("LongLastValueAggregation::Diff - Loss of type");
    return std::unique_ptr<Aggregation>(new LongLastValueAggregation());
  }

  if (curr_data->sample_ts_.time_since_epoch() > next_data->sample_ts_.time_since_epoch())
  {
    return std::unique_ptr<Aggregation>(new LongLastValueAggregation(*curr_data));
  }
  else
  {
    return std::unique_ptr<Aggregation>(new LongLastValueAggregation(*next_data));
  }
}

PointType LongLastValueAggregation::ToPoint() const noexcept
{
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  return point_data_;
}

DoubleLastValueAggregation::DoubleLastValueAggregation()
{
  point_data_.is_lastvalue_valid_ = false;
  point_data_.value_              = 0.0;
}

DoubleLastValueAggregation::DoubleLastValueAggregation(const LastValuePointData &data)
    : point_data_{data}
{}

void DoubleLastValueAggregation::Aggregate(double value,
                                           const PointAttributes & /* attributes */) noexcept
{
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  point_data_.is_lastvalue_valid_ = true;
  point_data_.value_              = value;
  point_data_.sample_ts_          = std::chrono::system_clock::now();
}

std::unique_ptr<Aggregation> DoubleLastValueAggregation::Merge(
    const Aggregation &delta) const noexcept
{
  auto curr_point  = ToPoint();
  auto delta_point = delta.ToPoint();

  const auto *curr_data  = nostd::get_if<LastValuePointData>(&curr_point);
  const auto *delta_data = nostd::get_if<LastValuePointData>(&delta_point);
  if (curr_data == nullptr || delta_data == nullptr)
  {
    OTEL_INTERNAL_LOG_ERROR("DoubleLastValueAggregation::Merge - Loss of type");
    return std::unique_ptr<Aggregation>(new DoubleLastValueAggregation());
  }

  if (curr_data->sample_ts_.time_since_epoch() > delta_data->sample_ts_.time_since_epoch())
  {
    return std::unique_ptr<Aggregation>(new DoubleLastValueAggregation(*curr_data));
  }
  else
  {
    return std::unique_ptr<Aggregation>(new DoubleLastValueAggregation(*delta_data));
  }
}

std::unique_ptr<Aggregation> DoubleLastValueAggregation::Diff(
    const Aggregation &next) const noexcept
{
  auto curr_point = ToPoint();
  auto next_point = next.ToPoint();

  const auto *curr_data = nostd::get_if<LastValuePointData>(&curr_point);
  const auto *next_data = nostd::get_if<LastValuePointData>(&next_point);
  if (curr_data == nullptr || next_data == nullptr)
  {
    OTEL_INTERNAL_LOG_ERROR("DoubleLastValueAggregation::Diff - Loss of type");
    return std::unique_ptr<Aggregation>(new DoubleLastValueAggregation());
  }

  if (curr_data->sample_ts_.time_since_epoch() > next_data->sample_ts_.time_since_epoch())
  {
    return std::unique_ptr<Aggregation>(new DoubleLastValueAggregation(*curr_data));
  }
  else
  {
    return std::unique_ptr<Aggregation>(new DoubleLastValueAggregation(*next_data));
  }
}

PointType DoubleLastValueAggregation::ToPoint() const noexcept
{
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  return point_data_;
}
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
