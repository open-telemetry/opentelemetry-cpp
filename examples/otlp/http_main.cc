// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/otlp/otlp_http_exporter_factory.h"
#include "opentelemetry/exporters/otlp/otlp_http_exporter_options.h"
#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/trace/processor.h"
#include "opentelemetry/sdk/trace/simple_processor_factory.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/sdk/trace/tracer_provider_factory.h"
#include "opentelemetry/trace/provider.h"

#include <string>

#include "foo_library/foo_library.h"

namespace trace     = opentelemetry::trace;
namespace trace_sdk = opentelemetry::sdk::trace;
namespace otlp      = opentelemetry::exporter::otlp;

namespace internal_log = opentelemetry::sdk::common::internal_log;

namespace
{
opentelemetry::exporter::otlp::OtlpHttpExporterOptions opts;

#ifdef OPENTELEMETRY_DEPRECATED_SDK_FACTORY
std::shared_ptr<opentelemetry::trace::TracerProvider> provider;
#else
std::shared_ptr<opentelemetry::sdk::trace::TracerProvider> provider;
#endif /* OPENTELEMETRY_DEPRECATED_SDK_FACTORY */

void InitTracer()
{
  // Create OTLP exporter instance
  auto exporter  = otlp::OtlpHttpExporterFactory::Create(opts);
  auto processor = trace_sdk::SimpleSpanProcessorFactory::Create(std::move(exporter));
  provider       = trace_sdk::TracerProviderFactory::Create(std::move(processor));
  // Set the global trace provider
  std::shared_ptr<opentelemetry::trace::TracerProvider> api_provider = provider;
  trace::Provider::SetTracerProvider(api_provider);
}

void CleanupTracer()
{
  // We call ForceFlush to prevent to cancel running exportings, It's optional.
  if (provider)
  {
#ifdef OPENTELEMETRY_DEPRECATED_SDK_FACTORY
    static_cast<opentelemetry::sdk::trace::TracerProvider *>(provider.get())->ForceFlush();
#else
    provider->ForceFlush();
#endif /* OPENTELEMETRY_DEPRECATED_SDK_FACTORY */
  }

  provider.reset();
  std::shared_ptr<opentelemetry::trace::TracerProvider> none;
  trace::Provider::SetTracerProvider(none);
}
}  // namespace

/*
  Usage:
  - example_otlp_http
  - example_otlp_http <URL>
  - example_otlp_http <URL> <DEBUG>
  - example_otlp_http <URL> <DEBUG> <BIN>
  <DEBUG> = yes|no, to turn console debug on or off
  <BIN> = bin, to export in binary format
*/
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
        opts.content_type = otlp::HttpRequestContentType::kBinary;
      }
    }
  }

  if (opts.console_debug)
  {
    internal_log::GlobalLogHandler::SetLogLevel(internal_log::LogLevel::Debug);
  }

  // Removing this line will leave the default noop TracerProvider in place.
  InitTracer();

  foo_library();

  CleanupTracer();
}
