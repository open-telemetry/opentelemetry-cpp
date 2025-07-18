// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/common/global_log_handler.h"

#include "custom_pull_metric_exporter.h"

opentelemetry::sdk::metrics::AggregationTemporality
CustomPullMetricExporter::GetAggregationTemporality(
    opentelemetry::sdk::metrics::InstrumentType /* instrument_type */) const noexcept
{
  OTEL_INTERNAL_LOG_ERROR("CustomPullMetricExporter::GetAggregationTemporality(): YOUR CODE HERE");
  return opentelemetry::sdk::metrics::AggregationTemporality::kCumulative;
}

bool CustomPullMetricExporter::OnForceFlush(std::chrono::microseconds /* timeout */) noexcept
{
  OTEL_INTERNAL_LOG_ERROR("CustomPullMetricExporter::OnForceFlush(): YOUR CODE HERE");
  return true;
}

bool CustomPullMetricExporter::OnShutDown(std::chrono::microseconds /* timeout */) noexcept
{
  OTEL_INTERNAL_LOG_ERROR("CustomPullMetricExporter::OnShutDown(): YOUR CODE HERE");
  return true;
}

void CustomPullMetricExporter::OnInitialized() noexcept
{
  OTEL_INTERNAL_LOG_ERROR("CustomPullMetricExporter::OnInitialized(): YOUR CODE HERE");
}
