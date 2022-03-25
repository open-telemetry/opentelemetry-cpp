// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW

#  include "opentelemetry/sdk/metrics/metric_reader.h"
#  include "opentelemetry/version.h"

#  include <atomic>
#  include <chrono>
#  include <condition_variable>
#  include <thread>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

class MetricExporter;
/**
 * Struct to hold batch PeriodicExortingMetricReader options.
 */

struct PeriodicExportingMetricReaderOptions
{

  /* The time interval between two consecutive exports. */
  std::chrono::milliseconds schedule_delay_millis = std::chrono::milliseconds(60000);

  /*  how long the export can run before it is cancelled. */
  std::chrono::milliseconds export_timeout_millis = std::chrono::milliseconds(30000);
};

class PeriodicExportingMetricReader : public MetricReader
{

public:
  PeriodicExportingMetricReader(
      std::unique_ptr<MetricExporter> exporter,
      const PeriodicExportingMetricReaderOptions &option,
      AggregationTemporality aggregation_temporality = AggregationTemporality::kCummulative);

private:
  bool OnForceFlush(std::chrono::microseconds timeout) noexcept override;

  bool OnShutDown(std::chrono::microseconds timeout) noexcept override;

  void OnInitialized() noexcept override;

  std::unique_ptr<MetricExporter> exporter_;
  std::chrono::milliseconds schedule_delay_millis_;
  std::chrono::milliseconds export_timeout_millis_;

  void DoBackgroundWork();

  /* The background worker thread */
  std::thread worker_thread_;

  /* Synchronization primitives */
  std::condition_variable cv_, force_flush_cv_;
  std::mutex cv_m_, force_flush_cv_m_, shutdown_m_;
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
