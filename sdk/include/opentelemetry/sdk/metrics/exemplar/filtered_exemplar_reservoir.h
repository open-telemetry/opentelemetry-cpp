// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>
#include <vector>
#include "opentelemetry/context/context.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/sdk/common/attribute_utils.h"
#include "opentelemetry/sdk/metrics/exemplar/filter.h"
#include "opentelemetry/sdk/metrics/exemplar/reservoir.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
class FilteredExemplarReservoir final : public ExemplarReservoir
{

public:
  FilteredExemplarReservoir(std::shared_ptr<ExemplarFilter> filter,
                            std::shared_ptr<ExemplarReservoir> reservoir)
      : filter_(filter), reservoir_(reservoir)
  {}

  void OfferMeasurement(int64_t value,
                        const MetricAttributes &attributes,
                        const opentelemetry::context::Context &context,
                        const opentelemetry::common::SystemTimestamp &timestamp) noexcept override
  {
    if (filter_->ShouldSampleMeasurement(value, attributes, context))
    {
      reservoir_->OfferMeasurement(value, attributes, context, timestamp);
    }
  }

  void OfferMeasurement(double value,
                        const MetricAttributes &attributes,
                        const opentelemetry::context::Context &context,
                        const opentelemetry::common::SystemTimestamp &timestamp) noexcept override
  {
    if (filter_->ShouldSampleMeasurement(value, attributes, context))
    {
      reservoir_->OfferMeasurement(value, attributes, context, timestamp);
    }
  }

  std::vector<std::shared_ptr<ExemplarData>> CollectAndReset(
      const MetricAttributes &pointAttributes) noexcept override
  {
    return reservoir_->CollectAndReset(pointAttributes);
  }

private:
  explicit FilteredExemplarReservoir() = default;
  std::shared_ptr<ExemplarFilter> filter_;
  std::shared_ptr<ExemplarReservoir> reservoir_;
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
