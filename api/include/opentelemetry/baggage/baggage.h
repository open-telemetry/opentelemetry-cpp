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

#include "opentelemetry/common/kv_properties.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE

namespace baggage
{

class Baggage
{
public:
  static constexpr size_t kMaxKeyValuePairs = 180;
  static constexpr size_t kMaxKeyValueSize  = 4096;
  static constexpr size_t kMaxSize          = 8192;
  static constexpr char kKeyValueSeparator  = '=';
  static constexpr char kMembersSeparator   = ',';
  static constexpr char kMetadataSeparator  = ';';

  Baggage() : kv_properties_(new opentelemetry::common::KeyValueProperties()) {}
  Baggage(size_t size) : kv_properties_(new opentelemetry::common::KeyValueProperties(size)){};

  template <class T>
  Baggage(const T &keys_and_values)
      : kv_properties_(new opentelemetry::common::KeyValueProperties(keys_and_values))
  {}

  static nostd::shared_ptr<Baggage> GetDefault()
  {
    static nostd::shared_ptr<Baggage> baggage{new Baggage()};
    return baggage;
  }

  /* Get value for key in the baggage
     @returns true if key is found, false otherwise
  */
  bool GetValue(const nostd::string_view &key, std::string &value) const
  {
    return kv_properties_->GetValue(key, value);
  }

  /* Returns shared_ptr of new baggage object which contains new key-value pair. If key or value is
     invalid, copy of current baggage is returned
  */
  nostd::shared_ptr<Baggage> Set(const nostd::string_view &key, const nostd::string_view &value)
  {

    nostd::shared_ptr<Baggage> baggage(new Baggage(kv_properties_->Size() + 1));
    const bool valid_kv = IsValidKey(key) && IsValidValue(value);

    if (valid_kv)
    {
      baggage->kv_properties_->AddEntry(key, value);
    }

    // add rest of the fields.
    kv_properties_->GetAllEntries(
        [&baggage, &key, &valid_kv](nostd::string_view e_key, nostd::string_view e_value) {
          // if key or value was not valid, add all the entries. Add only remaining entries
          // otherwise.
          if (!valid_kv || key != e_key)
          {
            baggage->kv_properties_->AddEntry(e_key, e_value);
          }

          return true;
        });

    return baggage;
  }

  // @return all key-values entries by repeatedly invoking the function reference passed as argument
  // for each entry
  bool GetAllEntries(
      nostd::function_ref<bool(nostd::string_view, nostd::string_view)> callback) const noexcept
  {
    return kv_properties_->GetAllEntries(callback);
  }

  // delete key from the baggage if it exists. Returns shared_ptr of new baggage object.
  nostd::shared_ptr<Baggage> Delete(const nostd::string_view &key)
  {
    // keeping size of baggage same as key might not be found in it
    nostd::shared_ptr<Baggage> baggage(new Baggage(kv_properties_->Size()));
    kv_properties_->GetAllEntries(
        [&baggage, &key](nostd::string_view e_key, nostd::string_view e_value) {
          if (key != e_key)
            baggage->kv_properties_->AddEntry(e_key, e_value);
          return true;
        });
    return baggage;
  }

