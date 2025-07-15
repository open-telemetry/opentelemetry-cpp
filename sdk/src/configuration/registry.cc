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

Registry::Registry()
{
  AddTextMapPropagatorBuilder("tracecontext", std::make_unique<TraceContextBuilder>());
  AddTextMapPropagatorBuilder("baggage", std::make_unique<BaggageBuilder>());
  AddTextMapPropagatorBuilder("b3", std::make_unique<B3Builder>());
  AddTextMapPropagatorBuilder("b3multi", std::make_unique<B3MultiBuilder>());
  AddTextMapPropagatorBuilder("jaeger", std::make_unique<JaegerBuilder>());
}

const TextMapPropagatorBuilder *Registry::GetTextMapPropagatorBuilder(const std::string &name) const
{
  TextMapPropagatorBuilder *builder = nullptr;
  auto search                       = propagator_builders_.find(name);
  if (search != propagator_builders_.end())
  {
    builder = search->second.get();
  }
  return builder;
}

void Registry::AddTextMapPropagatorBuilder(const std::string &name,
                                           std::unique_ptr<TextMapPropagatorBuilder> &&builder)
{
  propagator_builders_.insert_or_assign(name, std::move(builder));
}

const ExtensionSamplerBuilder *Registry::GetExtensionSamplerBuilder(const std::string &name) const
{
  ExtensionSamplerBuilder *builder = nullptr;
  auto search                      = sampler_builders_.find(name);
  if (search != sampler_builders_.end())
  {
    builder = search->second.get();
  }
  return builder;
}

void Registry::AddExtensionSamplerBuilder(const std::string &name,
                                          std::unique_ptr<ExtensionSamplerBuilder> &&builder)
{
  sampler_builders_.insert_or_assign(name, std::move(builder));
}

const ExtensionSpanExporterBuilder *Registry::GetExtensionSpanExporterBuilder(
    const std::string &name) const
{
  ExtensionSpanExporterBuilder *builder = nullptr;
  auto search                           = span_exporter_builders_.find(name);
  if (search != span_exporter_builders_.end())
  {
    builder = search->second.get();
  }
  return builder;
}

void Registry::AddExtensionSpanExporterBuilder(
    const std::string &name,
    std::unique_ptr<ExtensionSpanExporterBuilder> &&builder)
{
  span_exporter_builders_.insert_or_assign(name, std::move(builder));
}

const ExtensionSpanProcessorBuilder *Registry::GetExtensionSpanProcessorBuilder(
    const std::string &name) const
{
  ExtensionSpanProcessorBuilder *builder = nullptr;
  auto search                            = span_processor_builders_.find(name);
  if (search != span_processor_builders_.end())
  {
    builder = search->second.get();
  }
  return builder;
}

void Registry::AddExtensionSpanProcessorBuilder(
    const std::string &name,
    std::unique_ptr<ExtensionSpanProcessorBuilder> &&builder)
{
  span_processor_builders_.insert_or_assign(name, std::move(builder));
}

const ExtensionPushMetricExporterBuilder *Registry::GetExtensionPushMetricExporterBuilder(
    const std::string &name) const
{
  ExtensionPushMetricExporterBuilder *builder = nullptr;
  auto search                                 = push_metric_exporter_builders_.find(name);
  if (search != push_metric_exporter_builders_.end())
  {
    builder = search->second.get();
  }
  return builder;
}

void Registry::AddExtensionPushMetricExporterBuilder(
    const std::string &name,
    std::unique_ptr<ExtensionPushMetricExporterBuilder> &&builder)
{
  push_metric_exporter_builders_.insert_or_assign(name, std::move(builder));
}

const ExtensionPullMetricExporterBuilder *Registry::GetExtensionPullMetricExporterBuilder(
    const std::string &name) const
{
  ExtensionPullMetricExporterBuilder *builder = nullptr;
  auto search                                 = pull_metric_exporter_builders_.find(name);
  if (search != pull_metric_exporter_builders_.end())
  {
    builder = search->second.get();
  }
  return builder;
}

void Registry::AddExtensionPullMetricExporterBuilder(
    const std::string &name,
    std::unique_ptr<ExtensionPullMetricExporterBuilder> &&builder)
{
  pull_metric_exporter_builders_.insert_or_assign(name, std::move(builder));
}

const ExtensionLogRecordExporterBuilder *Registry::GetExtensionLogRecordExporterBuilder(
    const std::string &name) const
{
  ExtensionLogRecordExporterBuilder *builder = nullptr;
  auto search                                = log_record_exporter_builders_.find(name);
  if (search != log_record_exporter_builders_.end())
  {
    builder = search->second.get();
  }
  return builder;
}

void Registry::AddExtensionLogRecordExporterBuilder(
    const std::string &name,
    std::unique_ptr<ExtensionLogRecordExporterBuilder> &&builder)
{
  log_record_exporter_builders_.insert_or_assign(name, std::move(builder));
}

const ExtensionLogRecordProcessorBuilder *Registry::GetExtensionLogRecordProcessorBuilder(
    const std::string &name) const
{
  ExtensionLogRecordProcessorBuilder *builder = nullptr;
  auto search                                 = log_record_processor_builders_.find(name);
  if (search != log_record_processor_builders_.end())
  {
    builder = search->second.get();
  }
  return builder;
}

void Registry::AddExtensionLogRecordProcessorBuilder(
    const std::string &name,
    std::unique_ptr<ExtensionLogRecordProcessorBuilder> &&builder)
{
  log_record_processor_builders_.insert_or_assign(name, std::move(builder));
}

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
