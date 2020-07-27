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

#include <map>

#include <cstdint>
#include <cstring>

#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace trace
{

// TraceState carries tracing-system specific context in a list of key-value pairs. TraceState
// allows different vendors to propagate additional information and inter-operate with their legacy
// Id formats.
//
// Implementation is optimized for a small list of key-value pairs.
//
// Key is opaque string up to 256 characters printable. It MUST begin with a lowercase letter,
// and can only contain lowercase letters a-z, digits 0-9, underscores _, dashes -, asterisks *, and
// forward slashes /.
//
// Value is opaque string up to 256 characters, of printable ASCII RFC0020 characters (i.e. the
// range 0x20 to 0x7E) except comma , and equals =.
class TraceState
{
//public:
//  static constexpr int kKeyMaxSize       = 256;
//  static constexpr int kValueMaxSize     = 256;
//  static constexpr int kMaxKeyValuePairs = 32;
//
//  class Entry
//  {
//  public:
//    virtual ~Entry() {}
//
//    virtual nostd::string_view key()   = 0;
//    virtual nostd::string_view value() = 0;
//  };
//
//  // An empty TraceState.
//  TraceState() noexcept = default;
//
//  virtual ~TraceState() = default;
//
//  // Returns false if no such key, otherwise returns true and populates value.
//  virtual bool Get(nostd::string_view key, nostd::string_view *value) const noexcept
//  {
//    return false;
//  }
//
//  // Returns true if there are no keys.
//  virtual bool empty() const noexcept { return true; }
//
//  // Returns a span of all the entries. The TraceState object must outlive the span.
//  virtual nostd::span<Entry *> entries() const noexcept { return {}; }
//
//  // Key is opaque string up to 256 characters printable. It MUST begin with a lowercase letter, and
//  // can only contain lowercase letters a-z, digits 0-9, underscores _, dashes -, asterisks *, and
//  // forward slashes /.  For multi-tenant vendor scenarios, an at sign (@) can be used to prefix the
//  // vendor name.
//  static bool IsValidKey(nostd::string_view key)
//  {
//    if (key.empty() || key.length() > kKeyMaxSize || !IsNumberOrDigit(key[0]))
//    {
//      return false;
//    }
//    int ats     = 0;
//    const int n = key.length();
//    for (int i = 0; i < n; ++i)
//    {
//      char c = key[i];
//      if (!IsNumberOrDigit(c) && c != '_' && c != '-' && c != '@' && c != '*' && c != '/')
//      {
//        return false;
//      }
//      if ((c == '@') && (++ats > 1))
//      {
//        return false;
//      }
//    }
//    return true;
//  }
//
//  // TODO: IsValidValue
//
//private:
//  static bool IsNumberOrDigit(char c) { return (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9'); }

// TODO: Later change this back to normal, now I am using map to bypass this
public:
  std::map<nostd::string_view,nostd::string_view> tmp_map;
  static constexpr int kKeyMaxSize       = 256;
  static constexpr int kValueMaxSize     = 256;
  static constexpr int kMaxKeyValuePairs = 32;
  // An empty TraceState.
  TraceState() noexcept = default;
  TraceState(TraceState &&trace_state) {
    tmp_map = trace_state.tmp_map;
  }
  TraceState(const TraceState &trace_state) {
    tmp_map = trace_state.tmp_map;
  }

  ~TraceState() = default;

  // Returns false if no such key, otherwise returns true and populates value.
  bool Get(nostd::string_view key, nostd::string_view value) noexcept
  {
    return tmp_map[key] == value;
  }

  void Set(nostd::string_view key, nostd::string_view value) noexcept
  {
    tmp_map[key] = value;
  }

  // Returns true if there are no keys.
  bool empty() const noexcept { return tmp_map.size()==0; }

  // Returns a span of all the entries. The TraceState object must outlive the span.
  std::map<nostd::string_view,nostd::string_view> entries() noexcept { return tmp_map; }
//  virtual nostd::span<Entry *> entries() const noexcept { return {}; }

  // Key is opaque string up to 256 characters printable. It MUST begin with a lowercase letter, and
  // can only contain lowercase letters a-z, digits 0-9, underscores _, dashes -, asterisks *, and
  // forward slashes /.  For multi-tenant vendor scenarios, an at sign (@) can be used to prefix the
  // vendor name.
  static bool IsValidKey(nostd::string_view key)
  {
    if (key.empty() || key.length() > kKeyMaxSize || !IsNumberOrDigit(key[0]))
    {
      return false;
    }
    int ats     = 0;
    const int n = key.length();
    for (int i = 0; i < n; ++i)
    {
      char c = key[i];
      if (!IsNumberOrDigit(c) && c != '_' && c != '-' && c != '@' && c != '*' && c != '/')
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

  // TODO: IsValidValue

private:

  static bool IsNumberOrDigit(char c) { return (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9'); }
};

}  // namespace trace
OPENTELEMETRY_END_NAMESPACE // namespace opentelemetry