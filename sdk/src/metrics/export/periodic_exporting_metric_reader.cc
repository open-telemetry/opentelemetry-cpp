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
        is_force_wakeup_background_worker_.store(false, std::memory_order_release);
        return true;
      }
      return IsShutdown();
    });
  } while (IsShutdown() != true);
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
  bool notify_force_flush = is_force_flush_pending_.exchange(false, std::memory_order_acq_rel);
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
    return is_force_flush_notified_.load(std::memory_order_acquire);
  };

  auto wait_timeout = opentelemetry::common::DurationUtil::AdjustWaitForTimeout(
      timeout, std::chrono::microseconds::zero());
  std::chrono::steady_clock::duration timeout_steady =
      std::chrono::duration_cast<std::chrono::steady_clock::duration>(wait_timeout);
  if (timeout_steady <= std::chrono::steady_clock::duration::zero())
  {
    timeout_steady = (std::chrono::steady_clock::duration::max)();
  }

  bool result = false;
  while (!result && timeout_steady > std::chrono::steady_clock::duration::zero())
  {
    // When is_force_flush_notified_.store(true) and force_flush_cv_.notify_all() is called
    // between is_force_flush_pending_.load() and force_flush_cv_.wait(). We must not wait
    // for ever
    std::chrono::steady_clock::time_point start_timepoint = std::chrono::steady_clock::now();
    result = force_flush_cv_.wait_for(lk_cv, export_interval_millis_, break_condition);
    timeout_steady -= std::chrono::steady_clock::now() - start_timepoint;
  }

  // If it will be already signaled, we must wait until notified.
  // We use a spin lock here
  if (false == is_force_flush_pending_.exchange(false, std::memory_order_acq_rel))
  {
    for (int retry_waiting_times = 0;
         false == is_force_flush_notified_.load(std::memory_order_acquire); ++retry_waiting_times)
    {
      opentelemetry::common::SpinLockMutex::fast_yield();
      if ((retry_waiting_times & 127) == 127)
      {
        std::this_thread::yield();
      }
    }
  }
  is_force_flush_notified_.store(false, std::memory_order_release);

  if (result)
  {
    // - If original `timeout` is `zero`, use that in exporter::forceflush
    // - Else if remaining `timeout_steady` more than zero, use that in exporter::forceflush
    // - Else don't invoke exporter::forceflush ( as remaining time is zero or less)
    if (timeout <= std::chrono::steady_clock::duration::zero())
    {
      result =
          exporter_->ForceFlush(std::chrono::duration_cast<std::chrono::microseconds>(timeout));
    }
    else if (timeout_steady > std::chrono::steady_clock::duration::zero())
    {
      result = exporter_->ForceFlush(
          std::chrono::duration_cast<std::chrono::microseconds>(timeout_steady));
    }
    else
    {
      // remaining timeout_steady is zero or less
      result = false;
    }
  }
  return result;
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
