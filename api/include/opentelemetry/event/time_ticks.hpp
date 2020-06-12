#pragma once

#include "opentelemetry/version.h"

#include <cstdint>
#include <ctime>

OPENTELEMETRY_BEGIN_NAMESPACE

namespace event
{

/// <summary>
/// The number of ticks per second.
/// </summary>
const uint64_t ticksPerSecond = 10000000UL;

/// <summary>
/// The UNIX epoch: Thursday, January, 01, 1970, 12:00:00 AM.
/// </summary>
const uint64_t ticksUnixEpoch = 0x089f7ff5f7b58000;

/// <summary>
/// The time_ticks structure encapsulates time in .NET ticks:
/// https://docs.microsoft.com/en-us/dotnet/api/system.datetime.ticks?view=netframework-4.8
/// </summary>
/// <remarks>
/// A single tick represents one hundred nanoseconds, or one ten-millionth of a second.
/// There are 10,000 ticks in a millisecond, or 10 million ticks in a second.
/// The value of this property represents the number of 100 nanosecond intervals that have
/// elapsed since 12:00 AM, January, 1, 0001 (0:00 : 00 UTC on January 1, 0001, in
/// the Gregorian calendar), which represents DateTime.MinValue.
/// <b>Note:</b> This does not include the number  of ticks that are attributable to leap seconds.
/// </remarks>
struct time_ticks
{
  /// <summary>
  /// A raw 64-bit unsigned integer that represents the number of .NET ticks.
  /// </summary>
  uint64_t ticks;

  /// <summary>
  /// The default constructor for instantiating an empty time_ticks object.
  /// </summary>
  time_ticks() : ticks(0) {};

  /// <summary>
  /// Converts the number of .NET ticks into an instance of the time_ticks structure.
  /// </summary>
  time_ticks(uint64_t raw) : ticks(raw) {};

  /// <summary>
  /// Constructs a time_ticks object from a pointer to a time_t object from the standard library.
  /// <b>Note:</b> time_t time must contain a timestamp in UTC time.
  /// </summary>
  time_ticks(const std::time_t *time) : ticks(ticksUnixEpoch + ticksPerSecond * ((uint64_t)(*time))) {};

  /// <summary>
  /// The time_ticks copy constructor.
  /// </summary>
  time_ticks(const time_ticks &t) : ticks(t.ticks) {};
};

} // namespace event

OPENTELEMETRY_END_NAMESPACE
