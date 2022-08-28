// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/exemplar/filter.h"
#  include "opentelemetry/trace/context.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

class WithTraceSampleFilter final : public ExemplarFilter
{
public:
  static nostd::shared_ptr<ExemplarFilter> GetWithTraceSampleFilter()
  {
    nostd::shared_ptr<ExemplarFilter> withTraceSampleFilter{new WithTraceSampleFilter{}};
    return withTraceSampleFilter;
  }

  bool ShouldSampleMeasurement(long value,
                               const MetricAttributes &attributes,
                               const opentelemetry::context::Context &context) noexcept override
  {
    return hasSampledTrace(context);
  }

  bool ShouldSampleMeasurement(double value,
                               const MetricAttributes &attributes,
                               const opentelemetry::context::Context &context) noexcept override
  {
    return hasSampledTrace(context);
  }

private:
  explicit WithTraceSampleFilter() = default;
  static bool hasSampledTrace(const opentelemetry::context::Context &context)
  {
    return opentelemetry::trace::GetSpan(context)->GetContext().IsSampled();
  }
};
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
