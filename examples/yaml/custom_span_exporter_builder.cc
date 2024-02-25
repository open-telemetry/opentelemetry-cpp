// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/configuration/extension_span_exporter_configuration.h"
#include "opentelemetry/sdk/init/registry.h"
#include "opentelemetry/sdk/trace/exporter.h"

#include "custom_span_exporter.h"
#include "custom_span_exporter_builder.h"

std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> CustomSpanExporterBuilder::Build(
    const opentelemetry::sdk::configuration::ExtensionSpanExporterConfiguration *model) const
{
  // Read yaml attributes
  std::string comment = model->node->GetRequiredString("comment");

  std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> sdk(new CustomSpanExporter(comment));

  return sdk;
}

static CustomSpanExporterBuilder singleton;

void CustomSpanExporterBuilder::Register(opentelemetry::sdk::init::Registry *registry)
{
  registry->AddExtensionSpanExporterBuilder("my_custom_span_exporter", &singleton);
};
