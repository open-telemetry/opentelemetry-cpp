// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>

#include <cstddef>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "config_test_common.h"
#include "opentelemetry/logs/severity.h"
#include "opentelemetry/nostd/string_view.h"

#include "opentelemetry/sdk/configuration/always_off_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/always_on_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/extension_push_metric_exporter_builder.h"
#include "opentelemetry/sdk/configuration/extension_push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/instrument_type.h"
#include "opentelemetry/sdk/configuration/logger_config_configuration.h"
#include "opentelemetry/sdk/configuration/logger_configurator_configuration.h"
#include "opentelemetry/sdk/configuration/logger_matcher_and_config_configuration.h"
#include "opentelemetry/sdk/configuration/parent_based_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/periodic_metric_reader_builder.h"
#include "opentelemetry/sdk/configuration/periodic_metric_reader_configuration.h"
#include "opentelemetry/sdk/configuration/push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/registry.h"
#include "opentelemetry/sdk/configuration/sampler_configuration.h"
#include "opentelemetry/sdk/configuration/sdk_builder.h"
#include "opentelemetry/sdk/configuration/severity_number.h"
#include "opentelemetry/sdk/configuration/span_limits_configuration.h"
#include "opentelemetry/sdk/configuration/trace_id_ratio_based_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/tracer_provider_configuration.h"
#include "opentelemetry/sdk/configuration/view_configuration.h"
#include "opentelemetry/sdk/configuration/view_selector_configuration.h"
#include "opentelemetry/sdk/configuration/view_stream_configuration.h"

#include "opentelemetry/nostd/function_ref.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/sdk/instrumentationscope/instrumentation_scope.h"
#include "opentelemetry/sdk/instrumentationscope/scope_configurator.h"
#include "opentelemetry/sdk/logs/logger_config.h"
#include "opentelemetry/sdk/metrics/aggregation/aggregation.h"
#include "opentelemetry/sdk/metrics/aggregation/aggregation_config.h"
#include "opentelemetry/sdk/metrics/aggregation/default_aggregation.h"
#include "opentelemetry/sdk/metrics/data/point_data.h"
#include "opentelemetry/sdk/metrics/instruments.h"
#include "opentelemetry/sdk/metrics/metric_reader.h"
#include "opentelemetry/sdk/metrics/view/view.h"
#include "opentelemetry/sdk/metrics/view/view_registry.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/sdk/trace/sampler.h"
#include "opentelemetry/sdk/trace/span_limits.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"

using opentelemetry::sdk::configuration::Registry;
using opentelemetry::sdk::configuration::SdkBuilder;
using opentelemetry::sdk::configuration::SpanLimitsConfiguration;
using opentelemetry::sdk::configuration::TracerProviderConfiguration;

namespace logs       = opentelemetry::logs;
namespace logs_sdk   = opentelemetry::sdk::logs;
namespace scope_sdk  = opentelemetry::sdk::instrumentationscope;
namespace config_sdk = opentelemetry::sdk::configuration;

//------------------------------------------------------------------------------
// Tests for the SdkBuilder class methods that create SDK components from configuration models
// These tests focus on the API of the SdkBuilder for creating SDK components that can be
// independently verified. For full integration tests of the SdkBuilder with configuration models,
// see the programmatic_configuration_test.cc file.

TEST(SdkBuilder, SpanLimitsDefaults)
{
  auto model    = std::make_unique<TracerProviderConfiguration>();
  model->limits = nullptr;

  SdkBuilder builder(std::make_shared<Registry>());
  auto resource = opentelemetry::sdk::resource::Resource::Create({});
  auto provider = builder.CreateTracerProvider(model, resource);
  ASSERT_NE(provider, nullptr);

  const auto limits         = provider->GetSpanLimits();
  const auto default_limits = opentelemetry::sdk::trace::SpanLimits{};

  EXPECT_EQ(limits.attribute_count_limit, default_limits.attribute_count_limit);
  EXPECT_EQ(limits.event_count_limit, default_limits.event_count_limit);
  EXPECT_EQ(limits.link_count_limit, default_limits.link_count_limit);
  EXPECT_EQ(limits.event_attribute_count_limit, default_limits.event_attribute_count_limit);
  EXPECT_EQ(limits.link_attribute_count_limit, default_limits.link_attribute_count_limit);
  EXPECT_EQ(limits.attribute_value_length_limit, default_limits.attribute_value_length_limit);
}

