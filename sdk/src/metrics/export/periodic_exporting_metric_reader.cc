// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/export/periodic_exporting_metric_reader.h"
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
    const PeriodicExportingMetricReaderOptions &option)
    : exporter_{std::move(exporter)},
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
  cv_.wait_for(lk, timeout);

  std::atomic<bool> stop{false};
  auto future_receive = std::async(std::launch::async, [this, &stop] {
    Collect([this, &stop](MetricData data) {
      if (stop)
      {
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
      stop = true;
    }
  } while (status != std::future_status::ready);
}
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif