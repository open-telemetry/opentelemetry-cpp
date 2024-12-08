// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/ostream/console_log_record_builder.h"
#include "opentelemetry/exporters/ostream/log_record_exporter_factory.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace logs
{

static ConsoleLogRecordBuilder singleton;

void ConsoleLogRecordBuilder::Register(opentelemetry::sdk::init::Registry *registry)
{
  registry->SetConsoleLogRecordBuilder(&singleton);
}

std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> ConsoleLogRecordBuilder::Build(
    const opentelemetry::sdk::configuration::ConsoleLogRecordExporterConfiguration * /* model */)
    const
{
  return OStreamLogRecordExporterFactory::Create();
}

}  // namespace logs
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE