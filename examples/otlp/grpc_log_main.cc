// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_LOGS_PREVIEW
#  include "opentelemetry/exporters/otlp/otlp_grpc_exporter_factory.h"
#  include "opentelemetry/exporters/otlp/otlp_grpc_log_record_exporter_factory.h"
#  include "opentelemetry/logs/provider.h"
#  include "opentelemetry/sdk/logs/logger_provider_factory.h"
#  include "opentelemetry/sdk/logs/simple_log_record_processor_factory.h"
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
  auto exporter  = otlp::OtlpGrpcLogRecordExporterFactory::Create(opts);
  auto processor = logs_sdk::SimpleLogRecordProcessorFactory::Create(std::move(exporter));
  nostd::shared_ptr<logs::LoggerProvider> provider(
      logs_sdk::LoggerProviderFactory::Create(std::move(processor)));

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
