// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/state/metric_collector.h"
#  include "opentelemetry/sdk/common/global_log_handler.h"
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
    : meter_context_{context}, metric_reader_{std::move(metric_reader)}
{
  metric_reader_->SetMetricProducer(this->shared_from_this());
}

bool MetricCollector::Collect(nostd::function_ref<bool(MetricData)> callback) noexcept
{
  return metric_reader_->Collect(callback);
}

bool MetricCollector::ForceFlush(std::chrono::microseconds timeout) noexcept
{
  return metric_reader_->ForceFlush(timeout);
}

bool MetricCollector::Shutdown() noexcept
{
  return metric_reader_->Shutdown();
}

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif