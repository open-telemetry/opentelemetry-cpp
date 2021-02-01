/*
 * Copyright The OpenTelemetry Authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/common/key_value_iterable_view.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace common
{
/**
 * A counterpart to AttributeValue that makes sure a value is owned. This
 * replaces all non-owning references with owned copies.
 */
using OwnedAttributeValue = nostd::variant<bool,
                                           int32_t,
                                           uint32_t,
                                           int64_t,
                                           uint64_t,
                                           double,
                                           std::string,
#ifdef HAVE_SPAN_BYTE
                                           std::vector<uint8_t>,
#endif
                                           std::vector<bool>,
                                           std::vector<int32_t>,
                                           std::vector<uint32_t>,
                                           std::vector<int64_t>,
                                           std::vector<uint64_t>,
                                           std::vector<double>,
                                           std::vector<std::string>>;

/**
 * Creates an owned copy (OwnedAttributeValue) of a non-owning AttributeValue.
 */
struct AttributeConverter
{
  OwnedAttributeValue operator()(bool v) { return OwnedAttributeValue(v); }
  OwnedAttributeValue operator()(int32_t v) { return OwnedAttributeValue(v); }
  OwnedAttributeValue operator()(uint32_t v) { return OwnedAttributeValue(v); }
  OwnedAttributeValue operator()(int64_t v) { return OwnedAttributeValue(v); }
  OwnedAttributeValue operator()(uint64_t v) { return OwnedAttributeValue(v); }
  OwnedAttributeValue operator()(double v) { return OwnedAttributeValue(v); }
  OwnedAttributeValue operator()(nostd::string_view v)
  {
    return OwnedAttributeValue(std::string(v));
  }
#ifdef HAVE_SPAN_BYTE
  OwnedAttributeValue operator()(nostd::span<const uint8_t> v) { return convertSpan<uint8_t>(v); }
#endif
  OwnedAttributeValue operator()(nostd::span<const bool> v) { return convertSpan<bool>(v); }
  OwnedAttributeValue operator()(nostd::span<const int32_t> v) { return convertSpan<int32_t>(v); }
  OwnedAttributeValue operator()(nostd::span<const uint32_t> v) { return convertSpan<uint32_t>(v); }
  OwnedAttributeValue operator()(nostd::span<const int64_t> v) { return convertSpan<int64_t>(v); }
  OwnedAttributeValue operator()(nostd::span<const uint64_t> v) { return convertSpan<uint64_t>(v); }
  OwnedAttributeValue operator()(nostd::span<const double> v) { return convertSpan<double>(v); }
  OwnedAttributeValue operator()(nostd::span<const nostd::string_view> v)
  {
    return convertSpan<std::string>(v);
  }

  template <typename T, typename U = T>
  OwnedAttributeValue convertSpan(nostd::span<const U> vals)
  {
    const std::vector<T> copy(vals.begin(), vals.end());
    return OwnedAttributeValue(std::move(copy));
  }
};

/**
 * Class for storing attributes.
 */
class AttributeMap
{
public:
  // Contruct empty attribute map
  AttributeMap(){};

  // Contruct attribute map and populate with attributes
  AttributeMap(const opentelemetry::common::KeyValueIterable &attributes)
  {
    attributes.ForEachKeyValue(
        [&](nostd::string_view key, opentelemetry::common::AttributeValue value) noexcept {
          SetAttribute(key, value);
          return true;
        });
  }

  const std::unordered_map<std::string, OwnedAttributeValue> &GetAttributes() const noexcept
  {
    return attributes_;
  }

  void SetAttribute(nostd::string_view key,
                    const opentelemetry::common::AttributeValue &value) noexcept
  {
    attributes_[std::string(key)] = nostd::visit(converter_, value);
  }

private:
  std::unordered_map<std::string, OwnedAttributeValue> attributes_;
  AttributeConverter converter_;
};
}  // namespace common
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
