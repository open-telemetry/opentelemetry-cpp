// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <map>
#include <string>
#include "opentelemetry/sdk/common/attributemap_hash.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
class AttributesProcessor;
class FilteredOrderedAttributeMap : public opentelemetry::sdk::common::OrderedAttributeMap
{
public:
  FilteredOrderedAttributeMap() = default;
  FilteredOrderedAttributeMap(
      std::initializer_list<std::pair<nostd::string_view, opentelemetry::common::AttributeValue>>
          attributes)
      : OrderedAttributeMap(attributes)
  {}
  FilteredOrderedAttributeMap(const opentelemetry::common::KeyValueIterable &attributes)
      : FilteredOrderedAttributeMap(attributes, nullptr)
  {}
  FilteredOrderedAttributeMap(const opentelemetry::common::KeyValueIterable &attributes,
                              const opentelemetry::sdk::metrics::AttributesProcessor *processor);
  FilteredOrderedAttributeMap(
      std::initializer_list<std::pair<nostd::string_view, opentelemetry::common::AttributeValue>>
          attributes,
      const opentelemetry::sdk::metrics::AttributesProcessor *processor);
};
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
