// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include <chrono>
#  include "opentelemetry/sdk/common/global_log_handler.h"
#  include "opentelemetry/sdk/metrics/data/metric_data.h"
#  include "opentelemetry/sdk/metrics/instruments.h"
#  include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

/**
 * MetricReader defines the interface to collect metrics from SDK
 */
class MetricReader
{
public:
  MetricReader(
      AggregationTemporarily aggregation_temporarily = AggregationTemporarily::kCummulative)
      : aggregation_temporarily_(aggregation_temporarily), measurement_processor_callback_({})
  {}

  virtual ~MetricReader() = default;

  /**
   * Collect the metrics from SDK.
   * @return return the status of the operation.
   */
  bool Collect() noexcept
  {
    if (!measurement_processor_callback_)
    {
      OTEL_INTERNAL_LOG_WARN(
          "Cannot invoke Collect() for MetricReader. No collection callback registered!")
    }
    return measurement_processor_callback_(
        *this, aggregation_temporarily_,
        [&](MetricData metric_data) noexcept { return ProcessReceivedMetrics(metric_data); });
  }

  /**
   * Register the callback to Measurement Processor
   * This function is internal to SDK.
   */
  void RegisterCollectorCallback(
      std::function<bool(MetricReader &,
                         AggregationTemporarily,
                         nostd::function_ref<bool(MetricData)>)> measurement_processor_callback)
  {
    measurement_processor_callback_ = measurement_processor_callback;
  }

  /**
   * Process the metrics received through Measurement Processor.
   */
  virtual bool ProcessReceivedMetrics(MetricData &metric_data) noexcept = 0;

  /**
   * Shut down the metric reader.
   * @param timeout an optional timeout.
   * @return return the status of the operation.
   */
  virtual bool Shutdown() noexcept = 0;

private:
  std::function<bool(MetricReader &, AggregationTemporarily, nostd::function_ref<bool(MetricData)>)>
      measurement_processor_callback_;
  AggregationTemporarily aggregation_temporarily_;
};
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
