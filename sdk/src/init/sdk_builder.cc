// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <algorithm>
#include <chrono>
#include <map>
#include <memory>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include "opentelemetry/context/propagation/composite_propagator.h"
#include "opentelemetry/context/propagation/text_map_propagator.h"
#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/configuration/always_off_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/always_on_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/attributes_configuration.h"
#include "opentelemetry/sdk/configuration/batch_log_record_processor_configuration.h"
#include "opentelemetry/sdk/configuration/batch_span_processor_configuration.h"
#include "opentelemetry/sdk/configuration/configuration.h"
#include "opentelemetry/sdk/configuration/console_log_record_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/console_push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/console_span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/extension_log_record_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/extension_log_record_processor_configuration.h"
#include "opentelemetry/sdk/configuration/extension_pull_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/extension_push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/extension_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/extension_span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/extension_span_processor_configuration.h"
#include "opentelemetry/sdk/configuration/jaeger_remote_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/log_record_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/log_record_exporter_configuration_visitor.h"
#include "opentelemetry/sdk/configuration/log_record_processor_configuration.h"
#include "opentelemetry/sdk/configuration/log_record_processor_configuration_visitor.h"
#include "opentelemetry/sdk/configuration/logger_provider_configuration.h"
#include "opentelemetry/sdk/configuration/meter_provider_configuration.h"
#include "opentelemetry/sdk/configuration/metric_reader_configuration.h"
#include "opentelemetry/sdk/configuration/metric_reader_configuration_visitor.h"
#include "opentelemetry/sdk/configuration/otlp_log_record_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/otlp_push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/otlp_span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/parent_based_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/periodic_metric_reader_configuration.h"
#include "opentelemetry/sdk/configuration/prometheus_pull_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/propagator_configuration.h"
#include "opentelemetry/sdk/configuration/pull_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/pull_metric_exporter_configuration_visitor.h"
#include "opentelemetry/sdk/configuration/pull_metric_reader_configuration.h"
#include "opentelemetry/sdk/configuration/push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/push_metric_exporter_configuration_visitor.h"
#include "opentelemetry/sdk/configuration/resource_configuration.h"
#include "opentelemetry/sdk/configuration/sampler_configuration.h"
#include "opentelemetry/sdk/configuration/sampler_configuration_visitor.h"
#include "opentelemetry/sdk/configuration/selector_configuration.h"
#include "opentelemetry/sdk/configuration/simple_log_record_processor_configuration.h"
#include "opentelemetry/sdk/configuration/simple_span_processor_configuration.h"
#include "opentelemetry/sdk/configuration/span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/span_exporter_configuration_visitor.h"
#include "opentelemetry/sdk/configuration/span_processor_configuration.h"
#include "opentelemetry/sdk/configuration/span_processor_configuration_visitor.h"
#include "opentelemetry/sdk/configuration/stream_configuration.h"
#include "opentelemetry/sdk/configuration/trace_id_ratio_based_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/tracer_provider_configuration.h"
#include "opentelemetry/sdk/configuration/view_configuration.h"
#include "opentelemetry/sdk/configuration/zipkin_span_exporter_configuration.h"
#include "opentelemetry/sdk/init/configured_sdk.h"
#include "opentelemetry/sdk/init/console_log_record_exporter_builder.h"
#include "opentelemetry/sdk/init/console_push_metric_exporter_builder.h"
#include "opentelemetry/sdk/init/console_span_exporter_builder.h"
#include "opentelemetry/sdk/init/extension_log_record_exporter_builder.h"
#include "opentelemetry/sdk/init/extension_log_record_processor_builder.h"
#include "opentelemetry/sdk/init/extension_pull_metric_exporter_builder.h"
#include "opentelemetry/sdk/init/extension_push_metric_exporter_builder.h"
#include "opentelemetry/sdk/init/extension_sampler_builder.h"
#include "opentelemetry/sdk/init/extension_span_exporter_builder.h"
#include "opentelemetry/sdk/init/extension_span_processor_builder.h"
#include "opentelemetry/sdk/init/otlp_log_record_exporter_builder.h"
#include "opentelemetry/sdk/init/otlp_push_metric_exporter_builder.h"
#include "opentelemetry/sdk/init/otlp_span_exporter_builder.h"
#include "opentelemetry/sdk/init/prometheus_pull_metric_exporter_builder.h"
#include "opentelemetry/sdk/init/registry.h"
#include "opentelemetry/sdk/init/sdk_builder.h"
#include "opentelemetry/sdk/init/text_map_propagator_builder.h"
#include "opentelemetry/sdk/init/unsupported_exception.h"
#include "opentelemetry/sdk/init/zipkin_span_exporter_builder.h"
#include "opentelemetry/sdk/logs/batch_log_record_processor_factory.h"
#include "opentelemetry/sdk/logs/batch_log_record_processor_options.h"
#include "opentelemetry/sdk/logs/exporter.h"
#include "opentelemetry/sdk/logs/logger_provider.h"
#include "opentelemetry/sdk/logs/logger_provider_factory.h"
#include "opentelemetry/sdk/logs/processor.h"
#include "opentelemetry/sdk/logs/simple_log_record_processor_factory.h"
#include "opentelemetry/sdk/metrics/aggregation/aggregation_config.h"
#include "opentelemetry/sdk/metrics/export/periodic_exporting_metric_reader_factory.h"
#include "opentelemetry/sdk/metrics/export/periodic_exporting_metric_reader_options.h"
#include "opentelemetry/sdk/metrics/instruments.h"
#include "opentelemetry/sdk/metrics/meter_context.h"
#include "opentelemetry/sdk/metrics/meter_context_factory.h"
#include "opentelemetry/sdk/metrics/meter_provider.h"
#include "opentelemetry/sdk/metrics/meter_provider_factory.h"
#include "opentelemetry/sdk/metrics/metric_reader.h"
#include "opentelemetry/sdk/metrics/push_metric_exporter.h"
#include "opentelemetry/sdk/metrics/view/attributes_processor.h"
#include "opentelemetry/sdk/metrics/view/instrument_selector.h"
#include "opentelemetry/sdk/metrics/view/meter_selector.h"
#include "opentelemetry/sdk/metrics/view/view.h"
#include "opentelemetry/sdk/metrics/view/view_registry.h"
#include "opentelemetry/sdk/metrics/view/view_registry_factory.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/sdk/trace/batch_span_processor_factory.h"
#include "opentelemetry/sdk/trace/batch_span_processor_options.h"
#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/processor.h"
#include "opentelemetry/sdk/trace/sampler.h"
#include "opentelemetry/sdk/trace/samplers/always_off_factory.h"
#include "opentelemetry/sdk/trace/samplers/always_on_factory.h"
#include "opentelemetry/sdk/trace/samplers/parent_factory.h"
#include "opentelemetry/sdk/trace/samplers/trace_id_ratio_factory.h"
#include "opentelemetry/sdk/trace/simple_processor_factory.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/sdk/trace/tracer_provider_factory.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace init
{

class SamplerBuilder : public opentelemetry::sdk::configuration::SamplerConfigurationVisitor
{
public:
  SamplerBuilder(const SdkBuilder *b) : m_sdk_builder(b) {}
  ~SamplerBuilder() override = default;

  void VisitAlwaysOff(
      const opentelemetry::sdk::configuration::AlwaysOffSamplerConfiguration *model) override
  {
    sampler = m_sdk_builder->CreateAlwaysOffSampler(model);
  }

  void VisitAlwaysOn(
      const opentelemetry::sdk::configuration::AlwaysOnSamplerConfiguration *model) override
  {
    sampler = m_sdk_builder->CreateAlwaysOnSampler(model);
  }

  void VisitJaegerRemote(
      const opentelemetry::sdk::configuration::JaegerRemoteSamplerConfiguration *model) override
  {
    sampler = m_sdk_builder->CreateJaegerRemoteSampler(model);
  }

  void VisitParentBased(
      const opentelemetry::sdk::configuration::ParentBasedSamplerConfiguration *model) override
  {
    sampler = m_sdk_builder->CreateParentBasedSampler(model);
  }

  void VisitTraceIdRatioBased(
      const opentelemetry::sdk::configuration::TraceIdRatioBasedSamplerConfiguration *model)
      override
  {
    sampler = m_sdk_builder->CreateTraceIdRatioBasedSampler(model);
  }

  void VisitExtension(
      const opentelemetry::sdk::configuration::ExtensionSamplerConfiguration *model) override
  {
    sampler = m_sdk_builder->CreateExtensionSampler(model);
  }

  std::unique_ptr<opentelemetry::sdk::trace::Sampler> sampler;

private:
  const SdkBuilder *m_sdk_builder;
};

class SpanProcessorBuilder
    : public opentelemetry::sdk::configuration::SpanProcessorConfigurationVisitor
{
public:
  SpanProcessorBuilder(const SdkBuilder *b) : m_sdk_builder(b) {}
  ~SpanProcessorBuilder() override = default;

  void VisitBatch(
      const opentelemetry::sdk::configuration::BatchSpanProcessorConfiguration *model) override
  {
    processor = m_sdk_builder->CreateBatchSpanProcessor(model);
  }

  void VisitSimple(
      const opentelemetry::sdk::configuration::SimpleSpanProcessorConfiguration *model) override
  {
    processor = m_sdk_builder->CreateSimpleSpanProcessor(model);
  }

  void VisitExtension(
      const opentelemetry::sdk::configuration::ExtensionSpanProcessorConfiguration *model) override
  {
    processor = m_sdk_builder->CreateExtensionSpanProcessor(model);
  }

  std::unique_ptr<opentelemetry::sdk::trace::SpanProcessor> processor;

private:
  const SdkBuilder *m_sdk_builder;
};

class SpanExporterBuilder
    : public opentelemetry::sdk::configuration::SpanExporterConfigurationVisitor
{
public:
  SpanExporterBuilder(const SdkBuilder *b) : m_sdk_builder(b) {}
  ~SpanExporterBuilder() override = default;

  void VisitOtlp(
      const opentelemetry::sdk::configuration::OtlpSpanExporterConfiguration *model) override
  {
    exporter = m_sdk_builder->CreateOtlpSpanExporter(model);
  }

  void VisitConsole(
      const opentelemetry::sdk::configuration::ConsoleSpanExporterConfiguration *model) override
  {
    exporter = m_sdk_builder->CreateConsoleSpanExporter(model);
  }

  void VisitZipkin(
      const opentelemetry::sdk::configuration::ZipkinSpanExporterConfiguration *model) override
  {
    exporter = m_sdk_builder->CreateZipkinSpanExporter(model);
  }

  void VisitExtension(
      const opentelemetry::sdk::configuration::ExtensionSpanExporterConfiguration *model) override
  {
    exporter = m_sdk_builder->CreateExtensionSpanExporter(model);
  }

  std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> exporter;

private:
  const SdkBuilder *m_sdk_builder;
};

class MetricReaderBuilder
    : public opentelemetry::sdk::configuration::MetricReaderConfigurationVisitor
{
public:
  MetricReaderBuilder(const SdkBuilder *b) : m_sdk_builder(b) {}
  ~MetricReaderBuilder() override = default;

  void VisitPeriodic(
      const opentelemetry::sdk::configuration::PeriodicMetricReaderConfiguration *model) override
  {
    metric_reader = m_sdk_builder->CreatePeriodicMetricReader(model);
  }

  void VisitPull(
      const opentelemetry::sdk::configuration::PullMetricReaderConfiguration *model) override
  {
    metric_reader = m_sdk_builder->CreatePullMetricReader(model);
  }

  std::unique_ptr<opentelemetry::sdk::metrics::MetricReader> metric_reader;

private:
  const SdkBuilder *m_sdk_builder;
};

class PushMetricExporterBuilder
    : public opentelemetry::sdk::configuration::PushMetricExporterConfigurationVisitor
{
public:
  PushMetricExporterBuilder(const SdkBuilder *b) : m_sdk_builder(b) {}
  ~PushMetricExporterBuilder() override = default;

  void VisitOtlp(
      const opentelemetry::sdk::configuration::OtlpPushMetricExporterConfiguration *model) override
  {
    exporter = m_sdk_builder->CreateOtlpPushMetricExporter(model);
  }

  void VisitConsole(const opentelemetry::sdk::configuration::ConsolePushMetricExporterConfiguration
                        *model) override
  {
    exporter = m_sdk_builder->CreateConsolePushMetricExporter(model);
  }

  void VisitExtension(
      const opentelemetry::sdk::configuration::ExtensionPushMetricExporterConfiguration *model)
      override
  {
    exporter = m_sdk_builder->CreateExtensionPushMetricExporter(model);
  }

  std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter> exporter;

private:
  const SdkBuilder *m_sdk_builder;
};

class PullMetricExporterBuilder
    : public opentelemetry::sdk::configuration::PullMetricExporterConfigurationVisitor
{
public:
  PullMetricExporterBuilder(const SdkBuilder *b) : m_sdk_builder(b) {}
  ~PullMetricExporterBuilder() override = default;

  void VisitPrometheus(
      const opentelemetry::sdk::configuration::PrometheusPullMetricExporterConfiguration *model)
      override
  {
    exporter = m_sdk_builder->CreatePrometheusPullMetricExporter(model);
  }

  void VisitExtension(
      const opentelemetry::sdk::configuration::ExtensionPullMetricExporterConfiguration *model)
      override
  {
    exporter = m_sdk_builder->CreateExtensionPullMetricExporter(model);
  }

  std::unique_ptr<opentelemetry::sdk::metrics::MetricReader> exporter;

private:
  const SdkBuilder *m_sdk_builder;
};

class LogRecordProcessorBuilder
    : public opentelemetry::sdk::configuration::LogRecordProcessorConfigurationVisitor
{
public:
  LogRecordProcessorBuilder(const SdkBuilder *b) : m_sdk_builder(b) {}
  ~LogRecordProcessorBuilder() override = default;

  void VisitBatch(
      const opentelemetry::sdk::configuration::BatchLogRecordProcessorConfiguration *model) override
  {
    processor = m_sdk_builder->CreateBatchLogRecordProcessor(model);
  }

  void VisitSimple(const opentelemetry::sdk::configuration::SimpleLogRecordProcessorConfiguration
                       *model) override
  {
    processor = m_sdk_builder->CreateSimpleLogRecordProcessor(model);
  }

  void VisitExtension(
      const opentelemetry::sdk::configuration::ExtensionLogRecordProcessorConfiguration *model)
      override
  {
    processor = m_sdk_builder->CreateExtensionLogRecordProcessor(model);
  }

  std::unique_ptr<opentelemetry::sdk::logs::LogRecordProcessor> processor;

private:
  const SdkBuilder *m_sdk_builder;
};

class LogRecordExporterBuilder
    : public opentelemetry::sdk::configuration::LogRecordExporterConfigurationVisitor
{
public:
  LogRecordExporterBuilder(const SdkBuilder *b) : m_sdk_builder(b) {}
  ~LogRecordExporterBuilder() override = default;

  void VisitOtlp(
      const opentelemetry::sdk::configuration::OtlpLogRecordExporterConfiguration *model) override
  {
    exporter = m_sdk_builder->CreateOtlpLogRecordExporter(model);
  }

  void VisitConsole(const opentelemetry::sdk::configuration::ConsoleLogRecordExporterConfiguration
                        *model) override
  {
    exporter = m_sdk_builder->CreateConsoleLogRecordExporter(model);
  }

  void VisitExtension(
      const opentelemetry::sdk::configuration::ExtensionLogRecordExporterConfiguration *model)
      override
  {
    exporter = m_sdk_builder->CreateExtensionLogRecordExporter(model);
  }

  std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> exporter;

private:
  const SdkBuilder *m_sdk_builder;
};

std::unique_ptr<opentelemetry::sdk::trace::Sampler> SdkBuilder::CreateAlwaysOffSampler(
    const opentelemetry::sdk::configuration::AlwaysOffSamplerConfiguration * /* model */) const
{
  std::unique_ptr<opentelemetry::sdk::trace::Sampler> sdk;

  sdk = opentelemetry::sdk::trace::AlwaysOffSamplerFactory::Create();

  return sdk;
}

std::unique_ptr<opentelemetry::sdk::trace::Sampler> SdkBuilder::CreateAlwaysOnSampler(
    const opentelemetry::sdk::configuration::AlwaysOnSamplerConfiguration * /* model */) const
{
  std::unique_ptr<opentelemetry::sdk::trace::Sampler> sdk;

  sdk = opentelemetry::sdk::trace::AlwaysOnSamplerFactory::Create();

  return sdk;
}

std::unique_ptr<opentelemetry::sdk::trace::Sampler> SdkBuilder::CreateJaegerRemoteSampler(
    const opentelemetry::sdk::configuration::JaegerRemoteSamplerConfiguration * /* model */) const
{
  std::unique_ptr<opentelemetry::sdk::trace::Sampler> sdk;

  std::string die("JeagerRemoteSampler not available in opentelemetry-cpp");
  OTEL_INTERNAL_LOG_ERROR(die);
  throw UnsupportedException(die);

  return sdk;
}

std::unique_ptr<opentelemetry::sdk::trace::Sampler> SdkBuilder::CreateParentBasedSampler(
    const opentelemetry::sdk::configuration::ParentBasedSamplerConfiguration *model) const
{
  std::unique_ptr<opentelemetry::sdk::trace::Sampler> sdk;

  auto root_sdk                      = SdkBuilder::CreateSampler(model->root);
  auto remote_parent_sampled_sdk     = SdkBuilder::CreateSampler(model->remote_parent_sampled);
  auto remote_parent_not_sampled_sdk = SdkBuilder::CreateSampler(model->remote_parent_not_sampled);
  auto local_parent_sampled_sdk      = SdkBuilder::CreateSampler(model->local_parent_sampled);
  auto local_parent_not_sampled_sdk  = SdkBuilder::CreateSampler(model->local_parent_not_sampled);

  OTEL_INTERNAL_LOG_ERROR("CreateParentBasedSampler: FIXME, missing param in parent factory");
  std::shared_ptr<opentelemetry::sdk::trace::Sampler> delegate_sampler = std::move(root_sdk);
  sdk = opentelemetry::sdk::trace::ParentBasedSamplerFactory::Create(delegate_sampler);

  return sdk;
}

std::unique_ptr<opentelemetry::sdk::trace::Sampler> SdkBuilder::CreateTraceIdRatioBasedSampler(
    const opentelemetry::sdk::configuration::TraceIdRatioBasedSamplerConfiguration *model) const
{
  std::unique_ptr<opentelemetry::sdk::trace::Sampler> sdk;

  sdk = opentelemetry::sdk::trace::TraceIdRatioBasedSamplerFactory::Create(model->ratio);

  return sdk;
}

std::unique_ptr<opentelemetry::sdk::trace::Sampler> SdkBuilder::CreateExtensionSampler(
    const opentelemetry::sdk::configuration::ExtensionSamplerConfiguration *model) const
{
  std::unique_ptr<opentelemetry::sdk::trace::Sampler> sdk;
  std::string name = model->name;

  const ExtensionSamplerBuilder *builder = m_registry->GetExtensionSamplerBuilder(name);

  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateExtensionSampler() using registered builder " << name);
    sdk = builder->Build(model);
  }
  else
  {
    std::string die("CreateExtensionSampler() no builder for ");
    die.append(name);
    OTEL_INTERNAL_LOG_ERROR(die);
    throw UnsupportedException(die);
  }
  return sdk;
}

std::unique_ptr<opentelemetry::sdk::trace::Sampler> SdkBuilder::CreateSampler(
    const std::unique_ptr<opentelemetry::sdk::configuration::SamplerConfiguration> &model) const
{
  std::unique_ptr<opentelemetry::sdk::trace::Sampler> sdk;

  SamplerBuilder builder(this);
  model->Accept(&builder);
  sdk = std::move(builder.sampler);

  return sdk;
}

std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> SdkBuilder::CreateOtlpSpanExporter(
    const opentelemetry::sdk::configuration::OtlpSpanExporterConfiguration *model) const
{
  std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> sdk;
  const OtlpSpanExporterBuilder *builder;

  if (model->protocol.find("http/") == 0)
  {
    builder = m_registry->GetOtlpHttpSpanBuilder();
    if (builder != nullptr)
    {
      OTEL_INTERNAL_LOG_DEBUG("CreateOtlpSpanExporter() using registered http builder");
      sdk = builder->Build(model);
      return sdk;
    }

    std::string die("No http builder for OtlpSpanExporter");
    OTEL_INTERNAL_LOG_ERROR(die);
    throw UnsupportedException(die);
  }

  if (model->protocol.find("grpc/") == 0)
  {
    builder = m_registry->GetOtlpGrpcSpanBuilder();
    if (builder != nullptr)
    {
      OTEL_INTERNAL_LOG_DEBUG("CreateOtlpSpanExporter() using registered grpc builder");
      sdk = builder->Build(model);
      return sdk;
    }

    std::string die("No builder for OtlpSpanExporter");
    OTEL_INTERNAL_LOG_ERROR(die);
    throw UnsupportedException(die);
  }

  std::string die("OtlpSpanExporter: illegal protocol: ");
  die.append(model->protocol);
  OTEL_INTERNAL_LOG_ERROR(die);
  throw UnsupportedException(die);

  return sdk;
}

