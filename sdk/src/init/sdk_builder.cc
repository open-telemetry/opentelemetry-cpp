// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/common/global_log_handler.h"

#include "opentelemetry/baggage/propagation/baggage_propagator.h"
#include "opentelemetry/context/propagation/composite_propagator.h"
#include "opentelemetry/sdk/configuration/always_off_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/always_on_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/batch_span_processor_configuration.h"
#include "opentelemetry/sdk/configuration/composite_propagator_configuration.h"
#include "opentelemetry/sdk/configuration/configuration.h"
#include "opentelemetry/sdk/configuration/configuration_factory.h"
#include "opentelemetry/sdk/configuration/console_span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/document.h"
#include "opentelemetry/sdk/configuration/document_node.h"
#include "opentelemetry/sdk/configuration/extension_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/jaeger_remote_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/log_record_exporter_configuration_visitor.h"
#include "opentelemetry/sdk/configuration/log_record_processor_configuration_visitor.h"
#include "opentelemetry/sdk/configuration/otlp_span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/parent_based_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/propagator_configuration_visitor.h"
#include "opentelemetry/sdk/configuration/sampler_configuration_visitor.h"
#include "opentelemetry/sdk/configuration/selector_configuration.h"
#include "opentelemetry/sdk/configuration/simple_propagator_configuration.h"
#include "opentelemetry/sdk/configuration/simple_span_processor_configuration.h"
#include "opentelemetry/sdk/configuration/span_exporter_configuration_visitor.h"
#include "opentelemetry/sdk/configuration/span_processor_configuration_visitor.h"
#include "opentelemetry/sdk/configuration/trace_id_ratio_based_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/zipkin_span_exporter_configuration.h"
#include "opentelemetry/sdk/init/configured_sdk.h"
#include "opentelemetry/sdk/init/console_span_exporter_builder.h"
#include "opentelemetry/sdk/init/extension_sampler_builder.h"
#include "opentelemetry/sdk/init/extension_span_exporter_builder.h"
#include "opentelemetry/sdk/init/extension_span_processor_builder.h"
#include "opentelemetry/sdk/init/otlp_span_exporter_builder.h"
#include "opentelemetry/sdk/init/registry.h"
#include "opentelemetry/sdk/init/sdk_builder.h"
#include "opentelemetry/sdk/init/text_map_propagator_builder.h"
#include "opentelemetry/sdk/init/unsupported_exception.h"
#include "opentelemetry/sdk/init/zipkin_span_exporter_builder.h"
#include "opentelemetry/sdk/logs/logger_provider_factory.h"
#include "opentelemetry/sdk/logs/processor.h"
#include "opentelemetry/sdk/metrics/aggregation/aggregation.h"
#include "opentelemetry/sdk/metrics/aggregation/aggregation_config.h"
#include "opentelemetry/sdk/metrics/view/attributes_processor.h"
#include "opentelemetry/sdk/metrics/view/view_registry_factory.h"
#include "opentelemetry/sdk/trace/batch_span_processor_factory.h"
#include "opentelemetry/sdk/trace/batch_span_processor_options.h"
#include "opentelemetry/sdk/trace/processor.h"
#include "opentelemetry/sdk/trace/sampler.h"
#include "opentelemetry/sdk/trace/samplers/always_off_factory.h"
#include "opentelemetry/sdk/trace/samplers/always_on_factory.h"
#include "opentelemetry/sdk/trace/samplers/parent_factory.h"
#include "opentelemetry/sdk/trace/samplers/trace_id_ratio_factory.h"
#include "opentelemetry/sdk/trace/simple_processor_factory.h"
#include "opentelemetry/sdk/trace/tracer_provider_factory.h"
#include "opentelemetry/trace/propagation/b3_propagator.h"
#include "opentelemetry/trace/propagation/http_trace_context.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace init
{

class PropagatorBuilder : public opentelemetry::sdk::configuration::PropagatorConfigurationVisitor
{
public:
  PropagatorBuilder(const SdkBuilder *b) : m_sdk_builder(b) {}
  ~PropagatorBuilder() override = default;

  void VisitSimple(
      const opentelemetry::sdk::configuration::SimplePropagatorConfiguration *model) override
  {
    propagator = m_sdk_builder->CreateSimplePropagator(model);
  }

  void VisitComposite(
      const opentelemetry::sdk::configuration::CompositePropagatorConfiguration *model) override
  {
    propagator = m_sdk_builder->CreateCompositePropagator(model);
  }

  std::unique_ptr<opentelemetry::context::propagation::TextMapPropagator> propagator;

private:
  const SdkBuilder *m_sdk_builder;
};

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

  OTEL_INTERNAL_LOG_ERROR("CreateJaegerRemoteSampler: FIXME");
  throw UnsupportedException("JeagerRemoteSampler not available in opentelemetry-cpp");

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
    OTEL_INTERNAL_LOG_ERROR("CreateExtensionSampler() no builder for " << name);
    throw UnsupportedException("CreateExtensionSampler() no builder for " + name);
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
  const OtlpSpanExporterBuilder *builder = m_registry->GetOtlpBuilder();

  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateOtlpSpanExporter() using registered builder");
    sdk = builder->Build(model);
  }
  else
  {
    OTEL_INTERNAL_LOG_ERROR("No builder for OtlpSpanExporter");
    throw UnsupportedException("No builder for OtlpSpanExporter");
  }

  return sdk;
}

