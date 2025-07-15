// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <string>
#include <utility>

#include "opentelemetry/sdk/configuration/document_node.h"
#include "opentelemetry/sdk/configuration/extension_push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/registry.h"
#include "opentelemetry/sdk/metrics/push_metric_exporter.h"

#include "custom_push_metric_exporter.h"
#include "custom_push_metric_exporter_builder.h"

std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter>
CustomPushMetricExporterBuilder::Build(
    const opentelemetry::sdk::configuration::ExtensionPushMetricExporterConfiguration *model) const
{
  // Read yaml attributes
  std::string comment = model->node->GetRequiredString("comment");

  auto sdk = std::make_unique<CustomPushMetricExporter>(comment);

  return sdk;
}

void CustomPushMetricExporterBuilder::Register(
    opentelemetry::sdk::configuration::Registry *registry)
{
  auto builder = std::make_unique<CustomPushMetricExporterBuilder>();
  registry->SetExtensionPushMetricExporterBuilder("my_custom_push_metric_exporter",
                                                  std::move(builder));
}
