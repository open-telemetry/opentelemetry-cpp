// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <map>
#include <memory>
#include <string>
#include <utility>

#include "opentelemetry/baggage/propagation/baggage_propagator.h"
#include "opentelemetry/context/propagation/text_map_propagator.h"
#include "opentelemetry/sdk/configuration/always_off_sampler_builder.h"
#include "opentelemetry/sdk/configuration/always_on_sampler_builder.h"
#include "opentelemetry/sdk/configuration/batch_log_record_processor_builder.h"
#include "opentelemetry/sdk/configuration/batch_span_processor_builder.h"
#include "opentelemetry/sdk/configuration/composable_always_off_sampler_builder.h"
#include "opentelemetry/sdk/configuration/composable_always_on_sampler_builder.h"
#include "opentelemetry/sdk/configuration/composable_parent_threshold_sampler_builder.h"
#include "opentelemetry/sdk/configuration/composable_probability_sampler_builder.h"
#include "opentelemetry/sdk/configuration/composable_rule_based_sampler_builder.h"
#include "opentelemetry/sdk/configuration/composite_sampler_builder.h"
#include "opentelemetry/sdk/configuration/console_log_record_exporter_builder.h"
#include "opentelemetry/sdk/configuration/console_push_metric_exporter_builder.h"
#include "opentelemetry/sdk/configuration/console_span_exporter_builder.h"
#include "opentelemetry/sdk/configuration/container_resource_detector_builder.h"
#include "opentelemetry/sdk/configuration/extension_composable_sampler_builder.h"
#include "opentelemetry/sdk/configuration/extension_log_record_exporter_builder.h"
#include "opentelemetry/sdk/configuration/extension_log_record_processor_builder.h"
#include "opentelemetry/sdk/configuration/extension_metric_producer_builder.h"
#include "opentelemetry/sdk/configuration/extension_pull_metric_exporter_builder.h"
#include "opentelemetry/sdk/configuration/extension_push_metric_exporter_builder.h"
#include "opentelemetry/sdk/configuration/extension_resource_detector_builder.h"
#include "opentelemetry/sdk/configuration/extension_sampler_builder.h"
#include "opentelemetry/sdk/configuration/extension_span_exporter_builder.h"
#include "opentelemetry/sdk/configuration/extension_span_processor_builder.h"
#include "opentelemetry/sdk/configuration/host_resource_detector_builder.h"
#include "opentelemetry/sdk/configuration/jaeger_remote_sampler_builder.h"
#include "opentelemetry/sdk/configuration/logger_configurator_builder.h"
#include "opentelemetry/sdk/configuration/logger_provider_builder.h"
#include "opentelemetry/sdk/configuration/meter_configurator_builder.h"
#include "opentelemetry/sdk/configuration/meter_provider_builder.h"
#include "opentelemetry/sdk/configuration/open_census_metric_producer_builder.h"
#include "opentelemetry/sdk/configuration/otlp_file_log_record_exporter_builder.h"
#include "opentelemetry/sdk/configuration/otlp_file_push_metric_exporter_builder.h"
#include "opentelemetry/sdk/configuration/otlp_file_span_exporter_builder.h"
#include "opentelemetry/sdk/configuration/otlp_grpc_log_record_exporter_builder.h"
#include "opentelemetry/sdk/configuration/otlp_grpc_push_metric_exporter_builder.h"
#include "opentelemetry/sdk/configuration/otlp_grpc_span_exporter_builder.h"
#include "opentelemetry/sdk/configuration/otlp_http_log_record_exporter_builder.h"
#include "opentelemetry/sdk/configuration/otlp_http_push_metric_exporter_builder.h"
#include "opentelemetry/sdk/configuration/otlp_http_span_exporter_builder.h"
#include "opentelemetry/sdk/configuration/parent_based_sampler_builder.h"
#include "opentelemetry/sdk/configuration/periodic_metric_reader_builder.h"
#include "opentelemetry/sdk/configuration/probability_sampler_builder.h"
#include "opentelemetry/sdk/configuration/process_resource_detector_builder.h"
#include "opentelemetry/sdk/configuration/prometheus_pull_metric_exporter_builder.h"
#include "opentelemetry/sdk/configuration/pull_metric_reader_builder.h"
#include "opentelemetry/sdk/configuration/registry.h"
#include "opentelemetry/sdk/configuration/service_resource_detector_builder.h"
#include "opentelemetry/sdk/configuration/simple_log_record_processor_builder.h"
#include "opentelemetry/sdk/configuration/simple_span_processor_builder.h"
#include "opentelemetry/sdk/configuration/text_map_propagator_builder.h"
#include "opentelemetry/sdk/configuration/trace_id_ratio_based_sampler_builder.h"
#include "opentelemetry/sdk/configuration/tracer_configurator_builder.h"
#include "opentelemetry/sdk/configuration/tracer_provider_builder.h"
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

}  // namespace

Registry::Registry()
{
  SetTextMapPropagatorBuilder("tracecontext", std::make_unique<TraceContextBuilder>());
  SetTextMapPropagatorBuilder("baggage", std::make_unique<BaggageBuilder>());
  SetTextMapPropagatorBuilder("b3", std::make_unique<B3Builder>());
  SetTextMapPropagatorBuilder("b3multi", std::make_unique<B3MultiBuilder>());
  SetTextMapPropagatorBuilder("jaeger", std::make_unique<JaegerBuilder>());
}

Registry::~Registry() = default;

void Registry::SetOtlpHttpSpanBuilder(std::unique_ptr<OtlpHttpSpanExporterBuilder> &&builder)
{
  otlp_http_span_builder_ = std::move(builder);
}

void Registry::SetOtlpGrpcSpanBuilder(std::unique_ptr<OtlpGrpcSpanExporterBuilder> &&builder)
{
  otlp_grpc_span_builder_ = std::move(builder);
}

void Registry::SetOtlpFileSpanBuilder(std::unique_ptr<OtlpFileSpanExporterBuilder> &&builder)
{
  otlp_file_span_builder_ = std::move(builder);
}

void Registry::SetConsoleSpanBuilder(std::unique_ptr<ConsoleSpanExporterBuilder> &&builder)
{
  console_span_builder_ = std::move(builder);
}

void Registry::SetOtlpHttpPushMetricExporterBuilder(
    std::unique_ptr<OtlpHttpPushMetricExporterBuilder> &&builder)
{
  otlp_http_push_metric_builder_ = std::move(builder);
}

void Registry::SetOtlpGrpcPushMetricExporterBuilder(
    std::unique_ptr<OtlpGrpcPushMetricExporterBuilder> &&builder)
{
  otlp_grpc_push_metric_builder_ = std::move(builder);
}

void Registry::SetOtlpFilePushMetricExporterBuilder(
    std::unique_ptr<OtlpFilePushMetricExporterBuilder> &&builder)
{
  otlp_file_push_metric_builder_ = std::move(builder);
}

void Registry::SetConsolePushMetricExporterBuilder(
    std::unique_ptr<ConsolePushMetricExporterBuilder> &&builder)
{
  console_metric_builder_ = std::move(builder);
}

void Registry::SetPrometheusPullMetricExporterBuilder(
    std::unique_ptr<PrometheusPullMetricExporterBuilder> &&builder)
{
  prometheus_metric_builder_ = std::move(builder);
}

void Registry::SetOpenCensusMetricProducerBuilder(
    std::unique_ptr<OpenCensusMetricProducerBuilder> &&builder)
{
  open_census_metric_producer_builder_ = std::move(builder);
}

void Registry::SetPeriodicMetricReaderBuilder(
    std::unique_ptr<PeriodicMetricReaderBuilder> &&builder)
{
  periodic_metric_reader_builder_ = std::move(builder);
}

void Registry::SetPullMetricReaderBuilder(std::unique_ptr<PullMetricReaderBuilder> &&builder)
{
  pull_metric_reader_builder_ = std::move(builder);
}

