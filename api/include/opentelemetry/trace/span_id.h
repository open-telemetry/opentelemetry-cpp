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
#include <string>

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
  SpanId() : rep_{0} {}

  // Creates a SpanId by copying the first kSize bytes from the buffer.
  explicit SpanId(const uint8_t *buf) { memcpy(rep_, buf, kSize); }

  // Returns a 16-char hex string of the SpanId value.
  std::string ToHex() const
  {
    constexpr char kHex[] = "0123456789ABCDEF";
    std::string s(kSize * 2, ' ');
    for (int i = 0; i < kSize; ++i)
    {
      s[i * 2 + 0] = kHex[(rep_[i] >> 4) & 0xF];
      s[i * 2 + 1] = kHex[(rep_[i] >> 0) & 0xF];
    }
    return s;
  }

private:
  uint8_t rep_[kSize];
};

}  // namespace trace
}  // namespace opentelemetry
