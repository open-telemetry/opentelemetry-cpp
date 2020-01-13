// Copyright 2019, OpenTelemetry Authors
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

namespace opentelemetry
{
namespace trace
{

class SpanId final
{
public:
  // The size in bytes of the SpanId.
  static constexpr int kSize = 8;

  // An invalid SpanId (all zeros).
  SpanId() noexcept : rep_{0} {}

  // Creates a SpanId with the given ID.
  explicit SpanId(nostd::span<const uint8_t, kSize> id) noexcept { memcpy(rep_, id.data(), kSize); }

  // Populates the buffer with the hex representation of the ID.
  void ToHex(nostd::span<char, 2 * kSize> buffer) const noexcept
  {
    constexpr char kHex[] = "0123456789ABCDEF";
    for (int i = 0; i < kSize; ++i)
    {
      buffer[i * 2 + 0] = kHex[(rep_[i] >> 4) & 0xF];
      buffer[i * 2 + 1] = kHex[(rep_[i] >> 0) & 0xF];
    }
  }

  // Returns a nostd::span of the ID.
  nostd::span<const uint8_t, kSize> Id() const noexcept { return rep_; }

  bool operator==(const SpanId &that) const noexcept { return memcmp(rep_, that.rep_, kSize) == 0; }

  bool operator!=(const SpanId &that) const noexcept { return !(*this == that); }

  // Returns false if the SpanId is all zeros.
  bool IsValid() const noexcept { return *this != SpanId(); }

  // Copies the opaque SpanId data to a buffer.
  void CopyTo(nostd::span<uint8_t, kSize> dest) const noexcept { memcpy(dest.data(), rep_, kSize); }

private:
  uint8_t rep_[kSize];
};

}  // namespace trace
}  // namespace opentelemetry
