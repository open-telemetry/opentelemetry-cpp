// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/otlp/otlp_grpc_exporter_factory.h"
#include "opentelemetry/exporters/otlp/otlp_grpc_exporter_options.h"
#include "opentelemetry/exporters/otlp/otlp_grpc_log_record_exporter_factory.h"
#include "opentelemetry/exporters/otlp/otlp_grpc_log_record_exporter_options.h"
#include "opentelemetry/logs/provider.h"
#include "opentelemetry/sdk/logs/exporter.h"
#include "opentelemetry/sdk/logs/logger_provider.h"
#include "opentelemetry/sdk/logs/logger_provider_factory.h"
#include "opentelemetry/sdk/logs/processor.h"
#include "opentelemetry/sdk/logs/simple_log_record_processor_factory.h"
#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/processor.h"
#include "opentelemetry/sdk/trace/simple_processor_factory.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/sdk/trace/tracer_provider_factory.h"
#include "opentelemetry/trace/provider.h"

#include <string>

#include "logs_foo_library/foo_library.h"

namespace trace     = opentelemetry::trace;
namespace nostd     = opentelemetry::nostd;
namespace otlp      = opentelemetry::exporter::otlp;
namespace logs_sdk  = opentelemetry::sdk::logs;
namespace logs      = opentelemetry::logs;
namespace trace_sdk = opentelemetry::sdk::trace;

namespace
{
opentelemetry::exporter::otlp::OtlpGrpcExporterOptions opts;
opentelemetry::exporter::otlp::OtlpGrpcLogRecordExporterOptions log_opts;

#ifdef OPENTELEMETRY_DEPRECATED_SDK_FACTORY
std::shared_ptr<opentelemetry::trace::TracerProvider> tracer_provider;
std::shared_ptr<opentelemetry::logs::LoggerProvider> logger_provider;
#else
std::shared_ptr<opentelemetry::sdk::trace::TracerProvider> tracer_provider;
std::shared_ptr<opentelemetry::sdk::logs::LoggerProvider> logger_provider;
#endif /* OPENTELEMETRY_DEPRECATED_SDK_FACTORY */

void InitTracer()
{
  // Create OTLP exporter instance
  auto exporter   = otlp::OtlpGrpcExporterFactory::Create(opts);
  auto processor  = trace_sdk::SimpleSpanProcessorFactory::Create(std::move(exporter));
  tracer_provider = trace_sdk::TracerProviderFactory::Create(std::move(processor));

  // Set the global trace provider
  std::shared_ptr<opentelemetry::trace::TracerProvider> api_provider = tracer_provider;
  trace::Provider::SetTracerProvider(api_provider);
}

void CleanupTracer()
{
  // We call ForceFlush to prevent to cancel running exportings, It's optional.
  if (tracer_provider)
  {
#ifdef OPENTELEMETRY_DEPRECATED_SDK_FACTORY
    static_cast<opentelemetry::sdk::trace::TracerProvider *>(tracer_provider.get())->ForceFlush();
#else
    tracer_provider->ForceFlush();
#endif /* OPENTELEMETRY_DEPRECATED_SDK_FACTORY */
  }

  tracer_provider.reset();
  std::shared_ptr<opentelemetry::trace::TracerProvider> none;
  trace::Provider::SetTracerProvider(none);
}

void InitLogger()
{
  // Create OTLP exporter instance
  auto exporter   = otlp::OtlpGrpcLogRecordExporterFactory::Create(log_opts);
  auto processor  = logs_sdk::SimpleLogRecordProcessorFactory::Create(std::move(exporter));
  logger_provider = logs_sdk::LoggerProviderFactory::Create(std::move(processor));

  // Set the global logger provider
  std::shared_ptr<opentelemetry::logs::LoggerProvider> api_provider = logger_provider;
  opentelemetry::logs::Provider::SetLoggerProvider(api_provider);
}

void CleanupLogger()
{
  // We call ForceFlush to prevent to cancel running exportings, It's optional.
  if (logger_provider)
  {
#ifdef OPENTELEMETRY_DEPRECATED_SDK_FACTORY
    static_cast<opentelemetry::sdk::logs::LoggerProvider *>(logger_provider.get())->ForceFlush();
#else
    logger_provider->ForceFlush();
#endif /* OPENTELEMETRY_DEPRECATED_SDK_FACTORY */
  }

  logger_provider.reset();
  nostd::shared_ptr<logs::LoggerProvider> none;
  opentelemetry::logs::Provider::SetLoggerProvider(none);
}
}  // namespace

int main(int argc, char *argv[])
{
  if (argc > 1)
  {
    opts.endpoint     = argv[1];
    log_opts.endpoint = argv[1];
    if (argc > 2)
    {
      opts.use_ssl_credentials             = true;
      log_opts.use_ssl_credentials         = true;
      opts.ssl_credentials_cacert_path     = argv[2];
      log_opts.ssl_credentials_cacert_path = argv[2];
    }
  }
  InitLogger();
  InitTracer();
  foo_library();
  CleanupTracer();
  CleanupLogger();
}
