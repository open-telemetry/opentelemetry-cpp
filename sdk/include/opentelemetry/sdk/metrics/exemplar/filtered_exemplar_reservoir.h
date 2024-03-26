// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW

#include <memory>
#include <vector>

#include "opentelemetry/sdk/metrics/exemplar/filter_type.h"
#include "opentelemetry/sdk/metrics/exemplar/reservoir.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace common
{
class SystemTimestamp;
}  // namespace common

namespace context
{
class Context;
}  // namespace context

namespace sdk
{
namespace metrics
{
class FilteredExemplarReservoir final : public ExemplarReservoir
{

public:
  FilteredExemplarReservoir(ExemplarFilterType filter_type,
                            std::shared_ptr<ExemplarReservoir> reservoir)
      : filter_type_(filter_type), reservoir_(reservoir)
  {}

  void OfferMeasurement(int64_t value,
                        const MetricAttributes &attributes,
                        const opentelemetry::context::Context &context,
                        const opentelemetry::common::SystemTimestamp &timestamp) noexcept override
  {
    if (filter_type_ == ExemplarFilterType::kAlwaysOn || filter_type_ == ExemplarFilterType::kTraceBased && hasSampledTrace(context)
    {
      reservoir_->OfferMeasurement(value, attributes, context, timestamp);
      return;
    }
  }

  void OfferMeasurement(double value,
                        const MetricAttributes &attributes,
                        const opentelemetry::context::Context &context,
                        const opentelemetry::common::SystemTimestamp &timestamp) noexcept override
  {
    if (filter_type_ == ExemplarFilterType::kAlwaysOn || filter_type_ == ExemplarFilterType::kTraceBased && hasSampledTrace(context)
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
  static bool hasSampledTrace(const opentelemetry::context::Context &context)
  {
    return opentelemetry::trace::GetSpan(context)->GetContext().IsValid() &&
           opentelemetry::trace::GetSpan(context)->GetContext().IsSampled();
  }

  explicit FilteredExemplarReservoir() = default;
  ExemplarFilterType filter_type;
  std::shared_ptr<ExemplarReservoir> reservoir_;
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE

#endif  // ENABLE_METRICS_EXEMPLAR_PREVIEW