// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/otlp/otlp_http_exporter_factory.h"
#include "opentelemetry/exporters/otlp/otlp_http_log_record_exporter_factory.h"
#include "opentelemetry/exporters/otlp/otlp_http_log_record_exporter_options.h"
#include "opentelemetry/logs/provider.h"
#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/logs/logger_provider_factory.h"
#include "opentelemetry/sdk/logs/processor.h"
#include "opentelemetry/sdk/logs/simple_log_record_processor_factory.h"
#include "opentelemetry/sdk/trace/processor.h"
#include "opentelemetry/sdk/trace/simple_processor_factory.h"
#include "opentelemetry/sdk/trace/tracer_provider_factory.h"
#include "opentelemetry/trace/provider.h"

// sdk::TracerProvider and sdk::LoggerProvider is just used to call ForceFlush and prevent to cancel
// running exportings when destroy and shutdown exporters.It's optional to users.
#include "opentelemetry/sdk/logs/logger_provider.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"

#include <iostream>
#include <string>

#ifdef BAZEL_BUILD
#  include "examples/common/logs_foo_library/foo_library.h"
#else
#  include "logs_foo_library/foo_library.h"
#endif

namespace trace     = opentelemetry::trace;
namespace otlp      = opentelemetry::exporter::otlp;
namespace logs_sdk  = opentelemetry::sdk::logs;
namespace logs      = opentelemetry::logs;
namespace trace_sdk = opentelemetry::sdk::trace;

namespace internal_log = opentelemetry::sdk::common::internal_log;

namespace
{

opentelemetry::exporter::otlp::OtlpHttpExporterOptions trace_opts;
void InitTracer()
{
  if (trace_opts.url.size() > 9)
  {
    if (trace_opts.url.substr(trace_opts.url.size() - 8) == "/v1/logs")
    {
      trace_opts.url = trace_opts.url.substr(0, trace_opts.url.size() - 8) + "/v1/traces";
    }
    else if (trace_opts.url.substr(trace_opts.url.size() - 9) == "/v1/logs/")
    {
      trace_opts.url = trace_opts.url.substr(0, trace_opts.url.size() - 9) + "/v1/traces";
    }
    else
    {
      trace_opts.url = opentelemetry::exporter::otlp::GetOtlpDefaultHttpTracesEndpoint();
    }
  }
  std::cout << "Using " << trace_opts.url << " to export trace spans." << std::endl;
  // Create OTLP exporter instance
  auto exporter  = otlp::OtlpHttpExporterFactory::Create(trace_opts);
  auto processor = trace_sdk::SimpleSpanProcessorFactory::Create(std::move(exporter));
  std::shared_ptr<opentelemetry::trace::TracerProvider> provider =
      trace_sdk::TracerProviderFactory::Create(std::move(processor));
  // Set the global trace provider
  trace::Provider::SetTracerProvider(provider);
}

void CleanupTracer()
{
  // We call ForceFlush to prevent to cancel running exportings, It's optional.
  opentelemetry::nostd::shared_ptr<opentelemetry::trace::TracerProvider> provider =
      trace::Provider::GetTracerProvider();
  if (provider)
  {
    static_cast<trace_sdk::TracerProvider *>(provider.get())->ForceFlush();
  }

  std::shared_ptr<opentelemetry::trace::TracerProvider> none;
  trace::Provider::SetTracerProvider(none);
}

opentelemetry::exporter::otlp::OtlpHttpLogRecordExporterOptions logger_opts;
void InitLogger()
{
  std::cout << "Using " << logger_opts.url << " to export log records." << std::endl;
  logger_opts.console_debug = true;
  // Create OTLP exporter instance
  auto exporter  = otlp::OtlpHttpLogRecordExporterFactory::Create(logger_opts);
  auto processor = logs_sdk::SimpleLogRecordProcessorFactory::Create(std::move(exporter));
  std::shared_ptr<logs::LoggerProvider> provider =
      logs_sdk::LoggerProviderFactory::Create(std::move(processor));

  opentelemetry::logs::Provider::SetLoggerProvider(provider);
}

void CleanupLogger()
{
  // We call ForceFlush to prevent to cancel running exportings, It's optional.
  opentelemetry::nostd::shared_ptr<logs::LoggerProvider> provider =
      logs::Provider::GetLoggerProvider();
  if (provider)
  {
    static_cast<logs_sdk::LoggerProvider *>(provider.get())->ForceFlush();
  }

  std::shared_ptr<logs::LoggerProvider> none;
  opentelemetry::logs::Provider::SetLoggerProvider(none);
}
}  // namespace

/*
  Usage:
  - example_otlp_http_log
  - example_otlp_http_log <URL>
  - example_otlp_http_log <URL> <DEBUG>
  - example_otlp_http_log <URL> <DEBUG> <BIN>
  <DEBUG> = yes|no, to turn console debug on or off
  <BIN> = bin, to export in binary format
*/
int main(int argc, char *argv[])
{
  if (argc > 1)
  {
    trace_opts.url  = argv[1];
    logger_opts.url = argv[1];
    if (argc > 2)
    {
      std::string debug        = argv[2];
      trace_opts.console_debug = debug != "" && debug != "0" && debug != "no";
    }

    if (argc > 3)
    {
      std::string binary_mode = argv[3];
      if (binary_mode.size() >= 3 && binary_mode.substr(0, 3) == "bin")
      {
        trace_opts.content_type  = opentelemetry::exporter::otlp::HttpRequestContentType::kBinary;
        logger_opts.content_type = opentelemetry::exporter::otlp::HttpRequestContentType::kBinary;
      }
    }
  }

  if (trace_opts.console_debug)
  {
    internal_log::GlobalLogHandler::SetLogLevel(internal_log::LogLevel::Debug);
  }

  InitLogger();
  InitTracer();
  foo_library();
  CleanupTracer();
  CleanupLogger();
}
