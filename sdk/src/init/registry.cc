// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/common/global_log_handler.h"

#include "opentelemetry/baggage/propagation/baggage_propagator.h"
#include "opentelemetry/sdk/init/otlp_span_exporter_builder.h"
#include "opentelemetry/sdk/init/registry.h"
#include "opentelemetry/sdk/init/text_map_propagator_builder.h"
#include "opentelemetry/trace/propagation/b3_propagator.h"
#include "opentelemetry/trace/propagation/http_trace_context.h"
#include "opentelemetry/trace/propagation/jaeger.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace init
{

class TraceContextBuilder : public TextMapPropagatorBuilder
{
public:
  TraceContextBuilder()                   = default;
  virtual ~TraceContextBuilder() override = default;

  std::unique_ptr<opentelemetry::context::propagation::TextMapPropagator> Build() const override
  {
    return std::unique_ptr<opentelemetry::context::propagation::TextMapPropagator>(
        new opentelemetry::trace::propagation::HttpTraceContext());
  }
};

class BaggageBuilder : public TextMapPropagatorBuilder
{
public:
  BaggageBuilder()                   = default;
  virtual ~BaggageBuilder() override = default;

  std::unique_ptr<opentelemetry::context::propagation::TextMapPropagator> Build() const override
  {
    return std::unique_ptr<opentelemetry::context::propagation::TextMapPropagator>(
        new opentelemetry::baggage::propagation::BaggagePropagator());
  }
};

class B3Builder : public TextMapPropagatorBuilder
{
public:
  B3Builder()                   = default;
  virtual ~B3Builder() override = default;

  std::unique_ptr<opentelemetry::context::propagation::TextMapPropagator> Build() const override
  {
    return std::unique_ptr<opentelemetry::context::propagation::TextMapPropagator>(
        new opentelemetry::trace::propagation::B3Propagator());
  }
};

class B3MultiBuilder : public TextMapPropagatorBuilder
{
public:
  B3MultiBuilder()                   = default;
  virtual ~B3MultiBuilder() override = default;

  std::unique_ptr<opentelemetry::context::propagation::TextMapPropagator> Build() const override
  {
    return std::unique_ptr<opentelemetry::context::propagation::TextMapPropagator>(
        new opentelemetry::trace::propagation::B3PropagatorMultiHeader());
  }
};

class JaegerBuilder : public TextMapPropagatorBuilder
{
public:
  JaegerBuilder()                   = default;
  virtual ~JaegerBuilder() override = default;

  std::unique_ptr<opentelemetry::context::propagation::TextMapPropagator> Build() const override
  {
    return std::unique_ptr<opentelemetry::context::propagation::TextMapPropagator>(
        new opentelemetry::trace::propagation::JaegerPropagator());
  }
};

static TraceContextBuilder trace_context_builder;
static BaggageBuilder baggage_builder;
static B3Builder b3_builder;
static B3MultiBuilder b3_multi_builder;
static JaegerBuilder jaeger_builder;

Registry::Registry()
{
  std::pair<std::string, TextMapPropagatorBuilder *> entry;

  entry = {"tracecontext", &trace_context_builder};
  m_propagator_builders.insert(entry);

  entry = {"baggage", &baggage_builder};
  m_propagator_builders.insert(entry);

  entry = {"b3", &b3_builder};
  m_propagator_builders.insert(entry);

  entry = {"b3multi", &b3_multi_builder};
  m_propagator_builders.insert(entry);

  entry = {"jaeger", &jaeger_builder};
  m_propagator_builders.insert(entry);
}

const TextMapPropagatorBuilder *Registry::GetTextMapPropagatorBuilder(const std::string &name)
{
  TextMapPropagatorBuilder *builder = nullptr;
  auto search                       = m_propagator_builders.find(name);
  if (search != m_propagator_builders.end())
  {
    builder = search->second;
  }
  return builder;
}

void Registry::AddTextMapPropagatorBuilder(const std::string &name,
                                           TextMapPropagatorBuilder *builder)
{
  std::pair<std::string, TextMapPropagatorBuilder *> entry{name, builder};
  m_propagator_builders.insert(entry);
}

}  // namespace init
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
