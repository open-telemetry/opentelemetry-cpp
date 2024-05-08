// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/otlp/otlp_file_exporter_factory.h"
#include "opentelemetry/sdk/trace/processor.h"
#include "opentelemetry/sdk/trace/simple_processor_factory.h"
#include "opentelemetry/sdk/trace/tracer_provider_factory.h"
#include "opentelemetry/trace/provider.h"

// sdk::TracerProvider is just used to call ForceFlush and prevent to cancel running exportings when
// destroy and shutdown exporters.It's optional to users.
#include "opentelemetry/sdk/trace/tracer_provider.h"

#include <iostream>
#include <string>
#include <utility>

#ifdef BAZEL_BUILD
#  include "examples/common/foo_library/foo_library.h"
#else
#  include "foo_library/foo_library.h"
#endif

namespace trace     = opentelemetry::trace;
namespace trace_sdk = opentelemetry::sdk::trace;
namespace otlp      = opentelemetry::exporter::otlp;

namespace
{
opentelemetry::exporter::otlp::OtlpFileExporterOptions opts;

std::shared_ptr<opentelemetry::sdk::trace::TracerProvider> sdk_provider;

void InitTracer()
{
  // Create OTLP exporter instance
  auto exporter  = otlp::OtlpFileExporterFactory::Create(opts);
  auto processor = trace_sdk::SimpleSpanProcessorFactory::Create(std::move(exporter));
  sdk_provider = trace_sdk::TracerProviderFactory::Create(std::move(processor));
  // Set the global trace provider
  trace::Provider::SetTracerProvider(sdk_provider);
}

void CleanupTracer()
{
  // We call ForceFlush to prevent to cancel running exportings, It's optional.
  if (sdk_provider)
  {
    sdk_provider->ForceFlush(std::chrono::milliseconds(10000));
  }

  std::shared_ptr<opentelemetry::trace::TracerProvider> none;
  trace::Provider::SetTracerProvider(none);
}
}  // namespace

int main(int argc, char *argv[])
{
  if (argc > 1)
  {
    opentelemetry::exporter::otlp::OtlpFileClientFileSystemOptions fs_backend;
    fs_backend.file_pattern = argv[1];
    opts.backend_options    = fs_backend;
  }
  // Removing this line will leave the default noop TracerProvider in place.
  InitTracer();

  foo_library();

  CleanupTracer();
}
