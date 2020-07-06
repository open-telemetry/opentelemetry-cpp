#pragma once

// include libraries
#include <chrono>
#include <climits>
#include <string>
#include <unordered_map>
#include <vector>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace ext
{
namespace zpages
{

/**
 * Latency_Boundary_Name enum is used to index into the Latency_Boundaries vector that is declared later on,
 * using this enum lets you access the Latecy_Boundary at each index without using magic numbers
 */
typedef enum LatencyBoundaryName
{
  k0MicroTo10Micro = 0,
  k10MicroTo100Micro,
  k100MicroTo1Milli,
  k1MilliTo10Milli,
  k10MilliTo100Milli,
  k100MilliTo1Second,
  k1SecondTo10Second,
  k10SecondTo100Second,
  k100SecondToMax
} LatencyBoundaryName;

/**
 * kLatency_Boundaries constant that contains the 9 latency boundaries and enables them to be iterated over
 */
const std::vector<std::chrono::nanoseconds> kLatencyBoundaries = {
    std::chrono::nanoseconds(0),
    std::chrono::nanoseconds(std::chrono::microseconds(10)),
    std::chrono::nanoseconds(std::chrono::microseconds(100)), 
    std::chrono::nanoseconds(std::chrono::milliseconds(1)),
    std::chrono::nanoseconds(std::chrono::milliseconds(10)),
    std::chrono::nanoseconds(std::chrono::milliseconds(100)),
    std::chrono::nanoseconds(std::chrono::seconds(1)), 
    std::chrono::nanoseconds(std::chrono::seconds(10)),
    std::chrono::nanoseconds(std::chrono::seconds(100)),
  };

/*
 * Number of latency boundaries
 */
const int kNumberOfLatencyBoundaries = 9;

}  // namespace zpages
}  // namespace ext
OPENTELEMETRY_END_NAMESPACE
