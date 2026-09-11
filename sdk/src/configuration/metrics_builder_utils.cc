// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/configuration/metrics_builder_utils.h"

#include <cstddef>
#include <memory>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/configuration/aggregation_configuration.h"
#include "opentelemetry/sdk/configuration/aggregation_configuration_visitor.h"
#include "opentelemetry/sdk/configuration/base2_exponential_bucket_histogram_aggregation_configuration.h"
#include "opentelemetry/sdk/configuration/cardinality_limits_configuration.h"
#include "opentelemetry/sdk/configuration/console_push_metric_exporter_builder.h"
#include "opentelemetry/sdk/configuration/console_push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/explicit_bucket_histogram_aggregation_configuration.h"
#include "opentelemetry/sdk/configuration/extension_pull_metric_exporter_builder.h"
#include "opentelemetry/sdk/configuration/extension_pull_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/extension_push_metric_exporter_builder.h"
#include "opentelemetry/sdk/configuration/extension_push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/include_exclude_configuration.h"
#include "opentelemetry/sdk/configuration/instrument_type.h"
#include "opentelemetry/sdk/configuration/meter_configurator_builder.h"
#include "opentelemetry/sdk/configuration/meter_configurator_configuration.h"
#include "opentelemetry/sdk/configuration/metric_reader_configuration.h"
#include "opentelemetry/sdk/configuration/metric_reader_configuration_visitor.h"
#include "opentelemetry/sdk/configuration/otlp_file_push_metric_exporter_builder.h"
#include "opentelemetry/sdk/configuration/otlp_file_push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/otlp_grpc_push_metric_exporter_builder.h"
#include "opentelemetry/sdk/configuration/otlp_grpc_push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/otlp_http_push_metric_exporter_builder.h"
#include "opentelemetry/sdk/configuration/otlp_http_push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/periodic_metric_reader_builder.h"
#include "opentelemetry/sdk/configuration/periodic_metric_reader_configuration.h"
#include "opentelemetry/sdk/configuration/prometheus_pull_metric_exporter_builder.h"
#include "opentelemetry/sdk/configuration/prometheus_pull_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/pull_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/pull_metric_exporter_configuration_visitor.h"
#include "opentelemetry/sdk/configuration/pull_metric_reader_configuration.h"
#include "opentelemetry/sdk/configuration/push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/push_metric_exporter_configuration_visitor.h"
#include "opentelemetry/sdk/configuration/registry.h"
#include "opentelemetry/sdk/configuration/string_array_configuration.h"
#include "opentelemetry/sdk/configuration/unsupported_exception.h"
#include "opentelemetry/sdk/configuration/view_configuration.h"
#include "opentelemetry/sdk/configuration/view_selector_configuration.h"
#include "opentelemetry/sdk/configuration/view_stream_configuration.h"
#include "opentelemetry/sdk/instrumentationscope/scope_configurator.h"
#include "opentelemetry/sdk/metrics/aggregation/aggregation_config.h"
#include "opentelemetry/sdk/metrics/aggregation/default_aggregation.h"
#include "opentelemetry/sdk/metrics/cardinality_limits.h"
#include "opentelemetry/sdk/metrics/instruments.h"
#include "opentelemetry/sdk/metrics/meter_config.h"
#include "opentelemetry/sdk/metrics/metric_reader.h"
#include "opentelemetry/sdk/metrics/push_metric_exporter.h"
#include "opentelemetry/sdk/metrics/view/attributes_processor.h"
#include "opentelemetry/sdk/metrics/view/instrument_selector.h"
#include "opentelemetry/sdk/metrics/view/meter_selector.h"
#include "opentelemetry/sdk/metrics/view/predicate_factory.h"
#include "opentelemetry/sdk/metrics/view/view.h"
#include "opentelemetry/sdk/metrics/view/view_registry.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

namespace
{

static opentelemetry::sdk::metrics::CardinalityLimits ToCardinalityLimits(
    const opentelemetry::sdk::configuration::CardinalityLimitsConfiguration &cfg)
{
  opentelemetry::sdk::metrics::CardinalityLimits limits;
  const std::size_t d  = cfg.default_limit != CardinalityLimitsConfiguration::kInheritDefault
                             ? cfg.default_limit
                             : limits.default_limit;
  limits.default_limit = d;
  limits.counter = cfg.counter != CardinalityLimitsConfiguration::kInheritDefault ? cfg.counter : d;
  limits.gauge   = cfg.gauge != CardinalityLimitsConfiguration::kInheritDefault ? cfg.gauge : d;
  limits.histogram =
      cfg.histogram != CardinalityLimitsConfiguration::kInheritDefault ? cfg.histogram : d;
  limits.observable_counter =
      cfg.observable_counter != CardinalityLimitsConfiguration::kInheritDefault
          ? cfg.observable_counter
          : d;
  limits.observable_gauge = cfg.observable_gauge != CardinalityLimitsConfiguration::kInheritDefault
                                ? cfg.observable_gauge
                                : d;
  limits.observable_up_down_counter =
      cfg.observable_up_down_counter != CardinalityLimitsConfiguration::kInheritDefault
          ? cfg.observable_up_down_counter
          : d;
  limits.up_down_counter = cfg.up_down_counter != CardinalityLimitsConfiguration::kInheritDefault
                               ? cfg.up_down_counter
                               : d;
  return limits;
}

static opentelemetry::sdk::metrics::InstrumentType ConvertInstrumentType(
    enum opentelemetry::sdk::configuration::InstrumentType config)
{
  opentelemetry::sdk::metrics::InstrumentType sdk{
      opentelemetry::sdk::metrics::InstrumentType::kCounter};

  switch (config)
  {
    case opentelemetry::sdk::configuration::InstrumentType::counter:
      sdk = opentelemetry::sdk::metrics::InstrumentType::kCounter;
      break;
    case opentelemetry::sdk::configuration::InstrumentType::gauge:
      sdk = opentelemetry::sdk::metrics::InstrumentType::kGauge;
      break;
    case opentelemetry::sdk::configuration::InstrumentType::histogram:
      sdk = opentelemetry::sdk::metrics::InstrumentType::kHistogram;
      break;
    case opentelemetry::sdk::configuration::InstrumentType::observable_counter:
      sdk = opentelemetry::sdk::metrics::InstrumentType::kObservableCounter;
      break;
    case opentelemetry::sdk::configuration::InstrumentType::observable_gauge:
      sdk = opentelemetry::sdk::metrics::InstrumentType::kObservableGauge;
      break;
    case opentelemetry::sdk::configuration::InstrumentType::observable_up_down_counter:
      sdk = opentelemetry::sdk::metrics::InstrumentType::kObservableUpDownCounter;
      break;
    case opentelemetry::sdk::configuration::InstrumentType::up_down_counter:
      sdk = opentelemetry::sdk::metrics::InstrumentType::kUpDownCounter;
      break;
    case opentelemetry::sdk::configuration::InstrumentType::none:
    default:
      sdk = opentelemetry::sdk::metrics::InstrumentType::kCounter;
      break;
  }

  return sdk;
}

static std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter>
CreateOtlpHttpPushMetricExporter(
    const Registry *registry,
    const opentelemetry::sdk::configuration::OtlpHttpPushMetricExporterConfiguration *model)
{
  const OtlpHttpPushMetricExporterBuilder *builder =
      registry->GetOtlpHttpPushMetricExporterBuilder();
  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateOtlpHttpPushMetricExporter() using registered http builder");
    return builder->Build(model);
  }
  static const std::string message("No http builder for OtlpPushMetricExporter");
  throw UnsupportedException(message);
}

static std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter>
CreateOtlpGrpcPushMetricExporter(
    const Registry *registry,
    const opentelemetry::sdk::configuration::OtlpGrpcPushMetricExporterConfiguration *model)
{
  const OtlpGrpcPushMetricExporterBuilder *builder =
      registry->GetOtlpGrpcPushMetricExporterBuilder();
  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateOtlpGrpcPushMetricExporter() using registered grpc builder");
    return builder->Build(model);
  }
  static const std::string message("No grpc builder for OtlpPushMetricExporter");
  throw UnsupportedException(message);
}

