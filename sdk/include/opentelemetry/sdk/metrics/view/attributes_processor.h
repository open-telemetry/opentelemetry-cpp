// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>
#include <unordered_map>
#include <utility>

#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/common/key_value_iterable.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/metrics/state/filtered_ordered_attribute_map.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

using MetricAttributes = opentelemetry::sdk::metrics::FilteredOrderedAttributeMap;

/**
 * Hash and equality for nostd::string_view, enabling safe use in unordered_map
 * without requiring null termination.
 */
struct StringViewHash
{
  using is_transparent = void;

  std::size_t operator()(const std::string &s) const noexcept
  {
    return std::hash<std::string>{}(s);
  }
  std::size_t operator()(std::string_view sv) const noexcept
  {
    return std::hash<std::string_view>{}(sv);
  }
  std::size_t operator()(opentelemetry::nostd::string_view sv) const noexcept
  {
    return std::hash<std::string_view>{}(std::string_view{sv.data(), sv.size()});
  }
};

struct StringViewEqual
{
  using is_transparent = void;

  bool operator()(const std::string &lhs, const std::string &rhs) const noexcept
  {
    return lhs == rhs;
  }
  bool operator()(const std::string &lhs, std::string_view rhs) const noexcept
  {
    return lhs.size() == rhs.size() && std::memcmp(lhs.data(), rhs.data(), lhs.size()) == 0;
  }
  bool operator()(std::string_view lhs, const std::string &rhs) const noexcept
  {
    return lhs.size() == rhs.size() && std::memcmp(lhs.data(), rhs.data(), lhs.size()) == 0;
  }
  bool operator()(opentelemetry::nostd::string_view lhs, const std::string &rhs) const noexcept
  {
    return lhs.size() == rhs.size() && std::memcmp(lhs.data(), rhs.data(), lhs.size()) == 0;
  }
};

/**
 * Cross-platform heterogeneous lookup wrapper.
 * Falls back to std::string construction on libc++ (macOS),
 * but uses direct lookup on libstdc++ (Linux).
 */
template <typename Map, typename Key>
auto find_hetero(Map &map, Key &&key)
{
#if defined(_LIBCPP_VERSION)
  // libc++ (macOS) does not yet support heterogeneous lookup in unordered_map
  return map.find(std::string(key));
#else
  // libstdc++ (Linux, GCC/Clang) supports it
  return map.find(std::forward<Key>(key));
#endif
}

/**
 * The AttributesProcessor is responsible for customizing which
 * attribute(s) are to be reported as metrics dimension(s).
 */

class AttributesProcessor
{
public:
  // Process the metric instrument attributes.
  // @returns integer with individual bits set if they are to be filtered.

  virtual MetricAttributes process(
      const opentelemetry::common::KeyValueIterable &attributes) const noexcept = 0;

  virtual bool isPresent(nostd::string_view key) const noexcept = 0;

  virtual ~AttributesProcessor() = default;
};

/**
 * DefaultAttributesProcessor returns copy of input instrument attributes without
 * any modification.
 */

class DefaultAttributesProcessor : public AttributesProcessor
{
public:
  MetricAttributes process(
      const opentelemetry::common::KeyValueIterable &attributes) const noexcept override
  {
    MetricAttributes result(attributes);
    return result;
  }

  bool isPresent(nostd::string_view /*key*/) const noexcept override { return true; }
};

/**
 * FilteringAttributesProcessor  filters by allowed attribute names and drops any names
 * that are not in the allow list.
 */

class FilteringAttributesProcessor : public AttributesProcessor
{
public:
  FilteringAttributesProcessor(
      std::unordered_map<std::string, bool, StringViewHash, StringViewEqual>
          &&allowed_attribute_keys = {})
      : allowed_attribute_keys_(std::move(allowed_attribute_keys))
  {}

  FilteringAttributesProcessor(
      const std::unordered_map<std::string, bool, StringViewHash, StringViewEqual>
          &allowed_attribute_keys = {})
      : allowed_attribute_keys_(allowed_attribute_keys)
  {}

  MetricAttributes process(
      const opentelemetry::common::KeyValueIterable &attributes) const noexcept override
  {
    MetricAttributes result;
    attributes.ForEachKeyValue(
        [&](nostd::string_view key, opentelemetry::common::AttributeValue value) noexcept {
          if (find_hetero(allowed_attribute_keys_, key) != allowed_attribute_keys_.end())
          {
            result.SetAttribute(key, value);
            return true;
          }
          return true;
        });

    result.UpdateHash();
    return result;
  }

  bool isPresent(nostd::string_view key) const noexcept override
  {
    return (find_hetero(allowed_attribute_keys_, key) != allowed_attribute_keys_.end());
  }

private:
  std::unordered_map<std::string, bool, StringViewHash, StringViewEqual> allowed_attribute_keys_;
};

/**
 * FilteringExcludeAttributeProcessor filters by exclude attribute list and drops names if they are
 * present in the exclude list
 */

class FilteringExcludeAttributesProcessor : public AttributesProcessor
{
public:
  FilteringExcludeAttributesProcessor(
      std::unordered_map<std::string, bool, StringViewHash, StringViewEqual> &&exclude_list = {})
      : exclude_list_(std::move(exclude_list))
  {}

  FilteringExcludeAttributesProcessor(
      const std::unordered_map<std::string, bool, StringViewHash, StringViewEqual> &exclude_list =
          {})
      : exclude_list_(exclude_list)
  {}

  MetricAttributes process(
      const opentelemetry::common::KeyValueIterable &attributes) const noexcept override
  {
    MetricAttributes result;
    attributes.ForEachKeyValue(
        [&](nostd::string_view key, opentelemetry::common::AttributeValue value) noexcept {
          if (find_hetero(exclude_list_, key) == exclude_list_.end())
          {
            result.SetAttribute(key, value);
            return true;
          }
          return true;
        });

    result.UpdateHash();
    return result;
  }

  bool isPresent(nostd::string_view key) const noexcept override
  {
    return (find_hetero(exclude_list_, key) == exclude_list_.end());
  }

private:
  std::unordered_map<std::string, bool, StringViewHash, StringViewEqual> exclude_list_;
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