void Registry::SetOtlpHttpLogRecordBuilder(
    std::unique_ptr<OtlpHttpLogRecordExporterBuilder> &&builder)
{
  otlp_http_log_record_builder_ = std::move(builder);
}

void Registry::SetOtlpGrpcLogRecordBuilder(
    std::unique_ptr<OtlpGrpcLogRecordExporterBuilder> &&builder)
{
  otlp_grpc_log_record_builder_ = std::move(builder);
}

void Registry::SetOtlpFileLogRecordBuilder(
    std::unique_ptr<OtlpFileLogRecordExporterBuilder> &&builder)
{
  otlp_file_log_record_builder_ = std::move(builder);
}

void Registry::SetConsoleLogRecordBuilder(
    std::unique_ptr<ConsoleLogRecordExporterBuilder> &&builder)
{
  console_log_record_builder_ = std::move(builder);
}

void Registry::SetAlwaysOnSamplerBuilder(std::unique_ptr<AlwaysOnSamplerBuilder> &&builder)
{
  always_on_sampler_builder_ = std::move(builder);
}

void Registry::SetAlwaysOffSamplerBuilder(std::unique_ptr<AlwaysOffSamplerBuilder> &&builder)
{
  always_off_sampler_builder_ = std::move(builder);
}

void Registry::SetTraceIdRatioBasedSamplerBuilder(
    std::unique_ptr<TraceIdRatioBasedSamplerBuilder> &&builder)
{
  trace_id_ratio_based_sampler_builder_ = std::move(builder);
}

void Registry::SetProbabilitySamplerBuilder(std::unique_ptr<ProbabilitySamplerBuilder> &&builder)
{
  probability_sampler_builder_ = std::move(builder);
}

void Registry::SetParentBasedSamplerBuilder(std::unique_ptr<ParentBasedSamplerBuilder> &&builder)
{
  parent_based_sampler_builder_ = std::move(builder);
}

void Registry::SetJaegerRemoteSamplerBuilder(std::unique_ptr<JaegerRemoteSamplerBuilder> &&builder)
{
  jaeger_remote_sampler_builder_ = std::move(builder);
}

void Registry::SetComposableAlwaysOnSamplerBuilder(
    std::unique_ptr<ComposableAlwaysOnSamplerBuilder> &&builder)
{
  composable_always_on_sampler_builder_ = std::move(builder);
}

void Registry::SetComposableAlwaysOffSamplerBuilder(
    std::unique_ptr<ComposableAlwaysOffSamplerBuilder> &&builder)
{
  composable_always_off_sampler_builder_ = std::move(builder);
}

void Registry::SetComposableProbabilitySamplerBuilder(
    std::unique_ptr<ComposableProbabilitySamplerBuilder> &&builder)
{
  composable_probability_sampler_builder_ = std::move(builder);
}

void Registry::SetComposableParentThresholdSamplerBuilder(
    std::unique_ptr<ComposableParentThresholdSamplerBuilder> &&builder)
{
  composable_parent_threshold_sampler_builder_ = std::move(builder);
}

void Registry::SetComposableRuleBasedSamplerBuilder(
    std::unique_ptr<ComposableRuleBasedSamplerBuilder> &&builder)
{
  composable_rule_based_sampler_builder_ = std::move(builder);
}

void Registry::SetCompositeSamplerBuilder(std::unique_ptr<CompositeSamplerBuilder> &&builder)
{
  composite_sampler_builder_ = std::move(builder);
}

void Registry::SetBatchSpanProcessorBuilder(std::unique_ptr<BatchSpanProcessorBuilder> &&builder)
{
  batch_span_processor_builder_ = std::move(builder);
}

void Registry::SetSimpleSpanProcessorBuilder(std::unique_ptr<SimpleSpanProcessorBuilder> &&builder)
{
  simple_span_processor_builder_ = std::move(builder);
}

void Registry::SetBatchLogRecordProcessorBuilder(
    std::unique_ptr<BatchLogRecordProcessorBuilder> &&builder)
{
  batch_log_record_processor_builder_ = std::move(builder);
}

