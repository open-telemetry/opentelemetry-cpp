// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/export/periodic_exporting_metric_reader.h"
#  include "opentelemetry/sdk/common/global_log_handler.h"
#  include "opentelemetry/sdk/metrics/metric_exporter.h"

#  include <chrono>
#  include <future>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

PeriodicExportingMetricReader::PeriodicExportingMetricReader(
    std::unique_ptr<MetricExporter> exporter,
    const PeriodicExportingMetricReaderOptions &option,
    AggregationTemporality aggregation_temporality)
    : MetricReader(aggregation_temporality),
      exporter_{std::move(exporter)},
      export_interval_millis_{option.export_interval_millis},
      export_timeout_millis_{option.export_timeout_millis}
{
  if (export_interval_millis_ <= export_timeout_millis_)
  {
    OTEL_INTERNAL_LOG_WARN(
        "[Periodic Exporting Metric Reader] Invalid configuration: "
        "export_interval_millis_ should be less than export_timeout_millis_, using default values");
    export_interval_millis_ = kExportIntervalMillis;
    export_timeout_millis_  = kExportTimeOutMillis;
  }
}

void PeriodicExportingMetricReader::OnInitialized() noexcept
{
  worker_thread_ = std::thread(&PeriodicExportingMetricReader::DoBackgroundWork, this);
}

void PeriodicExportingMetricReader::DoBackgroundWork()
{
  std::unique_lock<std::mutex> lk(cv_m_);
  do
  {
    if (IsShutdown())
    {
      break;
    }
    std::atomic<bool> cancel_export_for_timeout{false};
    auto start          = std::chrono::steady_clock::now();
    auto future_receive = std::async(std::launch::async, [this, &cancel_export_for_timeout] {
      Collect([this, &cancel_export_for_timeout](ResourceMetrics &metric_data) {
        if (cancel_export_for_timeout)
        {
          OTEL_INTERNAL_LOG_ERROR(
              "[Periodic Exporting Metric Reader] Collect took longer configured time: "
              << export_timeout_millis_.count() << " ms, and timed out");
          return false;
        }
        this->exporter_->Export(metric_data);
        return true;
      });
    });
    std::future_status status;
    do
    {
      status = future_receive.wait_for(std::chrono::milliseconds(export_timeout_millis_));
      if (status == std::future_status::timeout)
      {
        cancel_export_for_timeout = true;
        break;
      }
    } while (status != std::future_status::ready);
    auto end            = std::chrono::steady_clock::now();
    auto export_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    auto remaining_wait_interval_ms = export_interval_millis_ - export_time_ms;
    cv_.wait_for(lk, remaining_wait_interval_ms);
  } while (true);
}

bool PeriodicExportingMetricReader::OnForceFlush(std::chrono::microseconds timeout) noexcept
{
  return exporter_->ForceFlush(timeout);
}

bool PeriodicExportingMetricReader::OnShutDown(std::chrono::microseconds timeout) noexcept
{
  if (worker_thread_.joinable())
  {
    cv_.notify_one();
    worker_thread_.join();
  }
  return exporter_->Shutdown(timeout);
}

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif