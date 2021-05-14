// Copyright 2021, OpenTelemetry Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <algorithm>
#include <chrono>
#include <cstdint>

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace common
{
/**
 * @brief A timepoint relative to the system clock epoch.
 *
 * This is used for marking the beginning and end of an operation.
 */
class SystemTimestamp
{
public:
  /**
   * @brief Initializes a system timestamp pointing to the start of the epoch.
   */
  SystemTimestamp() noexcept : nanos_since_epoch_{0} {}

  /**
   * @brief Initializes a system timestamp from a duration.
   *
   * @param time_since_epoch Time elapsed since the beginning of the epoch.
   */
  template <class Rep, class Period>
  explicit SystemTimestamp(const std::chrono::duration<Rep, Period> &time_since_epoch) noexcept
      : nanos_since_epoch_{static_cast<int64_t>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(time_since_epoch).count())}
  {}

  /**
   * @brief Initializes a system timestamp based on a point in time.
   *
   * @param time_point A point in time.
   */
  /*implicit*/ SystemTimestamp(const std::chrono::system_clock::time_point &time_point) noexcept
      : SystemTimestamp{time_point.time_since_epoch()}
  {}

  /**
   * @brief Returns a time point for the time stamp.
   *
   * @return A time point corresponding to the time stamp.
   */
  operator std::chrono::system_clock::time_point() const noexcept
  {
    return std::chrono::system_clock::time_point{
        std::chrono::duration_cast<std::chrono::system_clock::duration>(
            std::chrono::nanoseconds{nanos_since_epoch_})};
  }

  /**
   * @brief Returns the nanoseconds since the beginning of the epoch.
   *
   * @return Elapsed nanoseconds since the beginning of the epoch for this timestamp.
   */
  std::chrono::nanoseconds time_since_epoch() const noexcept
  {
    return std::chrono::nanoseconds{nanos_since_epoch_};
  }

  /**
   * @brief Compare two steady time stamps.
   *
   * @return true if the two time stamps are equal.
   */
  bool operator==(const SystemTimestamp &other) const noexcept
  {
    return nanos_since_epoch_ == other.nanos_since_epoch_;
  }

  /**
   * @brief Compare two steady time stamps for inequality.
   *
   * @return true if the two time stamps are not equal.
   */
  bool operator!=(const SystemTimestamp &other) const noexcept
  {
    return nanos_since_epoch_ != other.nanos_since_epoch_;
  }

private:
  int64_t nanos_since_epoch_;
};

/**
 * @brief A timepoint relative to the monotonic clock epoch
 *
 * This is used for calculating the duration of an operation.
 */
class SteadyTimestamp
{
public:
  /**
   * @brief Initializes a monotonic timestamp pointing to the start of the epoch.
   */
  SteadyTimestamp() noexcept : nanos_since_epoch_{0} {}

  /**
   * @brief Initializes a monotonic timestamp from a duration.
   *
   * @param time_since_epoch Time elapsed since the beginning of the epoch.
   */
  template <class Rep, class Period>
  explicit SteadyTimestamp(const std::chrono::duration<Rep, Period> &time_since_epoch) noexcept
      : nanos_since_epoch_{static_cast<int64_t>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(time_since_epoch).count())}
  {}

  /**
   * @brief Initializes a monotonic timestamp based on a point in time.
   *
   * @param time_point A point in time.
   */
  /*implicit*/ SteadyTimestamp(const std::chrono::steady_clock::time_point &time_point) noexcept
      : SteadyTimestamp{time_point.time_since_epoch()}
  {}

  /**
   * @brief Returns a time point for the time stamp.
   *
   * @return A time point corresponding to the time stamp.
   */
  operator std::chrono::steady_clock::time_point() const noexcept
  {
    return std::chrono::steady_clock::time_point{
        std::chrono::duration_cast<std::chrono::steady_clock::duration>(
            std::chrono::nanoseconds{nanos_since_epoch_})};
  }

  /**
   * @brief Returns the nanoseconds since the beginning of the epoch.
   *
   * @return Elapsed nanoseconds since the beginning of the epoch for this timestamp.
   */
  std::chrono::nanoseconds time_since_epoch() const noexcept
  {
    return std::chrono::nanoseconds{nanos_since_epoch_};
  }

  /**
   * @brief Compare two steady time stamps.
   *
   * @return true if the two time stamps are equal.
   */
  bool operator==(const SteadyTimestamp &other) const noexcept
  {
    return nanos_since_epoch_ == other.nanos_since_epoch_;
  }

  /**
   * @brief Compare two steady time stamps for inequality.
   *
   * @return true if the two time stamps are not equal.
   */
  bool operator!=(const SteadyTimestamp &other) const noexcept
  {
    return nanos_since_epoch_ != other.nanos_since_epoch_;
  }

private:
  int64_t nanos_since_epoch_;
};
}  // namespace common
OPENTELEMETRY_END_NAMESPACE