static std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter>
CreateOtlpFilePushMetricExporter(
    const Registry *registry,
    const opentelemetry::sdk::configuration::OtlpFilePushMetricExporterConfiguration *model)
{
  const OtlpFilePushMetricExporterBuilder *builder =
      registry->GetOtlpFilePushMetricExporterBuilder();
  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateOtlpFilePushMetricExporter() using registered file builder");
    return builder->Build(model);
  }
  static const std::string message("No file builder for OtlpPushMetricExporter");
  throw UnsupportedException(message);
}

static std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter>
CreateConsolePushMetricExporter(
    const Registry *registry,
    const opentelemetry::sdk::configuration::ConsolePushMetricExporterConfiguration *model)
{
  const ConsolePushMetricExporterBuilder *builder = registry->GetConsolePushMetricExporterBuilder();
  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateConsolePushMetricExporter() using registered builder");
    return builder->Build(model);
  }
  static const std::string message("No builder for ConsolePushMetricExporter");
  throw UnsupportedException(message);
}

static std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter>
CreateExtensionPushMetricExporter(
    const Registry *registry,
    const opentelemetry::sdk::configuration::ExtensionPushMetricExporterConfiguration *model)
{
  std::string name = model->name;
  const ExtensionPushMetricExporterBuilder *builder =
      registry->GetExtensionPushMetricExporterBuilder(name);
  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateExtensionPushMetricExporter() using registered builder "
                            << name);
    return builder->Build(model);
  }
  std::string message("No builder for ExtensionPushMetricExporter ");
  message.append(name);
  throw UnsupportedException(message);
}

class PushMetricExporterDispatcher
    : public opentelemetry::sdk::configuration::PushMetricExporterConfigurationVisitor
{
public:
  explicit PushMetricExporterDispatcher(const Registry *registry) : registry_(registry) {}
  PushMetricExporterDispatcher(PushMetricExporterDispatcher &&)                      = delete;
  PushMetricExporterDispatcher(const PushMetricExporterDispatcher &)                 = delete;
  PushMetricExporterDispatcher &operator=(PushMetricExporterDispatcher &&)           = delete;
  PushMetricExporterDispatcher &operator=(const PushMetricExporterDispatcher &other) = delete;
  ~PushMetricExporterDispatcher() override                                           = default;

  void VisitOtlpHttp(
      const opentelemetry::sdk::configuration::OtlpHttpPushMetricExporterConfiguration *model)
      override
  {
    exporter = CreateOtlpHttpPushMetricExporter(registry_, model);
  }

  void VisitOtlpGrpc(
      const opentelemetry::sdk::configuration::OtlpGrpcPushMetricExporterConfiguration *model)
      override
  {
    exporter = CreateOtlpGrpcPushMetricExporter(registry_, model);
  }

  void VisitOtlpFile(
      const opentelemetry::sdk::configuration::OtlpFilePushMetricExporterConfiguration *model)
      override
  {
    exporter = CreateOtlpFilePushMetricExporter(registry_, model);
  }

  void VisitConsole(const opentelemetry::sdk::configuration::ConsolePushMetricExporterConfiguration
                        *model) override
  {
    exporter = CreateConsolePushMetricExporter(registry_, model);
  }

  void VisitExtension(
      const opentelemetry::sdk::configuration::ExtensionPushMetricExporterConfiguration *model)
      override
  {
    exporter = CreateExtensionPushMetricExporter(registry_, model);
  }

  std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter> exporter;

private:
  const Registry *registry_;
};

static std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter> CreatePushMetricExporter(
    const Registry *registry,
    const std::unique_ptr<opentelemetry::sdk::configuration::PushMetricExporterConfiguration>
        &model)
{
  PushMetricExporterDispatcher dispatcher(registry);
  model->Accept(&dispatcher);
  return std::move(dispatcher.exporter);
}

