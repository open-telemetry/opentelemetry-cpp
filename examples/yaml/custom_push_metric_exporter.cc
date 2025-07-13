// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/metrics/push_metric_exporter.h"

#include "custom_push_metric_exporter.h"

opentelemetry::sdk::common::ExportResult CustomPushMetricExporter::Export(
    const opentelemetry::sdk::metrics::ResourceMetrics & /* data */) noexcept
{
  OTEL_INTERNAL_LOG_ERROR("CustomPushMetricExporter::Export(): YOUR CODE HERE");
  return opentelemetry::sdk::common::ExportResult::kSuccess;
}

opentelemetry::sdk::metrics::AggregationTemporality
CustomPushMetricExporter::GetAggregationTemporality(
    opentelemetry::sdk::metrics::InstrumentType /* instrument_type */) const noexcept
{
  OTEL_INTERNAL_LOG_ERROR("CustomPushMetricExporter::GetAggregationTemporality(): YOUR CODE HERE");
  return opentelemetry::sdk::metrics::AggregationTemporality::kCumulative;
}

bool CustomPushMetricExporter::ForceFlush(std::chrono::microseconds /* timeout */) noexcept
{
  OTEL_INTERNAL_LOG_ERROR("CustomPushMetricExporter::ForceFlush(): YOUR CODE HERE");
  return true;
}

bool CustomPushMetricExporter::Shutdown(std::chrono::microseconds /* timeout */) noexcept
{
  OTEL_INTERNAL_LOG_ERROR("CustomPushMetricExporter::Shutdown(): YOUR CODE HERE");
  return true;
}
