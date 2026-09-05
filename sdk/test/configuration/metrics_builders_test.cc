// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <chrono>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#if defined(ENABLE_METRICS_EXEMPLAR_PREVIEW) && !defined(NO_GETENV)
#  include <cstdlib>
#  include "opentelemetry/sdk/common/global_log_handler.h"
#  include "opentelemetry/sdk/configuration/exemplar_filter.h"
#  include "opentelemetry/test_common/sdk/common/scoped_test_log_handler.h"

#  if defined(_MSC_VER)
#    include "opentelemetry/sdk/common/env_variables.h"
#  endif
#else
#  include <cstddef>
#endif

#include "opentelemetry/common/key_value_iterable_view.h"
#include "opentelemetry/metrics/meter.h"
#include "opentelemetry/metrics/meter_provider.h"
#include "opentelemetry/metrics/provider.h"
#include "opentelemetry/metrics/sync_instruments.h"
#include "opentelemetry/nostd/function_ref.h"
#include "opentelemetry/nostd/utility.h"
#include "opentelemetry/nostd/variant.h"

#include "opentelemetry/sdk/configuration/aggregation_configuration.h"
#include "opentelemetry/sdk/configuration/base2_exponential_bucket_histogram_aggregation_configuration.h"
#include "opentelemetry/sdk/configuration/cardinality_limits_configuration.h"
#include "opentelemetry/sdk/configuration/configuration.h"
#include "opentelemetry/sdk/configuration/configured_sdk.h"
#include "opentelemetry/sdk/configuration/console_push_metric_exporter_builder.h"
#include "opentelemetry/sdk/configuration/console_push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/default_aggregation_configuration.h"
#include "opentelemetry/sdk/configuration/drop_aggregation_configuration.h"
#include "opentelemetry/sdk/configuration/explicit_bucket_histogram_aggregation_configuration.h"
#include "opentelemetry/sdk/configuration/extension_pull_metric_exporter_builder.h"
#include "opentelemetry/sdk/configuration/extension_pull_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/extension_push_metric_exporter_builder.h"
#include "opentelemetry/sdk/configuration/extension_push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/include_exclude_configuration.h"
#include "opentelemetry/sdk/configuration/instrument_type.h"
#include "opentelemetry/sdk/configuration/last_value_aggregation_configuration.h"
#include "opentelemetry/sdk/configuration/meter_configurator_builder.h"
#include "opentelemetry/sdk/configuration/meter_configurator_configuration.h"
#include "opentelemetry/sdk/configuration/meter_provider_configuration.h"
#include "opentelemetry/sdk/configuration/metric_producer_configuration.h"
#include "opentelemetry/sdk/configuration/metric_reader_configuration.h"
#include "opentelemetry/sdk/configuration/metrics_builders.h"
#include "opentelemetry/sdk/configuration/open_census_metric_producer_configuration.h"
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
#include "opentelemetry/sdk/configuration/pull_metric_reader_configuration.h"
#include "opentelemetry/sdk/configuration/push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/registry.h"
#include "opentelemetry/sdk/configuration/sdk_builder.h"
#include "opentelemetry/sdk/configuration/string_array_configuration.h"
#include "opentelemetry/sdk/configuration/sum_aggregation_configuration.h"
#include "opentelemetry/sdk/configuration/unsupported_exception.h"
#include "opentelemetry/sdk/configuration/view_configuration.h"
#include "opentelemetry/sdk/configuration/view_selector_configuration.h"
#include "opentelemetry/sdk/configuration/view_stream_configuration.h"
#include "opentelemetry/sdk/instrumentationscope/instrumentation_scope.h"
#include "opentelemetry/sdk/instrumentationscope/scope_configurator.h"

#include "opentelemetry/sdk/metrics/aggregation/aggregation.h"
#include "opentelemetry/sdk/metrics/aggregation/aggregation_config.h"
#include "opentelemetry/sdk/metrics/aggregation/default_aggregation.h"
#include "opentelemetry/sdk/metrics/data/point_data.h"
#include "opentelemetry/sdk/metrics/instruments.h"
#include "opentelemetry/sdk/metrics/meter_config.h"
#include "opentelemetry/sdk/metrics/meter_provider.h"
#include "opentelemetry/sdk/metrics/metric_reader.h"
#include "opentelemetry/sdk/metrics/push_metric_exporter.h"
#include "opentelemetry/sdk/metrics/view/attributes_processor.h"
#include "opentelemetry/sdk/metrics/view/view.h"
#include "opentelemetry/sdk/metrics/view/view_registry.h"
#include "opentelemetry/sdk/resource/resource.h"

#include "config_test_metrics.h"

namespace metrics     = opentelemetry::metrics;
namespace metrics_sdk = opentelemetry::sdk::metrics;
namespace scope_sdk   = opentelemetry::sdk::instrumentationscope;
namespace config_sdk  = opentelemetry::sdk::configuration;

