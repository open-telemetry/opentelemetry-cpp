// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/ostream/console_push_metric_builder.h"
#include "opentelemetry/exporters/ostream/metric_exporter_factory.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace metrics
{

static ConsolePushMetricBuilder singleton;

void ConsolePushMetricBuilder::Register(opentelemetry::sdk::init::Registry *registry)
{
  registry->SetConsolePushMetricExporterBuilder(&singleton);
}

std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter> ConsolePushMetricBuilder::Build(
    const opentelemetry::sdk::configuration::ConsolePushMetricExporterConfiguration * /* model */)
    const
{
  return OStreamMetricExporterFactory::Create();
}

}  // namespace metrics
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
