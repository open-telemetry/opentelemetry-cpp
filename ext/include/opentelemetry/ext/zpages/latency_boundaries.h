#pragma once

// include libraries
#include <chrono>
#include <array>

using std::chrono::nanoseconds;
using std::chrono::microseconds;
using std::chrono::milliseconds;
using std::chrono::seconds;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace ext
{
namespace zpages
{
/**
 * kLatencyBoundaries is a constant array that contains the 9 latency boundaries and enables them to be iterated over
 * Each value in the array represents the lower limit(inclusive) of the boundary(in nano seconds) and the upper limit(exclusive)
 * of the boundary is the lower limit of the next one. The upper limit of the last boundary is INF.
 */
const std::array<nanoseconds,9> kLatencyBoundaries = {
  nanoseconds(0),
  nanoseconds(microseconds(10)),
  nanoseconds(microseconds(100)), 
  nanoseconds(milliseconds(1)),
  nanoseconds(milliseconds(10)),
  nanoseconds(milliseconds(100)),
  nanoseconds(seconds(1)), 
  nanoseconds(seconds(10)),
  nanoseconds(seconds(100)),
};

/**
 * LatencyBoundaryName enum is used to index into the kLatencyBoundaries container.
 * Using this enum lets you access the latency boundary at each index without using magic numbers
 */
enum LatencyBoundaryName
{
  k0MicroTo10Micro,
  k10MicroTo100Micro,
  k100MicroTo1Milli,
  k1MilliTo10Milli,
  k10MilliTo100Milli,
  k100MilliTo1Second,
  k1SecondTo10Second,
  k10SecondTo100Second,
  k100SecondToMax
};

/** Overlaoding ++ operator for easy iteration **/
LatencyBoundaryName& operator ++ (LatencyBoundaryName& latencyBoundaryName)
{
    if (latencyBoundaryName == LatencyBoundaryName::k100SecondToMax) {
        throw std::out_of_range("for E& operator ++ (E&)");
    }
    latencyBoundaryName = LatencyBoundaryName(static_cast<std::underlying_type<LatencyBoundaryName>::type>(latencyBoundaryName) + 1);
    return latencyBoundaryName;
}

}  // namespace zpages
}  // namespace ext
OPENTELEMETRY_END_NAMESPACE
