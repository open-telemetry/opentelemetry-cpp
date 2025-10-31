// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>
#include <stdexcept>
#include <string>

#include "opentelemetry/sdk/metrics/aggregation/aggregation_config.h"
#include "opentelemetry/sdk/metrics/instruments.h"
#include "opentelemetry/sdk/metrics/view/attributes_processor.h"
#include "opentelemetry/version.h"

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
       const std::string &description                        = "",
       AggregationType aggregation_type                      = AggregationType::kDefault,
       std::shared_ptr<AggregationConfig> aggregation_config = nullptr,
       std::unique_ptr<opentelemetry::sdk::metrics::AttributesProcessor> attributes_processor =
           std::unique_ptr<opentelemetry::sdk::metrics::AttributesProcessor>(
               new opentelemetry::sdk::metrics::DefaultAttributesProcessor()))
      : name_(name),
        description_(description),
        aggregation_type_{aggregation_type},
        aggregation_config_{aggregation_config},
        attributes_processor_{std::move(attributes_processor)}
  {
    // Validate that aggregation_type and aggregation_config match
    if (aggregation_config_)
    {
      AggregationType config_type = aggregation_config_->GetType();
      bool valid                  = false;

      switch (aggregation_type_)
      {
        case AggregationType::kHistogram:
          valid = (config_type == AggregationType::kHistogram);
          break;
        case AggregationType::kBase2ExponentialHistogram:
          valid = (config_type == AggregationType::kBase2ExponentialHistogram);
          break;
        case AggregationType::kDefault:
        case AggregationType::kDrop:
        case AggregationType::kLastValue:
        case AggregationType::kSum:
          valid = (config_type == AggregationType::kDefault);
          break;
      }

      if (!valid)
      {
        throw std::invalid_argument("AggregationType and AggregationConfig type mismatch");
      }
    }
  }

  virtual ~View() = default;

  virtual std::string GetName() const noexcept { return name_; }

  virtual std::string GetDescription() const noexcept { return description_; }

  virtual AggregationType GetAggregationType() const noexcept { return aggregation_type_; }

  virtual AggregationConfig *GetAggregationConfig() const noexcept
  {
    return aggregation_config_.get();
  }

  virtual std::shared_ptr<const opentelemetry::sdk::metrics::AttributesProcessor>
  GetAttributesProcessor() const noexcept
  {
    return attributes_processor_;
  }

private:
  std::string name_;
  std::string description_;
  AggregationType aggregation_type_;
  std::shared_ptr<AggregationConfig> aggregation_config_;
  std::shared_ptr<AttributesProcessor> attributes_processor_;
};
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