static std::unique_ptr<opentelemetry::sdk::metrics::MetricReader>
CreatePrometheusPullMetricExporter(
    const Registry *registry,
    const opentelemetry::sdk::configuration::PrometheusPullMetricExporterConfiguration *model)
{
  const PrometheusPullMetricExporterBuilder *builder =
      registry->GetPrometheusPullMetricExporterBuilder();
  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreatePrometheusPullMetricExporter() using registered builder");
    return builder->Build(model);
  }
  static const std::string message("No builder for PrometheusMetricExporter");
  throw UnsupportedException(message);
}

static std::unique_ptr<opentelemetry::sdk::metrics::MetricReader> CreateExtensionPullMetricExporter(
    const Registry *registry,
    const opentelemetry::sdk::configuration::ExtensionPullMetricExporterConfiguration *model)
{
  std::string name = model->name;
  const ExtensionPullMetricExporterBuilder *builder =
      registry->GetExtensionPullMetricExporterBuilder(name);
  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateExtensionPullMetricExporter() using registered builder "
                            << name);
    return builder->Build(model);
  }
  std::string message("No builder for ExtensionPullMetricExporter ");
  message.append(name);
  throw UnsupportedException(message);
}

class PullMetricExporterDispatcher
    : public opentelemetry::sdk::configuration::PullMetricExporterConfigurationVisitor
{
public:
  explicit PullMetricExporterDispatcher(const Registry *registry) : registry_(registry) {}
  PullMetricExporterDispatcher(PullMetricExporterDispatcher &&)                      = delete;
  PullMetricExporterDispatcher(const PullMetricExporterDispatcher &)                 = delete;
  PullMetricExporterDispatcher &operator=(PullMetricExporterDispatcher &&)           = delete;
  PullMetricExporterDispatcher &operator=(const PullMetricExporterDispatcher &other) = delete;
  ~PullMetricExporterDispatcher() override                                           = default;

  void VisitPrometheus(
      const opentelemetry::sdk::configuration::PrometheusPullMetricExporterConfiguration *model)
      override
  {
    exporter = CreatePrometheusPullMetricExporter(registry_, model);
  }

  void VisitExtension(
      const opentelemetry::sdk::configuration::ExtensionPullMetricExporterConfiguration *model)
      override
  {
    exporter = CreateExtensionPullMetricExporter(registry_, model);
  }

  std::unique_ptr<opentelemetry::sdk::metrics::MetricReader> exporter;

private:
  const Registry *registry_;
};

static std::unique_ptr<opentelemetry::sdk::metrics::MetricReader> CreatePullMetricExporter(
    const Registry *registry,
    const std::unique_ptr<opentelemetry::sdk::configuration::PullMetricExporterConfiguration>
        &model)
{
  PullMetricExporterDispatcher dispatcher(registry);
  model->Accept(&dispatcher);
  return std::move(dispatcher.exporter);
}

static std::unique_ptr<opentelemetry::sdk::metrics::MetricReader> CreatePeriodicMetricReader(
    const Registry *registry,
    const opentelemetry::sdk::configuration::PeriodicMetricReaderConfiguration *model)
{
  auto exporter_sdk = CreatePushMetricExporter(registry, model->exporter);

  if (!model->producers.empty())
  {
    OTEL_INTERNAL_LOG_WARN("metric producer not supported, ignoring");
  }

  const PeriodicMetricReaderBuilder *builder = registry->GetPeriodicMetricReaderBuilder();
  if (builder == nullptr)
  {
    static const std::string message("No builder for PeriodicMetricReader");
    throw UnsupportedException(message);
  }

  OTEL_INTERNAL_LOG_DEBUG("CreatePeriodicMetricReader() using registered builder");
  auto sdk = builder->Build(model, std::move(exporter_sdk));

  if (model->cardinality_limits != nullptr)
  {
    sdk->SetCardinalityLimits(ToCardinalityLimits(*model->cardinality_limits));
  }

  return sdk;
}

static std::unique_ptr<opentelemetry::sdk::metrics::MetricReader> CreatePullMetricReader(
    const Registry *registry,
    const opentelemetry::sdk::configuration::PullMetricReaderConfiguration *model)
{
  auto sdk = CreatePullMetricExporter(registry, model->exporter);

  if (!model->producers.empty())
  {
    OTEL_INTERNAL_LOG_WARN("metric producer not supported, ignoring");
  }

  if (model->cardinality_limits != nullptr)
  {
    sdk->SetCardinalityLimits(ToCardinalityLimits(*model->cardinality_limits));
  }

  return sdk;
}

