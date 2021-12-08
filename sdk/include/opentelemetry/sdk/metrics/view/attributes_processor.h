// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/common/attribute_utils.h"
OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
using MetricAttributes = opentelemetry::sdk::common::AttributeMap;

class AttributesProcessor
{
public:
  virtual MetricAttributes process(
      const opentelemetry::common::KeyValueIterable &attributes) noexcept = 0;
};

class DefaultAttributesProcessor : public AttributesProcessor
{
  MetricAttributes process(
      const opentelemetry::common::KeyValueIterable &attributes) noexcept override
  {
    MetricAttributes result(attributes);
    return result;
  }
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif