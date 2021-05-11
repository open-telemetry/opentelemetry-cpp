#pragma once
#include "opentelemetry/exporters/ostream/span_exporter.h"
#include "opentelemetry/sdk/trace/simple_processor.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/trace/provider.h"

#include "opentelemetry/context/propagation/global_propagator.h"
#include "opentelemetry/context/propagation/text_map_propagator.h"
#include "opentelemetry/context/runtime_context.h"
#include "opentelemetry/trace/propagation/http_trace_context.h"
#include "opentelemetry/trace/scope.h"

#include <cstring>
#include <iostream>
#include <vector>
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/ext/http/client/http_client.h"

namespace
{
inline nostd::shared_ptr<opentelemetry::trace::Span> GetSpanFromContext(
    const opentelemetry::context::Context &context)
{
  opentelemetry::context::ContextValue span = context.GetValue(opentelemetry::trace::kSpanKey);
  if (nostd::holds_alternative<nostd::shared_ptr<opentelemetry::trace::Span>>(span))
  {
    //std::cout << "GETSPAN FROM CONTEXT YES\n";
    return nostd::get<nostd::shared_ptr<opentelemetry::trace::Span>>(span);
  }
  static nostd::shared_ptr<opentelemetry::trace::Span> invalid_span{
      new opentelemetry::trace::DefaultSpan(opentelemetry::trace::SpanContext::GetInvalid())};
  //std::cout << "GETSPANFROMCONTEXT no\n";
  return invalid_span;
}

void initTracer()
{
  auto exporter = std::unique_ptr<sdktrace::SpanExporter>(
      new opentelemetry::exporter::trace::OStreamSpanExporter);
  auto processor = std::unique_ptr<sdktrace::SpanProcessor>(
      new sdktrace::SimpleSpanProcessor(std::move(exporter)));
  std::vector<std::unique_ptr<sdktrace::SpanProcessor>> processors;
  processors.push_back(std::move(processor));
  // Default is an always-on sampler.
  auto context  = std::make_shared<sdktrace::TracerContext>(std::move(processors));
  auto provider = nostd::shared_ptr<opentelemetry::trace::TracerProvider>(
      new sdktrace::TracerProvider(context));
  // Set the global trace provide
  opentelemetry::trace::Provider::SetTracerProvider(provider);

  // set global propagato
  opentelemetry::context::propagation::GlobalTextMapPropagator::SetGlobalPropagator(
      nostd::shared_ptr<opentelemetry::context::propagation::TextMapPropagator>(
          new opentelemetry::trace::propagation::HttpTraceContext()));
}

nostd::shared_ptr<opentelemetry::trace::Tracer> get_tracer(std::string tracer_name)
{
  auto provider = opentelemetry::trace::Provider::GetTracerProvider();
  return provider->GetTracer(tracer_name);
}

// This function idempotently sets the global propagator and returns it to remove
// boilerplate from gRPC example services/library
nostd::shared_ptr<opentelemetry::context::propagation::TextMapPropagator> get_propagator()
{
  auto propagator =
      opentelemetry::context::propagation::GlobalTextMapPropagator::GetGlobalPropagator();
  return propagator;
}

}  // namespace
