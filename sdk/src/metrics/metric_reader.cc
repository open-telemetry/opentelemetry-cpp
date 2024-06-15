// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/metrics/metric_reader.h"
#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/metrics/export/metric_producer.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

MetricReader::MetricReader() : metric_producer_(nullptr), shutdown_(false) {}

void MetricReader::SetMetricProducer(MetricProducer *metric_producer)
{
  metric_producer_ = metric_producer;
  OnInitialized();
}

bool MetricReader::Collect(
    nostd::function_ref<bool(ResourceMetrics &metric_data)> callback) noexcept
{
  if (!metric_producer_)
  {
    OTEL_INTERNAL_LOG_WARN(
        "MetricReader::Collect Cannot invoke Collect(). No MetricProducer registered for "
        "collection!")
    return false;
  }
  if (IsShutdown())
  {
    // Continue with warning, and let pull and push MetricReader state machine handle this.
    OTEL_INTERNAL_LOG_WARN("MetricReader::Collect invoked while Shutdown in progress!");
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

  shutdown_.store(true, std::memory_order_release);

  if (!OnShutDown(timeout))
  {
    status = false;
    OTEL_INTERNAL_LOG_WARN("MetricReader::OnShutDown Shutdown failed. Will not be tried again!");
  }
  return status;
}

/** Flush metric read by this reader **/
bool MetricReader::ForceFlush(std::chrono::microseconds timeout) noexcept
{
  bool status = true;
  if (IsShutdown())
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
  return shutdown_.load(std::memory_order_acquire);
}

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
