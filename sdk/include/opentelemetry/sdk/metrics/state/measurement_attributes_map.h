// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstring>
#include <string>
#include <unordered_map>
#include <utility>

#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/common/key_value_iterable.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/common/custom_hash_equality.h"
#include "opentelemetry/sdk/metrics/state/filtered_ordered_attribute_map.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

// Hash function for AttributeValue (int and string only)
struct AttributeValueHash
{
  size_t operator()(const opentelemetry::common::AttributeValue &value) const noexcept
  {
    if (nostd::holds_alternative<int32_t>(value))
    {
      return std::hash<int32_t>{}(nostd::get<int32_t>(value));
    }
    else if (nostd::holds_alternative<int64_t>(value))
    {
      return std::hash<int64_t>{}(nostd::get<int64_t>(value));
    }
    else if (nostd::holds_alternative<const char *>(value))
    {
      return std::hash<nostd::string_view>{}(nostd::string_view(nostd::get<const char *>(value)));
    }
    else if (nostd::holds_alternative<nostd::string_view>(value))
    {
      return std::hash<nostd::string_view>{}(nostd::get<nostd::string_view>(value));
    }

    return 0;  // fallback for other types
  }
};

// Equality function for AttributeValue (int and string only)
struct AttributeValueEqual
{
  bool operator()(const opentelemetry::common::AttributeValue &a,
                  const opentelemetry::common::AttributeValue &b) const noexcept
  {
    if (a.index() != b.index())
    {
      return false;
    }

    // Compare int32_t
    if (nostd::holds_alternative<int32_t>(a))
    {
      return nostd::get<int32_t>(a) == nostd::get<int32_t>(b);
    }
    // Compare int64_t
    else if (nostd::holds_alternative<int64_t>(a))
    {
      return nostd::get<int64_t>(a) == nostd::get<int64_t>(b);
    }
    // Compare const char*
    else if (nostd::holds_alternative<const char *>(a))
    {
      return nostd::string_view(nostd::get<const char *>(a)) ==
             nostd::string_view(nostd::get<const char *>(b));
    }
    // Compare string_view
    else if (nostd::holds_alternative<nostd::string_view>(a))
    {
      return nostd::get<nostd::string_view>(a) == nostd::get<nostd::string_view>(b);
    }

    return false;
  }
};

// Hash function for unordered_map of key-value pairs
// This Custom Hash is only applied to strings and int for now
struct AttributeMapHash
{
  size_t operator()(
      const std::unordered_map<nostd::string_view, opentelemetry::common::AttributeValue> &map)
      const noexcept
  {
    size_t hash = 0;
    AttributeValueHash value_hasher;

    for (const auto &pair : map)
    {
      // Combine key hash
      size_t key_hash = std::hash<nostd::string_view>{}(pair.first);
      hash ^= key_hash + 0x9e3779b9 + (hash << 6) + (hash >> 2);

      // Combine value hash
      size_t value_hash = value_hasher(pair.second);
      hash ^= value_hash + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    }

    return hash;
  }
};

// Equality function for unordered_map of key-value pairs
// This Custom Equal is only applied to strings and int for now
struct AttributeMapEqual
{
  bool operator()(
      const std::unordered_map<nostd::string_view, opentelemetry::common::AttributeValue> &a,
      const std::unordered_map<nostd::string_view, opentelemetry::common::AttributeValue> &b)
      const noexcept
  {
    if (a.size() != b.size())
    {
      return false;
    }

    AttributeValueEqual value_equal;

    for (const auto &pair_a : a)
    {
      auto it = b.find(pair_a.first);
      if (it == b.end())
      {
        return false;  // key not found in b
      }

      // Compare values
      if (!value_equal(pair_a.second, it->second))
      {
        return false;
      }
    }

    return true;
  }
};

template <class T>
using Measurements = std::unordered_map<
    std::unordered_map<nostd::string_view, opentelemetry::common::AttributeValue>,
    T,
    AttributeMapHash,
    AttributeMapEqual>;

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
