// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/zipkin/zipkin_exporter_factory.h"
#include "opentelemetry/sdk/trace/simple_processor_factory.h"
#include "opentelemetry/sdk/trace/tracer_provider_factory.h"
#include "opentelemetry/trace/provider.h"

#ifdef BAZEL_BUILD
#  include "examples/common/foo_library/foo_library.h"
#else
#  include "foo_library/foo_library.h"
#endif

namespace trace     = opentelemetry::trace;
namespace nostd     = opentelemetry::nostd;
namespace trace_sdk = opentelemetry::sdk::trace;
namespace zipkin    = opentelemetry::exporter::zipkin;
namespace resource  = opentelemetry::sdk::resource;

namespace
{
zipkin::ZipkinExporterOptions opts;
void InitTracer()
{
  // Create zipkin exporter instance
  resource::ResourceAttributes attributes = {{"service.name", "zipkin_demo_service"}};
  auto resource                           = resource::Resource::Create(attributes);
  auto exporter                           = zipkin::ZipkinExporterFactory::Create(opts);
  auto processor = trace_sdk::SimpleSpanProcessorFactory::Create(std::move(exporter));
  std::shared_ptr<opentelemetry::trace::TracerProvider> provider =
      trace_sdk::TracerProviderFactory::Create(std::move(processor), resource);
  // Set the global trace provider
  trace::Provider::SetTracerProvider(provider);
}
}  // namespace

int main(int argc, char *argv[])
{
  if (argc == 2)
  {
    opts.endpoint = argv[1];
  }
  // Removing this line will leave the default noop TracerProvider in place.
  InitTracer();

  foo_library();
}
