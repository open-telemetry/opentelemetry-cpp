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
    Entry() : key_(nullptr), value_(nullptr){};

    // Copy constructor
    Entry(Entry &copy)
    {
      key_   = StringToPointer(nostd::string_view(copy.key_.get()));
      value_ = StringToPointer(nostd::string_view(copy.value_.get()));
    }

    // Assignment operator
    Entry &operator=(Entry &other)
    {
      key_   = StringToPointer(nostd::string_view(other.key_.get()));
      value_ = StringToPointer(nostd::string_view(other.value_.get()));
      return *this;
    }

    // Create an Entry for a given key-value pair.
    Entry(nostd::string_view key, nostd::string_view value) noexcept
    {
      key_   = StringToPointer(key);
      value_ = StringToPointer(value);
    }

    // Get the key associated with this entry.
    nostd::string_view GetKey() { return nostd::string_view(key_.get()); }

    // Get the value associated with this entry.
    nostd::string_view GetValue() { return nostd::string_view(value_.get()); }

    // Set the value for this entry. This overrides the previous value.
    void SetValue(nostd::string_view value) { value_ = StringToPointer(value); }

  private:
    // Store key and value as raw char pointers to avoid using std::string.
    nostd::unique_ptr<const char[]> key_;
    nostd::unique_ptr<const char[]> value_;

    // Copy string into a buffer and return a unique_ptr to the buffer.
    // This is a workaround for the fact that strcpy doesn't accept a const char* destination.
    nostd::unique_ptr<const char[]> StringToPointer(nostd::string_view str)
    {
      nostd::unique_ptr<char[]> temp(new char[str.size() + 1]);
      strcpy(temp.get(), str.data());
      return nostd::unique_ptr<const char[]>(temp.release());
    }
  };

  // An empty TraceState.
  TraceState() noexcept : entries_(new Entry[kMaxKeyValuePairs]), num_entries_(0) {}

  // Returns false if no such key, otherwise returns true and populates the value parameter with the
  // associated value.
  bool Get(nostd::string_view key, nostd::string_view &value) noexcept
  {
    for (int i = 0; i < num_entries_; i++)
    {
      if (key == (entries_.get())[i].GetKey())
      {
        value = (entries_.get())[i].GetValue();
        return true;
      }
    }
    return false;
  }

  // Creates an Entry for the key-value pair and adds it to entries.
  // If value is null, this function is a no-op.
  void Set(nostd::string_view key, nostd::string_view value) noexcept
  {
    if (value.data() == NULL)
      return;

    Entry entry(key, value);

    if (num_entries_ < kMaxKeyValuePairs)
    {
      (entries_.get())[num_entries_] = entry;
      num_entries_++;
    }
  }

  // Returns true if there are no keys, false otherwise.
  bool Empty() const noexcept { return num_entries_ == 0; }

  // Returns a span of all the entries. The TraceState object must outlive the span.
  nostd::span<Entry> Entries() noexcept { return nostd::span<Entry>(entries_.get(), num_entries_); }

  // Returns whether key is a valid key. See https://www.w3.org/TR/trace-context/#key
  static bool IsValidKey(nostd::string_view key)
  {
    if (key.empty() || key.size() > kKeyMaxSize || !IsLowerCaseAlphaOrDigit(key[0]))
    {
      return false;
    }

    int ats     = 0;
    const int n = key.size();

    for (int i = 0; i < n; ++i)
    {
      char c = key[i];
      if (!IsLowerCaseAlphaOrDigit(c) && c != '_' && c != '-' && c != '@' && c != '*' && c != '/')
      {
        return false;
      }
      if ((c == '@') && (++ats > 1))
      {
        return false;
      }
    }
    return true;
  }

  // Returns whether value is a valid value. See https://www.w3.org/TR/trace-context/#value
  static bool IsValidValue(nostd::string_view value)
  {
    if (value.empty() || value.size() > kValueMaxSize)
    {
      return false;
    }

    const int n = value.size();

    for (int i = 0; i < n; ++i)
    {
      char c = value[i];
      if (c < ' ' || c > '~' || c == ',' || c == '=')
      {
        return false;
      }
    }
    return true;
  }

private:
  // Store entries in a C-style array to avoid using std::array or std::vector.
  nostd::unique_ptr<Entry[]> entries_;

  // Maintain the number of entries in entries_. Must be in the range [0, kMaxKeyValuePairs].
  int num_entries_;

  static bool IsLowerCaseAlphaOrDigit(char c) { return isdigit(c) || islower(c); }
};

}  // namespace trace
}  // namespace opentelemetry