std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> SdkBuilder::CreateConsoleSpanExporter(
    const opentelemetry::sdk::configuration::ConsoleSpanExporterConfiguration *model) const
{
  std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> sdk;
  const ConsoleSpanExporterBuilder *builder = m_registry->GetConsoleBuilder();

  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateConsoleSpanExporter() using registered builder");
    sdk = builder->Build(model);
  }
  else
  {
    OTEL_INTERNAL_LOG_ERROR("No builder for ConsoleSpanExporter");
    throw UnsupportedException("No builder for ConsoleSpanExporter");
  }

  return sdk;
}

std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> SdkBuilder::CreateZipkinSpanExporter(
    const opentelemetry::sdk::configuration::ZipkinSpanExporterConfiguration *model) const
{
  std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> sdk;
  const ZipkinSpanExporterBuilder *builder = m_registry->GetZipkinBuilder();

  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateZipkinSpanExporter() using registered builder");
    sdk = builder->Build(model);
  }
  else
  {
    OTEL_INTERNAL_LOG_ERROR("No builder for ZipkinSpanExporter");
    throw UnsupportedException("No builder for ZipkinSpanExporter");
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
    OTEL_INTERNAL_LOG_ERROR("CreateExtensionSpanExporter() no builder for " << name);
    throw UnsupportedException("CreateExtensionSpanExporter() no builder for " + name);
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
    OTEL_INTERNAL_LOG_ERROR("CreateExtensionSpanProcessor() no builder for " << name);
    throw UnsupportedException("CreateExtensionSpanProcessor() no builder for " + name);
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
    const std::unique_ptr<opentelemetry::sdk::configuration::TracerProviderConfiguration> &model)
    const
{
  std::unique_ptr<opentelemetry::sdk::trace::TracerProvider> sdk;

  // https://github.com/open-telemetry/opentelemetry-configuration/issues/70
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
  sdk = opentelemetry::sdk::trace::TracerProviderFactory::Create(std::move(sdk_processors));

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
    OTEL_INTERNAL_LOG_ERROR("CreateTextMapPropagator() no builder for " << name);
    throw UnsupportedException("CreateTextMapPropagator() no builder for " + name);
  }
  return sdk;
}

std::unique_ptr<opentelemetry::context::propagation::TextMapPropagator>
SdkBuilder::CreateSimplePropagator(
    const opentelemetry::sdk::configuration::SimplePropagatorConfiguration *model) const
{
  std::unique_ptr<opentelemetry::context::propagation::TextMapPropagator> sdk;

  sdk = CreateTextMapPropagator(model->name);

  return sdk;
}

std::unique_ptr<opentelemetry::context::propagation::TextMapPropagator>
SdkBuilder::CreateCompositePropagator(
    const opentelemetry::sdk::configuration::CompositePropagatorConfiguration *model) const
{
  std::vector<std::unique_ptr<opentelemetry::context::propagation::TextMapPropagator>> propagators;
  std::unique_ptr<opentelemetry::context::propagation::TextMapPropagator> propagator;

  for (const auto &name : model->names)
  {
    propagator = CreateTextMapPropagator(name);
    propagators.push_back(std::move(propagator));
  }

  std::unique_ptr<opentelemetry::context::propagation::TextMapPropagator> sdk(
      new opentelemetry::context::propagation::CompositePropagator(std::move(propagators)));

  return sdk;
}

