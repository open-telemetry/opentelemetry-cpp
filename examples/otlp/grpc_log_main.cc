// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_LOGS_PREVIEW
// Make sure to include GRPC exporter first because otherwise Abseil may create
// ambiguity with `nostd::variant`. See issue:
// https://github.com/open-telemetry/opentelemetry-cpp/issues/880
#  include "opentelemetry/exporters/otlp/otlp_grpc_log_exporter.h"

#  include "opentelemetry/exporters/otlp/otlp_grpc_exporter_factory.h"
#  include "opentelemetry/logs/provider.h"
#  include "opentelemetry/sdk/logs/logger_provider.h"
#  include "opentelemetry/sdk/logs/simple_log_processor.h"
#  include "opentelemetry/sdk/trace/simple_processor_factory.h"
#  include "opentelemetry/sdk/trace/tracer_provider_factory.h"
#  include "opentelemetry/trace/provider.h"

#  include <string>

#  ifdef BAZEL_BUILD
#    include "examples/common/logs_foo_library/foo_library.h"
#  else
#    include "logs_foo_library/foo_library.h"
#  endif

namespace trace     = opentelemetry::trace;
namespace nostd     = opentelemetry::nostd;
namespace otlp      = opentelemetry::exporter::otlp;
namespace logs_sdk  = opentelemetry::sdk::logs;
namespace logs      = opentelemetry::logs;
namespace trace_sdk = opentelemetry::sdk::trace;

namespace
{
opentelemetry::exporter::otlp::OtlpGrpcExporterOptions opts;
void InitTracer()
{
  // Create OTLP exporter instance
  auto exporter  = otlp::OtlpGrpcExporterFactory::Create(opts);
  auto processor = trace_sdk::SimpleSpanProcessorFactory::Create(std::move(exporter));
  std::shared_ptr<opentelemetry::trace::TracerProvider> provider =
      trace_sdk::TracerProviderFactory::Create(std::move(processor));
  // Set the global trace provider
  trace::Provider::SetTracerProvider(provider);
}

void InitLogger()
{
  // Create OTLP exporter instance
  auto exporter    = std::unique_ptr<logs_sdk::LogExporter>(new otlp::OtlpGrpcLogExporter(opts));
  auto sdkProvider = std::shared_ptr<logs_sdk::LoggerProvider>(
      new logs_sdk::LoggerProvider(std::unique_ptr<logs_sdk::LogProcessor>(
          new logs_sdk::SimpleLogProcessor(std::move(exporter)))));
  auto apiProvider = nostd::shared_ptr<logs::LoggerProvider>(sdkProvider);
  auto provider    = nostd::shared_ptr<logs::LoggerProvider>(apiProvider);
  opentelemetry::logs::Provider::SetLoggerProvider(provider);
}
}  // namespace

int main(int argc, char *argv[])
{
  if (argc > 1)
  {
    opts.endpoint = argv[1];
    if (argc > 2)
    {
      opts.use_ssl_credentials         = true;
      opts.ssl_credentials_cacert_path = argv[2];
    }
  }
  InitLogger();
  InitTracer();
  foo_library();
}
#else
int main()
{
  return 0;
}
#endif
