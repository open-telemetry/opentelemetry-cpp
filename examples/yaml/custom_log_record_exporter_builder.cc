// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <string>

#include "opentelemetry/sdk/configuration/document_node.h"
#include "opentelemetry/sdk/configuration/extension_log_record_exporter_configuration.h"
#include "opentelemetry/sdk/init/registry.h"
#include "opentelemetry/sdk/logs/exporter.h"

#include "custom_log_record_exporter.h"
#include "custom_log_record_exporter_builder.h"

std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> CustomLogRecordExporterBuilder::Build(
    const opentelemetry::sdk::configuration::ExtensionLogRecordExporterConfiguration *model) const
{
  // Read yaml attributes
  std::string comment = model->node->GetRequiredString("comment");

  auto sdk = std::make_unique<CustomLogRecordExporter>(comment);

  return sdk;
}

static CustomLogRecordExporterBuilder singleton;

void CustomLogRecordExporterBuilder::Register(opentelemetry::sdk::init::Registry *registry)
{
  registry->AddExtensionLogRecordExporterBuilder("my_custom_log_record_exporter", &singleton);
}