namespace
{

class MetricsBuildersTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    registry_ = std::make_shared<config_sdk::Registry>();
    config_sdk::RegisterDefaultMetricsBuilders(registry_.get());
    registry_->SetConsolePushMetricExporterBuilder(
        std::make_unique<config_test::RecordingConsolePushMetricExporterBuilder>(nullptr));
    registry_->SetOtlpHttpPushMetricExporterBuilder(
        std::make_unique<config_test::RecordingOtlpHttpPushMetricExporterBuilder>(nullptr));
    registry_->SetOtlpGrpcPushMetricExporterBuilder(
        std::make_unique<config_test::RecordingOtlpGrpcPushMetricExporterBuilder>(nullptr));
    registry_->SetOtlpFilePushMetricExporterBuilder(
        std::make_unique<config_test::RecordingOtlpFilePushMetricExporterBuilder>(nullptr));
    registry_->SetPrometheusPullMetricExporterBuilder(
        std::make_unique<config_test::RecordingPrometheusPullMetricExporterBuilder>(nullptr));
  }

  std::unique_ptr<metrics_sdk::MetricReader> MakeMetricReader(
      std::unique_ptr<config_sdk::MetricReaderConfiguration> model)
  {
    config_sdk::SdkBuilder builder(registry_);
    return builder.CreateMetricReader(model);
  }

  static std::unique_ptr<config_sdk::ViewConfiguration> MakeViewWithAggregation(
      std::unique_ptr<config_sdk::AggregationConfiguration> aggregation)
  {
    auto model                       = std::make_unique<config_sdk::ViewConfiguration>();
    model->selector                  = std::make_unique<config_sdk::ViewSelectorConfiguration>();
    model->selector->instrument_type = config_sdk::InstrumentType::none;
    model->stream                    = std::make_unique<config_sdk::ViewStreamConfiguration>();
    model->stream->aggregation       = std::move(aggregation);
    return model;
  }

  static std::unique_ptr<config_sdk::ViewConfiguration> MakeCardinalityOnlyViewConfig(
      config_sdk::InstrumentType instrument_type,
      std::size_t cardinality_limit)
  {
    auto model                       = std::make_unique<config_sdk::ViewConfiguration>();
    model->selector                  = std::make_unique<config_sdk::ViewSelectorConfiguration>();
    model->selector->instrument_type = instrument_type;

    model->stream = std::make_unique<config_sdk::ViewStreamConfiguration>();
    model->stream->aggregation_cardinality_limit = cardinality_limit;

    return model;
  }

  std::unique_ptr<scope_sdk::ScopeConfigurator<metrics_sdk::MeterConfig>> MakeMeterConfigurator(
      std::unique_ptr<config_sdk::MeterConfiguratorConfiguration> model)
  {
    config_sdk::SdkBuilder builder(registry_);
    return builder.CreateMeterConfigurator(model);
  }

  void AddView(metrics_sdk::ViewRegistry *view_registry,
               const std::unique_ptr<config_sdk::ViewConfiguration> &model)
  {
    config_sdk::SdkBuilder builder(registry_);
    builder.AddView(view_registry, model);
  }

  std::unique_ptr<metrics_sdk::AttributesProcessor> CreateAttributesProcessor(
      const std::unique_ptr<config_sdk::IncludeExcludeConfiguration> &model)
  {
    config_sdk::SdkBuilder builder(registry_);
    return builder.CreateAttributesProcessor(model);
  }

  std::unique_ptr<metrics_sdk::MeterProvider> MakeMeterProvider(
      std::unique_ptr<config_sdk::MeterProviderConfiguration> model,
      const opentelemetry::sdk::resource::Resource &resource)
  {
    config_sdk::SdkBuilder builder(registry_);
    return builder.CreateMeterProvider(model, resource);
  }

  void CheckInstrumentType(
      config_sdk::InstrumentType config_type,
      metrics_sdk::InstrumentType sdk_type,
      metrics_sdk::AggregationType expected_aggregation = metrics_sdk::AggregationType::kDefault)
  {
    auto model = MakeCardinalityOnlyViewConfig(config_type, 7);

    metrics_sdk::ViewRegistry view_registry;
    AddView(&view_registry, model);

    metrics_sdk::InstrumentDescriptor descriptor{"test.instrument", "test description", "units",
                                                 sdk_type, metrics_sdk::InstrumentValueType::kLong};
    auto scope = scope_sdk::InstrumentationScope::Create("");

    std::size_t matched = 0;
    view_registry.FindViews(descriptor, *scope, [&](const metrics_sdk::View &view) {
      matched++;
      auto *config = view.GetAggregationConfig();
      EXPECT_NE(config, nullptr);
      if (config)
      {
        EXPECT_EQ(config->GetType(), expected_aggregation);
        EXPECT_EQ(config->cardinality_limit_, 7u);
      }
      return true;
    });
    EXPECT_EQ(matched, 1);
  }

  void CheckAggregationType(std::unique_ptr<config_sdk::AggregationConfiguration> aggregation,
                            metrics_sdk::AggregationType expected_type)
  {
    auto model = MakeViewWithAggregation(std::move(aggregation));
    metrics_sdk::ViewRegistry view_registry;
    AddView(&view_registry, model);

    auto scope = scope_sdk::InstrumentationScope::Create("");
    metrics_sdk::InstrumentDescriptor descriptor{"test.instrument", "test description", "units",
                                                 metrics_sdk::InstrumentType::kCounter,
                                                 metrics_sdk::InstrumentValueType::kLong};
    std::size_t matched = 0;
    view_registry.FindViews(descriptor, *scope, [&](const metrics_sdk::View &view) {
      EXPECT_EQ(view.GetAggregationType(), expected_type);
      matched++;
      return true;
    });
    EXPECT_EQ(matched, 1);
  }

  std::shared_ptr<config_sdk::Registry> registry_;
};
}  // namespace

TEST_F(MetricsBuildersTest, DefaultRegistry)
{
  auto registry = std::make_shared<config_sdk::Registry>();
  EXPECT_EQ(registry->GetPeriodicMetricReaderBuilder(), nullptr);
  EXPECT_EQ(registry->GetMeterConfiguratorBuilder(), nullptr);
}

