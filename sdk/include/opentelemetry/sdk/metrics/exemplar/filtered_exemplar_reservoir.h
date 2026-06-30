// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW

#  include <stdint.h>
#  include <memory>
#  include <utility>
#  include <vector>

#  include "opentelemetry/common/timestamp.h"
#  include "opentelemetry/context/context.h"
#  include "opentelemetry/nostd/shared_ptr.h"
#  include "opentelemetry/sdk/metrics/data/exemplar_data.h"
#  include "opentelemetry/sdk/metrics/exemplar/filter_type.h"
#  include "opentelemetry/sdk/metrics/exemplar/reservoir.h"
#  include "opentelemetry/trace/context.h"
#  include "opentelemetry/trace/span_context.h"
#  include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

/**
 * A reservoir that pre-filters measurements according to an ExemplarFilterType before
 * delegating eligible ones to a wrapped reservoir.
 */
class FilteredExemplarReservoir final : public ExemplarReservoir
{
public:
  FilteredExemplarReservoir(ExemplarFilterType filter_type,
                            nostd::shared_ptr<ExemplarReservoir> reservoir)
      : should_sample_(SelectFilter(filter_type)), reservoir_(std::move(reservoir))
  {}

  void OfferMeasurement(int64_t value,
                        const MetricAttributes &attributes,
                        const opentelemetry::context::Context &context,
                        const opentelemetry::common::SystemTimestamp &timestamp) noexcept override
  {
    if (should_sample_(context))
    {
      reservoir_->OfferMeasurement(value, attributes, context, timestamp);
    }
  }

  void OfferMeasurement(double value,
                        const MetricAttributes &attributes,
                        const opentelemetry::context::Context &context,
                        const opentelemetry::common::SystemTimestamp &timestamp) noexcept override
  {
    if (should_sample_(context))
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
  using ShouldSampleFn = bool (*)(const opentelemetry::context::Context &context);

  static bool AlwaysOn(const opentelemetry::context::Context & /* context */) noexcept
  {
    return true;
  }

  static bool AlwaysOff(const opentelemetry::context::Context & /* context */) noexcept
  {
    return false;
  }

  static bool TraceBased(const opentelemetry::context::Context &context) noexcept
  {
    const opentelemetry::trace::SpanContext span_context =
        opentelemetry::trace::GetSpanContext(context);
    return span_context.IsValid() && span_context.IsSampled();
  }

  static ShouldSampleFn SelectFilter(ExemplarFilterType filter_type) noexcept
  {
    switch (filter_type)
    {
      case ExemplarFilterType::kAlwaysOn:
        return &AlwaysOn;
      case ExemplarFilterType::kAlwaysOff:
        return &AlwaysOff;
      case ExemplarFilterType::kTraceBased:
        return &TraceBased;
    }
    return &TraceBased;  // unreachable; all enumerators handled above
  }

  ShouldSampleFn should_sample_;
  nostd::shared_ptr<ExemplarReservoir> reservoir_;
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE

#endif  // ENABLE_METRICS_EXEMPLAR_PREVIEW
