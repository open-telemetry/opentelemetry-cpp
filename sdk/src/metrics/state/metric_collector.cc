// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/metrics/state/metric_collector.h"
#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/metrics/meter.h"
#include "opentelemetry/sdk/metrics/meter_context.h"
#include "opentelemetry/sdk/metrics/metric_reader.h"
#include "opentelemetry/sdk_config.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

MetricCollector::MetricCollector(opentelemetry::sdk::metrics::MeterContext *context,
                                 std::shared_ptr<MetricReader> metric_reader)
    : meter_context_{context}, metric_reader_{metric_reader}
{
  metric_reader_->SetMetricProducer(this);
}

AggregationTemporality MetricCollector::GetAggregationTemporality(
    InstrumentType instrument_type) noexcept
{
  return metric_reader_->GetAggregationTemporality(instrument_type);
}

bool MetricCollector::Collect(
    nostd::function_ref<bool(ResourceMetrics &metric_data)> callback) noexcept
{
  if (!meter_context_)
  {
    OTEL_INTERNAL_LOG_ERROR("[MetricCollector::Collect] - Error during collecting."
                            << "The metric context is invalid");
    return false;
  }
  ResourceMetrics resource_metrics;
  meter_context_->ForEachMeter([&](std::shared_ptr<Meter> meter) noexcept {
    auto collection_ts = std::chrono::system_clock::now();
    auto metric_data   = meter->Collect(this, collection_ts);
    if (!metric_data.empty())
    {
      ScopeMetrics scope_metrics;
      scope_metrics.metric_data_ = std::move(metric_data);
      scope_metrics.scope_       = meter->GetInstrumentationScope();
      resource_metrics.scope_metric_data_.emplace_back(std::move(scope_metrics));
    }
    return true;
  });
  resource_metrics.resource_ = &meter_context_->GetResource();
  callback(resource_metrics);
  return true;
}

bool MetricCollector::ForceFlush(std::chrono::microseconds timeout) noexcept
{
  return metric_reader_->ForceFlush(timeout);
}

bool MetricCollector::Shutdown(std::chrono::microseconds timeout) noexcept
{
  return metric_reader_->Shutdown(timeout);
}

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