TEST_F(MetricsBuildersTest, RegisterDefaultMetricsBuilders)
{
  auto registry = std::make_shared<config_sdk::Registry>();
  config_sdk::RegisterDefaultMetricsBuilders(registry.get());

  EXPECT_NE(registry->GetPeriodicMetricReaderBuilder(), nullptr);
  EXPECT_NE(registry->GetMeterConfiguratorBuilder(), nullptr);
}

#if defined(ENABLE_METRICS_EXEMPLAR_PREVIEW) && !defined(NO_GETENV)

namespace
{
constexpr char kMetricsExemplarFilterEnv[] = "OTEL_METRICS_EXEMPLAR_FILTER";

#  if defined(_MSC_VER)
using opentelemetry::sdk::common::setenv;
using opentelemetry::sdk::common::unsetenv;
#  endif
}  // namespace

TEST_F(MetricsBuildersTest, DeclarativeExemplarFilterDoesNotReadEnvironment)
{
  unsetenv(kMetricsExemplarFilterEnv);
  opentelemetry::test_common::ScopedTestLogHandler log_handler{
      opentelemetry::sdk::common::internal_log::LogLevel::Warning};
  setenv(kMetricsExemplarFilterEnv, "invalid", 1);

  auto model             = std::make_unique<config_sdk::MeterProviderConfiguration>();
  model->exemplar_filter = config_sdk::ExemplarFilter::always_on;

  auto resource = opentelemetry::sdk::resource::Resource::Create({});
  auto provider = MakeMeterProvider(std::move(model), resource);
  ASSERT_NE(provider, nullptr);
  EXPECT_TRUE(log_handler.Drain().empty());
  unsetenv(kMetricsExemplarFilterEnv);
}
#endif

TEST_F(MetricsBuildersTest, AggregationTypeDefault)
{
  CheckAggregationType(std::make_unique<config_sdk::DefaultAggregationConfiguration>(),
                       metrics_sdk::AggregationType::kDefault);
}

TEST_F(MetricsBuildersTest, AggregationTypeSumAggregation)
{
  CheckAggregationType(std::make_unique<config_sdk::SumAggregationConfiguration>(),
                       metrics_sdk::AggregationType::kSum);
}

TEST_F(MetricsBuildersTest, AggregationTypeLastValueAggregation)
{
  CheckAggregationType(std::make_unique<config_sdk::LastValueAggregationConfiguration>(),
                       metrics_sdk::AggregationType::kLastValue);
}

TEST_F(MetricsBuildersTest, AggregationTypeDropAggregation)
{
  CheckAggregationType(std::make_unique<config_sdk::DropAggregationConfiguration>(),
                       metrics_sdk::AggregationType::kDrop);
}

TEST_F(MetricsBuildersTest, AggregationTypeExplicitBucketHistogramAggregation)
{
  CheckAggregationType(
      std::make_unique<config_sdk::ExplicitBucketHistogramAggregationConfiguration>(),
      metrics_sdk::AggregationType::kHistogram);
}

TEST_F(MetricsBuildersTest, AggregationTypeBase2ExponentialHistogramAggregation)
{
  CheckAggregationType(
      std::make_unique<config_sdk::Base2ExponentialBucketHistogramAggregationConfiguration>(),
      metrics_sdk::AggregationType::kBase2ExponentialHistogram);
}

TEST_F(MetricsBuildersTest, InstrumentTypeCounter)
{
  CheckInstrumentType(config_sdk::InstrumentType::counter, metrics_sdk::InstrumentType::kCounter);
}

TEST_F(MetricsBuildersTest, InstrumentTypeUpDownCounter)
{
  CheckInstrumentType(config_sdk::InstrumentType::up_down_counter,
                      metrics_sdk::InstrumentType::kUpDownCounter);
}

TEST_F(MetricsBuildersTest, InstrumentTypeObservableCounter)
{
  CheckInstrumentType(config_sdk::InstrumentType::observable_counter,
                      metrics_sdk::InstrumentType::kObservableCounter);
}

TEST_F(MetricsBuildersTest, InstrumentTypeObservableGauge)
{
  CheckInstrumentType(config_sdk::InstrumentType::observable_gauge,
                      metrics_sdk::InstrumentType::kObservableGauge);
}

TEST_F(MetricsBuildersTest, InstrumentTypeObservableUpDownCounter)
{
  CheckInstrumentType(config_sdk::InstrumentType::observable_up_down_counter,
                      metrics_sdk::InstrumentType::kObservableUpDownCounter);
}

TEST_F(MetricsBuildersTest, InstrumentTypeHistogram)
{
  CheckInstrumentType(config_sdk::InstrumentType::histogram,
                      metrics_sdk::InstrumentType::kHistogram,
                      metrics_sdk::AggregationType::kHistogram);
}

// Gauge is only supported in ABI v2.
TEST_F(MetricsBuildersTest, InstrumentTypeGauge)
{
#if OPENTELEMETRY_ABI_VERSION_NO < 2
  auto model = MakeCardinalityOnlyViewConfig(config_sdk::InstrumentType::gauge, 42);
  metrics_sdk::ViewRegistry view_registry;
  EXPECT_THROW(AddView(&view_registry, model), config_sdk::UnsupportedException);
#else
  CheckInstrumentType(config_sdk::InstrumentType::gauge, metrics_sdk::InstrumentType::kGauge);
#endif
}

