// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <atomic>
#include <chrono>
#include <cstddef>

#include "opentelemetry/nostd/function_ref.h"
#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/metrics/export/metric_producer.h"
#include "opentelemetry/sdk/metrics/instruments.h"
#include "opentelemetry/sdk/metrics/metric_reader.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

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
        "MetricReader::Collect Cannot invoke Produce(). No MetricProducer registered for "
        "collection!")
    return false;
  }
  if (IsShutdown())
  {
    // Continue with warning, and let pull and push MetricReader state machine handle this.
    OTEL_INTERNAL_LOG_WARN("MetricReader::Collect invoked while Shutdown in progress!");
  }

  auto result = metric_producer_->Produce();

  // According to the spec,
  //   When the Produce operation fails, the MetricProducer MAY return successfully collected
  //   results and a failed reasons list to the caller.
  // So we invoke the callback with whatever points we get back, even if the overall operation may
  // have failed.
  auto success = callback(result.points_);
  return (result.status_ == MetricProducer::Status::kSuccess) && success;
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

size_t MetricReader::GetCardinalityLimit(InstrumentType instrument_type) const noexcept
{
  switch (instrument_type)
  {
    case InstrumentType::kCounter:
      return cardinality_limit_options_.counter != 0 ? cardinality_limit_options_.counter
                                                     : cardinality_limit_options_.default_limit;
    case InstrumentType::kHistogram:
      return cardinality_limit_options_.histogram != 0 ? cardinality_limit_options_.histogram
                                                       : cardinality_limit_options_.default_limit;
    case InstrumentType::kUpDownCounter:
      return cardinality_limit_options_.up_down_counter != 0
                 ? cardinality_limit_options_.up_down_counter
                 : cardinality_limit_options_.default_limit;
    case InstrumentType::kObservableCounter:
      return cardinality_limit_options_.observable_counter != 0
                 ? cardinality_limit_options_.observable_counter
                 : cardinality_limit_options_.default_limit;
    case InstrumentType::kObservableGauge:
      return cardinality_limit_options_.observable_gauge != 0
                 ? cardinality_limit_options_.observable_gauge
                 : cardinality_limit_options_.default_limit;
    case InstrumentType::kObservableUpDownCounter:
      return cardinality_limit_options_.observable_up_down_counter != 0
                 ? cardinality_limit_options_.observable_up_down_counter
                 : cardinality_limit_options_.default_limit;
    case InstrumentType::kGauge:
      return cardinality_limit_options_.gauge != 0 ? cardinality_limit_options_.gauge
                                                   : cardinality_limit_options_.default_limit;
    default:
      return cardinality_limit_options_.default_limit;
  }
}

void MetricReader::SetCardinalityLimitOptions(const CardinalityLimitOptions &options) noexcept
{
  cardinality_limit_options_ = options;
}

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
