// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <memory>

#include "custom_http_client.h"
#include "opentelemetry/exporters/otlp/otlp_http_exporter_factory.h"
#include "opentelemetry/exporters/otlp/otlp_http_exporter_options.h"
#include "opentelemetry/sdk/trace/simple_processor_factory.h"
#include "opentelemetry/sdk/trace/tracer_provider_factory.h"

namespace trace_sdk   = opentelemetry::sdk::trace;
namespace otlp        = opentelemetry::exporter::otlp;
namespace http_client = opentelemetry::ext::http::client;

int main()
{
  otlp::OtlpHttpExporterOptions options;
  options.url = "http://localhost:4318/v1/traces";

  auto factory   = std::make_shared<http_client::LoggingHttpClientFactory>();
  auto exporter  = otlp::OtlpHttpExporterFactory::Create(options, factory);
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
