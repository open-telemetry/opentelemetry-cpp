// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <string>
#include <utility>

#include "opentelemetry/sdk/configuration/document_node.h"
#include "opentelemetry/sdk/configuration/extension_pull_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/registry.h"
#include "opentelemetry/sdk/metrics/metric_reader.h"

#include "custom_pull_metric_exporter.h"
#include "custom_pull_metric_exporter_builder.h"

std::unique_ptr<opentelemetry::sdk::metrics::MetricReader> CustomPullMetricExporterBuilder::Build(
    const opentelemetry::sdk::configuration::ExtensionPullMetricExporterConfiguration *model) const
{
  // Read yaml attributes
  std::string comment = model->node->GetRequiredString("comment");

  auto sdk = std::make_unique<CustomPullMetricExporter>(comment);

  return sdk;
}

void CustomPullMetricExporterBuilder::Register(
    opentelemetry::sdk::configuration::Registry *registry)
{
  auto builder = std::make_unique<CustomPullMetricExporterBuilder>();
  registry->SetExtensionPullMetricExporterBuilder("my_custom_pull_metric_exporter",
                                                  std::move(builder));
}
