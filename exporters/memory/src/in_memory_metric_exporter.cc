// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW

#  include "opentelemetry/exporters/memory/in_memory_metric_exporter.h"
#  include "opentelemetry/exporters/otlp/otlp_metric_utils.h"
#  include "opentelemetry/sdk_config.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace metrics
{

sdk::common::ExportResult InMemoryMetricExporter::Export(
    const sdk::metrics::ResourceMetrics &data) noexcept
{
  if (isShutdown())
  {
    OTEL_INTERNAL_LOG_ERROR("[InMemory Metric] Exporting "
                            << data.instrumentation_info_metric_data_.size()
                            << " records(s) failed, exporter is shutdown");
    return sdk::common::ExportResult::kFailure;
  }

  std::unique_ptr<proto::metrics::v1::ResourceMetrics> resource_metrics(
      new proto::metrics::v1::ResourceMetrics);
  otlp::OtlpMetricUtils::PopulateResourceMetrics(data, resource_metrics.get());
  data_->Add(std::move(resource_metrics));
  return sdk::common::ExportResult::kSuccess;
}

bool InMemoryMetricExporter::ForceFlush(std::chrono::microseconds timeout) noexcept
{
  return true;
}

bool InMemoryMetricExporter::Shutdown(std::chrono::microseconds timeout) noexcept
{
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  is_shutdown_ = true;
  return true;
}

bool InMemoryMetricExporter::isShutdown() const noexcept
{
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  return is_shutdown_;
}
}  // namespace metrics
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
#endif
