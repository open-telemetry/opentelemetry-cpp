// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <map>
#include <memory>
#include <string>
#include <utility>

#include "opentelemetry/baggage/propagation/baggage_propagator.h"
#include "opentelemetry/context/propagation/text_map_propagator.h"
#include "opentelemetry/sdk/configuration/registry.h"
#include "opentelemetry/sdk/configuration/text_map_propagator_builder.h"
#include "opentelemetry/trace/propagation/b3_propagator.h"
#include "opentelemetry/trace/propagation/http_trace_context.h"
#include "opentelemetry/trace/propagation/jaeger.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

class TraceContextBuilder : public TextMapPropagatorBuilder
{
public:
  std::unique_ptr<opentelemetry::context::propagation::TextMapPropagator> Build() const override
  {
    auto result = std::make_unique<opentelemetry::trace::propagation::HttpTraceContext>();
    return result;
  }
};

class BaggageBuilder : public TextMapPropagatorBuilder
{
public:
  std::unique_ptr<opentelemetry::context::propagation::TextMapPropagator> Build() const override
  {
    auto result = std::make_unique<opentelemetry::baggage::propagation::BaggagePropagator>();
    return result;
  }
};

class B3Builder : public TextMapPropagatorBuilder
{
public:
  std::unique_ptr<opentelemetry::context::propagation::TextMapPropagator> Build() const override
  {
    auto result = std::make_unique<opentelemetry::trace::propagation::B3Propagator>();
    return result;
  }
};

class B3MultiBuilder : public TextMapPropagatorBuilder
{
public:
  std::unique_ptr<opentelemetry::context::propagation::TextMapPropagator> Build() const override
  {
    auto result = std::make_unique<opentelemetry::trace::propagation::B3PropagatorMultiHeader>();
    return result;
  }
};

class JaegerBuilder : public TextMapPropagatorBuilder
{
public:
  std::unique_ptr<opentelemetry::context::propagation::TextMapPropagator> Build() const override
  {
    auto result = std::make_unique<opentelemetry::trace::propagation::JaegerPropagator>();
    return result;
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
  propagator_builders_.insert(entry);

  entry = {"baggage", &baggage_builder};
  propagator_builders_.insert(entry);

  entry = {"b3", &b3_builder};
  propagator_builders_.insert(entry);

  entry = {"b3multi", &b3_multi_builder};
  propagator_builders_.insert(entry);

  entry = {"jaeger", &jaeger_builder};
  propagator_builders_.insert(entry);
}

const TextMapPropagatorBuilder *Registry::GetTextMapPropagatorBuilder(const std::string &name) const
{
  TextMapPropagatorBuilder *builder = nullptr;
  auto search                       = propagator_builders_.find(name);
  if (search != propagator_builders_.end())
  {
    builder = search->second;
  }
  return builder;
}

void Registry::AddTextMapPropagatorBuilder(const std::string &name,
                                           TextMapPropagatorBuilder *builder)
{
  std::pair<std::string, TextMapPropagatorBuilder *> entry{name, builder};
  propagator_builders_.insert(entry);
}

const ExtensionSamplerBuilder *Registry::GetExtensionSamplerBuilder(const std::string &name) const
{
  ExtensionSamplerBuilder *builder = nullptr;
  auto search                      = sampler_builders_.find(name);
  if (search != sampler_builders_.end())
  {
    builder = search->second;
  }
  return builder;
}

void Registry::AddExtensionSamplerBuilder(const std::string &name, ExtensionSamplerBuilder *builder)
{
  std::pair<std::string, ExtensionSamplerBuilder *> entry{name, builder};
  sampler_builders_.insert(entry);
}

const ExtensionSpanExporterBuilder *Registry::GetExtensionSpanExporterBuilder(
    const std::string &name) const
{
  ExtensionSpanExporterBuilder *builder = nullptr;
  auto search                           = span_exporter_builders_.find(name);
  if (search != span_exporter_builders_.end())
  {
    builder = search->second;
  }
  return builder;
}

void Registry::AddExtensionSpanExporterBuilder(const std::string &name,
                                               ExtensionSpanExporterBuilder *builder)
{
  std::pair<std::string, ExtensionSpanExporterBuilder *> entry{name, builder};
  span_exporter_builders_.insert(entry);
}

const ExtensionSpanProcessorBuilder *Registry::GetExtensionSpanProcessorBuilder(
    const std::string &name) const
{
  ExtensionSpanProcessorBuilder *builder = nullptr;
  auto search                            = span_processor_builders_.find(name);
  if (search != span_processor_builders_.end())
  {
    builder = search->second;
  }
  return builder;
}

void Registry::AddExtensionSpanProcessorBuilder(const std::string &name,
                                                ExtensionSpanProcessorBuilder *builder)
{
  std::pair<std::string, ExtensionSpanProcessorBuilder *> entry{name, builder};
  span_processor_builders_.insert(entry);
}

const ExtensionPushMetricExporterBuilder *Registry::GetExtensionPushMetricExporterBuilder(
    const std::string &name) const
{
  ExtensionPushMetricExporterBuilder *builder = nullptr;
  auto search                                 = push_metric_exporter_builders_.find(name);
  if (search != push_metric_exporter_builders_.end())
  {
    builder = search->second;
  }
  return builder;
}

void Registry::AddExtensionPushMetricExporterBuilder(const std::string &name,
                                                     ExtensionPushMetricExporterBuilder *builder)
{
  std::pair<std::string, ExtensionPushMetricExporterBuilder *> entry{name, builder};
  push_metric_exporter_builders_.insert(entry);
}

const ExtensionPullMetricExporterBuilder *Registry::GetExtensionPullMetricExporterBuilder(
    const std::string &name) const
{
  ExtensionPullMetricExporterBuilder *builder = nullptr;
  auto search                                 = pull_metric_exporter_builders_.find(name);
  if (search != pull_metric_exporter_builders_.end())
  {
    builder = search->second;
  }
  return builder;
}

void Registry::AddExtensionPullMetricExporterBuilder(const std::string &name,
                                                     ExtensionPullMetricExporterBuilder *builder)
{
  std::pair<std::string, ExtensionPullMetricExporterBuilder *> entry{name, builder};
  pull_metric_exporter_builders_.insert(entry);
}

const ExtensionLogRecordExporterBuilder *Registry::GetExtensionLogRecordExporterBuilder(
    const std::string &name) const
{
  ExtensionLogRecordExporterBuilder *builder = nullptr;
  auto search                                = log_record_exporter_builders_.find(name);
  if (search != log_record_exporter_builders_.end())
  {
    builder = search->second;
  }
  return builder;
}

void Registry::AddExtensionLogRecordExporterBuilder(const std::string &name,
                                                    ExtensionLogRecordExporterBuilder *builder)
{
  std::pair<std::string, ExtensionLogRecordExporterBuilder *> entry{name, builder};
  log_record_exporter_builders_.insert(entry);
}

const ExtensionLogRecordProcessorBuilder *Registry::GetExtensionLogRecordProcessorBuilder(
    const std::string &name) const
{
  ExtensionLogRecordProcessorBuilder *builder = nullptr;
  auto search                                 = log_record_processor_builders_.find(name);
  if (search != log_record_processor_builders_.end())
  {
    builder = search->second;
  }
  return builder;
}

void Registry::AddExtensionLogRecordProcessorBuilder(const std::string &name,
                                                     ExtensionLogRecordProcessorBuilder *builder)
{
  std::pair<std::string, ExtensionLogRecordProcessorBuilder *> entry{name, builder};
  log_record_processor_builders_.insert(entry);
}

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
