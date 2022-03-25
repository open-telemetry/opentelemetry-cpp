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
      schedule_delay_millis_{option.schedule_delay_millis},
      export_timeout_millis_{option.export_timeout_millis}
{}

void PeriodicExportingMetricReader::OnInitialized() noexcept
{
  worker_thread_ = std::thread(&PeriodicExportingMetricReader::DoBackgroundWork, this);
}

void PeriodicExportingMetricReader::DoBackgroundWork()
{
  auto timeout = schedule_delay_millis_;
  std::unique_lock<std::mutex> lk(cv_m_);
  do
  {
    cv_.wait_for(lk, timeout);

    if (IsShutdown())
    {
      break;
    }

    std::atomic<bool> cancel_export_for_timeout{false};
    auto future_receive = std::async(std::launch::async, [this, &cancel_export_for_timeout] {
      Collect([this, &cancel_export_for_timeout](MetricData data) {
        if (cancel_export_for_timeout)
        {
          OTEL_INTERNAL_LOG_ERROR(
              "[Periodic Exporting Metric Reader] Collect took longer configured time: "
              << export_timeout_millis_.count() << " ms, and timed out");
          return false;
        }
        this->exporter_->Export(data);
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
      }
    } while (status != std::future_status::ready);

  } while (true);
}
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif