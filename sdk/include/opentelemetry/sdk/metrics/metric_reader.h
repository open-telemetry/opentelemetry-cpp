// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/common/spin_lock_mutex.h"
#  include "opentelemetry/sdk/common/global_log_handler.h"
#  include "opentelemetry/sdk/metrics/data/metric_data.h"
#  include "opentelemetry/sdk/metrics/export/metric_producer.h"
#  include "opentelemetry/sdk/metrics/instruments.h"
#  include "opentelemetry/version.h"

#  include <chrono>
#  include <memory>

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
      AggregationTemporality aggregation_temporality = AggregationTemporality::kCumulative);

  void SetMetricProducer(MetricProducer *metric_producer);

  /**
   * Collect the metrics from SDK.
   * @return return the status of the operation.
   */
  bool Collect(nostd::function_ref<bool(ResourceMetrics &metric_data)> callback) noexcept;

  AggregationTemporality GetAggregationTemporality() const noexcept;

  /**
   * Shutdown the meter reader.
   */
  bool Shutdown(std::chrono::microseconds timeout = std::chrono::microseconds::max()) noexcept;

  /**
   * Force flush the metric read by the reader.
   */
  bool ForceFlush(std::chrono::microseconds timeout = std::chrono::microseconds::max()) noexcept;

  virtual ~MetricReader() = default;

private:
  virtual bool OnForceFlush(std::chrono::microseconds timeout) noexcept = 0;

  virtual bool OnShutDown(std::chrono::microseconds timeout) noexcept = 0;

  virtual void OnInitialized() noexcept {}

protected:
  bool IsShutdown() const noexcept;

private:
  MetricProducer *metric_producer_;
  AggregationTemporality aggregation_temporality_;
  mutable opentelemetry::common::SpinLockMutex lock_;
  bool shutdown_;
};
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
