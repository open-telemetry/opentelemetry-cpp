// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/configuration/extension_span_processor_configuration.h"
#include "opentelemetry/sdk/init/registry.h"
#include "opentelemetry/sdk/trace/processor.h"

#include "custom_span_processor.h"
#include "custom_span_processor_builder.h"

std::unique_ptr<opentelemetry::sdk::trace::SpanProcessor> CustomSpanProcessorBuilder::Build(
    const opentelemetry::sdk::configuration::ExtensionSpanProcessorConfiguration *model) const
{
  // Read yaml attributes
  std::string comment = model->node->GetRequiredString("comment");

  std::unique_ptr<opentelemetry::sdk::trace::SpanProcessor> sdk(new CustomSpanProcessor(comment));

  return sdk;
}

static CustomSpanProcessorBuilder singleton;

void CustomSpanProcessorBuilder::Register(opentelemetry::sdk::init::Registry *registry)
{
  registry->AddExtensionSpanProcessorBuilder("custom", &singleton);
};
