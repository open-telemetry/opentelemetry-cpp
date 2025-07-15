// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <string>

#include "opentelemetry/sdk/configuration/document_node.h"
#include "opentelemetry/sdk/configuration/extension_span_processor_configuration.h"
#include "opentelemetry/sdk/configuration/registry.h"
#include "opentelemetry/sdk/trace/processor.h"

#include "custom_span_processor.h"
#include "custom_span_processor_builder.h"

std::unique_ptr<opentelemetry::sdk::trace::SpanProcessor> CustomSpanProcessorBuilder::Build(
    const opentelemetry::sdk::configuration::ExtensionSpanProcessorConfiguration *model) const
{
  // Read yaml attributes
  std::string comment = model->node->GetRequiredString("comment");

  auto sdk = std::make_unique<CustomSpanProcessor>(comment);

  return sdk;
}

static CustomSpanProcessorBuilder singleton;

void CustomSpanProcessorBuilder::Register(opentelemetry::sdk::configuration::Registry *registry)
{
  registry->AddExtensionSpanProcessorBuilder("my_custom_span_processor", &singleton);
}