TEST(SdkBuilder, SpanLimitsConfiguration)
{
  auto model                                  = std::make_unique<TracerProviderConfiguration>();
  model->limits                               = std::make_unique<SpanLimitsConfiguration>();
  model->limits->attribute_value_length_limit = 1111;
  model->limits->attribute_count_limit        = 2222;
  model->limits->event_count_limit            = 3333;
  model->limits->link_count_limit             = 4444;
  model->limits->event_attribute_count_limit  = 5555;
  model->limits->link_attribute_count_limit   = 6666;

  SdkBuilder builder(std::make_shared<Registry>());
  auto resource = opentelemetry::sdk::resource::Resource::Create({});
  auto provider = builder.CreateTracerProvider(model, resource);
  ASSERT_NE(provider, nullptr);

  auto limits = provider->GetSpanLimits();
  EXPECT_EQ(limits.attribute_value_length_limit, model->limits->attribute_value_length_limit);
  EXPECT_EQ(limits.attribute_count_limit, model->limits->attribute_count_limit);
  EXPECT_EQ(limits.event_count_limit, model->limits->event_count_limit);
  EXPECT_EQ(limits.link_count_limit, model->limits->link_count_limit);
  EXPECT_EQ(limits.event_attribute_count_limit, model->limits->event_attribute_count_limit);
  EXPECT_EQ(limits.link_attribute_count_limit, model->limits->link_attribute_count_limit);
}

TEST(SdkBuilder, CreateLoggerConfigurator)
{
  config_sdk::LoggerConfigConfiguration default_config;
  default_config.enabled          = true;
  default_config.minimum_severity = config_sdk::SeverityNumber::warn;
  default_config.trace_based      = false;

  config_sdk::LoggerMatcherAndConfigConfiguration matcher1;
  matcher1.name                    = "enabled_minsev_error_not_trace_based";
  matcher1.config.enabled          = true;
  matcher1.config.minimum_severity = config_sdk::SeverityNumber::error3;
  matcher1.config.trace_based      = false;

  config_sdk::LoggerMatcherAndConfigConfiguration matcher2;
  matcher2.name                    = "disabled_minsev_info_trace_based";
  matcher2.config.enabled          = false;
  matcher2.config.minimum_severity = config_sdk::SeverityNumber::debug;
  matcher2.config.trace_based      = true;

  auto model            = std::make_unique<config_sdk::LoggerConfiguratorConfiguration>();
  model->default_config = default_config;
  model->loggers.push_back(matcher1);
  model->loggers.push_back(matcher2);

  config_sdk::SdkBuilder builder(std::make_shared<config_sdk::Registry>());

  auto logger_configurator = builder.CreateLoggerConfigurator(model);
  ASSERT_NE(logger_configurator, nullptr);

  auto default_scope = scope_sdk::InstrumentationScope::Create("default_scope");
  logs_sdk::LoggerConfig sdk_logger_config_default =
      logger_configurator->ComputeConfig(*default_scope);

  auto scope_1 = scope_sdk::InstrumentationScope::Create(matcher1.name);
  logs_sdk::LoggerConfig sdk_logger_config_1 = logger_configurator->ComputeConfig(*scope_1);

  auto scope_2 = scope_sdk::InstrumentationScope::Create(matcher2.name);
  logs_sdk::LoggerConfig sdk_logger_config_2 = logger_configurator->ComputeConfig(*scope_2);

  EXPECT_TRUE(sdk_logger_config_default.IsEnabled());
  EXPECT_EQ(sdk_logger_config_default.GetMinimumSeverity(), logs::Severity::kWarn);
  EXPECT_FALSE(sdk_logger_config_default.IsTraceBased());

  EXPECT_TRUE(sdk_logger_config_1.IsEnabled());
  EXPECT_EQ(sdk_logger_config_1.GetMinimumSeverity(), logs::Severity::kError3);
  EXPECT_FALSE(sdk_logger_config_1.IsTraceBased());

  EXPECT_FALSE(sdk_logger_config_2.IsEnabled());
  EXPECT_EQ(sdk_logger_config_2.GetMinimumSeverity(), logs::Severity::kDebug);
  EXPECT_TRUE(sdk_logger_config_2.IsTraceBased());
}

