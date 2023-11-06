// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <windows.h>

// Include API header files here for exporting
#include <opentelemetry/trace/provider.h>

#include <opentelemetry/logs/logger_provider.h>
#include <opentelemetry/logs/provider.h>

#include <opentelemetry/exporters/otlp/otlp_grpc_exporter_factory.h>
#include <opentelemetry/exporters/otlp/otlp_grpc_exporter_options.h>
#include <opentelemetry/exporters/otlp/otlp_grpc_log_record_exporter_factory.h>
#include <opentelemetry/exporters/otlp/otlp_grpc_log_record_exporter_options.h>
#include <opentelemetry/exporters/otlp/otlp_grpc_metric_exporter_factory.h>
#include <opentelemetry/exporters/otlp/otlp_grpc_metric_exporter_options.h>
#include <opentelemetry/exporters/otlp/otlp_http_exporter_factory.h>
#include <opentelemetry/exporters/otlp/otlp_http_exporter_options.h>
#include <opentelemetry/exporters/otlp/otlp_http_log_record_exporter_factory.h>
#include <opentelemetry/exporters/otlp/otlp_http_log_record_exporter_options.h>
#include <opentelemetry/exporters/otlp/otlp_http_metric_exporter_factory.h>
#include <opentelemetry/exporters/otlp/otlp_http_metric_exporter_options.h>

extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
  UNREFERENCED_PARAMETER(hInstance);
  UNREFERENCED_PARAMETER(dwReason);
  UNREFERENCED_PARAMETER(lpReserved);

  return TRUE;
}

extern std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> test_otlp_http_trace()
{
  opentelemetry::exporter::otlp::OtlpHttpExporterOptions options;
  options.url = "your endpoint";

  auto e = opentelemetry::exporter::otlp::OtlpHttpExporterFactory::Create(options);
  return e;
}

extern std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter> test_otlp_http_metrics()
{
  opentelemetry::exporter::otlp::OtlpHttpMetricExporterOptions options;
  options.url = "your endpoint";

  auto e = opentelemetry::exporter::otlp::OtlpHttpMetricExporterFactory::Create(options);
  return e;
}

extern std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> test_otlp_http_logs()
{
  opentelemetry::exporter::otlp::OtlpHttpLogRecordExporterOptions options;
  options.url = "your endpoint";

  auto e = opentelemetry::exporter::otlp::OtlpHttpLogRecordExporterFactory::Create(options);
  return e;
}

extern std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> test_otlp_grpc_trace()
{
  opentelemetry::exporter::otlp::OtlpGrpcExporterOptions options;
  options.endpoint = "your endpoint";

  auto e = opentelemetry::exporter::otlp::OtlpGrpcExporterFactory::Create(options);
  return e;
}

extern std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter> test_otlp_grpc_metrics()
{
  opentelemetry::exporter::otlp::OtlpGrpcMetricExporterOptions options;
  options.endpoint = "your endpoint";

  auto e = opentelemetry::exporter::otlp::OtlpGrpcMetricExporterFactory::Create(options);
  return e;
}

extern std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> test_otlp_grpc_logs()
{
  opentelemetry::exporter::otlp::OtlpGrpcLogRecordExporterOptions options;
  options.endpoint = "your endpoint";

  auto e = opentelemetry::exporter::otlp::OtlpGrpcLogRecordExporterFactory::Create(options);
  return e;
}
