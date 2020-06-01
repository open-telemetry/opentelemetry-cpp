#pragma once

#include <algorithm>
#include <chrono>
#include <cstdint>

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace core
{
/**
 * Represents a timepoint relative to the system clock epoch
 */
class SystemTimestamp
{
public:
  SystemTimestamp() noexcept : nanos_since_epoch_{0} {}

  template <class Rep, class Period>
  explicit SystemTimestamp(const std::chrono::duration<Rep, Period> &time_since_epoch) noexcept
      : nanos_since_epoch_{static_cast<int64_t>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(time_since_epoch).count())}
  {}

  /*implicit*/ SystemTimestamp(const std::chrono::system_clock::time_point &time_point) noexcept
      : SystemTimestamp{time_point.time_since_epoch()}
  {}

  operator std::chrono::system_clock::time_point() const noexcept
  {
    return std::chrono::system_clock::time_point{
        std::chrono::duration_cast<std::chrono::system_clock::duration>(
            std::chrono::nanoseconds{nanos_since_epoch_})};
  }

  /**
   * @return the amount of time that has passed since the system clock epoch
   */
  std::chrono::nanoseconds time_since_epoch() const noexcept
  {
    return std::chrono::nanoseconds{nanos_since_epoch_};
  }

  bool operator==(const SystemTimestamp &other) const noexcept
  {
    return nanos_since_epoch_ == other.nanos_since_epoch_;
  }

private:
  int64_t nanos_since_epoch_;
};

/**
 * Represents a timepoint relative to the monotonic clock epoch
 */
class SteadyTimestamp
{
public:
  SteadyTimestamp() noexcept : nanos_since_epoch_{0} {}

  template <class Rep, class Period>
  explicit SteadyTimestamp(const std::chrono::duration<Rep, Period> &time_since_epoch) noexcept
      : nanos_since_epoch_{static_cast<int64_t>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(time_since_epoch).count())}
  {}

  /*implicit*/ SteadyTimestamp(const std::chrono::steady_clock::time_point &time_point) noexcept
      : SteadyTimestamp{time_point.time_since_epoch()}
  {}

  operator std::chrono::steady_clock::time_point() const noexcept
  {
    return std::chrono::steady_clock::time_point{
        std::chrono::duration_cast<std::chrono::steady_clock::duration>(
            std::chrono::nanoseconds{nanos_since_epoch_})};
  }

  /**
   * @return the amount of time that has passed since the monotonic clock epoch
   */
  std::chrono::nanoseconds time_since_epoch() const noexcept
  {
    return std::chrono::nanoseconds{nanos_since_epoch_};
  }

  bool operator==(const SteadyTimestamp &other) const noexcept
  {
    return nanos_since_epoch_ == other.nanos_since_epoch_;
  }

private:
  int64_t nanos_since_epoch_;
};
}  // namespace core
OPENTELEMETRY_END_NAMESPACE