TEST(SdkBuilder, CreateParentBasedSampler)
{
  // parent based with no root configured should default to always on
  {
    config_sdk::ParentBasedSamplerConfiguration parent_based_sampler_config;
    parent_based_sampler_config.root = nullptr;
    config_sdk::SdkBuilder builder(std::make_shared<config_sdk::Registry>());
    auto sampler = builder.CreateParentBasedSampler(&parent_based_sampler_config);
    ASSERT_NE(sampler, nullptr);
    EXPECT_EQ(std::string{sampler->GetDescription()}, R"(ParentBased{AlwaysOnSampler})");
  }

  // parent based with root always on
  {
    config_sdk::ParentBasedSamplerConfiguration parent_based_sampler_config;
    parent_based_sampler_config.root = std::make_unique<config_sdk::AlwaysOnSamplerConfiguration>();
    config_sdk::SdkBuilder builder(std::make_shared<config_sdk::Registry>());
    auto sampler = builder.CreateParentBasedSampler(&parent_based_sampler_config);
    ASSERT_NE(sampler, nullptr);
    EXPECT_EQ(std::string{sampler->GetDescription()}, R"(ParentBased{AlwaysOnSampler})");
  }

  // parent based with root always off
  {
    config_sdk::ParentBasedSamplerConfiguration parent_based_sampler_config;
    parent_based_sampler_config.root =
        std::make_unique<config_sdk::AlwaysOffSamplerConfiguration>();
    config_sdk::SdkBuilder builder(std::make_shared<config_sdk::Registry>());
    auto sampler = builder.CreateParentBasedSampler(&parent_based_sampler_config);
    ASSERT_NE(sampler, nullptr);
    EXPECT_EQ(std::string{sampler->GetDescription()}, R"(ParentBased{AlwaysOffSampler})");
  }

  // parent based with a custom root sampler
  {
    config_sdk::ParentBasedSamplerConfiguration parent_based_sampler_config;
    auto trace_id_ratio_based_sampler_config =
        std::make_unique<config_sdk::TraceIdRatioBasedSamplerConfiguration>();
    trace_id_ratio_based_sampler_config->ratio = 0.5;
    parent_based_sampler_config.root           = std::move(trace_id_ratio_based_sampler_config);
    config_sdk::SdkBuilder builder(std::make_shared<config_sdk::Registry>());
    auto sampler = builder.CreateParentBasedSampler(&parent_based_sampler_config);
    ASSERT_NE(sampler, nullptr);
    EXPECT_EQ(std::string{sampler->GetDescription()},
              R"(ParentBased{TraceIdRatioBasedSampler{0.500000}})");
  }

  // parent based with all sub samplers set
  {
    config_sdk::ParentBasedSamplerConfiguration parent_based_sampler_config;
    auto trace_id_ratio_based_sampler_config =
        std::make_unique<config_sdk::TraceIdRatioBasedSamplerConfiguration>();
    trace_id_ratio_based_sampler_config->ratio = 0.25;
    parent_based_sampler_config.root           = std::move(trace_id_ratio_based_sampler_config);

    auto always_off_sampler_config = std::make_unique<config_sdk::AlwaysOffSamplerConfiguration>();
    parent_based_sampler_config.remote_parent_sampled = std::move(always_off_sampler_config);

    auto always_on_sampler_config = std::make_unique<config_sdk::AlwaysOnSamplerConfiguration>();
    parent_based_sampler_config.remote_parent_not_sampled = std::move(always_on_sampler_config);

    auto trace_id_ratio_based_sampler_config_2 =
        std::make_unique<config_sdk::TraceIdRatioBasedSamplerConfiguration>();
    trace_id_ratio_based_sampler_config_2->ratio = 0.35;
    parent_based_sampler_config.local_parent_sampled =
        std::move(trace_id_ratio_based_sampler_config_2);

    auto always_off_sampler_config_2 =
        std::make_unique<config_sdk::AlwaysOffSamplerConfiguration>();
    parent_based_sampler_config.local_parent_not_sampled = std::move(always_off_sampler_config_2);

    config_sdk::SdkBuilder builder(std::make_shared<config_sdk::Registry>());
    auto sampler = builder.CreateParentBasedSampler(&parent_based_sampler_config);
    ASSERT_NE(sampler, nullptr);
    EXPECT_EQ(std::string{sampler->GetDescription()},
              R"(ParentBased{TraceIdRatioBasedSampler{0.250000}})");
  }
}

TEST(SdkBuilder, CreatePeriodicMetricReader)
{
  auto exporter  = std::make_unique<config_sdk::ExtensionPushMetricExporterConfiguration>();
  exporter->name = "noop";

  config_sdk::PeriodicMetricReaderConfiguration model;
  model.exporter = std::move(exporter);
  model.interval = 12345;
  model.timeout  = 678;

  auto captured = std::make_shared<config_test::CapturedPeriodicReaderArgs>();

  auto registry = std::make_shared<config_sdk::Registry>();
  registry->SetExtensionPushMetricExporterBuilder(
      "noop", std::make_unique<config_test::NoopPushMetricExporterBuilder>());
  registry->SetPeriodicMetricReaderBuilder(
      std::make_unique<config_test::CapturingPeriodicMetricReaderBuilder>(captured));

  config_sdk::SdkBuilder builder(registry);
  auto reader = builder.CreatePeriodicMetricReader(&model);
  ASSERT_NE(reader, nullptr);

  EXPECT_TRUE(captured->called);
  EXPECT_EQ(captured->interval, model.interval);
  EXPECT_EQ(captured->timeout, model.timeout);
  EXPECT_TRUE(captured->exporter != nullptr);
}

