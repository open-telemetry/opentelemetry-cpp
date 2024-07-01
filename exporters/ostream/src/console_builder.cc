// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/ostream/console_builder.h"
#include "opentelemetry/exporters/ostream/span_exporter.h"
#include "opentelemetry/exporters/ostream/span_exporter_factory.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace trace
{

static ConsoleBuilder singleton;

void ConsoleBuilder::Register(opentelemetry::sdk::init::Registry *registry)
{
  registry->SetConsoleBuilder(&singleton);
}

std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> ConsoleBuilder::Build(
    const opentelemetry::sdk::configuration::ConsoleSpanExporterConfiguration * /* model */) const
{
  return OStreamSpanExporterFactory::Create();
}

}  // namespace trace
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
