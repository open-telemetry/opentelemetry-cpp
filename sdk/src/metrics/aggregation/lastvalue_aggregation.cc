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

LongLastValueAggregation::LongLastValueAggregation() : is_lastvalue_valid_(false) {}

void LongLastValueAggregation::Aggregate(long value, const PointAttributes &attributes) noexcept
{
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  is_lastvalue_valid_ = true;
  value_              = value;
}

PointType LongLastValueAggregation::Collect() noexcept
{
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  if (!is_lastvalue_valid_)
  {
    return LastValuePointData{
        opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now()), false, 0l};
  }
  return LastValuePointData{
      opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now()), true, value_};
}

DoubleLastValueAggregation::DoubleLastValueAggregation() : is_lastvalue_valid_(false) {}

void DoubleLastValueAggregation::Aggregate(double value, const PointAttributes &attributes) noexcept
{
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  is_lastvalue_valid_ = true;
  value_              = value;
}

PointType DoubleLastValueAggregation::Collect() noexcept
{
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  if (!is_lastvalue_valid_)
  {
    return LastValuePointData{
        opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now()), false, 0.0};
  }
  return LastValuePointData{
      opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now()), true, value_};
}
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif