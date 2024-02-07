// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "opentelemetry/sdk/init/console_span_exporter_builder.h"
#include "opentelemetry/sdk/init/registry.h"
#include "opentelemetry/sdk/trace/exporter.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace trace
{

class OPENTELEMETRY_EXPORT ConsoleBuilder
    : public opentelemetry::sdk::init::ConsoleSpanExporterBuilder
{
public:
  static void Register(opentelemetry::sdk::init::Registry *registry);

  ConsoleBuilder()           = default;
  ~ConsoleBuilder() override = default;

  std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> Build(
      const opentelemetry::sdk::configuration::ConsoleSpanExporterConfiguration *model)
      const override;
};

}  // namespace trace
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