TEST_F(MetricsBuildersTest, ViewSelectorInstrumentTypeNone)
{
  namespace metrics_sdk = metrics_sdk;

  auto model = MakeCardinalityOnlyViewConfig(config_sdk::InstrumentType::none, 42);

  metrics_sdk::ViewRegistry view_registry;
  AddView(&view_registry, model);

  auto instrumentation_scope = scope_sdk::InstrumentationScope::Create("");
  std::vector<metrics_sdk::InstrumentType> supported_instrument_types{
      metrics_sdk::InstrumentType::kCounter,
      metrics_sdk::InstrumentType::kHistogram,
      metrics_sdk::InstrumentType::kUpDownCounter,
      metrics_sdk::InstrumentType::kObservableCounter,
      metrics_sdk::InstrumentType::kObservableGauge,
      metrics_sdk::InstrumentType::kObservableUpDownCounter};
#if OPENTELEMETRY_ABI_VERSION_NO >= 2
  supported_instrument_types.push_back(metrics_sdk::InstrumentType::kGauge);
#endif

  for (auto instrument_type : supported_instrument_types)
  {
    metrics_sdk::InstrumentDescriptor instrument_descriptor{
        "test.instrument", "test description", "units", instrument_type,
        metrics_sdk::InstrumentValueType::kLong};
    std::size_t matched = 0;
    view_registry.FindViews(instrument_descriptor, *instrumentation_scope,
                            [&](const metrics_sdk::View &view) {
                              auto *config = view.GetAggregationConfig();
                              EXPECT_NE(config, nullptr);
                              if (config != nullptr)
                              {
                                EXPECT_EQ(config->cardinality_limit_, 42u);
                                matched++;
                              }
                              return true;
                            });
    EXPECT_EQ(matched, 1);
  }
}

TEST_F(MetricsBuildersTest, ViewSelectorWithHistogramCardinalityLimit)
{
  // Verify that AddView populates default bucket boundaries on a cardinality-only
  // histogram view, rather than leaving boundaries_ empty (which would produce a
  // single-bucket histogram instead of the spec's 15-bucket default).
  auto model = MakeCardinalityOnlyViewConfig(config_sdk::InstrumentType::histogram, 42);

  metrics_sdk::ViewRegistry view_registry;
  AddView(&view_registry, model);

  metrics_sdk::InstrumentDescriptor instrument_descriptor{
      "test.instrument", "test description", "units", metrics_sdk::InstrumentType::kHistogram,
      metrics_sdk::InstrumentValueType::kLong};
  auto instrumentation_scope = scope_sdk::InstrumentationScope::Create("");

  view_registry.FindViews(
      instrument_descriptor, *instrumentation_scope, [&](const metrics_sdk::View &view) {
        auto *aggregation_config = view.GetAggregationConfig();
        EXPECT_NE(aggregation_config, nullptr);
        if (!aggregation_config)
          return true;
        auto aggregation = metrics_sdk::DefaultAggregation::CreateAggregation(
            metrics_sdk::AggregationType::kHistogram, instrument_descriptor, aggregation_config);
        EXPECT_NE(aggregation, nullptr);
        if (!aggregation)
          return true;
        auto histogram_data =
            opentelemetry::nostd::get<metrics_sdk::HistogramPointData>(aggregation->ToPoint());
        EXPECT_EQ(histogram_data.boundaries_.size(), 15u);
        EXPECT_EQ(histogram_data.counts_.size(), 16u);
        return true;
      });
}

TEST_F(MetricsBuildersTest, ViewSelectorWithHistogramExplicitAggregation)
{
  auto model = MakeCardinalityOnlyViewConfig(config_sdk::InstrumentType::histogram, 42);
  auto aggregation =
      std::make_unique<config_sdk::ExplicitBucketHistogramAggregationConfiguration>();
  aggregation->boundaries    = {1.0, 2.0};
  model->stream->aggregation = std::move(aggregation);

  metrics_sdk::ViewRegistry view_registry;
  AddView(&view_registry, model);

  metrics_sdk::InstrumentDescriptor instrument_descriptor{
      "test.instrument", "test description", "units", metrics_sdk::InstrumentType::kHistogram,
      metrics_sdk::InstrumentValueType::kLong};
  auto instrumentation_scope = scope_sdk::InstrumentationScope::Create("");

  std::size_t matched = 0;
  view_registry.FindViews(
      instrument_descriptor, *instrumentation_scope, [&](const metrics_sdk::View &view) {
        ++matched;
        auto *aggregation_config = view.GetAggregationConfig();
        EXPECT_NE(aggregation_config, nullptr);
        if (aggregation_config)
        {
          EXPECT_EQ(aggregation_config->GetType(), metrics_sdk::AggregationType::kHistogram);
          EXPECT_EQ(aggregation_config->cardinality_limit_, 42u);
          auto *histogram_config =
              static_cast<const metrics_sdk::HistogramAggregationConfig *>(aggregation_config);
          EXPECT_EQ(histogram_config->boundaries_, (std::vector<double>{1.0, 2.0}));
        }
        return true;
      });

  EXPECT_EQ(matched, 1);
}

TEST_F(MetricsBuildersTest, ViewSelectorWithAttributesProcessor)
{

  auto model                       = std::make_unique<config_sdk::ViewConfiguration>();
  model->selector                  = std::make_unique<config_sdk::ViewSelectorConfiguration>();
  model->selector->instrument_type = config_sdk::InstrumentType::counter;
  model->stream                    = std::make_unique<config_sdk::ViewStreamConfiguration>();
  model->stream->attribute_keys    = std::make_unique<config_sdk::IncludeExcludeConfiguration>();
  model->stream->attribute_keys->included =
      std::make_unique<config_sdk::StringArrayConfiguration>();
  model->stream->attribute_keys->included->string_array = {"allowed"};

  metrics_sdk::ViewRegistry view_registry;
  AddView(&view_registry, model);

  auto scope = scope_sdk::InstrumentationScope::Create("");
  metrics_sdk::InstrumentDescriptor descriptor{"m", "", "", metrics_sdk::InstrumentType::kCounter,
                                               metrics_sdk::InstrumentValueType::kLong};

  std::map<std::string, int> attributes = {{"allowed", 1}, {"filtered_out", 2}};
  opentelemetry::common::KeyValueIterableView<std::map<std::string, int>> attr_view{attributes};

  view_registry.FindViews(descriptor, *scope, [&](const metrics_sdk::View &view) {
    auto processor = view.GetAttributesProcessor();
    EXPECT_NE(processor, nullptr);
    if (!processor)
      return true;
    auto result = processor->process(attr_view);
    EXPECT_EQ(result.size(), 1u);
    EXPECT_NE(result.find("allowed"), result.end());
    EXPECT_EQ(result.find("filtered_out"), result.end());
    return true;
  });
}

