// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/metric_reader.h"
#  include "opentelemetry/sdk/metrics/export/metric_producer.h"

#  include <mutex>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

MetricReader::MetricReader(AggregationTemporality aggregation_temporality)
    : aggregation_temporality_(aggregation_temporality)
{}

void MetricReader::SetMetricProducer(MetricProducer *metric_producer)
{
  metric_producer_ = metric_producer;
}

AggregationTemporality MetricReader::GetAggregationTemporality() const noexcept
{
  return aggregation_temporality_;
}

bool MetricReader::Collect(nostd::function_ref<bool(MetricData)> callback) noexcept
{
  if (!metric_producer_)
  {
    OTEL_INTERNAL_LOG_WARN(
        "MetricReader::Collect Cannot invoke Collect(). No MetricProducer registered for "
        "collection!")
  }
  if (IsShutdown())
  {
    OTEL_INTERNAL_LOG_WARN("MetricReader::Collect Cannot invoke Collect(). Shutdown in progress!");
  }

  return metric_producer_->Collect(callback);
}

bool MetricReader::Shutdown(std::chrono::microseconds timeout) noexcept
{
  bool status = true;

  if (IsShutdown())
  {
    OTEL_INTERNAL_LOG_WARN("MetricReader::Shutdown - Cannot invoke shutdown twice!");
  }
  if (!OnShutDown(timeout))
  {
    status = false;
    OTEL_INTERNAL_LOG_WARN("MetricReader::OnShutDown Shutdown failed. Will not be tried again!");
  }
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  shutdown_ = true;
  return status;
}

/** Flush metric read by this reader **/
bool MetricReader::ForceFlush(std::chrono::microseconds timeout) noexcept
{
  bool status = true;
  if (shutdown_)
  {
    OTEL_INTERNAL_LOG_WARN("MetricReader::Shutdown Cannot invoke Force flush on shutdown reader!");
  }
  if (!OnForceFlush(timeout))
  {
    status = false;
    OTEL_INTERNAL_LOG_ERROR("MetricReader::OnForceFlush failed!");
  }
  return status;
}

bool MetricReader::IsShutdown() const noexcept
{
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  return shutdown_;
}

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
