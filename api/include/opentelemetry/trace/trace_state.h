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
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/unique_ptr.h"

namespace opentelemetry
{
namespace trace
{

/**
 * TraceState carries tracing-system specific context in a list of key-value pairs. TraceState
 * allows different vendors to propagate additional information and inter-operate with their legacy
 * id formats.
 *
 * For more information, see the W3C Trace Context specification:
 * https://www.w3.org/TR/trace-context
 */
class TraceState
{
public:
  static constexpr int kKeyMaxSize       = 256;
  static constexpr int kValueMaxSize     = 256;
  static constexpr int kMaxKeyValuePairs = 32;

  // Class to store key-value pairs.
  class Entry
  {
  public:
    Entry() noexcept = default;

    // Copy constructor.
    Entry(const Entry &copy);

    // Creates an Entry for a given key-value pair.
    Entry(nostd::string_view key, nostd::string_view value) noexcept;

    nostd::string_view GetKey();
    nostd::string_view GetValue();

  private:
    // Store key and value as raw char pointers to avoid using std::string.
    nostd::unique_ptr<const char[]> key_;
    nostd::unique_ptr<const char[]> value_;
  };

  // An empty TraceState.
  TraceState() noexcept : num_entries_(0) {}

  // Returns false if no such key, otherwise returns true and populates value.
  bool Get(nostd::string_view key, nostd::string_view value) const noexcept { return false; }

  // Creates an Entry for the key-value pair and adds it to entries.
  // If value is null, this function is a no-op.
  void Set(nostd::string_view key, nostd::string_view value) const noexcept;

  // Returns true if there are no keys, false otherwise.
  bool Empty() const noexcept { return true; }

  // Returns a span of all the entries. The TraceState object must outlive the span.
  nostd::span<Entry> Entries() const noexcept { return {}; }

  // Returns whether key is a valid key. See https://www.w3.org/TR/trace-context/#key
  static bool IsValidKey(nostd::string_view key);

  // Returns whether value is a valid value. See https://www.w3.org/TR/trace-context/#value
  static bool IsValidValue(nostd::string_view value);

private:
  // Store entries in a C-style array to avoid using std::array or std::vector.
  Entry entries_[kMaxKeyValuePairs];

  // Maintain the number of entries in entries_. Must be in the range [0, kMaxKeyValuePairs].
  int num_entries_;
};

}  // namespace trace
}  // namespace opentelemetry
