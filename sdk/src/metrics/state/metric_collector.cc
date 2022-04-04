// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/state/metric_collector.h"
#  include "opentelemetry/sdk/common/global_log_handler.h"
#  include "opentelemetry/sdk/metrics/meter.h"
#  include "opentelemetry/sdk/metrics/meter_context.h"
#  include "opentelemetry/sdk/metrics/metric_reader.h"
#  include "opentelemetry/sdk_config.h"
#  include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

MetricCollector::MetricCollector(
    std::shared_ptr<opentelemetry::sdk::metrics::MeterContext> &&context,
    std::unique_ptr<MetricReader> metric_reader)
    : meter_context_{std::move(context)}, metric_reader_{std::move(metric_reader)}
{
  metric_reader_->SetMetricProducer(this);
}

AggregationTemporality MetricCollector::GetAggregationTemporality() noexcept
{
  return metric_reader_->GetAggregationTemporality();
}

bool MetricCollector::Collect(
    nostd::function_ref<bool(ResourceMetrics &metric_data)> callback) noexcept
{
  ResourceMetrics resource_metrics;
  for (auto &meter : meter_context_->GetMeters())
  {
    auto collection_ts = std::chrono::system_clock::now();
    InstrumentationInfoMetrics instrumentation_info_metrics;
    instrumentation_info_metrics.metric_data_             = meter->Collect(this, collection_ts);
    instrumentation_info_metrics.instrumentation_library_ = meter->GetInstrumentationLibrary();
    resource_metrics.instrumentation_info_metric_data_.push_back(instrumentation_info_metrics);
  }
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
#endif