  // Returns shared_ptr of baggage after extracting key-value pairs from header
  static nostd::shared_ptr<Baggage> FromHeader(nostd::string_view header)
  {
    if (header.size() > kMaxSize)
    {
      // header size exceeds maximum threshold, return empty baggage
      return GetDefault();
    }

    common::KeyValueStringTokenizer kv_str_tokenizer(header);
    size_t cnt = kv_str_tokenizer.NumTokens();  // upper bound on number of kv pairs
    if (cnt > kMaxKeyValuePairs)
    {
      cnt = kMaxKeyValuePairs;
    }

    nostd::shared_ptr<Baggage> baggage(new Baggage(cnt));
    bool kv_valid;
    nostd::string_view key, value;

    while (kv_str_tokenizer.next(kv_valid, key, value) && baggage->kv_properties_->Size() < cnt)
    {
      if (!kv_valid || (key.size() + value.size() > kMaxKeyValueSize))
      {
        // if kv pair is not valid, skip it
        continue;
      }

      // NOTE : metadata is kept as part of value only as it does not have any semantic meaning.
      // but, we need to extract it (else Decode on value will return error)
      nostd::string_view metadata;
      auto metadata_separator = value.find(kMetadataSeparator);
      if (metadata_separator != std::string::npos)
      {
        metadata = value.substr(metadata_separator);
        value    = value.substr(0, metadata_separator);
      }

      int err = 0;
      key     = Decode(common::StringUtil::Trim(key), err);
      value   = Decode(common::StringUtil::Trim(value), err);

      if (err == 0 && IsValidKey(key) && IsValidValue(value))
      {
        if (!metadata.empty())
        {
          std::string value_with_metadata(value.data(), value.size());
          value_with_metadata.append(metadata.data(), metadata.size());
          value = value_with_metadata;
        }
        baggage->kv_properties_->AddEntry(key, value);
      }
    }

    return baggage;
  }

  // Creates string from baggage object.
  std::string ToHeader() const
  {
    std::string header_s;
    bool first = true;
    kv_properties_->GetAllEntries([&](nostd::string_view key, nostd::string_view value) {
      if (!first)
      {
        header_s.push_back(kMembersSeparator);
      }
      else
      {
        first = false;
      }
      header_s.append(Encode(key));
      header_s.push_back(kKeyValueSeparator);
      header_s.append(Encode(value));
      return true;
    });
    return header_s;
  }

private:
  static bool IsPrintableString(nostd::string_view str)
  {
    for (const auto &ch : str)
    {
      if (ch < ' ' || ch > '~')
      {
        return false;
      }
    }

    return true;
  }

  static bool IsValidKey(nostd::string_view key) { return key.size() && IsPrintableString(key); }

  static bool IsValidValue(nostd::string_view value) { return IsPrintableString(value); }

  // Uri encode key value pairs before injecting into header
  // Implementation inspired from : https://golang.org/src/net/url/url.go?s=7851:7884#L264
  static std::string Encode(nostd::string_view str)
  {
    auto to_hex = [](char c) -> char {
      static const char *hex = "0123456789ABCDEF";
      return hex[c & 15];
    };

    std::string ret;

    for (auto c : str)
    {
      if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
      {
        ret.push_back(c);
      }
      else if (c == ' ')
      {
        ret.push_back('+');
      }
      else
      {
        ret.push_back('%');
        ret.push_back(to_hex(c >> 4));
        ret.push_back(to_hex(c & 15));
      }
    }

    return ret;
  }

  // Uri decode key value pairs after extracting from header
  static std::string Decode(nostd::string_view str, int &err)
  {
    auto IsHex = [](char c) {
      return std::isdigit(c) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
    };

    auto from_hex = [](char c) -> char {
      return std::isdigit(c) ? c - '0' : std::toupper(c) - 'A' + 10;
    };

    std::string ret;

    for (int i = 0; i < str.size(); i++)
    {
      if (str[i] == '%')
      {
        if (i + 2 >= str.size() || !IsHex(str[i + 1]) || !IsHex(str[i + 2]))
        {
          err = 1;
          return "";
        }
        ret.push_back(from_hex(str[i + 1]) << 4 | from_hex(str[i + 2]));
        i += 2;
      }
      else if (str[i] == '+')
      {
        ret.push_back(' ');
      }
      else if (std::isalnum(str[i]) || str[i] == '-' || str[i] == '_' || str[i] == '.' ||
               str[i] == '~')
      {
        ret.push_back(str[i]);
      }
      else
      {
        err = 1;
        return "";
      }
    }

    return ret;
  }

private:
  // Store entries in a C-style array to avoid using std::array or std::vector.
  nostd::unique_ptr<opentelemetry::common::KeyValueProperties> kv_properties_;
};

}  // namespace baggage

OPENTELEMETRY_END_NAMESPACE
