// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <string>
#include <utility>

#include "opentelemetry/sdk/configuration/document_node.h"
#include "opentelemetry/sdk/configuration/extension_span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/registry.h"
#include "opentelemetry/sdk/trace/exporter.h"

#include "custom_span_exporter.h"
#include "custom_span_exporter_builder.h"

std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> CustomSpanExporterBuilder::Build(
    const opentelemetry::sdk::configuration::ExtensionSpanExporterConfiguration *model) const
{
  // Read yaml attributes
  std::string comment = model->node->GetRequiredString("comment");

  auto sdk = std::make_unique<CustomSpanExporter>(comment);

  return sdk;
}

void CustomSpanExporterBuilder::Register(opentelemetry::sdk::configuration::Registry *registry)
{
  auto builder = std::make_unique<CustomSpanExporterBuilder>();
  registry->SetExtensionSpanExporterBuilder("my_custom_span_exporter", std::move(builder));
}