TEST_F(MetricsBuildersTest, CreateBuiltInPeriodicMetricReader)
{
  auto model      = std::make_unique<config_sdk::PeriodicMetricReaderConfiguration>();
  model->interval = 2000;
  model->timeout  = 500;

  const auto *builder = registry_->GetPeriodicMetricReaderBuilder();
  ASSERT_NE(builder, nullptr);
  auto reader =
      builder->Build(model.get(), std::make_unique<config_test::NoopPushMetricExporter>());
  ASSERT_NE(reader, nullptr);
  EXPECT_TRUE(reader->Shutdown(std::chrono::seconds(5)));
}

TEST_F(MetricsBuildersTest, CreatePullMetricReaderWithExtensionExporter)
{
  registry_->SetExtensionPullMetricExporterBuilder(
      "test_pull", std::make_unique<config_test::NoopPullMetricExporterBuilder>());

  auto exporter_config  = std::make_unique<config_sdk::ExtensionPullMetricExporterConfiguration>();
  exporter_config->name = "test_pull";
  auto model            = std::make_unique<config_sdk::PullMetricReaderConfiguration>();
  model->exporter       = std::move(exporter_config);
  std::unique_ptr<config_sdk::MetricReaderConfiguration> reader_config = std::move(model);

  auto reader = MakeMetricReader(std::move(reader_config));
  ASSERT_NE(reader, nullptr);
  EXPECT_TRUE(reader->Shutdown(std::chrono::seconds(5)));
}

TEST_F(MetricsBuildersTest, CreatePullMetricReaderWithProducer)
{
  registry_->SetExtensionPullMetricExporterBuilder(
      "test_pull", std::make_unique<config_test::NoopPullMetricExporterBuilder>());

  auto exporter_config  = std::make_unique<config_sdk::ExtensionPullMetricExporterConfiguration>();
  exporter_config->name = "test_pull";
  auto model            = std::make_unique<config_sdk::PullMetricReaderConfiguration>();
  model->exporter       = std::move(exporter_config);
  model->producers.push_back(std::make_unique<config_sdk::OpenCensusMetricProducerConfiguration>());
  std::unique_ptr<config_sdk::MetricReaderConfiguration> reader_config = std::move(model);

  auto reader = MakeMetricReader(std::move(reader_config));
  ASSERT_NE(reader, nullptr);
  EXPECT_TRUE(reader->Shutdown(std::chrono::seconds(5)));
}

TEST_F(MetricsBuildersTest, CreatePullMetricReaderWithCardinalityLimits)
{
  registry_->SetExtensionPullMetricExporterBuilder(
      "test_pull", std::make_unique<config_test::NoopPullMetricExporterBuilder>());

  auto exporter_config  = std::make_unique<config_sdk::ExtensionPullMetricExporterConfiguration>();
  exporter_config->name = "test_pull";
  auto model            = std::make_unique<config_sdk::PullMetricReaderConfiguration>();
  model->exporter       = std::move(exporter_config);
  model->cardinality_limits = std::make_unique<config_sdk::CardinalityLimitsConfiguration>();
  model->cardinality_limits->counter                                   = 42;
  std::unique_ptr<config_sdk::MetricReaderConfiguration> reader_config = std::move(model);

  auto reader = MakeMetricReader(std::move(reader_config));
  ASSERT_NE(reader, nullptr);
  EXPECT_EQ(reader->GetCardinalityLimit(metrics_sdk::InstrumentType::kCounter), 42u);
  EXPECT_TRUE(reader->Shutdown(std::chrono::seconds(5)));
}

