// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <initializer_list>
#include <limits>
#include <utility>

#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/common/key_value_iterable.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/common/attribute_utils.h"
#include "opentelemetry/sdk/common/attributemap_hash.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
class AttributesProcessor;  // IWYU pragma: keep

class FilteredOrderedAttributeMap : public opentelemetry::sdk::common::OrderedAttributeMap
{
public:
  FilteredOrderedAttributeMap() = default;

  FilteredOrderedAttributeMap(
      std::initializer_list<std::pair<nostd::string_view, opentelemetry::common::AttributeValue>>
          attributes)
      : OrderedAttributeMap(attributes)
  {
    UpdateHash();
  }

  FilteredOrderedAttributeMap(const opentelemetry::common::KeyValueIterable &attributes)
      : FilteredOrderedAttributeMap(attributes, nullptr)
  {
    // No need to update hash here as it is already updated in the constructor above
  }

  FilteredOrderedAttributeMap(const opentelemetry::common::KeyValueIterable &attributes,
                              const opentelemetry::sdk::metrics::AttributesProcessor *processor);

  FilteredOrderedAttributeMap(
      std::initializer_list<std::pair<nostd::string_view, opentelemetry::common::AttributeValue>>
          attributes,
      const opentelemetry::sdk::metrics::AttributesProcessor *processor);

  //
  // Copy and move constructors, assignment operators
  //
  FilteredOrderedAttributeMap(const FilteredOrderedAttributeMap &other)            = default;
  FilteredOrderedAttributeMap(FilteredOrderedAttributeMap &&other)                 = default;
  FilteredOrderedAttributeMap &operator=(const FilteredOrderedAttributeMap &other) = default;
  FilteredOrderedAttributeMap &operator=(FilteredOrderedAttributeMap &&other)      = default;

  size_t GetHash() const { return _hash; }

  void UpdateHash() { _hash = GetHashForAttributeMap(*this); }

private:
  size_t _hash = (std::numeric_limits<size_t>::max)();
};

class FilteredOrderedAttributeMapHash
{
public:
  size_t operator()(
      const opentelemetry::sdk::metrics::FilteredOrderedAttributeMap &attributes) const
  {
    return attributes.GetHash();
  }
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
