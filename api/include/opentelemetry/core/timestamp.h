#pragma once

#include <algorithm>
#include <chrono>
#include <cstdint>

namespace opentelemetry
{
namespace core
{
/**
 * Timestamp contains a time point measured from the system or steady clock or both.
 */
class Timestamp
{
public:
  Timestamp() noexcept {}

  Timestamp(const std::chrono::system_clock::time_point &system_time_point,
            const std::chrono::steady_clock::time_point &steady_time_point) noexcept
      : system_time_point_{ConvertTimePoint(system_time_point)},
        steady_time_point_{ConvertTimePoint(steady_time_point)}
  {}

  /*implicit*/ Timestamp(const std::chrono::system_clock::time_point &system_time_point) noexcept
      : Timestamp{system_time_point, std::chrono::steady_clock::time_point{}}
  {}

  /*implicit*/ Timestamp(const std::chrono::steady_clock::time_point &steady_time_point) noexcept
      : Timestamp{std::chrono::system_clock::time_point{}, steady_time_point}
  {}

  /**
   * @return the timestamp from the system clock
   */
  std::chrono::system_clock::time_point system_timestamp() const noexcept
  {
    return std::chrono::system_clock::time_point{
        std::chrono::duration_cast<std::chrono::system_clock::duration>(
            std::chrono::nanoseconds{system_time_point_})};
  }

  /**
   * @return the timestamp of the steady clock
   */
  std::chrono::steady_clock::time_point steady_timestamp() const noexcept
  {
    return std::chrono::steady_clock::time_point{
        std::chrono::duration_cast<std::chrono::steady_clock::duration>(
            std::chrono::nanoseconds{steady_time_point_})};
  }

  /**
   * @return true if Timestamp contains a system timestamp
   */
  bool HasSystemTimestamp() const noexcept { return system_time_point_ != 0; }

  /**
   * @return true if Timestamp contains a steady timestamp
   */
  bool HasSteadyTimestamp() const noexcept { return steady_time_point_ != 0; }

private:
  uint64_t system_time_point_{0};
  uint64_t steady_time_point_{0};

  template <class Clock, class Duration>
  static uint64_t ConvertTimePoint(
      const std::chrono::time_point<Clock, Duration> &timestamp) noexcept
  {
    auto time_since_epoch =
        std::chrono::duration_cast<std::chrono::nanoseconds>(timestamp.time_since_epoch());
    time_since_epoch = std::max(std::chrono::nanoseconds::zero(), time_since_epoch);
    return static_cast<uint64_t>(time_since_epoch.count());
  }
};
}  // namespace core
}  // namespace opentelemetry
