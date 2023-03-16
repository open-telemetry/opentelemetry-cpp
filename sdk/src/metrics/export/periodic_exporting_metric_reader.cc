// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/metrics/export/periodic_exporting_metric_reader.h"
#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/metrics/push_metric_exporter.h"

#include <chrono>
#if defined(_MSC_VER)
#  pragma warning(suppress : 5204)
#  include <future>
#else
#  include <future>
#endif

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

PeriodicExportingMetricReader::PeriodicExportingMetricReader(
    std::unique_ptr<PushMetricExporter> exporter,
    const PeriodicExportingMetricReaderOptions &option)
    : exporter_{std::move(exporter)},
      export_interval_millis_{option.export_interval_millis},
      export_timeout_millis_{option.export_timeout_millis}
{
  if (export_interval_millis_ <= export_timeout_millis_)
  {
    OTEL_INTERNAL_LOG_WARN(
        "[Periodic Exporting Metric Reader] Invalid configuration: "
        "export_timeout_millis_ should be less than export_interval_millis_, using default values");
    export_interval_millis_ = kExportIntervalMillis;
    export_timeout_millis_  = kExportTimeOutMillis;
  }
}

AggregationTemporality PeriodicExportingMetricReader::GetAggregationTemporality(
    InstrumentType instrument_type) const noexcept
{
  return exporter_->GetAggregationTemporality(instrument_type);
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
    auto start  = std::chrono::steady_clock::now();
    auto status = CollectAndExportOnce();
    if (!status)
    {
      OTEL_INTERNAL_LOG_ERROR("[Periodic Exporting Metric Reader]  Collect-Export Cycle Failure.")
    }
    auto end            = std::chrono::steady_clock::now();
    auto export_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    auto remaining_wait_interval_ms = export_interval_millis_ - export_time_ms;
    cv_.wait_for(lk, remaining_wait_interval_ms, [this]() { 
      if (is_force_wakeup_background_worker_.load(std::memory_order_acquire))
      {
        return true;
      }
      if (IsShutdown())
      {
        return true;
      }
      return false;
    });
    is_force_wakeup_background_worker_.store(false, std::memory_order_release);
  } while (IsShutdown() != true);
  // One last Collect and Export before shutdown
  auto status = CollectAndExportOnce();
  if (!status)
  {
    OTEL_INTERNAL_LOG_ERROR("[Periodic Exporting Metric Reader]  Collect-Export Cycle Failure.")
  }
}

bool PeriodicExportingMetricReader::CollectAndExportOnce()
{
  std::atomic<bool> cancel_export_for_timeout{false};
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
  bool notify_force_flush =
        is_force_flush_pending_.exchange(false, std::memory_order_acq_rel);
  if (notify_force_flush)
  {
    is_force_flush_notified_.store(true, std::memory_order_release);
    force_flush_cv_.notify_one();
  }
  
  return true;
}

bool PeriodicExportingMetricReader::OnForceFlush(std::chrono::microseconds timeout) noexcept
{
  std::unique_lock<std::mutex> lk_cv(force_flush_m_);
  is_force_flush_pending_.store(true, std::memory_order_release);
  auto break_condition = [this]() {
    if (IsShutdown())
    {
      return true;
    }

    // Wake up the worker thread once.
    if (is_force_flush_pending_.load(std::memory_order_acquire))
    {
      is_force_wakeup_background_worker_.store(true, std::memory_order_release);
      cv_.notify_one();
    }

    return synchronization_data_->is_force_flush_notified.load(std::memory_order_acquire);
  };



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