std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> SdkBuilder::CreateConsoleSpanExporter(
    const opentelemetry::sdk::configuration::ConsoleSpanExporterConfiguration *model) const
{
  std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> sdk;
  const ConsoleSpanExporterBuilder *builder = m_registry->GetConsoleSpanBuilder();

  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateConsoleSpanExporter() using registered builder");
    sdk = builder->Build(model);
  }
  else
  {
    std::string die("No builder for ConsoleSpanExporter");
    OTEL_INTERNAL_LOG_ERROR(die);
    throw UnsupportedException(die);
  }

  return sdk;
}

std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> SdkBuilder::CreateZipkinSpanExporter(
    const opentelemetry::sdk::configuration::ZipkinSpanExporterConfiguration *model) const
{
  std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> sdk;
  const ZipkinSpanExporterBuilder *builder = m_registry->GetZipkinSpanBuilder();

  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateZipkinSpanExporter() using registered builder");
    sdk = builder->Build(model);
  }
  else
  {
    std::string die("No builder for ZipkinSpanExporter");
    OTEL_INTERNAL_LOG_ERROR(die);
    throw UnsupportedException(die);
  }

  return sdk;
}

std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> SdkBuilder::CreateExtensionSpanExporter(
    const opentelemetry::sdk::configuration::ExtensionSpanExporterConfiguration *model) const
{
  std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> sdk;
  std::string name = model->name;

  const ExtensionSpanExporterBuilder *builder = m_registry->GetExtensionSpanExporterBuilder(name);

  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateExtensionSpanExporter() using registered builder " << name);
    sdk = builder->Build(model);
  }
  else
  {
    std::string die("CreateExtensionSpanExporter() no builder for ");
    die.append(name);
    OTEL_INTERNAL_LOG_ERROR(die);
    throw UnsupportedException(die);
  }
  return sdk;
}

std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> SdkBuilder::CreateSpanExporter(
    const std::unique_ptr<opentelemetry::sdk::configuration::SpanExporterConfiguration> &model)
    const
{
  std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> sdk;

  SpanExporterBuilder builder(this);
  model->Accept(&builder);
  sdk = std::move(builder.exporter);

  return sdk;
}

std::unique_ptr<opentelemetry::sdk::trace::SpanProcessor> SdkBuilder::CreateBatchSpanProcessor(
    const opentelemetry::sdk::configuration::BatchSpanProcessorConfiguration *model) const
{
  std::unique_ptr<opentelemetry::sdk::trace::SpanProcessor> sdk;
  opentelemetry::sdk::trace::BatchSpanProcessorOptions options;

  options.schedule_delay_millis = std::chrono::milliseconds(model->schedule_delay);

#ifdef LATER
  options.xxx = model->export_timeout;
#endif

  options.max_queue_size        = model->max_queue_size;
  options.max_export_batch_size = model->max_export_batch_size;

  auto exporter_sdk = CreateSpanExporter(model->exporter);

  sdk = opentelemetry::sdk::trace::BatchSpanProcessorFactory::Create(std::move(exporter_sdk),
                                                                     options);
  return sdk;
}

std::unique_ptr<opentelemetry::sdk::trace::SpanProcessor> SdkBuilder::CreateSimpleSpanProcessor(
    const opentelemetry::sdk::configuration::SimpleSpanProcessorConfiguration *model) const
{
  std::unique_ptr<opentelemetry::sdk::trace::SpanProcessor> sdk;

  auto exporter_sdk = CreateSpanExporter(model->exporter);

  sdk = opentelemetry::sdk::trace::SimpleSpanProcessorFactory::Create(std::move(exporter_sdk));

  return sdk;
}

