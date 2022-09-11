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

class NeverSampleFilter final : public ExemplarFilter
{
public:
  bool ShouldSampleMeasurement(long value,
                               const MetricAttributes &attributes,
                               const opentelemetry::context::Context &context) noexcept override
  {
    return false;
  }

  bool ShouldSampleMeasurement(double value,
                               const MetricAttributes &attributes,
                               const opentelemetry::context::Context &context) noexcept override
  {
    return false;
  }

  explicit NeverSampleFilter() = default;
};
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