std::unique_ptr<opentelemetry::context::propagation::TextMapPropagator>
SdkBuilder::CreatePropagator(
    const std::unique_ptr<opentelemetry::sdk::configuration::PropagatorConfiguration> &model) const
{
  std::unique_ptr<opentelemetry::context::propagation::TextMapPropagator> sdk;

  PropagatorBuilder builder(this);
  model->Accept(&builder);
  sdk = std::move(builder.propagator);

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
    const std::unique_ptr<opentelemetry::sdk::configuration::MeterProviderConfiguration> &model)
    const
{
  std::unique_ptr<opentelemetry::sdk::metrics::MeterProvider> sdk;

  auto view_registry = opentelemetry::sdk::metrics::ViewRegistryFactory::Create();

  for (const auto &view_configuration : model->views)
  {
    AddView(view_registry.get(), view_configuration);
  }

  OTEL_INTERNAL_LOG_ERROR("CreateMeterProvider() FIXME");

  return sdk;
}

std::unique_ptr<opentelemetry::sdk::logs::LogRecordProcessor>
SdkBuilder::CreateBatchLogRecordProcessor(
    const opentelemetry::sdk::configuration::BatchLogRecordProcessorConfiguration * /* model */)
    const
{
  std::unique_ptr<opentelemetry::sdk::logs::LogRecordProcessor> sdk;

  OTEL_INTERNAL_LOG_ERROR("CreateBatchLogRecordProcessor() FIXME");

  return sdk;
}

std::unique_ptr<opentelemetry::sdk::logs::LogRecordProcessor>
SdkBuilder::CreateSimpleLogRecordProcessor(
    const opentelemetry::sdk::configuration::SimpleLogRecordProcessorConfiguration * /* model */)
    const
{
  std::unique_ptr<opentelemetry::sdk::logs::LogRecordProcessor> sdk;

  OTEL_INTERNAL_LOG_ERROR("CreateSimpleLogRecordProcessor() FIXME");

  return sdk;
}

std::unique_ptr<opentelemetry::sdk::logs::LogRecordProcessor>
SdkBuilder::CreateExtensionLogRecordProcessor(
    const opentelemetry::sdk::configuration::ExtensionLogRecordProcessorConfiguration * /* model */)
    const
{
  std::unique_ptr<opentelemetry::sdk::logs::LogRecordProcessor> sdk;

  OTEL_INTERNAL_LOG_ERROR("CreateExtensionLogRecordProcessor() FIXME");

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
    const std::unique_ptr<opentelemetry::sdk::configuration::LoggerProviderConfiguration> &model)
    const
{
  std::unique_ptr<opentelemetry::sdk::logs::LoggerProvider> sdk;

  std::vector<std::unique_ptr<opentelemetry::sdk::logs::LogRecordProcessor>> sdk_processors;

  for (const auto &processor_model : model->processors)
  {
    sdk_processors.push_back(CreateLogRecordProcessor(processor_model));
  }

  // FIXME: use limits
  sdk = opentelemetry::sdk::logs::LoggerProviderFactory::Create(std::move(sdk_processors));

  return sdk;
}

std::unique_ptr<ConfiguredSdk> SdkBuilder::CreateConfiguredSdk(
    const std::unique_ptr<opentelemetry::sdk::configuration::Configuration> &model) const
{
  std::unique_ptr<ConfiguredSdk> sdk(new ConfiguredSdk);

  if (!model->disabled)
  {
    if (model->tracer_provider)
    {
      sdk->m_tracer_provider = CreateTracerProvider(model->tracer_provider);
    }

    if (model->propagator)
    {
      sdk->m_propagator = CreatePropagator(model->propagator);
    }

    if (model->meter_provider)
    {
      sdk->m_meter_provider = CreateMeterProvider(model->meter_provider);
    }

    if (model->logger_provider)
    {
      sdk->m_logger_provider = CreateLoggerProvider(model->logger_provider);
    }
  }

  return sdk;
}

}  // namespace init
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