std::unique_ptr<opentelemetry::sdk::trace::SpanProcessor> SdkBuilder::CreateExtensionSpanProcessor(
    const opentelemetry::sdk::configuration::ExtensionSpanProcessorConfiguration *model) const
{
  std::unique_ptr<opentelemetry::sdk::trace::SpanProcessor> sdk;
  std::string name = model->name;

  const ExtensionSpanProcessorBuilder *builder = m_registry->GetExtensionSpanProcessorBuilder(name);

  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateExtensionSpanProcessor() using registered builder " << name);
    sdk = builder->Build(model);
  }
  else
  {
    std::string die("CreateExtensionSpanProcessor() no builder for ");
    die.append(name);
    OTEL_INTERNAL_LOG_ERROR(die);
    throw UnsupportedException(die);
  }
  return sdk;
}

std::unique_ptr<opentelemetry::sdk::trace::SpanProcessor> SdkBuilder::CreateSpanProcessor(
    const std::unique_ptr<opentelemetry::sdk::configuration::SpanProcessorConfiguration> &model)
    const
{
  std::unique_ptr<opentelemetry::sdk::trace::SpanProcessor> sdk;

  SpanProcessorBuilder builder(this);
  model->Accept(&builder);
  sdk = std::move(builder.processor);

  return sdk;
}

std::unique_ptr<opentelemetry::sdk::trace::TracerProvider> SdkBuilder::CreateTracerProvider(
    const std::unique_ptr<opentelemetry::sdk::configuration::TracerProviderConfiguration> &model,
    const opentelemetry::sdk::resource::Resource &resource) const
{
  std::unique_ptr<opentelemetry::sdk::trace::TracerProvider> sdk;

  // FIXME-CONFIG: https://github.com/open-telemetry/opentelemetry-configuration/issues/70
  OTEL_INTERNAL_LOG_ERROR("CreateTracerProvider: FIXME (IdGenerator)");

  std::unique_ptr<opentelemetry::sdk::trace::Sampler> sampler;

  if (model->sampler)
  {
    sampler = CreateSampler(model->sampler);
  }

  std::vector<std::unique_ptr<opentelemetry::sdk::trace::SpanProcessor>> sdk_processors;

  for (const auto &processor_model : model->processors)
  {
    sdk_processors.push_back(CreateSpanProcessor(processor_model));
  }

  // FIXME: use sampler, limits, id_generator, ...
  sdk =
      opentelemetry::sdk::trace::TracerProviderFactory::Create(std::move(sdk_processors), resource);

  return sdk;
}

std::unique_ptr<opentelemetry::context::propagation::TextMapPropagator>
SdkBuilder::CreateTextMapPropagator(const std::string &name) const
{
  std::unique_ptr<opentelemetry::context::propagation::TextMapPropagator> sdk;

  const TextMapPropagatorBuilder *builder = m_registry->GetTextMapPropagatorBuilder(name);

  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateTextMapPropagator() using registered builder " << name);
    sdk = builder->Build();
  }
  else
  {
    std::string die("CreateTextMapPropagator() no builder for ");
    die.append(name);
    OTEL_INTERNAL_LOG_ERROR(die);
    throw UnsupportedException(die);
  }
  return sdk;
}

std::unique_ptr<opentelemetry::context::propagation::TextMapPropagator>
SdkBuilder::CreatePropagator(
    const std::unique_ptr<opentelemetry::sdk::configuration::PropagatorConfiguration> &model) const
{
  std::vector<std::unique_ptr<opentelemetry::context::propagation::TextMapPropagator>> propagators;
  std::unique_ptr<opentelemetry::context::propagation::TextMapPropagator> propagator;

  for (const auto &name : model->composite)
  {
    propagator = CreateTextMapPropagator(name);
    propagators.push_back(std::move(propagator));
  }

  std::unique_ptr<opentelemetry::context::propagation::TextMapPropagator> sdk(
      new opentelemetry::context::propagation::CompositePropagator(std::move(propagators)));

  return sdk;
}

static opentelemetry::sdk::metrics::InstrumentType ConvertInstrumentType(
    enum opentelemetry::sdk::configuration::enum_instrument_type config)
{
  opentelemetry::sdk::metrics::InstrumentType sdk;

  switch (config)
  {
    case opentelemetry::sdk::configuration::counter:
      sdk = opentelemetry::sdk::metrics::InstrumentType::kCounter;
      break;
    case opentelemetry::sdk::configuration::histogram:
      sdk = opentelemetry::sdk::metrics::InstrumentType::kHistogram;
      break;
    case opentelemetry::sdk::configuration::observable_counter:
      sdk = opentelemetry::sdk::metrics::InstrumentType::kObservableCounter;
      break;
    case opentelemetry::sdk::configuration::observable_gauge:
      sdk = opentelemetry::sdk::metrics::InstrumentType::kObservableGauge;
      break;
    case opentelemetry::sdk::configuration::observable_up_down_counter:
      sdk = opentelemetry::sdk::metrics::InstrumentType::kObservableUpDownCounter;
      break;
    case opentelemetry::sdk::configuration::up_down_counter:
      sdk = opentelemetry::sdk::metrics::InstrumentType::kUpDownCounter;
      break;
  }

  return sdk;
}

std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter>
SdkBuilder::CreateOtlpPushMetricExporter(
    const opentelemetry::sdk::configuration::OtlpPushMetricExporterConfiguration *model) const
{
  std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter> sdk;
  const OtlpPushMetricExporterBuilder *builder;

  if (model->protocol.find("http/") == 0)
  {
    builder = m_registry->GetOtlpHttpPushMetricExporterBuilder();
    if (builder != nullptr)
    {
      OTEL_INTERNAL_LOG_DEBUG("CreateOtlpPushMetricExporter() using registered http builder");
      sdk = builder->Build(model);
      return sdk;
    }

    std::string die("No http builder for OtlpPushMetricExporter");
    OTEL_INTERNAL_LOG_ERROR(die);
    throw UnsupportedException(die);
  }

  if (model->protocol.find("grpc/") == 0)
  {
    builder = m_registry->GetOtlpGrpcPushMetricExporterBuilder();
    if (builder != nullptr)
    {
      OTEL_INTERNAL_LOG_DEBUG("CreateOtlpPushMetricExporter() using registered grpc builder");
      sdk = builder->Build(model);
      return sdk;
    }

    std::string die("No grpc builder for OtlpPushMetricExporter");
    OTEL_INTERNAL_LOG_ERROR(die);
    throw UnsupportedException(die);
  }

  std::string die("OtlpPushMetricExporter: illegal protocol: ");
  die.append(model->protocol);
  OTEL_INTERNAL_LOG_ERROR(die);
  throw UnsupportedException(die);

  return sdk;
}

