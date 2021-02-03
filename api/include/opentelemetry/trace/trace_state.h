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
#include <string>

#include <regex>
#if (__GNUC__ == 4 && (__GNUC_MINOR__ == 8))
#  define HAVE_WORKING_REGEX 0
#else
#  define HAVE_WORKING_REGEX 1
#endif

#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/unique_ptr.h"

OPENTELEMETRY_BEGIN_NAMESPACE
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
  static constexpr int kKeyMaxSize         = 256;
  static constexpr int kValueMaxSize       = 256;
  static constexpr int kMaxKeyValuePairs   = 32;
  static constexpr auto kKeyValueSeparator = '=';
  static constexpr auto kMembersSeparator  = ',';

  // Class to store key-value pairs.
  class Entry
  {
  public:
    Entry() : key_(nullptr), value_(nullptr){};

    // Copy constructor
    Entry(const Entry &copy)
    {
      key_   = CopyStringToPointer(copy.key_.get());
      value_ = CopyStringToPointer(copy.value_.get());
    }

    // Copy assignment operator
    Entry &operator=(Entry &other)
    {
      key_   = CopyStringToPointer(other.key_.get());
      value_ = CopyStringToPointer(other.value_.get());
      return *this;
    }

    // Move contructor and assignment operator
    Entry(Entry &&other) = default;
    Entry &operator=(Entry &&other) = default;

    // Creates an Entry for a given key-value pair.
    Entry(nostd::string_view key, nostd::string_view value) noexcept
    {
      key_   = CopyStringToPointer(key);
      value_ = CopyStringToPointer(value);
    }

    // Gets the key associated with this entry.
    nostd::string_view GetKey() const { return key_.get(); }

    // Gets the value associated with this entry.
    nostd::string_view GetValue() const { return value_.get(); }

    // Sets the value for this entry. This overrides the previous value.
    void SetValue(nostd::string_view value) { value_ = CopyStringToPointer(value); }

  private:
    // Store key and value as raw char pointers to avoid using std::string.
    nostd::unique_ptr<const char[]> key_;
    nostd::unique_ptr<const char[]> value_;

    // Copies string into a buffer and returns a unique_ptr to the buffer.
    // This is a workaround for the fact that memcpy doesn't accept a const destination.
    nostd::unique_ptr<const char[]> CopyStringToPointer(nostd::string_view str)
    {
      char *temp = new char[str.size() + 1];
      memcpy(temp, str.data(), str.size());
      temp[str.size()] = '\0';
      return nostd::unique_ptr<const char[]>(temp);
    }
  };

  /**
   * Returns a newly created TraceState parsed from the header provided.
   * @param header Encoding of the tracestate header defined by
   * the W3C Trace Context specification https://www.w3.org/TR/trace-context/
   *  @return Tracestate A new TraceState instance or DEFAULT
   */
  static TraceState FromHeader(nostd::string_view header)
  {
    TraceState ts;

    std::size_t begin{0};
    std::size_t end{0};
    bool invalid_header = false;
    while (begin < header.size() && ts.num_entries_ < kMaxKeyValuePairs)
    {
      // find list-member
      end = header.find(kMembersSeparator, begin);
      if (end == 0)
      {
        // special case where "," is first char, move to next list member
        begin = 1;
        continue;
      }
      if (end == std::string::npos)
      {
        // last list member. `end` points to end of it.
        end = header.size() - 1;
      }
      else
      {
        // `end` points to end of current list member
        end--;
      }
      auto list_member = TrimString(header, begin, end);  // OWS handling
      if (list_member.size() == 0)
      {
        // empty list member, move to next in list
        begin = end + 2;  // begin points to start of next member
        continue;
      }
      auto key_end_pos = list_member.find(kKeyValueSeparator);
      if (key_end_pos == std::string::npos)
      {
        // Error: invalid list member, return empty TraceState
        ts.entries_.reset(nullptr);
        ts.num_entries_ = 0;
        break;
      }
      auto key   = list_member.substr(0, key_end_pos);
      auto value = list_member.substr(key_end_pos + 1);
      if (!IsValidKey(key) || !IsValidValue(value))
      {
        // invalid header. return empty TraceState
        ts.entries_.reset(nullptr);
        ts.num_entries_ = 0;
        break;
      }
      Entry entry(key, value);
      (ts.entries_.get())[ts.num_entries_] = entry;
      ts.num_entries_++;

      begin = end + 2;
    }
    return ts;
  }

  /**
   * Creates a w3c tracestate header from TraceState object
   */
  std::string ToHeader()
  {
    std::string header_s;
    for (size_t count = 0; count < num_entries_; count++)
    {
      if (count != 0)
      {
        header_s.append(",");
      }

      auto entry = (entries_.get())[count];
      header_s.append(std::string(entry.GetKey()));
      header_s.append(1, kKeyValueSeparator);
      header_s.append(std::string(entry.GetValue()));
    }
    return header_s;
  }

  /**
   *  Returns `value` associated with `key` passed as argument
   *  Returns empty string if key is invalid  or not found
   */
  std::string Get(nostd::string_view key) const noexcept
  {
    if (!IsValidKey(key))
    {
      return std::string();
    }
    for (size_t i = 0; i < num_entries_; i++)
    {
      auto entry = (entries_.get())[i];
      if (key == entry.GetKey())
      {
        return std::string(entry.GetValue());
      }
    }
    return std::string();
  }

  /**
   * Returns `new` TraceState object with following mutations applied to the existing instance:
   *  Update Key value: The updated value must be moved to beginning of List
   *  Add : The new key-value pair SHOULD be added to beginning of List
   *
   * If the provided key-value pair is invalid, or results in transtate that violates the
   * tracecontext specification, empty TraceState instance will be returned.
   *
   * If the existing object has maximum list members, it's copy is returned.
   */
  TraceState Set(const nostd::string_view &key, const nostd::string_view &value)
  {
    TraceState ts;
    if ((!IsValidKey(key) || !IsValidValue(value)))
    {
      // max size reached or invalid key/value. Returning empty TraceState
      return ts;  // empty instance
    }

    // add new key-value pair at beginning if list is not reached to its limit
    if (num_entries_ < kMaxKeyValuePairs)
    {
      Entry e(key, value);
      (ts.entries_.get())[ts.num_entries_++] = e;
    }
    for (size_t i = 0; i < num_entries_; i++)
    {
      auto entry = (entries_.get())[i];
      auto key   = entry.GetKey();
      auto value = entry.GetValue();
      Entry e(key, value);
      (ts.entries_.get())[ts.num_entries_++] = e;
    }
    return ts;
  }

  /**
   * Returns `new` TraceState object after removing the attribute with given key ( if present )
   * @returns empty TraceState object if key is invalid
   * @returns copy of original TraceState object if key is not present (??)
   */
  TraceState Delete(const nostd::string_view &key)
  {
    TraceState ts;
    if (!IsValidKey(key))
    {
      return ts;
    }
    for (size_t i = 0; i < num_entries_; i++)
    {
      auto entry = (entries_.get())[i];
      if ((entries_.get())[i].GetKey() != key)
      {
        auto key   = entry.GetKey();
        auto value = entry.GetValue();
        Entry e(key, value);
        (ts.entries_.get())[ts.num_entries_++] = e;
      }
    }
    return ts;
  }

  // Returns true if there are no keys, false otherwise.
  bool Empty() const noexcept { return num_entries_ == 0; }

  // Returns a span of all the entries. The TraceState object must outlive the span.
  nostd::span<Entry> Entries() const noexcept
  {
    return nostd::span<Entry>(entries_.get(), num_entries_);
  }

  /** Returns whether key is a valid key. See https://www.w3.org/TR/trace-context/#key
   * Identifiers MUST begin with a lowercase letter or a digit, and can only contain
   * lowercase letters (a-z), digits (0-9), underscores (_), dashes (-), asterisks (*),
   * and forward slashes (/).
   * For multi-tenant vendor scenarios, an at sign (@) can be used to prefix the vendor name.
   *
   */
  static bool IsValidKey(nostd::string_view key)
  {
#if HAVE_WORKING_REGEX
    return IsValidKeyRegEx(key);
#else
    return IsValidKeyNonRegEx(key);
#endif
  }

  /** Returns whether value is a valid value. See https://www.w3.org/TR/trace-context/#value
   * The value is an opaque string containing up to 256 printable ASCII (RFC0020)
   *  characters ((i.e., the range 0x20 to 0x7E) except comma , and equal =)
   */
  static bool IsValidValue(nostd::string_view value)
  {
#if HAVE_WORKING_REGEX
    return IsValidValueRegEx(value);
#else
    return IsValidValueNonRegEx(value);
#endif
  }