namespace
{

// Builds a ViewConfiguration selecting the given instrument type, with only
// aggregation_cardinality_limit set on the stream (no explicit `aggregation` block).
std::unique_ptr<config_sdk::ViewConfiguration> MakeCardinalityOnlyViewConfig(
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

}  // namespace

TEST(SdkBuilder, AddViewHistogramCardinalityLimitOnly)
{
  namespace metrics_sdk = opentelemetry::sdk::metrics;

  auto model = MakeCardinalityOnlyViewConfig(config_sdk::InstrumentType::histogram, 42);

  auto registry = std::make_shared<config_sdk::Registry>();
  config_sdk::SdkBuilder builder(registry);

  metrics_sdk::ViewRegistry view_registry;
  builder.AddView(&view_registry, model);

  metrics_sdk::InstrumentDescriptor instrument_descriptor{
      "", "", "", metrics_sdk::InstrumentType::kHistogram, metrics_sdk::InstrumentValueType::kLong};
  auto instrumentation_scope = scope_sdk::InstrumentationScope::Create("");

  int matched = 0;
  view_registry.FindViews(
      instrument_descriptor, *instrumentation_scope, [&](const metrics_sdk::View &view) {
        matched++;
        // The view must not be rejected: it should carry a
        // HistogramAggregationConfig (not a plain AggregationConfig), since
        // the instrument's default aggregation for kHistogram is kHistogram.
        auto *aggregation_config = view.GetAggregationConfig();
        EXPECT_NE(aggregation_config, nullptr);
        if (aggregation_config)
        {
          EXPECT_EQ(aggregation_config->GetType(), metrics_sdk::AggregationType::kHistogram);
          EXPECT_EQ(aggregation_config->cardinality_limit_, 42u);

          // Pin what users actually receive: building the aggregation from this config
          // must keep the SDK's default bucket boundaries, not silently collapse to a
          // single bucket. A default-constructed HistogramAggregationConfig has empty
          // boundaries_, which the aggregation takes literally (as opposed to a null
          // config pointer, which falls back to the default boundaries), so AddView()
          // must populate boundaries_ explicitly.
          auto aggregation = metrics_sdk::DefaultAggregation::CreateAggregation(
              metrics_sdk::AggregationType::kHistogram, instrument_descriptor, aggregation_config);
          EXPECT_NE(aggregation, nullptr);
          if (aggregation)
          {
            auto histogram_data =
                opentelemetry::nostd::get<metrics_sdk::HistogramPointData>(aggregation->ToPoint());
            EXPECT_EQ(histogram_data.boundaries_.size(), 15u);
            EXPECT_EQ(histogram_data.counts_.size(), 16u);
          }
        }
        return true;
      });

  EXPECT_EQ(matched, 1);
}

TEST(SdkBuilder, AddViewCounterCardinalityLimitOnly)
{
  namespace metrics_sdk = opentelemetry::sdk::metrics;

  auto model = MakeCardinalityOnlyViewConfig(config_sdk::InstrumentType::counter, 7);

  auto registry = std::make_shared<config_sdk::Registry>();
  config_sdk::SdkBuilder builder(registry);

  metrics_sdk::ViewRegistry view_registry;
  builder.AddView(&view_registry, model);

  metrics_sdk::InstrumentDescriptor instrument_descriptor{
      "", "", "", metrics_sdk::InstrumentType::kCounter, metrics_sdk::InstrumentValueType::kLong};
  auto instrumentation_scope = scope_sdk::InstrumentationScope::Create("");

  int matched = 0;
  view_registry.FindViews(
      instrument_descriptor, *instrumentation_scope, [&](const metrics_sdk::View &view) {
        matched++;
        auto *aggregation_config = view.GetAggregationConfig();
        EXPECT_NE(aggregation_config, nullptr);
        if (aggregation_config)
        {
          EXPECT_EQ(aggregation_config->GetType(), metrics_sdk::AggregationType::kDefault);
          EXPECT_EQ(aggregation_config->cardinality_limit_, 7u);
        }
        return true;
      });

  EXPECT_EQ(matched, 1);
}
