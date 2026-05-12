// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <memory>

#include "opentelemetry/exporters/otlp/otlp_http_exporter_factory.h"
#include "opentelemetry/exporters/otlp/otlp_http_exporter_options.h"
#include "opentelemetry/sdk/trace/simple_processor_factory.h"
#include "opentelemetry/sdk/trace/tracer_provider_factory.h"

namespace trace_sdk = opentelemetry::sdk::trace;
namespace otlp      = opentelemetry::exporter::otlp;

int main()
{
  otlp::OtlpHttpExporterOptions options;
  options.url = "http://localhost:4318/v1/traces";

  auto exporter  = otlp::OtlpHttpExporterFactory::Create(options);
  auto processor = trace_sdk::SimpleSpanProcessorFactory::Create(std::move(exporter));

  std::shared_ptr<trace_sdk::TracerProvider> provider =
      trace_sdk::TracerProviderFactory::Create(std::move(processor));

  auto tracer = provider->GetTracer("custom-http-client-example");
  auto span   = tracer->StartSpan("custom-span");
  span->SetAttribute("example.key", "value");
  span->End();

  provider->ForceFlush();
  return 0;
}
