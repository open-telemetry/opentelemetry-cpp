// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <string>
#include <utility>

#include "opentelemetry/sdk/configuration/document_node.h"
#include "opentelemetry/sdk/configuration/extension_log_record_processor_configuration.h"
#include "opentelemetry/sdk/configuration/registry.h"
#include "opentelemetry/sdk/logs/processor.h"

#include "custom_log_record_processor.h"
#include "custom_log_record_processor_builder.h"

std::unique_ptr<opentelemetry::sdk::logs::LogRecordProcessor>
CustomLogRecordProcessorBuilder::Build(
    const opentelemetry::sdk::configuration::ExtensionLogRecordProcessorConfiguration *model) const
{
  // Read yaml attributes
  std::string comment = model->node->GetRequiredString("comment");

  auto sdk = std::make_unique<CustomLogRecordProcessor>(comment);

  return sdk;
}

void CustomLogRecordProcessorBuilder::Register(
    opentelemetry::sdk::configuration::Registry *registry)
{
  auto builder = std::make_unique<CustomLogRecordProcessorBuilder>();
  registry->SetExtensionLogRecordProcessorBuilder("my_custom_log_record_processor",
                                                  std::move(builder));
}