std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter>
SdkBuilder::CreateConsolePushMetricExporter(
    const opentelemetry::sdk::configuration::ConsolePushMetricExporterConfiguration *model) const
{
  std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter> sdk;

  const ConsolePushMetricExporterBuilder *builder =
      m_registry->GetConsolePushMetricExporterBuilder();

  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateConsolePushMetricExporter() using registered builder");
    sdk = builder->Build(model);
  }
  else
  {
    std::string die("No builder for ConsolePushMetricExporter");
    OTEL_INTERNAL_LOG_ERROR(die);
    throw UnsupportedException(die);
  }

  return sdk;
}

std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter>
SdkBuilder::CreateExtensionPushMetricExporter(
    const opentelemetry::sdk::configuration::ExtensionPushMetricExporterConfiguration *model) const
{
  std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter> sdk;
  std::string name = model->name;

  const ExtensionPushMetricExporterBuilder *builder =
      m_registry->GetExtensionPushMetricExporterBuilder(name);

  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateExtensionPushMetricExporter() using registered builder" << name);
    sdk = builder->Build(model);
  }
  else
  {
    std::string die("No builder for ExtensionPushMetricExporter ");
    die.append(name);
    OTEL_INTERNAL_LOG_ERROR(die);
    throw UnsupportedException(die);
  }

  return sdk;
}

std::unique_ptr<opentelemetry::sdk::metrics::MetricReader>
SdkBuilder::CreatePrometheusPullMetricExporter(
    const opentelemetry::sdk::configuration::PrometheusPullMetricExporterConfiguration *model) const
{
  std::unique_ptr<opentelemetry::sdk::metrics::MetricReader> sdk;

  const PrometheusPullMetricExporterBuilder *builder =
      m_registry->GetPrometheusPullMetricExporterBuilder();

  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreatePrometheusPullMetricExporter() using registered builder");
    sdk = builder->Build(model);
  }
  else
  {
    std::string die("No builder for PrometheusMetricExporter");
    OTEL_INTERNAL_LOG_ERROR(die);
    throw UnsupportedException(die);
  }

  return sdk;
}

std::unique_ptr<opentelemetry::sdk::metrics::MetricReader>
SdkBuilder::CreateExtensionPullMetricExporter(
    const opentelemetry::sdk::configuration::ExtensionPullMetricExporterConfiguration *model) const
{
  std::unique_ptr<opentelemetry::sdk::metrics::MetricReader> sdk;
  std::string name = model->name;

  const ExtensionPullMetricExporterBuilder *builder =
      m_registry->GetExtensionPullMetricExporterBuilder(name);

  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateExtensionPullMetricExporter() using registered builder" << name);
    sdk = builder->Build(model);
  }
  else
  {
    std::string die("No builder for ExtensionPullMetricExporter ");
    die.append(name);
    OTEL_INTERNAL_LOG_ERROR(die);
    throw UnsupportedException(die);
  }

  return sdk;
}

std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter>
SdkBuilder::CreatePushMetricExporter(
    const std::unique_ptr<opentelemetry::sdk::configuration::PushMetricExporterConfiguration>
        &model) const
{
  std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter> sdk;

  OTEL_INTERNAL_LOG_ERROR("SdkBuilder::CreatePushMetricExporter: FIXME");

  PushMetricExporterBuilder builder(this);
  model->Accept(&builder);
  sdk = std::move(builder.exporter);

  return sdk;
}

std::unique_ptr<opentelemetry::sdk::metrics::MetricReader> SdkBuilder::CreatePullMetricExporter(
    const std::unique_ptr<opentelemetry::sdk::configuration::PullMetricExporterConfiguration>
        &model) const
{
  std::unique_ptr<opentelemetry::sdk::metrics::MetricReader> sdk;

  OTEL_INTERNAL_LOG_ERROR("SdkBuilder::CreatePullMetricExporter: FIXME");

  PullMetricExporterBuilder builder(this);
  model->Accept(&builder);
  sdk = std::move(builder.exporter);

  return sdk;
}

std::unique_ptr<opentelemetry::sdk::metrics::MetricReader> SdkBuilder::CreatePeriodicMetricReader(
    const opentelemetry::sdk::configuration::PeriodicMetricReaderConfiguration *model) const
{
  std::unique_ptr<opentelemetry::sdk::metrics::MetricReader> sdk;

  opentelemetry::sdk::metrics::PeriodicExportingMetricReaderOptions options;

  options.export_interval_millis = std::chrono::milliseconds(model->interval);
  options.export_timeout_millis  = std::chrono::milliseconds(model->timeout);

  auto exporter_sdk = CreatePushMetricExporter(model->exporter);

  sdk = opentelemetry::sdk::metrics::PeriodicExportingMetricReaderFactory::Create(
      std::move(exporter_sdk), options);

  return sdk;
}

std::unique_ptr<opentelemetry::sdk::metrics::MetricReader> SdkBuilder::CreatePullMetricReader(
    const opentelemetry::sdk::configuration::PullMetricReaderConfiguration *model) const
{
  std::unique_ptr<opentelemetry::sdk::metrics::MetricReader> sdk;

  OTEL_INTERNAL_LOG_ERROR("SdkBuilder::CreatePullMetricReader: FIXME");
  sdk = CreatePullMetricExporter(model->exporter);

  return sdk;
}

std::unique_ptr<opentelemetry::sdk::metrics::MetricReader> SdkBuilder::CreateMetricReader(
    const std::unique_ptr<opentelemetry::sdk::configuration::MetricReaderConfiguration> &model)
    const
{
  std::unique_ptr<opentelemetry::sdk::metrics::MetricReader> sdk;

  MetricReaderBuilder builder(this);
  model->Accept(&builder);
  sdk = std::move(builder.metric_reader);

  return sdk;
}

