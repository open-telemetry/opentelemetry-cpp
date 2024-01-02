// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/ostream/span_exporter_factory.h"
#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/processor.h"
#include "opentelemetry/sdk/trace/simple_processor_factory.h"
#include "opentelemetry/sdk/trace/tracer_provider_factory.h"
#include "opentelemetry/trace/provider.h"

namespace trace_api = opentelemetry::trace;
namespace trace_sdk = opentelemetry::sdk::trace;
namespace trace_exporter = opentelemetry::exporter::trace;

int main() {
	auto exporter = trace_exporter::OStreamSpanExporterFactory::Create();
	auto processor = trace_sdk::SimpleSpanProcessorFactory::Create(std::move(exporter));
	std::shared_ptr<opentelemetry::trace::TracerProvider> provider = trace_sdk::TracerProviderFactory::Create(std::move(processor));

	// Set the global trace provider
	trace_api::Provider::SetTracerProvider(provider);

	return 0;
}
