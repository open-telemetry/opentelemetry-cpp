// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <memory>
#include <utility>

#include "opentelemetry/exporters/ostream/span_exporter_factory.h"
#include "opentelemetry/sdk/trace/processor.h"
#include "opentelemetry/sdk/trace/recordable.h"
#include "opentelemetry/sdk/trace/simple_processor_factory.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/sdk/trace/tracer_provider_factory.h"
#include "opentelemetry/trace/provider.h"
#include "opentelemetry/trace/span_id.h"
#include "opentelemetry/trace/tracer_provider.h"

#ifdef BAZEL_BUILD
#  include "examples/common/foo_library/foo_library.h"
#else
#  include "foo_library/foo_library.h"
#endif

namespace trace_api      = opentelemetry::trace;
namespace trace_sdk      = opentelemetry::sdk::trace;
namespace trace_exporter = opentelemetry::exporter::trace;

namespace
{
void InitTracer()
{
  auto exporter  = trace_exporter::OStreamSpanExporterFactory::Create();
  auto processor = trace_sdk::SimpleSpanProcessorFactory::Create(std::move(exporter));

#ifdef OPENTELEMETRY_DEPRECATED_SDK_FACTORY
  std::shared_ptr<opentelemetry::trace::TracerProvider> provider =
      opentelemetry::sdk::trace::TracerProviderFactory::Create(std::move(processor));
#else
  std::shared_ptr<opentelemetry::sdk::trace::TracerProvider> provider =
      opentelemetry::sdk::trace::TracerProviderFactory::Create(std::move(processor));
#endif /* OPENTELEMETRY_DEPRECATED_SDK_FACTORY */

  // Set the global trace provider
  const std::shared_ptr<opentelemetry::trace::TracerProvider> &api_provider = provider;
  trace_api::Provider::SetTracerProvider(api_provider);
}

void CleanupTracer()
{
  std::shared_ptr<opentelemetry::trace::TracerProvider> none;
  trace_api::Provider::SetTracerProvider(none);
}
}  // namespace

int main()
{
  // Removing this line will leave the default noop TracerProvider in place.
  InitTracer();

  foo_library();

  CleanupTracer();
}
