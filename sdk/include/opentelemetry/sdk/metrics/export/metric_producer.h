// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/instrumentationlibrary/instrumentation_library.h"
#  include "opentelemetry/sdk/metrics/data/metric_data.h"
#  include "opentelemetry/sdk/resource/resource.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

/**
 * Metric Data to be exported along with resources and
 * Instrumentation library.
 */
struct InstrumentationInfoMetrics
{
  const opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary
      *instrumentation_library_;
  std::vector<MetricData> metric_data_;
};

struct ResourceMetrics
{
  const opentelemetry::sdk::resource::Resource *resource_;
  std::vector<InstrumentationInfoMetrics> instrumentation_info_metric_data_;
};

/**
 * MetricProducer is the interface that is used to make metric data available to the
 * OpenTelemetry exporters. Implementations should be stateful, in that each call to
 * `Collect` will return any metric generated since the last call was made.
 *
 * <p>Implementations must be thread-safe.
 */

class MetricProducer
{
public:
  /**
   * The callback to be called for each metric exporter. This will only be those
   * metrics that have been produced since the last time this method was called.
   *
   * @return a status of completion of method.
   */
  virtual bool Collect(
      nostd::function_ref<bool(ResourceMetrics &metric_data)> callback) noexcept = 0;
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
