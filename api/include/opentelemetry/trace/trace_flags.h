// Copyright 2020, OpenTelemetry Authors
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

#include <cstdint>
#include <cstring>

#include "opentelemetry/nostd/span.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace trace
{

// TraceFlags represents options for a Trace. These options are propagated to all child Spans
// and determine features such as whether a Span should be traced. TraceFlags
// are implemented as a bitmask.
class TraceFlags final
{
public:
  static constexpr uint8_t kIsSampled = 1;
  static constexpr uint8_t kForbidden = 0xFF;

  TraceFlags() noexcept : rep_{0} {}

  explicit TraceFlags(uint8_t flags) noexcept : rep_(flags) {}

  bool IsSampled() const noexcept { return rep_ & kIsSampled; }

  bool IsValid() const noexcept { return rep_ != kForbidden; }

  // Populates the buffer with the lowercase base16 representation of the flags.
  void ToLowerBase16(nostd::span<char, 2> buffer) const noexcept
  {
    constexpr char kHex[] = "0123456789ABCDEF";
    buffer[0]             = kHex[(rep_ >> 4) & 0xF];
    buffer[1]             = kHex[(rep_ >> 0) & 0xF];
  }

  uint8_t flags() const noexcept { return rep_; }

  bool operator==(const TraceFlags &that) const noexcept { return rep_ == that.rep_; }

  bool operator!=(const TraceFlags &that) const noexcept { return !(*this == that); }

  // Copies the TraceFlags to dest.
  void CopyBytesTo(nostd::span<uint8_t, 1> dest) const noexcept { dest[0] = rep_; }

private:
  uint8_t rep_;
};

}  // namespace trace
OPENTELEMETRY_END_NAMESPACE
