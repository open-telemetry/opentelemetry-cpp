#pragma once
#include "opentelemetry/exporters/ostream/span_exporter.h"
#include "opentelemetry/sdk/trace/simple_processor.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/trace/provider.h"

#include "opentelemetry/context/propagation/global_propagator.h"
#include "opentelemetry/trace/propagation/http_trace_context.h"
#include "opentelemetry/context/propagation/text_map_propagator.h"

#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/ext/http/client/http_client.h"
#include <iostream>
#include <vector>

namespace {

//TBD - This function be removed once #723 is merged
inline nostd::shared_ptr<opentelemetry::trace::Span> GetSpanFromContext(const opentelemetry::context::Context &context)
{
   opentelemetry::context::ContextValue span = context.GetValue(opentelemetry::trace::kSpanKey);
  if (nostd::holds_alternative<nostd::shared_ptr<opentelemetry::trace::Span>>(span))
  {
    return nostd::get<nostd::shared_ptr<opentelemetry::trace::Span>>(span);
  }
  static nostd::shared_ptr<opentelemetry::trace::Span> invalid_span{new opentelemetry::trace::DefaultSpan(opentelemetry::trace::SpanContext::GetInvalid())};
  return invalid_span;
}

template <typename T>
class HttpTextMapCarrier : public opentelemetry::trace::propagation::TextMapCarrier
{
public:

  HttpTextMapCarrier<T>(T &headers):headers_(headers)
  {}
  HttpTextMapCarrier() = default;
  virtual nostd::string_view Get(nostd::string_view key) const noexcept override
  {
    auto it = headers_.find(std::string(key));
    if (it != headers_.end())
    {
      return nostd::string_view(it->second);
    }
    return "";
  }
  virtual void Set(nostd::string_view key, nostd::string_view value) noexcept override
  {
    headers_.insert(std::pair<std::string, std::string>(std::string(key), std::string(value)));
  }

  T headers_;

};

void initTracer() {
  auto exporter = std::unique_ptr<sdktrace::SpanExporter>(
      new opentelemetry::exporter::trace::OStreamSpanExporter);
  auto processor = std::unique_ptr<sdktrace::SpanProcessor>(
      new sdktrace::SimpleSpanProcessor(std::move(exporter)));
  std::vector<std::unique_ptr<sdktrace::SpanProcessor>> processors;
  processors.push_back(std::move(processor));
  // Default is an always-on sampler.
  auto context = std::make_shared<sdktrace::TracerContext>(std::move(processors));
  auto provider =  nostd::shared_ptr<opentelemetry::trace::TracerProvider>(
      new sdktrace::TracerProvider(context));
  // Set the global trace provider
  opentelemetry::trace::Provider::SetTracerProvider(provider);

  //set http propagator
  opentelemetry::context::propagation::GlobalTextMapPropagator::SetGlobalPropagator(
      nostd::shared_ptr<opentelemetry::context::propagation::TextMapPropagator>(new opentelemetry::trace::propagation::HttpTraceContext()));
}

nostd::shared_ptr<opentelemetry::trace::Tracer> get_tracer(std::string tracer_name)
{
  auto provider = opentelemetry::trace::Provider::GetTracerProvider();
  return provider->GetTracer(tracer_name);
}

}
