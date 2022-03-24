// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/exemplar/filter.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

class AlwaysSampleFilter final : public ExemplarFilter
{
public:
  static nostd::shared_ptr<ExemplarFilter> GetAlwaysSampleFilter()
  {
    static nostd::shared_ptr<ExemplarFilter> alwaysSampleFilter{new AlwaysSampleFilter{}};
    return alwaysSampleFilter;
  }

  bool ShouldSampleMeasurement(long value,
                               const MetricAttributes &attributes,
                               const opentelemetry::context::Context &context) noexcept override
  {
    return true;
  }

  bool ShouldSampleMeasurement(double value,
                               const MetricAttributes &attributes,
                               const opentelemetry::context::Context &context) noexcept override
  {
    return true;
  }

private:
  explicit AlwaysSampleFilter() = default;
};
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
