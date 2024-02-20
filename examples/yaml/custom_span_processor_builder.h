// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/sdk/init/extension_span_processor_builder.h"

class CustomSpanProcessorBuilder : public opentelemetry::sdk::init::ExtensionSpanProcessorBuilder
{
public:
  static void Register(opentelemetry::sdk::init::Registry *registry);

  CustomSpanProcessorBuilder()           = default;
  ~CustomSpanProcessorBuilder() override = default;

  std::unique_ptr<opentelemetry::sdk::trace::SpanProcessor> Build(
      const opentelemetry::sdk::configuration::ExtensionSpanProcessorConfiguration *model)
      const override;
};
