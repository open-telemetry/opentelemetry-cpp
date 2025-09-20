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
#include "opentelemetry/sdk/metrics/state/filtered_ordered_attribute_map.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

using MetricAttributes = opentelemetry::sdk::metrics::FilteredOrderedAttributeMap;

/**
 * Generic FNV hash implementation
 */

template <typename Ty, size_t Size>
struct FnvPrime;

template <typename Ty>
struct FnvPrime<Ty, 4>
{
  static constexpr Ty value = static_cast<Ty>(0x01000193U);
};

template <typename Ty>
struct FnvPrime<Ty, 8>
{
  static constexpr Ty value = static_cast<Ty>(0x100000001b3ULL);
};

// FNV offset basis values
template <typename Ty, size_t Size>
struct FnvOffset;

template <typename Ty>
struct FnvOffset<Ty, 4>
{
  static constexpr Ty value = static_cast<Ty>(0x811C9DC5U);

  static constexpr Ty Fix(Ty hval) noexcept { return hval; }
};

template <typename Ty>
struct FnvOffset<Ty, 8>
{
  static constexpr Ty value = static_cast<Ty>(0xCBF29CE484222325ULL);

  static constexpr Ty Fix(Ty hval) noexcept { return hval ^ (hval >> 32); }
};

// FNV-1a
template <typename Ty>
inline Ty Fnv1a(const void *buf, size_t len, Ty hval = FnvOffset<Ty, sizeof(Ty)>::value) noexcept
{
  const unsigned char *bp = reinterpret_cast<const unsigned char *>(buf);
  const unsigned char *be = bp + len;
  Ty prime                = FnvPrime<Ty, sizeof(Ty)>::value;

  while (bp < be)
  {
    hval ^= static_cast<Ty>(*bp++);
    hval *= prime;
  }
  return FnvOffset<Ty, sizeof(Ty)>::Fix(hval);
}

/**
 * Hash and equality for nostd::string_view, enabling safe use in unordered_map
 * without requiring null termination.
 */

struct StringViewHash
{
#if defined(OPENTELEMETRY_STL_VERSION)
#  if OPENTELEMETRY_STL_VERSION >= 2020
  using is_transparent = void;
#  endif
#endif

  std::size_t operator()(const std::string &s) const noexcept
  {
    return Fnv1a<std::size_t>(s.data(), s.size());
  }

  std::size_t operator()(opentelemetry::nostd::string_view sv) const noexcept
  {
    return Fnv1a<std::size_t>(sv.data(), sv.size());
  }
};

struct StringViewEqual
{
#if defined(OPENTELEMETRY_STL_VERSION)
#  if OPENTELEMETRY_STL_VERSION >= 2020
  using is_transparent = void;
#  endif
#endif

  template <typename Lhs, typename Rhs>
  bool operator()(const Lhs &lhs, const Rhs &rhs) const noexcept
  {
    opentelemetry::nostd::string_view lsv(opentelemetry::nostd::data(lhs),
                                          opentelemetry::nostd::size(lhs));
    opentelemetry::nostd::string_view rsv(opentelemetry::nostd::data(rhs),
                                          opentelemetry::nostd::size(rhs));

    return lsv.size() == rsv.size() && std::memcmp(lsv.data(), rsv.data(), lsv.size()) == 0;
  }
};

/**
 * Cross-platform heterogeneous lookup wrapper.
 * Falls back to std::string construction on libc++ (macOS) and pre-c++20,
 * but uses direct lookup on libstdc++ (Linux).
 */

template <typename MapType>
inline auto find_hetero(MapType &&map, opentelemetry::nostd::string_view key)
{
#if defined(_LIBCPP_VERSION) || \
    (!defined(OPENTELEMETRY_STL_VERSION) || OPENTELEMETRY_STL_VERSION < 2020)
  return map.find(std::string(key));
#else
  // libstdc++ + C++20: heterogeneous lookup works
  return map.find(key);
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
