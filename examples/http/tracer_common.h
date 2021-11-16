// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#include "opentelemetry/exporters/ostream/span_exporter.h"
#include "opentelemetry/sdk/trace/simple_processor.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/trace/provider.h"

#include "opentelemetry/context/propagation/global_propagator.h"
#include "opentelemetry/context/propagation/text_map_propagator.h"
#include "opentelemetry/trace/propagation/http_trace_context.h"

#include <cstring>
#include <iostream>
#include <vector>
#include "opentelemetry/ext/http/client/http_client.h"
#include "opentelemetry/nostd/shared_ptr.h"

namespace
{

template <typename T>
class HttpTextMapCarrier : public opentelemetry::context::propagation::TextMapCarrier
{
public:
  HttpTextMapCarrier<T>(T &headers) : headers_(headers) {}
  HttpTextMapCarrier() = default;
  virtual opentelemetry::nostd::string_view Get(
      opentelemetry::nostd::string_view key) const noexcept override
  {
    std::string key_to_compare = key.data();
    // Header's first letter seems to be  automatically capitaliazed by our test http-server, so
    // compare accordingly.
    if (key == opentelemetry::trace::propagation::kTraceParent)
    {
      key_to_compare = "Traceparent";
    }
    else if (key == opentelemetry::trace::propagation::kTraceState)
    {
      key_to_compare = "Tracestate";
    }
    auto it = headers_.find(key_to_compare);
    if (it != headers_.end())
    {
      return it->second;
    }
    return "";
  }

  virtual void Set(opentelemetry::nostd::string_view key,
                   opentelemetry::nostd::string_view value) noexcept override
  {
    headers_.insert(std::pair<std::string, std::string>(std::string(key), std::string(value)));
  }

  T headers_;
};

void initTracer()
{
  auto exporter = std::unique_ptr<opentelemetry::sdk::trace::SpanExporter>(
      new opentelemetry::exporter::trace::OStreamSpanExporter);
  auto processor = std::unique_ptr<opentelemetry::sdk::trace::SpanProcessor>(
      new opentelemetry::sdk::trace::SimpleSpanProcessor(std::move(exporter)));
  std::vector<std::unique_ptr<opentelemetry::sdk::trace::SpanProcessor>> processors;
  processors.push_back(std::move(processor));
  // Default is an always-on sampler.
  auto context  = std::make_shared<opentelemetry::sdk::trace::TracerContext>(std::move(processors));
  auto provider = opentelemetry::nostd::shared_ptr<opentelemetry::trace::TracerProvider>(
      new opentelemetry::sdk::trace::TracerProvider(context));
  // Set the global trace provider
  opentelemetry::trace::Provider::SetTracerProvider(provider);

  // set global propagator
  opentelemetry::context::propagation::GlobalTextMapPropagator::SetGlobalPropagator(
      opentelemetry::nostd::shared_ptr<opentelemetry::context::propagation::TextMapPropagator>(
          new opentelemetry::trace::propagation::HttpTraceContext()));
}

opentelemetry::nostd::shared_ptr<opentelemetry::trace::Tracer> get_tracer(std::string tracer_name)
{
  auto provider = opentelemetry::trace::Provider::GetTracerProvider();
  return provider->GetTracer(tracer_name);
}

}  // namespace