private:
  // Store entries in a C-style array to avoid using std::array or std::vector.
  nostd::unique_ptr<Entry[]> entries_;

  // Maintain the number of entries in entries_. Must be in the range [0, kMaxKeyValuePairs].
  size_t num_entries_;

  // An empty TraceState.
  TraceState() noexcept : entries_(new Entry[kMaxKeyValuePairs]), num_entries_(0) {}

  static nostd::string_view TrimString(nostd::string_view str, size_t left, size_t right)
  {
    while (str[static_cast<std::size_t>(right)] == ' ' && left < right)
    {
      right--;
    }
    while (str[static_cast<std::size_t>(left)] == ' ' && left < right)
    {
      left++;
    }
    return str.substr(left, right - left + 1);
  }

  static bool IsValidKeyRegEx(nostd::string_view key)
  {
    static std::regex reg_key("^[a-z0-9][a-z0-9*_\\-/]{0,255}$");
    static std::regex reg_key_multitenant(
        "^[a-z0-9][a-z0-9*_\\-/]{0,240}(@)[a-z0-9][a-z0-9*_\\-/]{0,13}$");
    std::string key_s(key);
    if (std::regex_match(key_s, reg_key) || std::regex_match(key_s, reg_key_multitenant))
    {
      return true;
    }
    return false;
  }

  static bool IsValidValueRegEx(nostd::string_view value)
  {
    // Hex 0x20 to 0x2B, 0x2D to 0x3C, 0x3E to 0x7E
    static std::regex reg_value(
        "^[\\x20-\\x2B\\x2D-\\x3C\\x3E-\\x7E]{0,255}[\\x21-\\x2B\\x2D-\\x3C\\x3E-\\x7E]$");
    // Need to benchmark without regex, as a string object is created here.
    return std::regex_match(std::string(value), reg_value);
  }

  static bool IsValidKeyNonRegEx(nostd::string_view key)
  {
    if (key.empty() || key.size() > kKeyMaxSize || !IsLowerCaseAlphaOrDigit(key[0]))
    {
      return false;
    }

    int ats = 0;

    for (const char c : key)
    {
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

  static bool IsValidValueNonRegEx(nostd::string_view value)
  {
    if (value.empty() || value.size() > kValueMaxSize)
    {
      return false;
    }

    for (const char c : value)
    {
      if (c < ' ' || c > '~' || c == ',' || c == '=')
      {
        return false;
      }
    }
    return true;
  }

  static bool IsLowerCaseAlphaOrDigit(char c) { return isdigit(c) || islower(c); }
};
}  // namespace trace
OPENTELEMETRY_END_NAMESPACE
