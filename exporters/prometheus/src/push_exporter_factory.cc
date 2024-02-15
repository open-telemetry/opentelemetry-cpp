// Copyright 2023, OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/prometheus/push_exporter_factory.h"

#include <prometheus/labels.h>

#include <memory>

#include "opentelemetry/exporters/prometheus/push_exporter.h"
#include "opentelemetry/exporters/prometheus/push_exporter_options.h"

OPENTELEMETRY_BEGIN_NAMESPACE

namespace exporter
{
namespace metrics
{

std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter>
PrometheusPushExporterFactory::Create(const PrometheusPushExporterOptions &options)
{
  return std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter>(
      new PrometheusPushExporter(options));
}

}  // namespace metrics
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
