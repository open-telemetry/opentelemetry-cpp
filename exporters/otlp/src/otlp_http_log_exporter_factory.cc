// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_LOGS_PREVIEW

#  include "opentelemetry/exporters/otlp/otlp_http_log_exporter_factory.h"
#  include "opentelemetry/exporters/otlp/otlp_http_log_exporter.h"
#  include "opentelemetry/exporters/otlp/otlp_http_log_exporter_options.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

std::unique_ptr<opentelemetry::sdk::logs::LogExporter> OtlpHttpLogExporterFactory::Create()
{
  OtlpHttpLogExporterOptions options;
  return Create(options);
}

std::unique_ptr<opentelemetry::sdk::logs::LogExporter> OtlpHttpLogExporterFactory::Create(
    const OtlpHttpLogExporterOptions &options)
{
  std::unique_ptr<opentelemetry::sdk::logs::LogExporter> exporter(new OtlpHttpLogExporter(options));
  return exporter;
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE

#endif
