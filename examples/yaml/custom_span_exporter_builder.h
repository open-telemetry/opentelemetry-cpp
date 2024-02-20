// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/sdk/init/extension_span_exporter_builder.h"

class CustomSpanExporterBuilder : public opentelemetry::sdk::init::ExtensionSpanExporterBuilder
{
public:
  static void Register(opentelemetry::sdk::init::Registry *registry);

  CustomSpanExporterBuilder()           = default;
  ~CustomSpanExporterBuilder() override = default;

  std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> Build(
      const opentelemetry::sdk::configuration::ExtensionSpanExporterConfiguration *model)
      const override;
};
