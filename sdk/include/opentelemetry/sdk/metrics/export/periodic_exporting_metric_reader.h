// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/sdk/metrics/metric_reader.h"
#include "opentelemetry/version.h"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <thread>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

class PushMetricExporter;
/**
 * Struct to hold PeriodicExortingMetricReader options.
 */

constexpr std::chrono::milliseconds kExportIntervalMillis = std::chrono::milliseconds(60000);
constexpr std::chrono::milliseconds kExportTimeOutMillis  = std::chrono::milliseconds(30000);
struct PeriodicExportingMetricReaderOptions
{

  /* The time interval between two consecutive exports. */
  std::chrono::milliseconds export_interval_millis =
      std::chrono::milliseconds(kExportIntervalMillis);

  /*  how long the export can run before it is cancelled. */
  std::chrono::milliseconds export_timeout_millis = std::chrono::milliseconds(kExportTimeOutMillis);
};

class PeriodicExportingMetricReader : public MetricReader
{

public:
  PeriodicExportingMetricReader(std::unique_ptr<PushMetricExporter> exporter,
                                const PeriodicExportingMetricReaderOptions &option);

  AggregationTemporality GetAggregationTemporality(
      InstrumentType instrument_type) const noexcept override;

private:
  bool OnForceFlush(std::chrono::microseconds timeout) noexcept override;

  bool OnShutDown(std::chrono::microseconds timeout) noexcept override;

  void OnInitialized() noexcept override;

  std::unique_ptr<PushMetricExporter> exporter_;
  std::chrono::milliseconds export_interval_millis_;
  std::chrono::milliseconds export_timeout_millis_;

  std::mutex force_flush_m_;
  std::atomic<bool> is_force_flush_pending_;
  std::atomic<bool> is_force_wakeup_background_worker_;


  void DoBackgroundWork();
  bool CollectAndExportOnce();

  /* The background worker thread */
  std::thread worker_thread_;

  /* Synchronization primitives */
  std::condition_variable cv_;
  std::mutex cv_m_;
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
