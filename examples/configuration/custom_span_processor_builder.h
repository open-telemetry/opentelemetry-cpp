// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "opentelemetry/sdk/configuration/extension_span_processor_builder.h"
#include "opentelemetry/sdk/configuration/extension_span_processor_configuration.h"
#include "opentelemetry/sdk/configuration/registry.h"
#include "opentelemetry/sdk/trace/processor.h"

class CustomSpanProcessorBuilder
    : public opentelemetry::sdk::configuration::ExtensionSpanProcessorBuilder
{
public:
  static void Register(opentelemetry::sdk::configuration::Registry *registry);

  std::unique_ptr<opentelemetry::sdk::trace::SpanProcessor> Build(
      const opentelemetry::sdk::configuration::ExtensionSpanProcessorConfiguration *model)
      const override;
};
