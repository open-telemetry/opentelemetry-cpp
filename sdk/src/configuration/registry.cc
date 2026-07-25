// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <chrono>
#include <map>
#include <memory>
#include <string>
#include <utility>

#include "opentelemetry/baggage/propagation/baggage_propagator.h"
#include "opentelemetry/sdk/configuration/extension_log_record_exporter_builder.h"
#include "opentelemetry/sdk/configuration/extension_log_record_processor_builder.h"
#include "opentelemetry/sdk/configuration/extension_pull_metric_exporter_builder.h"
#include "opentelemetry/sdk/configuration/extension_push_metric_exporter_builder.h"
#include "opentelemetry/sdk/configuration/extension_sampler_builder.h"
#include "opentelemetry/sdk/configuration/extension_span_exporter_builder.h"
#include "opentelemetry/sdk/configuration/extension_span_processor_builder.h"
#include "opentelemetry/sdk/configuration/periodic_metric_reader_builder.h"
#include "opentelemetry/sdk/configuration/periodic_metric_reader_configuration.h"
#include "opentelemetry/sdk/configuration/registry.h"
#include "opentelemetry/sdk/configuration/text_map_propagator_builder.h"
#include "opentelemetry/sdk/metrics/export/periodic_exporting_metric_reader_factory.h"
#include "opentelemetry/sdk/metrics/export/periodic_exporting_metric_reader_options.h"
#include "opentelemetry/sdk/metrics/metric_reader.h"
#include "opentelemetry/sdk/metrics/push_metric_exporter.h"
#include "opentelemetry/trace/propagation/b3_propagator.h"
#include "opentelemetry/trace/propagation/http_trace_context.h"
#include "opentelemetry/trace/propagation/jaeger.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

namespace
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

class DefaultPeriodicMetricReaderBuilder : public PeriodicMetricReaderBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::metrics::MetricReader> Build(
      const opentelemetry::sdk::configuration::PeriodicMetricReaderConfiguration *model,
      std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter> &&exporter) const override
  {
    opentelemetry::sdk::metrics::PeriodicExportingMetricReaderOptions options;

    options.export_interval_millis = std::chrono::milliseconds(model->interval);
    options.export_timeout_millis  = std::chrono::milliseconds(model->timeout);

    return opentelemetry::sdk::metrics::PeriodicExportingMetricReaderFactory::Create(
        std::move(exporter), options);
  }
};

}  // namespace

Registry::Registry()
{
  SetTextMapPropagatorBuilder("tracecontext", std::make_unique<TraceContextBuilder>());
  SetTextMapPropagatorBuilder("baggage", std::make_unique<BaggageBuilder>());
  SetTextMapPropagatorBuilder("b3", std::make_unique<B3Builder>());
  SetTextMapPropagatorBuilder("b3multi", std::make_unique<B3MultiBuilder>());
  SetTextMapPropagatorBuilder("jaeger", std::make_unique<JaegerBuilder>());

  SetPeriodicMetricReaderBuilder(std::make_unique<DefaultPeriodicMetricReaderBuilder>());
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

void Registry::SetTextMapPropagatorBuilder(const std::string &name,
                                           std::unique_ptr<TextMapPropagatorBuilder> &&builder)
{
  propagator_builders_.erase(name);
  propagator_builders_.insert({name, std::move(builder)});
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

void Registry::SetExtensionSamplerBuilder(const std::string &name,
                                          std::unique_ptr<ExtensionSamplerBuilder> &&builder)
{
  sampler_builders_.erase(name);
  sampler_builders_.insert({name, std::move(builder)});
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

void Registry::SetExtensionSpanExporterBuilder(
    const std::string &name,
    std::unique_ptr<ExtensionSpanExporterBuilder> &&builder)
{
  span_exporter_builders_.erase(name);
  span_exporter_builders_.insert({name, std::move(builder)});
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

void Registry::SetExtensionSpanProcessorBuilder(
    const std::string &name,
    std::unique_ptr<ExtensionSpanProcessorBuilder> &&builder)
{
  span_processor_builders_.erase(name);
  span_processor_builders_.insert({name, std::move(builder)});
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

void Registry::SetExtensionPushMetricExporterBuilder(
    const std::string &name,
    std::unique_ptr<ExtensionPushMetricExporterBuilder> &&builder)
{
  push_metric_exporter_builders_.erase(name);
  push_metric_exporter_builders_.insert({name, std::move(builder)});
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

void Registry::SetExtensionPullMetricExporterBuilder(
    const std::string &name,
    std::unique_ptr<ExtensionPullMetricExporterBuilder> &&builder)
{
  pull_metric_exporter_builders_.erase(name);
  pull_metric_exporter_builders_.insert({name, std::move(builder)});
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

void Registry::SetExtensionLogRecordExporterBuilder(
    const std::string &name,
    std::unique_ptr<ExtensionLogRecordExporterBuilder> &&builder)
{
  log_record_exporter_builders_.erase(name);
  log_record_exporter_builders_.insert({name, std::move(builder)});
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

void Registry::SetExtensionLogRecordProcessorBuilder(
    const std::string &name,
    std::unique_ptr<ExtensionLogRecordProcessorBuilder> &&builder)
{
  log_record_processor_builders_.erase(name);
  log_record_processor_builders_.insert({name, std::move(builder)});
}

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