void SdkBuilder::AddView(
    opentelemetry::sdk::metrics::ViewRegistry *registry,
    const std::unique_ptr<opentelemetry::sdk::configuration::ViewConfiguration> &model) const
{
  auto *selector = model->selector.get();

  auto sdk_instrument_type = ConvertInstrumentType(selector->instrument_type);

  std::unique_ptr<opentelemetry::sdk::metrics::InstrumentSelector> sdk_instrument_selector(
      new opentelemetry::sdk::metrics::InstrumentSelector(
          sdk_instrument_type, selector->instrument_name, selector->unit));

  std::unique_ptr<opentelemetry::sdk::metrics::MeterSelector> sdk_meter_selector(
      new opentelemetry::sdk::metrics::MeterSelector(selector->meter_name, selector->meter_version,
                                                     selector->meter_schema_url));

  auto *stream = model->stream.get();

  std::string unit("FIXME");

  opentelemetry::sdk::metrics::AggregationType sdk_aggregation_type =
      opentelemetry::sdk::metrics::AggregationType::kDefault;  // FIXME

  std::shared_ptr<opentelemetry::sdk::metrics::AggregationConfig> sdk_aggregation_config =
      nullptr;  // FIXME

  std::unique_ptr<opentelemetry::sdk::metrics::AttributesProcessor>
      sdk_attribute_processor;  // FIXME

  std::unique_ptr<opentelemetry::sdk::metrics::View> sdk_view(new opentelemetry::sdk::metrics::View(
      stream->name, stream->description, unit, sdk_aggregation_type, sdk_aggregation_config,
      std::move(sdk_attribute_processor)));

  registry->AddView(std::move(sdk_instrument_selector), std::move(sdk_meter_selector),
                    std::move(sdk_view));

  OTEL_INTERNAL_LOG_ERROR("AddView() FIXME");
}

std::unique_ptr<opentelemetry::sdk::metrics::MeterProvider> SdkBuilder::CreateMeterProvider(
    const std::unique_ptr<opentelemetry::sdk::configuration::MeterProviderConfiguration> &model,
    const opentelemetry::sdk::resource::Resource &resource) const
{
  std::unique_ptr<opentelemetry::sdk::metrics::MeterProvider> sdk;

  auto view_registry = opentelemetry::sdk::metrics::ViewRegistryFactory::Create();

  for (const auto &view_configuration : model->views)
  {
    AddView(view_registry.get(), view_configuration);
  }

  auto meter_context =
      opentelemetry::sdk::metrics::MeterContextFactory::Create(std::move(view_registry), resource);

  OTEL_INTERNAL_LOG_ERROR("CreateMeterProvider() FIXME");

  for (const auto &reader_configuration : model->readers)
  {
    std::shared_ptr<opentelemetry::sdk::metrics::MetricReader> metric_reader;
    metric_reader = CreateMetricReader(reader_configuration);
    meter_context->AddMetricReader(metric_reader);
  }

  sdk = opentelemetry::sdk::metrics::MeterProviderFactory::Create(std::move(meter_context));

  return sdk;
}

std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter>
SdkBuilder::CreateOtlpLogRecordExporter(
    const opentelemetry::sdk::configuration::OtlpLogRecordExporterConfiguration *model) const
{
  std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> sdk;
  const OtlpLogRecordExporterBuilder *builder;

  if (model->protocol.find("http/") == 0)
  {
    builder = m_registry->GetOtlpHttpLogRecordBuilder();
    if (builder != nullptr)
    {
      OTEL_INTERNAL_LOG_DEBUG("CreateOtlpLogRecordExporter() using registered http builder");
      sdk = builder->Build(model);
      return sdk;
    }

    std::string die("No http builder for OtlpLogRecordExporter");
    OTEL_INTERNAL_LOG_ERROR(die);
    throw UnsupportedException(die);
  }

  if (model->protocol.find("grpc/") == 0)
  {
    builder = m_registry->GetOtlpGrpcLogRecordBuilder();
    if (builder != nullptr)
    {
      builder = m_registry->GetOtlpGrpcLogRecordBuilder();
      OTEL_INTERNAL_LOG_DEBUG("CreateOtlpLogRecordExporter() using registered grpc builder");
      sdk = builder->Build(model);
      return sdk;
    }

    std::string die("No grpc builder for OtlpLogRecordExporter");
    OTEL_INTERNAL_LOG_ERROR(die);
    throw UnsupportedException(die);
  }

  std::string die("OtlpLogRecordExporter: illegal protocol: ");
  die.append(model->protocol);
  OTEL_INTERNAL_LOG_ERROR(die);
  throw UnsupportedException(die);

  return sdk;
}

std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter>
SdkBuilder::CreateConsoleLogRecordExporter(
    const opentelemetry::sdk::configuration::ConsoleLogRecordExporterConfiguration *model) const
{
  std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> sdk;
  const ConsoleLogRecordExporterBuilder *builder = m_registry->GetConsoleLogRecordBuilder();

  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateConsoleLogRecordExporter() using registered builder");
    sdk = builder->Build(model);
  }
  else
  {
    std::string die("No builder for ConsoleLogRecordExporter");
    OTEL_INTERNAL_LOG_ERROR(die);
    throw UnsupportedException(die);
  }

  return sdk;
}

std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter>
SdkBuilder::CreateExtensionLogRecordExporter(
    const opentelemetry::sdk::configuration::ExtensionLogRecordExporterConfiguration *model) const
{
  std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> sdk;
  std::string name = model->name;

  const ExtensionLogRecordExporterBuilder *builder =
      m_registry->GetExtensionLogRecordExporterBuilder(name);

  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateExtensionLogRecordExporter() using registered builder " << name);
    sdk = builder->Build(model);
  }
  else
  {
    std::string die("CreateExtensionLogRecordExporter() no builder for ");
    die.append(name);
    OTEL_INTERNAL_LOG_ERROR(die);
    throw UnsupportedException(die);
  }
  return sdk;
}

