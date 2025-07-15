// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/otlp/otlp_http_log_record_builder.h"
#include "opentelemetry/exporters/otlp/otlp_http_log_record_exporter_factory.h"
#include "opentelemetry/exporters/otlp/otlp_http_log_record_exporter_options.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

void OtlpHttpLogRecordBuilder::Register(opentelemetry::sdk::configuration::Registry *registry)
{
  auto builder = std::make_unique<OtlpHttpLogRecordBuilder>();
  registry->SetOtlpHttpLogRecordBuilder(std::move(builder));
}

std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> OtlpHttpLogRecordBuilder::Build(
    const opentelemetry::sdk::configuration::OtlpHttpLogRecordExporterConfiguration * /* model */)
    const
{
  // FIXME, use model
  OtlpHttpLogRecordExporterOptions options;
  return OtlpHttpLogRecordExporterFactory::Create(options);
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
