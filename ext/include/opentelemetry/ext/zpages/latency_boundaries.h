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

#include <array>
#include <chrono>

#include "opentelemetry/version.h"

using std::chrono::microseconds;
using std::chrono::milliseconds;
using std::chrono::nanoseconds;
using std::chrono::seconds;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace ext
{
namespace zpages
{
/**
 * kLatencyBoundaries is a constant array that contains the 9 latency
 * boundaries. Each value in the array represents the lower limit(inclusive) of
 * the boundary(in nano seconds) and the upper limit(exclusive) of the boundary
 * is the lower limit of the next one. The upper limit of the last boundary is
 * INF.
 */
const std::array<nanoseconds, 9> kLatencyBoundaries = {
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
 * LatencyBoundary enum is used to index into the kLatencyBoundaries container.
 * Using this enum lets you access the latency boundary at each index without
 * using magic numbers
 */
enum LatencyBoundary
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

}  // namespace zpages
}  // namespace ext
OPENTELEMETRY_END_NAMESPACE