void Registry::SetSimpleLogRecordProcessorBuilder(
    std::unique_ptr<SimpleLogRecordProcessorBuilder> &&builder)
{
  simple_log_record_processor_builder_ = std::move(builder);
}

void Registry::SetTracerConfiguratorBuilder(std::unique_ptr<TracerConfiguratorBuilder> &&builder)
{
  tracer_configurator_builder_ = std::move(builder);
}

void Registry::SetMeterConfiguratorBuilder(std::unique_ptr<MeterConfiguratorBuilder> &&builder)
{
  meter_configurator_builder_ = std::move(builder);
}

void Registry::SetLoggerConfiguratorBuilder(std::unique_ptr<LoggerConfiguratorBuilder> &&builder)
{
  logger_configurator_builder_ = std::move(builder);
}

void Registry::SetTracerProviderBuilder(std::unique_ptr<TracerProviderBuilder> &&builder)
{
  tracer_provider_builder_ = std::move(builder);
}

void Registry::SetMeterProviderBuilder(std::unique_ptr<MeterProviderBuilder> &&builder)
{
  meter_provider_builder_ = std::move(builder);
}

void Registry::SetLoggerProviderBuilder(std::unique_ptr<LoggerProviderBuilder> &&builder)
{
  logger_provider_builder_ = std::move(builder);
}

void Registry::SetContainerResourceDetectorBuilder(
    std::unique_ptr<ContainerResourceDetectorBuilder> &&builder)
{
  container_resource_detector_builder_ = std::move(builder);
}

void Registry::SetHostResourceDetectorBuilder(
    std::unique_ptr<HostResourceDetectorBuilder> &&builder)
{
  host_resource_detector_builder_ = std::move(builder);
}

void Registry::SetProcessResourceDetectorBuilder(
    std::unique_ptr<ProcessResourceDetectorBuilder> &&builder)
{
  process_resource_detector_builder_ = std::move(builder);
}

void Registry::SetServiceResourceDetectorBuilder(
    std::unique_ptr<ServiceResourceDetectorBuilder> &&builder)
{
  service_resource_detector_builder_ = std::move(builder);
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

const ExtensionComposableSamplerBuilder *Registry::GetExtensionComposableSamplerBuilder(
    const std::string &name) const
{
  ExtensionComposableSamplerBuilder *builder = nullptr;
  auto search                                = composable_sampler_builders_.find(name);
  if (search != composable_sampler_builders_.end())
  {
    builder = search->second.get();
  }
  return builder;
}

void Registry::SetExtensionComposableSamplerBuilder(
    const std::string &name,
    std::unique_ptr<ExtensionComposableSamplerBuilder> &&builder)
{
  composable_sampler_builders_.erase(name);
  composable_sampler_builders_.insert({name, std::move(builder)});
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

const ExtensionMetricProducerBuilder *Registry::GetExtensionMetricProducerBuilder(
    const std::string &name) const
{
  ExtensionMetricProducerBuilder *builder = nullptr;
  auto search                             = metric_producer_builders_.find(name);
  if (search != metric_producer_builders_.end())
  {
    builder = search->second.get();
  }
  return builder;
}

void Registry::SetExtensionMetricProducerBuilder(
    const std::string &name,
    std::unique_ptr<ExtensionMetricProducerBuilder> &&builder)
{
  metric_producer_builders_.erase(name);
  metric_producer_builders_.insert({name, std::move(builder)});
}

const ExtensionResourceDetectorBuilder *Registry::GetExtensionResourceDetectorBuilder(
    const std::string &name) const
{
  ExtensionResourceDetectorBuilder *builder = nullptr;
  auto search                               = resource_detector_builders_.find(name);
  if (search != resource_detector_builders_.end())
  {
    builder = search->second.get();
  }
  return builder;
}

void Registry::SetExtensionResourceDetectorBuilder(
    const std::string &name,
    std::unique_ptr<ExtensionResourceDetectorBuilder> &&builder)
{
  resource_detector_builders_.erase(name);
  resource_detector_builders_.insert({name, std::move(builder)});
}

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
