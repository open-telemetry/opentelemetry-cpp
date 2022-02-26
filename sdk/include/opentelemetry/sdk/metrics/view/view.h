// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/nostd/string_view.h"
#  include "opentelemetry/sdk/metrics/aggregation/default_aggregation.h"
#  include "opentelemetry/sdk/metrics/instruments.h"
#  include "opentelemetry/sdk/metrics/view/attributes_processor.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
/**
 * View defines the interface to allow SDK user to
 * customize the metrics before exported.
 */

class View
{
public:
  View(const std::string &name,
       const std::string &description   = "",
       AggregationType aggregation_type = AggregationType::kDrop,
       std::unique_ptr<opentelemetry::sdk::metrics::AttributesProcessor> attributes_processor =
           std::unique_ptr<opentelemetry::sdk::metrics::AttributesProcessor>(
               new opentelemetry::sdk::metrics::DefaultAttributesProcessor()))
      : name_(name),
        description_(description),
        aggregation_type_{aggregation_type},
        attributes_processor_{std::move(attributes_processor)}
  {}

  virtual std::string GetName() const noexcept { return name_; }

  virtual std::string GetDescription() const noexcept { return description_; }

  virtual AggregationType GetAggregationType() const noexcept { return aggregation_type_; }

  virtual const opentelemetry::sdk::metrics::AttributesProcessor &GetAttributesProcessor()
      const noexcept
  {
    return *attributes_processor_.get();
  }

private:
  std::string name_;
  std::string description_;
  AggregationType aggregation_type_;
  std::unique_ptr<opentelemetry::sdk::metrics::AttributesProcessor> attributes_processor_;
};
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
