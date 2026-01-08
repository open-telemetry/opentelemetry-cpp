// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/common/key_value_iterable.h"
#include "opentelemetry/nostd/function_ref.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/common/attribute_utils.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace common
{

OPENTELEMETRY_EXPORT bool AttributeIsValidString(nostd::string_view value) noexcept;

/**
 * Validate if an attribute value is valid.
 */
struct AttributeValidator
{
  bool operator()(bool /*v*/) noexcept { return true; }
  bool operator()(int32_t /*v*/) noexcept { return true; }
  bool operator()(uint32_t /*v*/) noexcept { return true; }
  bool operator()(int64_t /*v*/) noexcept { return true; }
  bool operator()(uint64_t /*v*/) noexcept { return true; }
  bool operator()(double /*v*/) noexcept { return true; }
  bool operator()(nostd::string_view v) noexcept { return AttributeIsValidString(v); }
  bool operator()(const std::string &v) noexcept { return AttributeIsValidString(v); }
  bool operator()(const char *v) noexcept { return AttributeIsValidString(v); }
  bool operator()(nostd::span<const uint8_t> /*v*/) noexcept { return true; }
  bool operator()(nostd::span<const bool> /*v*/) noexcept { return true; }
  bool operator()(nostd::span<const int32_t> /*v*/) noexcept { return true; }
  bool operator()(nostd::span<const uint32_t> /*v*/) noexcept { return true; }
  bool operator()(nostd::span<const int64_t> /*v*/) noexcept { return true; }
  bool operator()(nostd::span<const uint64_t> /*v*/) noexcept { return true; }
  bool operator()(nostd::span<const double> /*v*/) noexcept { return true; }
  bool operator()(nostd::span<const nostd::string_view> v) noexcept
  {
    for (const auto &s : v)
    {
      if (!AttributeIsValidString(s))
      {
        return false;
      }
    }
    return true;
  }
  bool operator()(const std::vector<bool> & /*v*/) noexcept { return true; }
  bool operator()(const std::vector<int32_t> & /*v*/) noexcept { return true; }
  bool operator()(const std::vector<uint32_t> & /*v*/) noexcept { return true; }
  bool operator()(const std::vector<int64_t> & /*v*/) noexcept { return true; }
  bool operator()(const std::vector<double> & /*v*/) noexcept { return true; }
  bool operator()(const std::vector<std::string> &v)
  {
    for (const auto &s : v)
    {
      if (!AttributeIsValidString(s))
      {
        return false;
      }
    }
    return true;
  }
  bool operator()(const std::vector<uint64_t> & /*v*/) noexcept { return true; }
  bool operator()(const std::vector<uint8_t> & /*v*/) noexcept { return true; }

  OPENTELEMETRY_EXPORT static bool IsValid(const std::string &value) noexcept;

  OPENTELEMETRY_EXPORT static bool IsValid(nostd::string_view value) noexcept;

  OPENTELEMETRY_EXPORT static bool IsValid(const OwnedAttributeValue &value) noexcept;

  OPENTELEMETRY_EXPORT static bool IsValid(
      const opentelemetry::common::AttributeValue &value) noexcept;

  OPENTELEMETRY_EXPORT static bool IsAllValid(const AttributeMap &attributes) noexcept;

  OPENTELEMETRY_EXPORT static bool IsAllValid(const OrderedAttributeMap &attributes) noexcept;

  OPENTELEMETRY_EXPORT static void Filter(AttributeMap &attributes, nostd::string_view log_hint);

  OPENTELEMETRY_EXPORT static void Filter(OrderedAttributeMap &attributes,
                                          nostd::string_view log_hint);
};

/**
 * Supports internal iteration over a collection of key-value pairs and filtering of invalid
 * attributes.
 */
class OPENTELEMETRY_EXPORT KeyValueFilterIterable : public opentelemetry::common::KeyValueIterable
{
public:
  KeyValueFilterIterable(const opentelemetry::common::KeyValueIterable &origin,
                         opentelemetry::nostd::string_view log_hint) noexcept;

  ~KeyValueFilterIterable() override;

  bool ForEachKeyValue(
      opentelemetry::nostd::function_ref<bool(opentelemetry::nostd::string_view,
                                              opentelemetry::common::AttributeValue)> callback)
      const noexcept override;

  size_t size() const noexcept override;

private:
  // Pointer to the original KeyValueIterable
  const opentelemetry::common::KeyValueIterable *origin_;

  // Size of valid attributes
  mutable size_t size_;

  // Log hint for invalid attributes
  opentelemetry::nostd::string_view log_hint_;
};

}  // namespace common
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
