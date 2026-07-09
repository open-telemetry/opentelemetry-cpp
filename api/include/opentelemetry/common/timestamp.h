// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cctype>
#include <chrono>
#include <cstdint>
#include <ctime>
#include <iomanip>
#include <limits>
#include <sstream>
#include <string>

#include "opentelemetry/common/string_util.h"
#include "opentelemetry/nostd/string_view.h"
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

class DurationUtil
{
public:
  template <class Rep, class Period>
  static std::chrono::duration<Rep, Period> AdjustWaitForTimeout(
      std::chrono::duration<Rep, Period> timeout,
      std::chrono::duration<Rep, Period> indefinite_value) noexcept
  {
    // Do not call now() when this duration is max value, now() may have a expensive cost.
    if (timeout == (std::chrono::duration<Rep, Period>::max)())
    {
      return indefinite_value;
    }

    // std::future<T>::wait_for, std::this_thread::sleep_for, and std::condition_variable::wait_for
    // may use steady_clock or system_clock.We need make sure now() + timeout do not overflow.
    auto max_timeout = std::chrono::duration_cast<std::chrono::duration<Rep, Period>>(
        (std::chrono::steady_clock::time_point::max)() - std::chrono::steady_clock::now());
    if (timeout >= max_timeout)
    {
      return indefinite_value;
    }
    max_timeout = std::chrono::duration_cast<std::chrono::duration<Rep, Period>>(
        (std::chrono::system_clock::time_point::max)() - std::chrono::system_clock::now());
    if (timeout >= max_timeout)
    {
      return indefinite_value;
    }

    return timeout;
  }
};

/**
 * @brief Parsing helpers for RFC 7231 value formats (HTTP-date, section
 * 7.1.1.1, and delay-seconds, section 7.1.3) that appear in headers such
 * as Retry-After, Date, Last-Modified, and Expires.
 */
class HttpUtil
{
public:
  static bool ParseDelaySeconds(nostd::string_view value, std::chrono::seconds &delay) noexcept
  {
    value = StringUtil::Trim(value);
    if (value.empty())
    {
      return false;
    }

    std::chrono::seconds::rep result = 0;
    for (const char c : value)
    {
      if (!std::isdigit(static_cast<unsigned char>(c)))
      {
        return false;
      }
      auto digit = c - '0';
      if (result > (std::numeric_limits<std::chrono::seconds::rep>::max() - digit) / 10)
      {
        return false;
      }
      result = result * 10 + digit;
    }

    delay = std::chrono::seconds(result);
    return true;
  }

  static bool ParseHttpDate(nostd::string_view value, std::chrono::system_clock::time_point &date)
  {
    value = StringUtil::Trim(value);
    std::string str(value.data(), value.size());
    std::tm tm = {};
    std::istringstream ss(str);

    ss >> std::get_time(&tm, "%a, %d %b %Y %H:%M:%S");
    if (!ss.fail())
    {
      date = std::chrono::system_clock::from_time_t(PortableTimegm(&tm));
      return true;
    }

    ss.clear();
    ss.str(str);
    tm = {};
    ss >> std::get_time(&tm, "%A, %d-%b-%y %H:%M:%S");
    if (!ss.fail())
    {
      std::time_t now_t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
      int current_year  = 1970 + static_cast<int>(now_t / 31556952);
      int full_year     = 1900 + tm.tm_year;
      if (full_year - current_year > 50)
      {
        full_year -= 100;
      }
      tm.tm_year = full_year - 1900;
      date       = std::chrono::system_clock::from_time_t(PortableTimegm(&tm));
      return true;
    }

    ss.clear();
    ss.str(str);
    tm = {};
    ss >> std::get_time(&tm, "%a %b %d %H:%M:%S %Y");
    if (!ss.fail())
    {
      date = std::chrono::system_clock::from_time_t(PortableTimegm(&tm));
      return true;
    }

    return false;
  }

private:
  static std::time_t PortableTimegm(std::tm *tm) noexcept
  {
    int year  = tm->tm_year + 1900;
    int month = tm->tm_mon + 1;
    if (month <= 2)
    {
      year -= 1;
      month += 12;
    }
    int day  = tm->tm_mday;
    int days = 365 * year + year / 4 - year / 100 + year / 400 + (153 * (month - 3) + 2) / 5 + day -
               719469;
    return static_cast<std::time_t>(days) * 86400 + tm->tm_hour * 3600 + tm->tm_min * 60 +
           tm->tm_sec;
  }
};

}  // namespace common
OPENTELEMETRY_END_NAMESPACE