std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> SdkBuilder::CreateLogRecordExporter(
    const std::unique_ptr<opentelemetry::sdk::configuration::LogRecordExporterConfiguration> &model)
    const
{
  std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> sdk;

  LogRecordExporterBuilder builder(this);
  model->Accept(&builder);
  sdk = std::move(builder.exporter);

  return sdk;
}

std::unique_ptr<opentelemetry::sdk::logs::LogRecordProcessor>
SdkBuilder::CreateBatchLogRecordProcessor(
    const opentelemetry::sdk::configuration::BatchLogRecordProcessorConfiguration *model) const
{
  std::unique_ptr<opentelemetry::sdk::logs::LogRecordProcessor> sdk;
  opentelemetry::sdk::logs::BatchLogRecordProcessorOptions options;

  options.schedule_delay_millis = std::chrono::milliseconds(model->schedule_delay);
  options.max_queue_size        = model->max_queue_size;
  options.max_export_batch_size = model->max_export_batch_size;

  auto exporter_sdk = CreateLogRecordExporter(model->exporter);

  sdk = opentelemetry::sdk::logs::BatchLogRecordProcessorFactory::Create(std::move(exporter_sdk),
                                                                         options);

  return sdk;
}

std::unique_ptr<opentelemetry::sdk::logs::LogRecordProcessor>
SdkBuilder::CreateSimpleLogRecordProcessor(
    const opentelemetry::sdk::configuration::SimpleLogRecordProcessorConfiguration *model) const
{
  std::unique_ptr<opentelemetry::sdk::logs::LogRecordProcessor> sdk;

  auto exporter_sdk = CreateLogRecordExporter(model->exporter);

  sdk = opentelemetry::sdk::logs::SimpleLogRecordProcessorFactory::Create(std::move(exporter_sdk));

  return sdk;
}

std::unique_ptr<opentelemetry::sdk::logs::LogRecordProcessor>
SdkBuilder::CreateExtensionLogRecordProcessor(
    const opentelemetry::sdk::configuration::ExtensionLogRecordProcessorConfiguration *model) const
{
  std::unique_ptr<opentelemetry::sdk::logs::LogRecordProcessor> sdk;
  std::string name = model->name;

  const ExtensionLogRecordProcessorBuilder *builder =
      m_registry->GetExtensionLogRecordProcessorBuilder(name);

  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateExtensionLogRecordProcessor() using registered builder "
                            << name);
    sdk = builder->Build(model);
  }
  else
  {
    std::string die("CreateExtensionLogRecordProcessor() no builder for ");
    die.append(name);
    OTEL_INTERNAL_LOG_ERROR(die);
    throw UnsupportedException(die);
  }
  return sdk;
}

std::unique_ptr<opentelemetry::sdk::logs::LogRecordProcessor> SdkBuilder::CreateLogRecordProcessor(
    const std::unique_ptr<opentelemetry::sdk::configuration::LogRecordProcessorConfiguration>
        &model) const
{
  std::unique_ptr<opentelemetry::sdk::logs::LogRecordProcessor> sdk;

  LogRecordProcessorBuilder builder(this);
  model->Accept(&builder);
  sdk = std::move(builder.processor);

  return sdk;
}

std::unique_ptr<opentelemetry::sdk::logs::LoggerProvider> SdkBuilder::CreateLoggerProvider(
    const std::unique_ptr<opentelemetry::sdk::configuration::LoggerProviderConfiguration> &model,
    const opentelemetry::sdk::resource::Resource &resource) const
{
  std::unique_ptr<opentelemetry::sdk::logs::LoggerProvider> sdk;

  std::vector<std::unique_ptr<opentelemetry::sdk::logs::LogRecordProcessor>> sdk_processors;

  for (const auto &processor_model : model->processors)
  {
    sdk_processors.push_back(CreateLogRecordProcessor(processor_model));
  }

  // FIXME: use limits
  sdk =
      opentelemetry::sdk::logs::LoggerProviderFactory::Create(std::move(sdk_processors), resource);

  return sdk;
}

void SdkBuilder::SetResource(
    opentelemetry::sdk::resource::Resource &resource,
    const std::unique_ptr<opentelemetry::sdk::configuration::ResourceConfiguration> &opt_model)
    const
{
  if (opt_model)
  {
    opentelemetry::sdk::resource::ResourceAttributes sdk_attributes;

    if (opt_model->attributes)
    {
      for (const auto &kv : opt_model->attributes->kv_map)
      {
        sdk_attributes.SetAttribute(kv.first, kv.second);
      }
    }

    if (opt_model->attributes_list.size() != 0)
    {
      OTEL_INTERNAL_LOG_ERROR("SdkBuilder::SetResource: FIXME attributes_list");
    }

    if (opt_model->detectors != nullptr)
    {
      OTEL_INTERNAL_LOG_ERROR("SdkBuilder::SetResource: FIXME detectors");
    }

    auto sdk_resource =
        opentelemetry::sdk::resource::Resource::Create(sdk_attributes, opt_model->schema_url);
    resource = resource.Merge(sdk_resource);
  }
  else
  {
    resource = opentelemetry::sdk::resource::Resource::GetDefault();
  }
}

std::unique_ptr<ConfiguredSdk> SdkBuilder::CreateConfiguredSdk(
    const std::unique_ptr<opentelemetry::sdk::configuration::Configuration> &model) const
{
  std::unique_ptr<ConfiguredSdk> sdk(new ConfiguredSdk);

  if (!model->disabled)
  {
    SetResource(sdk->m_resource, model->resource);

    if (model->attribute_limits)
    {
      OTEL_INTERNAL_LOG_ERROR("SdkBuilder::CreateConfiguredSdk: FIXME limits");
      // Ignore limits
      // throw UnsupportedException("attribute_limits not available in opentelemetry-cpp");
    }

    if (model->tracer_provider)
    {
      sdk->m_tracer_provider = CreateTracerProvider(model->tracer_provider, sdk->m_resource);
    }

    if (model->propagator)
    {
      sdk->m_propagator = CreatePropagator(model->propagator);
    }

    if (model->meter_provider)
    {
      sdk->m_meter_provider = CreateMeterProvider(model->meter_provider, sdk->m_resource);
    }

    if (model->logger_provider)
    {
      sdk->m_logger_provider = CreateLoggerProvider(model->logger_provider, sdk->m_resource);
    }
  }

  return sdk;
}

}  // namespace init
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