class MetricReaderDispatcher
    : public opentelemetry::sdk::configuration::MetricReaderConfigurationVisitor
{
public:
  explicit MetricReaderDispatcher(const Registry *registry) : registry_(registry) {}
  MetricReaderDispatcher(MetricReaderDispatcher &&)                      = delete;
  MetricReaderDispatcher(const MetricReaderDispatcher &)                 = delete;
  MetricReaderDispatcher &operator=(MetricReaderDispatcher &&)           = delete;
  MetricReaderDispatcher &operator=(const MetricReaderDispatcher &other) = delete;
  ~MetricReaderDispatcher() override                                     = default;

  void VisitPeriodic(
      const opentelemetry::sdk::configuration::PeriodicMetricReaderConfiguration *model) override
  {
    metric_reader = CreatePeriodicMetricReader(registry_, model);
  }

  void VisitPull(
      const opentelemetry::sdk::configuration::PullMetricReaderConfiguration *model) override
  {
    metric_reader = CreatePullMetricReader(registry_, model);
  }

  std::unique_ptr<opentelemetry::sdk::metrics::MetricReader> metric_reader;

private:
  const Registry *registry_;
};

static std::unique_ptr<opentelemetry::sdk::metrics::Base2ExponentialHistogramAggregationConfig>
CreateBase2ExponentialBucketHistogramAggregation(
    const opentelemetry::sdk::configuration::Base2ExponentialBucketHistogramAggregationConfiguration
        *model)
{
  auto sdk =
      std::make_unique<opentelemetry::sdk::metrics::Base2ExponentialHistogramAggregationConfig>();
  sdk->max_size_       = model->max_size;
  sdk->max_scale_      = model->max_scale;
  sdk->record_min_max_ = model->record_min_max;
  return sdk;
}

static std::unique_ptr<opentelemetry::sdk::metrics::HistogramAggregationConfig>
CreateExplicitBucketHistogramAggregation(
    const opentelemetry::sdk::configuration::ExplicitBucketHistogramAggregationConfiguration *model)
{
  auto sdk         = std::make_unique<opentelemetry::sdk::metrics::HistogramAggregationConfig>();
  sdk->boundaries_ = model->boundaries;
  sdk->record_min_max_ = model->record_min_max;
  return sdk;
}

class AggregationConfigDispatcher
    : public opentelemetry::sdk::configuration::AggregationConfigurationVisitor
{
public:
  AggregationConfigDispatcher()                                                    = default;
  AggregationConfigDispatcher(AggregationConfigDispatcher &&)                      = delete;
  AggregationConfigDispatcher(const AggregationConfigDispatcher &)                 = delete;
  AggregationConfigDispatcher &operator=(AggregationConfigDispatcher &&)           = delete;
  AggregationConfigDispatcher &operator=(const AggregationConfigDispatcher &other) = delete;
  ~AggregationConfigDispatcher() override                                          = default;

  void VisitBase2ExponentialBucketHistogram(
      const opentelemetry::sdk::configuration::
          Base2ExponentialBucketHistogramAggregationConfiguration *model) override
  {
    aggregation_type   = opentelemetry::sdk::metrics::AggregationType::kBase2ExponentialHistogram;
    aggregation_config = CreateBase2ExponentialBucketHistogramAggregation(model);
  }

  void VisitDefault(const opentelemetry::sdk::configuration::DefaultAggregationConfiguration
                        * /* model */) override
  {
    aggregation_type = opentelemetry::sdk::metrics::AggregationType::kDefault;
  }

  void VisitDrop(
      const opentelemetry::sdk::configuration::DropAggregationConfiguration * /* model */) override
  {
    aggregation_type = opentelemetry::sdk::metrics::AggregationType::kDrop;
  }

  void VisitExplicitBucketHistogram(
      const opentelemetry::sdk::configuration::ExplicitBucketHistogramAggregationConfiguration
          *model) override
  {
    aggregation_type   = opentelemetry::sdk::metrics::AggregationType::kHistogram;
    aggregation_config = CreateExplicitBucketHistogramAggregation(model);
  }

  void VisitLastValue(const opentelemetry::sdk::configuration::LastValueAggregationConfiguration
                          * /* model */) override
  {
    aggregation_type = opentelemetry::sdk::metrics::AggregationType::kLastValue;
  }

  void VisitSum(
      const opentelemetry::sdk::configuration::SumAggregationConfiguration * /* model */) override
  {
    aggregation_type = opentelemetry::sdk::metrics::AggregationType::kSum;
  }

  opentelemetry::sdk::metrics::AggregationType aggregation_type{
      opentelemetry::sdk::metrics::AggregationType::kDefault};
  std::unique_ptr<opentelemetry::sdk::metrics::AggregationConfig> aggregation_config;
};

