// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/nostd/string_view.h"
#  include "opentelemetry/sdk/metrics/view/aggregation.h"
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
       const std::string &description = "",
       std::unique_ptr<opentelemetry::sdk::metrics::Aggregation> aggregation =
           std::unique_ptr<opentelemetry::sdk::metrics::Aggregation>(
               new opentelemetry::sdk::metrics::DefaultAggregation()),
       std::unique_ptr<opentelemetry::sdk::metrics::AttributesProcessor> attributes_processor =
           std::unique_ptr<opentelemetry::sdk::metrics::AttributesProcessor>(
               new opentelemetry::sdk::metrics::DefaultAttributesProcessor()))
      : name_(name),
        description_(description),
        aggregation_{std::move(aggregation)},
        attributes_processor_{std::move(attributes_processor)}
  {}

  virtual std::string GetName() const noexcept { return name_; }

  virtual std::string GetDescription() const noexcept { return description_; }

  virtual const opentelemetry::sdk::metrics::Aggregation &GetAggregation() const noexcept
  {
    return *aggregation_.get();
  }

  virtual const opentelemetry::sdk::metrics::AttributesProcessor &GetAttributesProcessor()
      const noexcept
  {
    return *attributes_processor_.get();
  }

private:
  std::string name_;
  std::string description_;
  std::unique_ptr<opentelemetry::sdk::metrics::Aggregation> aggregation_;
  std::unique_ptr<opentelemetry::sdk::metrics::AttributesProcessor> attributes_processor_;
};
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
