// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <iostream>
#include <memory>
#include <utility>

#include "opentelemetry/exporters/ostream/console_push_metric_builder.h"
#include "opentelemetry/exporters/ostream/metric_exporter_factory.h"
#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/configuration/console_push_metric_exporter_builder.h"
#include "opentelemetry/sdk/configuration/console_push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/default_histogram_aggregation.h"
#include "opentelemetry/sdk/configuration/registry.h"
#include "opentelemetry/sdk/configuration/temporality_preference.h"
#include "opentelemetry/sdk/metrics/instruments.h"
#include "opentelemetry/sdk/metrics/push_metric_exporter.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace metrics
{

void ConsolePushMetricBuilder::Register(opentelemetry::sdk::configuration::Registry *registry)
{
  auto builder = std::make_unique<ConsolePushMetricBuilder>();
  registry->SetConsolePushMetricExporterBuilder(std::move(builder));
}

std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter> ConsolePushMetricBuilder::Build(
    const opentelemetry::sdk::configuration::ConsolePushMetricExporterConfiguration *model) const
{
  // FIXME-SDK: default_histogram_aggregation is parsed but not implemented by the SDK.
  if (model->default_histogram_aggregation !=
      opentelemetry::sdk::configuration::DefaultHistogramAggregation::explicit_bucket_histogram)
  {
    OTEL_INTERNAL_LOG_WARN(
        "[Console Exporter] default_histogram_aggregation is not supported and will be ignored");
  }

  sdk::metrics::AggregationTemporality aggregation_temporality{
      sdk::metrics::AggregationTemporality::kUnspecified};

  switch (model->temporality_preference)
  {
    case opentelemetry::sdk::configuration::TemporalityPreference::delta:
      aggregation_temporality = sdk::metrics::AggregationTemporality::kDelta;
      break;
    case opentelemetry::sdk::configuration::TemporalityPreference::low_memory:
      /*
       * FIXME: OStreamMetricExporter does not support "low_memory"
       * temporality
       */
      OTEL_INTERNAL_LOG_WARN("[Console Exporter] TemporalityPreference::low_memory not supported");
      aggregation_temporality = sdk::metrics::AggregationTemporality::kCumulative;
      break;
    case opentelemetry::sdk::configuration::TemporalityPreference::cumulative:
    default:
      aggregation_temporality = sdk::metrics::AggregationTemporality::kCumulative;
      break;
  };

  return OStreamMetricExporterFactory::Create(std::cout, aggregation_temporality);
}

}  // namespace metrics
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