static std::unique_ptr<opentelemetry::sdk::metrics::AggregationConfig> CreateAggregationConfig(
    const std::unique_ptr<opentelemetry::sdk::configuration::AggregationConfiguration> &model,
    opentelemetry::sdk::metrics::AggregationType &aggregation_type)
{
  AggregationConfigDispatcher dispatcher;
  model->Accept(&dispatcher);
  aggregation_type = dispatcher.aggregation_type;
  return std::move(dispatcher.aggregation_config);
}

}  // namespace

std::unique_ptr<opentelemetry::sdk::metrics::MetricReader> MetricsBuilderUtils::CreateMetricReader(
    const Registry *registry,
    const std::unique_ptr<MetricReaderConfiguration> &model)
{
  MetricReaderDispatcher dispatcher(registry);
  model->Accept(&dispatcher);
  return std::move(dispatcher.metric_reader);
}

std::unique_ptr<opentelemetry::sdk::metrics::AttributesProcessor>
MetricsBuilderUtils::CreateAttributesProcessor(
    const std::unique_ptr<IncludeExcludeConfiguration> &model)
{
  using opentelemetry::sdk::metrics::DefaultAttributesProcessor;
  using opentelemetry::sdk::metrics::IncludeExcludeAttributesProcessor;

  if (model->included == nullptr && model->excluded == nullptr)
  {
    return std::make_unique<DefaultAttributesProcessor>();
  }

  std::vector<std::string> included_patterns;
  if (model->included != nullptr)
  {
    included_patterns = model->included->string_array;
  }

  std::vector<std::string> excluded_patterns;
  if (model->excluded != nullptr)
  {
    excluded_patterns = model->excluded->string_array;
  }

  return std::make_unique<IncludeExcludeAttributesProcessor>(
      included_patterns.empty(), std::move(included_patterns), std::move(excluded_patterns));
}