TEST_F(MetricsBuildersTest, CreatePeriodicMetricReader)
{
  auto exporter  = std::make_unique<config_sdk::ExtensionPushMetricExporterConfiguration>();
  exporter->name = "noop";

  auto model                = std::make_unique<config_sdk::PeriodicMetricReaderConfiguration>();
  model->exporter           = std::move(exporter);
  model->interval           = 12345;
  model->timeout            = 678;
  model->cardinality_limits = std::make_unique<config_sdk::CardinalityLimitsConfiguration>();
  model->cardinality_limits->default_limit              = 100;
  model->cardinality_limits->counter                    = 200;
  model->cardinality_limits->gauge                      = 300;
  model->cardinality_limits->histogram                  = 400;
  model->cardinality_limits->observable_counter         = 500;
  model->cardinality_limits->observable_gauge           = 600;
  model->cardinality_limits->observable_up_down_counter = 700;
  model->cardinality_limits->up_down_counter            = 800;

  const auto captured          = std::make_shared<config_test::CapturedPeriodicReaderArgs>();
  const auto expected_interval = model->interval;
  const auto expected_timeout  = model->timeout;

  registry_->SetExtensionPushMetricExporterBuilder(
      "noop", std::make_unique<config_test::NoopPushMetricExporterBuilder>());
  registry_->SetPeriodicMetricReaderBuilder(
      std::make_unique<config_test::CapturingPeriodicMetricReaderBuilder>(captured));

  std::unique_ptr<config_sdk::MetricReaderConfiguration> reader_config = std::move(model);
  auto reader = MakeMetricReader(std::move(reader_config));
  ASSERT_NE(reader, nullptr);

  EXPECT_TRUE(captured->called);
  EXPECT_EQ(captured->interval, expected_interval);
  EXPECT_EQ(captured->timeout, expected_timeout);
  EXPECT_TRUE(captured->exporter != nullptr);
  EXPECT_EQ(reader->GetCardinalityLimit(metrics_sdk::InstrumentType::kCounter), 200u);
  EXPECT_EQ(reader->GetCardinalityLimit(metrics_sdk::InstrumentType::kGauge), 300u);
  EXPECT_EQ(reader->GetCardinalityLimit(metrics_sdk::InstrumentType::kHistogram), 400u);
  EXPECT_EQ(reader->GetCardinalityLimit(metrics_sdk::InstrumentType::kObservableCounter), 500u);
  EXPECT_EQ(reader->GetCardinalityLimit(metrics_sdk::InstrumentType::kObservableGauge), 600u);
  EXPECT_EQ(reader->GetCardinalityLimit(metrics_sdk::InstrumentType::kObservableUpDownCounter),
            700u);
  EXPECT_EQ(reader->GetCardinalityLimit(metrics_sdk::InstrumentType::kUpDownCounter), 800u);
}

TEST_F(MetricsBuildersTest, CreateAttributesProcessor)
{
  std::map<std::string, int> attributes = {{"included", 1}, {"excluded", 2}, {"unlisted", 3}};
  opentelemetry::common::KeyValueIterableView<std::map<std::string, int>> iterable(attributes);

  // When both lists are configured, exclusion takes precedence over inclusion.
  {
    auto model                    = std::make_unique<config_sdk::IncludeExcludeConfiguration>();
    model->included               = std::make_unique<config_sdk::StringArrayConfiguration>();
    model->included->string_array = {"included", "excluded"};
    model->excluded               = std::make_unique<config_sdk::StringArrayConfiguration>();
    model->excluded->string_array = {"excluded"};

    auto processor = CreateAttributesProcessor(model);
    ASSERT_NE(processor, nullptr);
    auto filtered = processor->process(iterable);

    EXPECT_EQ(filtered.size(), 1u);
    EXPECT_NE(filtered.find("included"), filtered.end());
  }

  // Wildcard patterns are evaluated per key, with exclusion taking precedence.
  {
    std::map<std::string, int> wildcard_attributes = {
        {"foo.bar", 1}, {"foo.baz", 2}, {"question.x", 3}, {"question.xy", 4}, {"other", 5}};
    opentelemetry::common::KeyValueIterableView<std::map<std::string, int>> wildcard_iterable(
        wildcard_attributes);

    auto model                    = std::make_unique<config_sdk::IncludeExcludeConfiguration>();
    model->included               = std::make_unique<config_sdk::StringArrayConfiguration>();
    model->included->string_array = {"foo.*", "question.?"};
    model->excluded               = std::make_unique<config_sdk::StringArrayConfiguration>();
    model->excluded->string_array = {"foo.bar"};

    auto processor = CreateAttributesProcessor(model);
    ASSERT_NE(processor, nullptr);
    auto filtered = processor->process(wildcard_iterable);

    EXPECT_EQ(filtered.size(), 2u);
    EXPECT_NE(filtered.find("foo.baz"), filtered.end());
    EXPECT_NE(filtered.find("question.x"), filtered.end());
  }

  // An exclude-only configuration retains every key that is not excluded.
  {
    auto model                    = std::make_unique<config_sdk::IncludeExcludeConfiguration>();
    model->excluded               = std::make_unique<config_sdk::StringArrayConfiguration>();
    model->excluded->string_array = {"excluded"};

    auto processor = CreateAttributesProcessor(model);
    ASSERT_NE(processor, nullptr);
    auto filtered = processor->process(iterable);

    EXPECT_EQ(filtered.size(), 2u);
    EXPECT_EQ(filtered.find("excluded"), filtered.end());
  }

  // A null include/exclude block leaves attributes unchanged.
  {
    auto model     = std::make_unique<config_sdk::IncludeExcludeConfiguration>();
    auto processor = CreateAttributesProcessor(model);
    ASSERT_NE(processor, nullptr);
    auto filtered = processor->process(iterable);

    EXPECT_EQ(filtered.size(), attributes.size());
  }

  // An empty include list leaves attributes unchanged.
  {
    auto model      = std::make_unique<config_sdk::IncludeExcludeConfiguration>();
    model->included = std::make_unique<config_sdk::StringArrayConfiguration>();
    model->excluded = std::make_unique<config_sdk::StringArrayConfiguration>();

    auto processor = CreateAttributesProcessor(model);
    ASSERT_NE(processor, nullptr);
    auto filtered = processor->process(iterable);

    EXPECT_EQ(filtered.size(), attributes.size());
  }
}

// ---------------------------------------------------------------------------
// MetricsBuildersTest: Verify that unregistered builders throw UnsupportedException.

TEST_F(MetricsBuildersTest, UnregisteredMeterConfiguratorBuilder)
{
  registry_->SetMeterConfiguratorBuilder(nullptr);
  auto model = std::make_unique<config_sdk::MeterConfiguratorConfiguration>();
  EXPECT_THROW(MakeMeterConfigurator(std::move(model)), config_sdk::UnsupportedException);
}

