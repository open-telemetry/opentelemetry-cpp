// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_LOGS_PREVIEW
#  include "opentelemetry/exporters/otlp/otlp_http_exporter.h"
#  include "opentelemetry/exporters/otlp/otlp_http_log_exporter.h"
#  include "opentelemetry/logs/provider.h"
#  include "opentelemetry/sdk/logs/logger_provider.h"
#  include "opentelemetry/sdk/logs/simple_log_processor.h"
#  include "opentelemetry/sdk/trace/simple_processor.h"
#  include "opentelemetry/sdk/trace/tracer_provider.h"
#  include "opentelemetry/trace/provider.h"

#  include <string>

#  ifdef BAZEL_BUILD
#    include "examples/common/logs_foo_library/foo_library.h"
#  else
#    include "logs_foo_library/foo_library.h"
#  endif

namespace trace    = opentelemetry::trace;
namespace nostd    = opentelemetry::nostd;
namespace otlp     = opentelemetry::exporter::otlp;
namespace sdklogs  = opentelemetry::sdk::logs;
namespace logs_api = opentelemetry::logs;

namespace sdktrace = opentelemetry::sdk::trace;

namespace
{

opentelemetry::exporter::otlp::OtlpHttpExporterOptions opts;
void InitTracer()
{
  // Create OTLP exporter instance
  auto exporter  = std::unique_ptr<sdktrace::SpanExporter>(new otlp::OtlpHttpExporter(opts));
  auto processor = std::unique_ptr<sdktrace::SpanProcessor>(
      new sdktrace::SimpleSpanProcessor(std::move(exporter)));
  auto provider =
      nostd::shared_ptr<trace::TracerProvider>(new sdktrace::TracerProvider(std::move(processor)));
  // Set the global trace provider
  trace::Provider::SetTracerProvider(provider);
}

opentelemetry::exporter::otlp::OtlpHttpLogExporterOptions logger_opts;
void InitLogger()
{
  logger_opts.console_debug = true;
  // Create OTLP exporter instance
  auto exporter = std::unique_ptr<sdklogs::LogExporter>(new otlp::OtlpHttpLogExporter(logger_opts));
  auto processor =
      std::shared_ptr<sdklogs::LogProcessor>(new sdklogs::SimpleLogProcessor(std::move(exporter)));
  auto sdkProvider = std::shared_ptr<sdklogs::LoggerProvider>(new sdklogs::LoggerProvider());
  sdkProvider->SetProcessor(processor);
  auto apiProvider = nostd::shared_ptr<logs_api::LoggerProvider>(sdkProvider);
  auto provider    = nostd::shared_ptr<logs_api::LoggerProvider>(apiProvider);
  opentelemetry::logs::Provider::SetLoggerProvider(provider);
}
}  // namespace

int main(int argc, char *argv[])
{
  if (argc > 1)
  {
    opts.url = argv[1];
    if (argc > 2)
    {
      std::string debug  = argv[2];
      opts.console_debug = debug != "" && debug != "0" && debug != "no";
    }

    if (argc > 3)
    {
      std::string binary_mode = argv[3];
      if (binary_mode.size() >= 3 && binary_mode.substr(0, 3) == "bin")
      {
        opts.content_type = opentelemetry::exporter::otlp::HttpRequestContentType::kBinary;
      }
    }
  }
  // Removing this line will leave the default noop TracerProvider in place.
  InitLogger();
  InitTracer();
  foo_library();
}

#endif