void MetricsBuilderUtils::AddView(opentelemetry::sdk::metrics::ViewRegistry *view_registry,
                                  const std::unique_ptr<ViewConfiguration> &model)
{
  auto *selector = model->selector.get();

  // Synchronous gauge instruments are not supported in ABIv1
#if OPENTELEMETRY_ABI_VERSION_NO < 2
  if (selector->instrument_type == opentelemetry::sdk::configuration::InstrumentType::gauge)
  {
    std::string message("Runtime does not support instrument_type: gauge with ABI version 1");
    throw UnsupportedException(message);
  }
#endif

  auto add_view = [&](opentelemetry::sdk::metrics::InstrumentType sdk_instrument_type) {
    const std::string instrument_name =
        selector->instrument_name.empty() ? "*" : selector->instrument_name;

    auto sdk_instrument_selector =
        std::make_unique<opentelemetry::sdk::metrics::InstrumentSelector>(
            sdk_instrument_type, instrument_name, selector->unit,
            opentelemetry::sdk::metrics::PredicateType::kWildcard);

    auto sdk_meter_selector = std::make_unique<opentelemetry::sdk::metrics::MeterSelector>(
        selector->meter_name, selector->meter_version, selector->meter_schema_url);

    auto *stream = model->stream.get();

    opentelemetry::sdk::metrics::AggregationType sdk_aggregation_type =
        opentelemetry::sdk::metrics::AggregationType::kDefault;
    std::shared_ptr<opentelemetry::sdk::metrics::AggregationConfig> sdk_aggregation_config;

    if (stream->aggregation)
    {
      sdk_aggregation_config = CreateAggregationConfig(stream->aggregation, sdk_aggregation_type);
    }

    if (stream->aggregation_cardinality_limit != 0)
    {
      if (sdk_aggregation_config)
      {
        sdk_aggregation_config->cardinality_limit_ = stream->aggregation_cardinality_limit;
      }
      else
      {
        // ViewRegistry::AddView() rejects a config whose type doesn't match the instrument's
        // effective aggregation; reproduce that type here rather than using a plain
        // AggregationConfig.
        auto effective_aggregation_type = sdk_aggregation_type;
        if (effective_aggregation_type == opentelemetry::sdk::metrics::AggregationType::kDefault)
        {
          bool is_monotonic{false};
          effective_aggregation_type =
              opentelemetry::sdk::metrics::DefaultAggregation::GetDefaultAggregationType(
                  sdk_instrument_type, is_monotonic);
        }

        switch (effective_aggregation_type)
        {
          case opentelemetry::sdk::metrics::AggregationType::kHistogram: {
            auto histogram_config =
                std::make_shared<opentelemetry::sdk::metrics::HistogramAggregationConfig>(
                    stream->aggregation_cardinality_limit);
            // A default-constructed config has empty boundaries_; synthesized config must carry
            // DefaultBoundaries() to preserve the instrument's default histogram shape.
            histogram_config->boundaries_ =
                opentelemetry::sdk::metrics::HistogramAggregationConfig::DefaultBoundaries();
            sdk_aggregation_config = histogram_config;
            break;
          }

          default:
            sdk_aggregation_config =
                std::make_shared<opentelemetry::sdk::metrics::AggregationConfig>(
                    stream->aggregation_cardinality_limit);
            break;
        }
      }
    }

    std::unique_ptr<opentelemetry::sdk::metrics::AttributesProcessor> sdk_attribute_processor;
    if (stream->attribute_keys != nullptr)
    {
      sdk_attribute_processor = CreateAttributesProcessor(stream->attribute_keys);
    }

    auto sdk_view = std::make_unique<opentelemetry::sdk::metrics::View>(
        stream->name, stream->description, sdk_aggregation_type, sdk_aggregation_config,
        std::move(sdk_attribute_processor));

    view_registry->AddView(std::move(sdk_instrument_selector), std::move(sdk_meter_selector),
                           std::move(sdk_view));
  };

  // FIXME-SDK: register a single view instead once InstrumentSelector supports a match-all type.
  if (selector->instrument_type == opentelemetry::sdk::configuration::InstrumentType::none)
  {
    add_view(opentelemetry::sdk::metrics::InstrumentType::kCounter);
    add_view(opentelemetry::sdk::metrics::InstrumentType::kHistogram);
    add_view(opentelemetry::sdk::metrics::InstrumentType::kUpDownCounter);
    add_view(opentelemetry::sdk::metrics::InstrumentType::kObservableCounter);
    add_view(opentelemetry::sdk::metrics::InstrumentType::kObservableGauge);
    add_view(opentelemetry::sdk::metrics::InstrumentType::kObservableUpDownCounter);
#if OPENTELEMETRY_ABI_VERSION_NO >= 2
    add_view(opentelemetry::sdk::metrics::InstrumentType::kGauge);
#endif
  }
  else
  {
    add_view(ConvertInstrumentType(selector->instrument_type));
  }
}

std::unique_ptr<opentelemetry::sdk::instrumentationscope::ScopeConfigurator<
    opentelemetry::sdk::metrics::MeterConfig>>
MetricsBuilderUtils::CreateMeterConfigurator(
    const Registry *registry,
    const std::unique_ptr<MeterConfiguratorConfiguration> &model)
{
  const MeterConfiguratorBuilder *builder = registry->GetMeterConfiguratorBuilder();
  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateMeterConfigurator() using registered builder");
    return builder->Build(model.get());
  }
  static const std::string message("No builder for MeterConfigurator");
  throw UnsupportedException(message);
}

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