TEST_F(MetricsBuildersTest, UnregisteredExtensionPushMetricExporterBuilder)
{
  auto ext         = std::make_unique<config_sdk::ExtensionPushMetricExporterConfiguration>();
  ext->name        = "unregistered";
  auto reader      = std::make_unique<config_sdk::PeriodicMetricReaderConfiguration>();
  reader->exporter = std::move(ext);
  std::unique_ptr<config_sdk::MetricReaderConfiguration> model = std::move(reader);
  EXPECT_THROW(MakeMetricReader(std::move(model)), config_sdk::UnsupportedException);
}

TEST_F(MetricsBuildersTest, UnregisteredExtensionPullMetricExporterBuilder)
{
  auto ext         = std::make_unique<config_sdk::ExtensionPullMetricExporterConfiguration>();
  ext->name        = "unregistered";
  auto reader      = std::make_unique<config_sdk::PullMetricReaderConfiguration>();
  reader->exporter = std::move(ext);
  std::unique_ptr<config_sdk::MetricReaderConfiguration> model = std::move(reader);
  EXPECT_THROW(MakeMetricReader(std::move(model)), config_sdk::UnsupportedException);
}

TEST_F(MetricsBuildersTest, UnregisteredPeriodicMetricReaderBuilder)
{
  registry_->SetPeriodicMetricReaderBuilder(nullptr);

  auto exporter    = std::make_unique<config_sdk::ConsolePushMetricExporterConfiguration>();
  auto reader      = std::make_unique<config_sdk::PeriodicMetricReaderConfiguration>();
  reader->exporter = std::move(exporter);
  std::unique_ptr<config_sdk::MetricReaderConfiguration> model = std::move(reader);

  EXPECT_THROW(MakeMetricReader(std::move(model)), config_sdk::UnsupportedException);
}

TEST_F(MetricsBuildersTest, UnregisteredConsolePushMetricExporterBuilder)
{
  registry_->SetConsolePushMetricExporterBuilder(nullptr);

  auto exporter    = std::make_unique<config_sdk::ConsolePushMetricExporterConfiguration>();
  auto reader      = std::make_unique<config_sdk::PeriodicMetricReaderConfiguration>();
  reader->exporter = std::move(exporter);
  std::unique_ptr<config_sdk::MetricReaderConfiguration> model = std::move(reader);

  EXPECT_THROW(MakeMetricReader(std::move(model)), config_sdk::UnsupportedException);
}

TEST_F(MetricsBuildersTest, UnregisteredOtlpHttpPushMetricExporterBuilder)
{
  registry_->SetOtlpHttpPushMetricExporterBuilder(nullptr);

  auto exporter    = std::make_unique<config_sdk::OtlpHttpPushMetricExporterConfiguration>();
  auto reader      = std::make_unique<config_sdk::PeriodicMetricReaderConfiguration>();
  reader->exporter = std::move(exporter);
  std::unique_ptr<config_sdk::MetricReaderConfiguration> model = std::move(reader);

  EXPECT_THROW(MakeMetricReader(std::move(model)), config_sdk::UnsupportedException);
}

TEST_F(MetricsBuildersTest, UnregisteredOtlpGrpcPushMetricExporterBuilder)
{
  registry_->SetOtlpGrpcPushMetricExporterBuilder(nullptr);

  auto exporter    = std::make_unique<config_sdk::OtlpGrpcPushMetricExporterConfiguration>();
  auto reader      = std::make_unique<config_sdk::PeriodicMetricReaderConfiguration>();
  reader->exporter = std::move(exporter);
  std::unique_ptr<config_sdk::MetricReaderConfiguration> model = std::move(reader);

  EXPECT_THROW(MakeMetricReader(std::move(model)), config_sdk::UnsupportedException);
}

TEST_F(MetricsBuildersTest, UnregisteredOtlpFilePushMetricExporterBuilder)
{
  registry_->SetOtlpFilePushMetricExporterBuilder(nullptr);

  auto exporter    = std::make_unique<config_sdk::OtlpFilePushMetricExporterConfiguration>();
  auto reader      = std::make_unique<config_sdk::PeriodicMetricReaderConfiguration>();
  reader->exporter = std::move(exporter);
  std::unique_ptr<config_sdk::MetricReaderConfiguration> model = std::move(reader);

  EXPECT_THROW(MakeMetricReader(std::move(model)), config_sdk::UnsupportedException);
}

TEST_F(MetricsBuildersTest, UnregisteredPrometheusPullMetricExporterBuilder)
{
  registry_->SetPrometheusPullMetricExporterBuilder(nullptr);

  auto exporter    = std::make_unique<config_sdk::PrometheusPullMetricExporterConfiguration>();
  auto reader      = std::make_unique<config_sdk::PullMetricReaderConfiguration>();
  reader->exporter = std::move(exporter);
  std::unique_ptr<config_sdk::MetricReaderConfiguration> model = std::move(reader);

  EXPECT_THROW(MakeMetricReader(std::move(model)), config_sdk::UnsupportedException);
}

// ---------------------------------------------------------------------------
// MeterProviderConfigTest: Use ConfiguredSdk::Create to verify meter provider configuration and
// installation.

