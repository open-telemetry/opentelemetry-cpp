// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

/* API */

#include "opentelemetry/trace/provider.h"

/* SDK */

#include "opentelemetry/sdk/trace/simple_processor_factory.h"
#include "opentelemetry/sdk/trace/tracer_provider_factory.h"

/* Exporter */

#include "opentelemetry/exporters/ostream/span_exporter_factory.h"

/* Application code */

#ifdef BAZEL_BUILD
#  include "examples/common/foo_library/foo_library.h"
#else
#  include "foo_library/foo_library.h"
#endif

namespace trace_api = opentelemetry::trace;
namespace trace_sdk = opentelemetry::sdk::trace;
namespace trace_exporter = opentelemetry::exporter::trace;

namespace
{
void initTracer()
{
  // Using an exporter that simply dumps span data to stdout.
  auto exporter = trace_exporter::OStreamSpanExporterFactory::Build(std::cout);
  auto processor = trace_sdk::SimpleSpanProcessorFactory::Build(std::move(exporter));
  auto provider = trace_sdk::TracerProviderFactory::Build(std::move(processor));

  // Set the global trace provider
  trace_api::Provider::SetTracerProvider(provider);
}
}  // namespace

int main()
{
  // Removing this line will leave the default noop TracerProvider in place.
  initTracer();

  foo_library();
}