namespace
{

class MeterProviderConfigTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    registry_      = std::make_shared<config_sdk::Registry>();
    metric_buffer_ = std::make_shared<config_test::MetricBuffer>();
    config_sdk::RegisterDefaultMetricsBuilders(registry_.get());
    registry_->SetPeriodicMetricReaderBuilder(
        std::make_unique<config_test::SyncPeriodicMetricReaderBuilder>());
  }

  void TearDown() override
  {
    if (sdk_)
      sdk_->UnInstall();
  }

  static std::unique_ptr<config_sdk::Configuration> MakeMeterProviderConfig(
      std::unique_ptr<config_sdk::PushMetricExporterConfiguration> exporter)
  {
    auto reader      = std::make_unique<config_sdk::PeriodicMetricReaderConfiguration>();
    reader->exporter = std::move(exporter);
    auto mp          = std::make_unique<config_sdk::MeterProviderConfiguration>();
    mp->readers.emplace_back(std::move(reader));
    auto model            = std::make_unique<config_sdk::Configuration>();
    model->meter_provider = std::move(mp);
    return model;
  }

  static std::unique_ptr<config_sdk::Configuration> MakeMeterProviderConfig(
      std::unique_ptr<config_sdk::PullMetricExporterConfiguration> exporter)
  {
    auto reader      = std::make_unique<config_sdk::PullMetricReaderConfiguration>();
    reader->exporter = std::move(exporter);
    auto mp          = std::make_unique<config_sdk::MeterProviderConfiguration>();
    mp->readers.emplace_back(std::move(reader));
    auto model            = std::make_unique<config_sdk::Configuration>();
    model->meter_provider = std::move(mp);
    return model;
  }

  void BuildAndInstall(const std::unique_ptr<config_sdk::Configuration> &model)
  {
    sdk_ = config_sdk::ConfiguredSdk::Create(registry_, model);
    ASSERT_NE(sdk_, nullptr);
    sdk_->Install();
  }

  void EmitAndVerify()
  {
    ASSERT_NE(sdk_, nullptr);
    ASSERT_NE(sdk_->meter_provider, nullptr);
    ASSERT_EQ(sdk_->tracer_provider, nullptr);
    ASSERT_EQ(sdk_->logger_provider, nullptr);

    auto provider = metrics::Provider::GetMeterProvider();
    ASSERT_NE(provider, nullptr);
    provider->GetMeter("meter")->CreateUInt64Counter("counter")->Add(1);
    ASSERT_TRUE(sdk_->meter_provider->ForceFlush(std::chrono::milliseconds(5000)));
    ASSERT_TRUE(sdk_->meter_provider->Shutdown(std::chrono::milliseconds(5000)));
    EXPECT_FALSE(metric_buffer_->empty());
  }

  std::shared_ptr<config_sdk::Registry> registry_;
  std::shared_ptr<config_test::MetricBuffer> metric_buffer_;
  std::unique_ptr<config_sdk::ConfiguredSdk> sdk_;
};

}  // namespace

TEST_F(MeterProviderConfigTest, MeterProviderWithExtensionPushMetricExporter)
{
  registry_->SetExtensionPushMetricExporterBuilder(
      "recording",
      std::make_unique<config_test::RecordingPushMetricExporterBuilder>(metric_buffer_));

  auto exporter  = std::make_unique<config_sdk::ExtensionPushMetricExporterConfiguration>();
  exporter->name = "recording";
  auto model     = MakeMeterProviderConfig(std::move(exporter));
  BuildAndInstall(model);
  EmitAndVerify();
}

TEST_F(MeterProviderConfigTest, MeterProviderWithConsolePushMetricExporter)
{
  registry_->SetConsolePushMetricExporterBuilder(
      std::make_unique<config_test::RecordingConsolePushMetricExporterBuilder>(metric_buffer_));
  auto model = MakeMeterProviderConfig(
      std::make_unique<config_sdk::ConsolePushMetricExporterConfiguration>());
  BuildAndInstall(model);
  EmitAndVerify();
}

TEST_F(MeterProviderConfigTest, MeterProviderWithOtlpHttpPushMetricExporter)
{
  registry_->SetOtlpHttpPushMetricExporterBuilder(
      std::make_unique<config_test::RecordingOtlpHttpPushMetricExporterBuilder>(metric_buffer_));
  auto model = MakeMeterProviderConfig(
      std::make_unique<config_sdk::OtlpHttpPushMetricExporterConfiguration>());
  BuildAndInstall(model);
  EmitAndVerify();
}

TEST_F(MeterProviderConfigTest, MeterProviderWithOtlpGrpcPushMetricExporter)
{
  registry_->SetOtlpGrpcPushMetricExporterBuilder(
      std::make_unique<config_test::RecordingOtlpGrpcPushMetricExporterBuilder>(metric_buffer_));
  auto model = MakeMeterProviderConfig(
      std::make_unique<config_sdk::OtlpGrpcPushMetricExporterConfiguration>());
  BuildAndInstall(model);
  EmitAndVerify();
}

TEST_F(MeterProviderConfigTest, MeterProviderWithOtlpFilePushMetricExporter)
{
  registry_->SetOtlpFilePushMetricExporterBuilder(
      std::make_unique<config_test::RecordingOtlpFilePushMetricExporterBuilder>(metric_buffer_));
  auto model = MakeMeterProviderConfig(
      std::make_unique<config_sdk::OtlpFilePushMetricExporterConfiguration>());
  BuildAndInstall(model);
  EmitAndVerify();
}

TEST_F(MeterProviderConfigTest, MeterProviderWithPrometheusPullMetricExporter)
{
  registry_->SetPrometheusPullMetricExporterBuilder(
      std::make_unique<config_test::RecordingPrometheusPullMetricExporterBuilder>(metric_buffer_));
  auto model = MakeMeterProviderConfig(
      std::make_unique<config_sdk::PrometheusPullMetricExporterConfiguration>());
  BuildAndInstall(model);
  EmitAndVerify();
}
