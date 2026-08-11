// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>

#if defined(ENABLE_METRICS_EXEMPLAR_PREVIEW) && !defined(NO_GETENV)
#  include <cstdlib>
#else
#  include <cstddef>
#endif

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "config_test_common.h"
#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/common/key_value_iterable_view.h"
#include "opentelemetry/logs/severity.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/utility.h"

#include "opentelemetry/sdk/configuration/aggregation_configuration.h"
#include "opentelemetry/sdk/configuration/always_off_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/always_on_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/cardinality_limits_configuration.h"
#include "opentelemetry/sdk/configuration/composable_always_off_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/composable_always_on_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/composable_parent_threshold_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/composable_probability_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/composable_rule_based_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/composable_rule_based_sampler_rule_attribute_patterns_configuration.h"
#include "opentelemetry/sdk/configuration/composable_rule_based_sampler_rule_attribute_values_configuration.h"
#include "opentelemetry/sdk/configuration/composable_rule_based_sampler_rule_configuration.h"
#include "opentelemetry/sdk/configuration/composable_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/composite_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/explicit_bucket_histogram_aggregation_configuration.h"
#include "opentelemetry/sdk/configuration/extension_push_metric_exporter_builder.h"
#include "opentelemetry/sdk/configuration/extension_push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/include_exclude_configuration.h"
#include "opentelemetry/sdk/configuration/instrument_type.h"
#include "opentelemetry/sdk/configuration/logger_config_configuration.h"
#include "opentelemetry/sdk/configuration/logger_configurator_configuration.h"
#include "opentelemetry/sdk/configuration/logger_matcher_and_config_configuration.h"
#include "opentelemetry/sdk/configuration/parent_based_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/periodic_metric_reader_builder.h"
#include "opentelemetry/sdk/configuration/periodic_metric_reader_configuration.h"
#include "opentelemetry/sdk/configuration/probability_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/registry.h"
#include "opentelemetry/sdk/configuration/registry_factory.h"
#include "opentelemetry/sdk/configuration/sampler_configuration.h"
#include "opentelemetry/sdk/configuration/sdk_builder.h"
#include "opentelemetry/sdk/configuration/severity_number.h"
#include "opentelemetry/sdk/configuration/span_limits_configuration.h"
#include "opentelemetry/sdk/configuration/string_array_configuration.h"
#include "opentelemetry/sdk/configuration/trace_id_ratio_based_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/tracer_provider_configuration.h"
#include "opentelemetry/sdk/configuration/unsupported_exception.h"
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
#include "opentelemetry/sdk/metrics/view/attributes_processor.h"
#include "opentelemetry/sdk/metrics/view/view.h"
#include "opentelemetry/sdk/metrics/view/view_registry.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/sdk/trace/sampler.h"
#include "opentelemetry/sdk/trace/span_limits.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/trace/span_context.h"
#include "opentelemetry/trace/span_context_kv_iterable_view.h"
#include "opentelemetry/trace/span_id.h"
#include "opentelemetry/trace/span_metadata.h"
#include "opentelemetry/trace/trace_flags.h"
#include "opentelemetry/trace/trace_id.h"

#if defined(ENABLE_METRICS_EXEMPLAR_PREVIEW) && !defined(NO_GETENV)
#  include "opentelemetry/sdk/common/global_log_handler.h"
#  include "opentelemetry/sdk/configuration/exemplar_filter.h"
#  include "opentelemetry/sdk/configuration/meter_provider_configuration.h"
#  include "opentelemetry/sdk/metrics/meter_provider.h"
#  include "opentelemetry/test_common/sdk/common/scoped_test_log_handler.h"

#  if defined(_MSC_VER)
#    include "opentelemetry/sdk/common/env_variables.h"
using opentelemetry::sdk::common::setenv;
using opentelemetry::sdk::common::unsetenv;
#  endif
#endif

using opentelemetry::sdk::configuration::Registry;
using opentelemetry::sdk::configuration::RegistryFactory;
using opentelemetry::sdk::configuration::SdkBuilder;
using opentelemetry::sdk::configuration::SpanLimitsConfiguration;
using opentelemetry::sdk::configuration::TracerProviderConfiguration;

namespace logs       = opentelemetry::logs;
namespace logs_sdk   = opentelemetry::sdk::logs;
namespace scope_sdk  = opentelemetry::sdk::instrumentationscope;
namespace config_sdk = opentelemetry::sdk::configuration;
namespace trace_api  = opentelemetry::trace;

namespace
{

using RuleAttrMap = std::map<std::string, opentelemetry::common::AttributeValue>;

opentelemetry::sdk::trace::Decision SampleWith(opentelemetry::sdk::trace::Sampler &sampler,
                                               const trace_api::SpanContext &parent,
                                               trace_api::SpanKind span_kind,
                                               const RuleAttrMap &attrs)
{
  uint8_t trace_buf[trace_api::TraceId::kSize] = {1};
  std::vector<std::pair<trace_api::SpanContext, std::map<std::string, std::string>>> links;
  opentelemetry::common::KeyValueIterableView<RuleAttrMap> attrs_view{attrs};
  trace_api::SpanContextKeyValueIterableView<decltype(links)> links_view{links};
  auto result = sampler.ShouldSample(parent, trace_api::TraceId(trace_buf), "span", span_kind,
                                     attrs_view, links_view);
  return result.decision;
}

trace_api::SpanContext MakeRuleParent(bool sampled, bool is_remote)
{
  uint8_t trace_buf[trace_api::TraceId::kSize] = {1};
  uint8_t span_buf[trace_api::SpanId::kSize]   = {1};
  return trace_api::SpanContext(trace_api::TraceId(trace_buf), trace_api::SpanId(span_buf),
                                trace_api::TraceFlags(sampled ? 1 : 0), is_remote);
}

// Builds composite(rule_based{[rule]}) where the rule maps to always_on.
std::unique_ptr<opentelemetry::sdk::trace::Sampler> BuildRuleSampler(
    std::unique_ptr<config_sdk::ComposableRuleBasedSamplerRuleConfiguration> rule)
{
  rule->sampler          = std::make_unique<config_sdk::ComposableAlwaysOnSamplerConfiguration>();
  auto rule_based_config = std::make_unique<config_sdk::ComposableRuleBasedSamplerConfiguration>();
  rule_based_config->rules.push_back(std::move(rule));
  auto composite                = std::make_unique<config_sdk::CompositeSamplerConfiguration>();
  composite->composable_sampler = std::move(rule_based_config);
  std::unique_ptr<config_sdk::SamplerConfiguration> sampler_config = std::move(composite);
  config_sdk::SdkBuilder builder(config_sdk::RegistryFactory::Create());
  return builder.CreateSampler(sampler_config);
}

}  // namespace

#if defined(ENABLE_METRICS_EXEMPLAR_PREVIEW) && !defined(NO_GETENV)
namespace
{

constexpr char kMetricsExemplarFilterEnv[] = "OTEL_METRICS_EXEMPLAR_FILTER";

class SdkBuilderExemplarFilterEnvironmentTest : public ::testing::Test
{
protected:
  void SetUp() override { unsetenv(kMetricsExemplarFilterEnv); }

  void TearDown() override { unsetenv(kMetricsExemplarFilterEnv); }
};

}  // namespace
#endif

//------------------------------------------------------------------------------
// Tests for the SdkBuilder class methods that create SDK components from configuration models
// These tests focus on the API of the SdkBuilder for creating SDK components that can be
// independently verified. For full integration tests of the SdkBuilder with configuration models,
// see the programmatic_configuration_test.cc file.

TEST(SdkBuilder, SpanLimitsDefaults)
{
  auto model    = std::make_unique<TracerProviderConfiguration>();
  model->limits = nullptr;

  SdkBuilder builder(RegistryFactory::Create());
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

  SdkBuilder builder(RegistryFactory::Create());
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

#if defined(ENABLE_METRICS_EXEMPLAR_PREVIEW) && !defined(NO_GETENV)
TEST_F(SdkBuilderExemplarFilterEnvironmentTest, DeclarativeExemplarFilterDoesNotReadEnvironment)
{
  opentelemetry::test_common::ScopedTestLogHandler log_handler{
      opentelemetry::sdk::common::internal_log::LogLevel::Warning};
  setenv(kMetricsExemplarFilterEnv, "invalid", 1);

  auto model             = std::make_unique<config_sdk::MeterProviderConfiguration>();
  model->exemplar_filter = config_sdk::ExemplarFilter::always_on;

  SdkBuilder builder(RegistryFactory::Create());
  auto resource = opentelemetry::sdk::resource::Resource::Create({});
  auto provider = builder.CreateMeterProvider(model, resource);
  ASSERT_NE(provider, nullptr);

  EXPECT_TRUE(log_handler.Drain().empty());
}
#endif

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

  config_sdk::SdkBuilder builder(config_sdk::RegistryFactory::Create());

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
    config_sdk::SdkBuilder builder(config_sdk::RegistryFactory::Create());
    auto sampler = builder.CreateParentBasedSampler(&parent_based_sampler_config);
    ASSERT_NE(sampler, nullptr);
    EXPECT_EQ(std::string{sampler->GetDescription()}, R"(ParentBased{AlwaysOnSampler})");
  }

  // parent based with root always on
  {
    config_sdk::ParentBasedSamplerConfiguration parent_based_sampler_config;
    parent_based_sampler_config.root = std::make_unique<config_sdk::AlwaysOnSamplerConfiguration>();
    config_sdk::SdkBuilder builder(config_sdk::RegistryFactory::Create());
    auto sampler = builder.CreateParentBasedSampler(&parent_based_sampler_config);
    ASSERT_NE(sampler, nullptr);
    EXPECT_EQ(std::string{sampler->GetDescription()}, R"(ParentBased{AlwaysOnSampler})");
  }

  // parent based with root always off
  {
    config_sdk::ParentBasedSamplerConfiguration parent_based_sampler_config;
    parent_based_sampler_config.root =
        std::make_unique<config_sdk::AlwaysOffSamplerConfiguration>();
    config_sdk::SdkBuilder builder(config_sdk::RegistryFactory::Create());
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
    config_sdk::SdkBuilder builder(config_sdk::RegistryFactory::Create());
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

    config_sdk::SdkBuilder builder(config_sdk::RegistryFactory::Create());
    auto sampler = builder.CreateParentBasedSampler(&parent_based_sampler_config);
    ASSERT_NE(sampler, nullptr);
    EXPECT_EQ(std::string{sampler->GetDescription()},
              R"(ParentBased{TraceIdRatioBasedSampler{0.250000}})");
  }
}

TEST(SdkBuilder, CreateProbabilitySampler)
{
  // default ratio is 1.0
  {
    config_sdk::ProbabilitySamplerConfiguration probability_sampler_config;
    config_sdk::SdkBuilder builder(config_sdk::RegistryFactory::Create());
    auto sampler = builder.CreateProbabilitySampler(&probability_sampler_config);
    ASSERT_NE(sampler, nullptr);
    EXPECT_EQ(std::string{sampler->GetDescription()}, R"(ProbabilitySampler{1.000000})");
  }

  // explicit ratio, dispatched through CreateSampler
  {
    auto probability_sampler_config =
        std::make_unique<config_sdk::ProbabilitySamplerConfiguration>();
    probability_sampler_config->ratio = 0.5;
    std::unique_ptr<config_sdk::SamplerConfiguration> sampler_config =
        std::move(probability_sampler_config);
    config_sdk::SdkBuilder builder(config_sdk::RegistryFactory::Create());
    auto sampler = builder.CreateSampler(sampler_config);
    ASSERT_NE(sampler, nullptr);
    EXPECT_EQ(std::string{sampler->GetDescription()}, R"(ProbabilitySampler{0.500000})");
  }
}

TEST(SdkBuilder, CreateComposableAlwaysOnSampler)
{
  auto composable_config = std::make_unique<config_sdk::ComposableAlwaysOnSamplerConfiguration>();
  auto composite         = std::make_unique<config_sdk::CompositeSamplerConfiguration>();
  composite->composable_sampler                                    = std::move(composable_config);
  std::unique_ptr<config_sdk::SamplerConfiguration> sampler_config = std::move(composite);
  config_sdk::SdkBuilder builder(config_sdk::RegistryFactory::Create());
  auto sampler = builder.CreateSampler(sampler_config);
  ASSERT_NE(sampler, nullptr);
  EXPECT_EQ(std::string{sampler->GetDescription()},
            R"(CompositeSampler{ComposableAlwaysOnSampler})");
}

TEST(SdkBuilder, CreateComposableAlwaysOffSampler)
{
  auto composable_config = std::make_unique<config_sdk::ComposableAlwaysOffSamplerConfiguration>();
  auto composite         = std::make_unique<config_sdk::CompositeSamplerConfiguration>();
  composite->composable_sampler                                    = std::move(composable_config);
  std::unique_ptr<config_sdk::SamplerConfiguration> sampler_config = std::move(composite);
  config_sdk::SdkBuilder builder(config_sdk::RegistryFactory::Create());
  auto sampler = builder.CreateSampler(sampler_config);
  ASSERT_NE(sampler, nullptr);
  EXPECT_EQ(std::string{sampler->GetDescription()},
            R"(CompositeSampler{ComposableAlwaysOffSampler})");
}

TEST(SdkBuilder, CreateComposableProbabilitySampler)
{
  auto composable_probability_sampler_config =
      std::make_unique<config_sdk::ComposableProbabilitySamplerConfiguration>();
  composable_probability_sampler_config->ratio = 0.25;
  auto composite                = std::make_unique<config_sdk::CompositeSamplerConfiguration>();
  composite->composable_sampler = std::move(composable_probability_sampler_config);
  std::unique_ptr<config_sdk::SamplerConfiguration> sampler_config = std::move(composite);
  config_sdk::SdkBuilder builder(config_sdk::RegistryFactory::Create());
  auto sampler = builder.CreateSampler(sampler_config);
  ASSERT_NE(sampler, nullptr);
  EXPECT_EQ(std::string{sampler->GetDescription()},
            R"(CompositeSampler{ComposableProbabilitySampler{0.250000}})");
}

TEST(SdkBuilder, CreateComposableParentThresholdSampler)
{
  auto root_config   = std::make_unique<config_sdk::ComposableProbabilitySamplerConfiguration>();
  root_config->ratio = 0.25;
  auto parent_config =
      std::make_unique<config_sdk::ComposableParentThresholdSamplerConfiguration>();
  parent_config->root           = std::move(root_config);
  auto composite                = std::make_unique<config_sdk::CompositeSamplerConfiguration>();
  composite->composable_sampler = std::move(parent_config);
  std::unique_ptr<config_sdk::SamplerConfiguration> sampler_config = std::move(composite);
  config_sdk::SdkBuilder builder(config_sdk::RegistryFactory::Create());
  auto sampler = builder.CreateSampler(sampler_config);
  ASSERT_NE(sampler, nullptr);
  EXPECT_EQ(
      std::string{sampler->GetDescription()},
      R"(CompositeSampler{ComposableParentThresholdSampler{ComposableProbabilitySampler{0.250000}}})");
}

TEST(SdkBuilder, CreateComposableParentThresholdSamplerNullRoot)
{
  auto parent_config =
      std::make_unique<config_sdk::ComposableParentThresholdSamplerConfiguration>();
  parent_config->root           = nullptr;
  auto composite                = std::make_unique<config_sdk::CompositeSamplerConfiguration>();
  composite->composable_sampler = std::move(parent_config);
  std::unique_ptr<config_sdk::SamplerConfiguration> sampler_config = std::move(composite);
  config_sdk::SdkBuilder builder(config_sdk::RegistryFactory::Create());
  auto sampler = builder.CreateSampler(sampler_config);
  ASSERT_NE(sampler, nullptr);
  EXPECT_EQ(std::string{sampler->GetDescription()},
            R"(CompositeSampler{ComposableParentThresholdSampler{ComposableAlwaysOnSampler}})");
}

TEST(SdkBuilder, CreateComposableParentThresholdSamplerNestedDepth3)
{
  auto innermost_config = std::make_unique<config_sdk::ComposableProbabilitySamplerConfiguration>();
  innermost_config->ratio = 0.25;

  auto middle_config =
      std::make_unique<config_sdk::ComposableParentThresholdSamplerConfiguration>();
  middle_config->root = std::move(innermost_config);

  auto outer_config = std::make_unique<config_sdk::ComposableParentThresholdSamplerConfiguration>();
  outer_config->root            = std::move(middle_config);
  auto composite                = std::make_unique<config_sdk::CompositeSamplerConfiguration>();
  composite->composable_sampler = std::move(outer_config);
  std::unique_ptr<config_sdk::SamplerConfiguration> sampler_config = std::move(composite);
  config_sdk::SdkBuilder builder(config_sdk::RegistryFactory::Create());
  auto sampler = builder.CreateSampler(sampler_config);
  ASSERT_NE(sampler, nullptr);
  EXPECT_EQ(std::string{sampler->GetDescription()},
            R"(CompositeSampler{ComposableParentThresholdSampler{ComposableParentThresholdSampler{)"
            R"(ComposableProbabilitySampler{0.250000}}}})");
}

namespace
{

// Builds a chain of parent_threshold nodes ending in an always_on leaf.
// Total nesting depth, root included, is num_parent_nodes + 1.
std::unique_ptr<config_sdk::SamplerConfiguration> MakeNestedComposableConfig(
    std::size_t num_parent_nodes)
{
  std::unique_ptr<config_sdk::ComposableSamplerConfiguration> node =
      std::make_unique<config_sdk::ComposableAlwaysOnSamplerConfiguration>();
  for (std::size_t i = 0; i < num_parent_nodes; ++i)
  {
    auto parent  = std::make_unique<config_sdk::ComposableParentThresholdSamplerConfiguration>();
    parent->root = std::move(node);
    node         = std::move(parent);
  }
  auto composite                = std::make_unique<config_sdk::CompositeSamplerConfiguration>();
  composite->composable_sampler = std::move(node);
  return composite;
}

}  // namespace

TEST(SdkBuilder, CreateComposableSamplerAtMaxDepth)
{
  // 9 parent nodes + leaf = depth 10, the default maximum.
  auto sampler_config = MakeNestedComposableConfig(9);
  config_sdk::SdkBuilder builder(config_sdk::RegistryFactory::Create());
  auto sampler = builder.CreateSampler(sampler_config);
  ASSERT_NE(sampler, nullptr);
}

TEST(SdkBuilder, CreateComposableSamplerBeyondMaxDepth)
{
  // 10 parent nodes + leaf = depth 11, exceeding the default maximum.
  auto sampler_config = MakeNestedComposableConfig(10);
  config_sdk::SdkBuilder builder(config_sdk::RegistryFactory::Create());
  EXPECT_THROW(builder.CreateSampler(sampler_config), config_sdk::UnsupportedException);
}

TEST(SdkBuilder, CreateComposableRuleBasedSampler)
{
  auto rule_based_config = std::make_unique<config_sdk::ComposableRuleBasedSamplerConfiguration>();

  auto rule = std::make_unique<config_sdk::ComposableRuleBasedSamplerRuleConfiguration>();
  auto attribute_values =
      std::make_unique<config_sdk::ComposableRuleBasedSamplerRuleAttributeValuesConfiguration>();
  attribute_values->key    = "http.route";
  attribute_values->values = {"/health"};
  rule->attribute_values   = std::move(attribute_values);
  rule->sampler = std::make_unique<config_sdk::ComposableAlwaysOffSamplerConfiguration>();
  rule_based_config->rules.push_back(std::move(rule));

  auto fallback     = std::make_unique<config_sdk::ComposableRuleBasedSamplerRuleConfiguration>();
  fallback->sampler = std::make_unique<config_sdk::ComposableAlwaysOnSamplerConfiguration>();
  rule_based_config->rules.push_back(std::move(fallback));
  auto composite                = std::make_unique<config_sdk::CompositeSamplerConfiguration>();
  composite->composable_sampler = std::move(rule_based_config);
  std::unique_ptr<config_sdk::SamplerConfiguration> sampler_config = std::move(composite);
  config_sdk::SdkBuilder builder(config_sdk::RegistryFactory::Create());
  auto sampler = builder.CreateSampler(sampler_config);
  ASSERT_NE(sampler, nullptr);
  EXPECT_EQ(
      std::string{sampler->GetDescription()},
      R"(CompositeSampler{ComposableRuleBasedSampler{ComposableAlwaysOffSampler,ComposableAlwaysOnSampler}})");
}

TEST(SdkBuilder, RuleBasedPredicateAttributeValues)
{
  using config_sdk::ComposableRuleBasedSamplerRuleAttributeValuesConfiguration;
  using config_sdk::ComposableRuleBasedSamplerRuleConfiguration;
  using opentelemetry::sdk::trace::Decision;

  auto parent = MakeRuleParent(true, true);

  // string attribute, exact match
  {
    auto rule      = std::make_unique<ComposableRuleBasedSamplerRuleConfiguration>();
    auto values    = std::make_unique<ComposableRuleBasedSamplerRuleAttributeValuesConfiguration>();
    values->key    = "http.route";
    values->values = {"/health", "/metrics"};
    rule->attribute_values = std::move(values);
    auto sampler           = BuildRuleSampler(std::move(rule));
    ASSERT_NE(sampler, nullptr);

    EXPECT_EQ(
        SampleWith(*sampler, parent, trace_api::SpanKind::kServer, {{"http.route", "/health"}}),
        Decision::RECORD_AND_SAMPLE);
    EXPECT_EQ(
        SampleWith(*sampler, parent, trace_api::SpanKind::kServer, {{"http.route", "/users"}}),
        Decision::DROP);
    EXPECT_EQ(SampleWith(*sampler, parent, trace_api::SpanKind::kServer, {}), Decision::DROP);
  }

  // non-string attribute matches via string representation
  {
    auto rule      = std::make_unique<ComposableRuleBasedSamplerRuleConfiguration>();
    auto values    = std::make_unique<ComposableRuleBasedSamplerRuleAttributeValuesConfiguration>();
    values->key    = "http.response.status_code";
    values->values = {"404"};
    rule->attribute_values = std::move(values);
    auto sampler           = BuildRuleSampler(std::move(rule));
    ASSERT_NE(sampler, nullptr);

    EXPECT_EQ(SampleWith(*sampler, parent, trace_api::SpanKind::kServer,
                         {{"http.response.status_code", static_cast<int64_t>(404)}}),
              Decision::RECORD_AND_SAMPLE);
    EXPECT_EQ(SampleWith(*sampler, parent, trace_api::SpanKind::kServer,
                         {{"http.response.status_code", static_cast<int64_t>(200)}}),
              Decision::DROP);
  }

  // array attribute matches if any element matches
  {
    auto rule      = std::make_unique<ComposableRuleBasedSamplerRuleConfiguration>();
    auto values    = std::make_unique<ComposableRuleBasedSamplerRuleAttributeValuesConfiguration>();
    values->key    = "tags";
    values->values = {"b"};
    rule->attribute_values = std::move(values);
    auto sampler           = BuildRuleSampler(std::move(rule));
    ASSERT_NE(sampler, nullptr);

    std::vector<opentelemetry::nostd::string_view> matching_tags{"a", "b"};
    EXPECT_EQ(
        SampleWith(*sampler, parent, trace_api::SpanKind::kServer,
                   {{"tags", opentelemetry::nostd::span<const opentelemetry::nostd::string_view>(
                                 matching_tags.data(), matching_tags.size())}}),
        Decision::RECORD_AND_SAMPLE);

    std::vector<opentelemetry::nostd::string_view> non_matching_tags{"x", "z"};
    EXPECT_EQ(
        SampleWith(*sampler, parent, trace_api::SpanKind::kServer,
                   {{"tags", opentelemetry::nostd::span<const opentelemetry::nostd::string_view>(
                                 non_matching_tags.data(), non_matching_tags.size())}}),
        Decision::DROP);
  }
}

TEST(SdkBuilder, RuleBasedPredicateAttributePatterns)
{
  using config_sdk::ComposableRuleBasedSamplerRuleAttributePatternsConfiguration;
  using config_sdk::ComposableRuleBasedSamplerRuleConfiguration;
  using opentelemetry::sdk::trace::Decision;

  auto parent = MakeRuleParent(true, true);

  // included glob, excluded overrides
  auto rule     = std::make_unique<ComposableRuleBasedSamplerRuleConfiguration>();
  auto patterns = std::make_unique<ComposableRuleBasedSamplerRuleAttributePatternsConfiguration>();
  patterns->key = "url.path";
  patterns->included       = {"/api/*"};
  patterns->excluded       = {"/api/health?"};
  rule->attribute_patterns = std::move(patterns);
  auto sampler             = BuildRuleSampler(std::move(rule));
  ASSERT_NE(sampler, nullptr);

  EXPECT_EQ(
      SampleWith(*sampler, parent, trace_api::SpanKind::kServer, {{"url.path", "/api/users"}}),
      Decision::RECORD_AND_SAMPLE);
  EXPECT_EQ(
      SampleWith(*sampler, parent, trace_api::SpanKind::kServer, {{"url.path", "/api/healthz"}}),
      Decision::DROP);
  EXPECT_EQ(SampleWith(*sampler, parent, trace_api::SpanKind::kServer, {{"url.path", "/other"}}),
            Decision::DROP);

  // excluded only: no included patterns means match-all, then filter by excluded
  {
    auto excluded_only_rule = std::make_unique<ComposableRuleBasedSamplerRuleConfiguration>();
    auto excluded_only_patterns =
        std::make_unique<ComposableRuleBasedSamplerRuleAttributePatternsConfiguration>();
    excluded_only_patterns->key            = "url.path";
    excluded_only_patterns->excluded       = {"/internal/*"};
    excluded_only_rule->attribute_patterns = std::move(excluded_only_patterns);
    auto excluded_only_sampler             = BuildRuleSampler(std::move(excluded_only_rule));
    ASSERT_NE(excluded_only_sampler, nullptr);

    EXPECT_EQ(SampleWith(*excluded_only_sampler, parent, trace_api::SpanKind::kServer,
                         {{"url.path", "/internal/x"}}),
              Decision::DROP);
    EXPECT_EQ(SampleWith(*excluded_only_sampler, parent, trace_api::SpanKind::kServer,
                         {{"url.path", "/public/x"}}),
              Decision::RECORD_AND_SAMPLE);
  }
}

TEST(SdkBuilder, RuleBasedPredicateSpanKindAndParent)
{
  using config_sdk::ComposableRuleBasedSamplerRuleConfiguration;
  using opentelemetry::sdk::trace::Decision;

  // span kind
  {
    auto rule                    = std::make_unique<ComposableRuleBasedSamplerRuleConfiguration>();
    rule->match_span_kind_server = true;
    auto sampler                 = BuildRuleSampler(std::move(rule));
    ASSERT_NE(sampler, nullptr);
    auto parent = MakeRuleParent(true, true);

    EXPECT_EQ(SampleWith(*sampler, parent, trace_api::SpanKind::kServer, {}),
              Decision::RECORD_AND_SAMPLE);
    EXPECT_EQ(SampleWith(*sampler, parent, trace_api::SpanKind::kClient, {}), Decision::DROP);
  }

  // parent: remote only
  {
    auto rule                 = std::make_unique<ComposableRuleBasedSamplerRuleConfiguration>();
    rule->match_parent_remote = true;
    auto sampler              = BuildRuleSampler(std::move(rule));
    ASSERT_NE(sampler, nullptr);

    EXPECT_EQ(SampleWith(*sampler, MakeRuleParent(true, true), trace_api::SpanKind::kServer, {}),
              Decision::RECORD_AND_SAMPLE);
    EXPECT_EQ(SampleWith(*sampler, MakeRuleParent(true, false), trace_api::SpanKind::kServer, {}),
              Decision::DROP);
    EXPECT_EQ(SampleWith(*sampler, trace_api::SpanContext::GetInvalid(),
                         trace_api::SpanKind::kServer, {}),
              Decision::DROP);
  }

  // parent: local only
  {
    auto rule                = std::make_unique<ComposableRuleBasedSamplerRuleConfiguration>();
    rule->match_parent_local = true;
    auto sampler             = BuildRuleSampler(std::move(rule));
    ASSERT_NE(sampler, nullptr);

    EXPECT_EQ(SampleWith(*sampler, MakeRuleParent(true, false), trace_api::SpanKind::kServer, {}),
              Decision::RECORD_AND_SAMPLE);
    EXPECT_EQ(SampleWith(*sampler, MakeRuleParent(true, true), trace_api::SpanKind::kServer, {}),
              Decision::DROP);
    EXPECT_EQ(SampleWith(*sampler, trace_api::SpanContext::GetInvalid(),
                         trace_api::SpanKind::kServer, {}),
              Decision::DROP);
  }

  // parent: none only (root spans)
  {
    auto rule               = std::make_unique<ComposableRuleBasedSamplerRuleConfiguration>();
    rule->match_parent_none = true;
    auto sampler            = BuildRuleSampler(std::move(rule));
    ASSERT_NE(sampler, nullptr);

    EXPECT_EQ(SampleWith(*sampler, trace_api::SpanContext::GetInvalid(),
                         trace_api::SpanKind::kServer, {}),
              Decision::RECORD_AND_SAMPLE);
    EXPECT_EQ(SampleWith(*sampler, MakeRuleParent(true, true), trace_api::SpanKind::kServer, {}),
              Decision::DROP);
  }

  // span kind and attribute values: both conditions must match (AND, not OR)
  {
    using config_sdk::ComposableRuleBasedSamplerRuleAttributeValuesConfiguration;

    auto rule                    = std::make_unique<ComposableRuleBasedSamplerRuleConfiguration>();
    rule->match_span_kind_server = true;
    auto values    = std::make_unique<ComposableRuleBasedSamplerRuleAttributeValuesConfiguration>();
    values->key    = "http.route";
    values->values = {"/health"};
    rule->attribute_values = std::move(values);
    auto sampler           = BuildRuleSampler(std::move(rule));
    ASSERT_NE(sampler, nullptr);
    auto parent = MakeRuleParent(true, true);

    EXPECT_EQ(
        SampleWith(*sampler, parent, trace_api::SpanKind::kServer, {{"http.route", "/health"}}),
        Decision::RECORD_AND_SAMPLE);
    EXPECT_EQ(
        SampleWith(*sampler, parent, trace_api::SpanKind::kServer, {{"http.route", "/other"}}),
        Decision::DROP);
    EXPECT_EQ(SampleWith(*sampler, parent, trace_api::SpanKind::kServer, {}), Decision::DROP);
    EXPECT_EQ(
        SampleWith(*sampler, parent, trace_api::SpanKind::kClient, {{"http.route", "/health"}}),
        Decision::DROP);
  }
}

TEST(SdkBuilder, RuleBasedFirstMatchAndDefaults)
{
  using config_sdk::ComposableRuleBasedSamplerRuleConfiguration;
  using opentelemetry::sdk::trace::Decision;

  auto parent = MakeRuleParent(true, true);

  // first match wins: rule 1 (no conditions -> matches all) is always_off, rule 2 always_on
  {
    auto rule_based_config =
        std::make_unique<config_sdk::ComposableRuleBasedSamplerConfiguration>();
    auto first     = std::make_unique<ComposableRuleBasedSamplerRuleConfiguration>();
    first->sampler = std::make_unique<config_sdk::ComposableAlwaysOffSamplerConfiguration>();
    rule_based_config->rules.push_back(std::move(first));
    auto second     = std::make_unique<ComposableRuleBasedSamplerRuleConfiguration>();
    second->sampler = std::make_unique<config_sdk::ComposableAlwaysOnSamplerConfiguration>();
    rule_based_config->rules.push_back(std::move(second));
    auto composite                = std::make_unique<config_sdk::CompositeSamplerConfiguration>();
    composite->composable_sampler = std::move(rule_based_config);
    std::unique_ptr<config_sdk::SamplerConfiguration> sampler_config = std::move(composite);
    config_sdk::SdkBuilder builder(config_sdk::RegistryFactory::Create());
    auto sampler = builder.CreateSampler(sampler_config);
    ASSERT_NE(sampler, nullptr);

    EXPECT_EQ(SampleWith(*sampler, parent, trace_api::SpanKind::kServer, {}), Decision::DROP);
  }

  // empty rules -> nothing matches -> drop
  {
    auto rule_based_config =
        std::make_unique<config_sdk::ComposableRuleBasedSamplerConfiguration>();
    auto composite                = std::make_unique<config_sdk::CompositeSamplerConfiguration>();
    composite->composable_sampler = std::move(rule_based_config);
    std::unique_ptr<config_sdk::SamplerConfiguration> sampler_config = std::move(composite);
    config_sdk::SdkBuilder builder(config_sdk::RegistryFactory::Create());
    auto sampler = builder.CreateSampler(sampler_config);
    ASSERT_NE(sampler, nullptr);

    EXPECT_EQ(SampleWith(*sampler, parent, trace_api::SpanKind::kServer, {}), Decision::DROP);
  }
}

TEST(SdkBuilder, CreatePeriodicMetricReader)
{
  auto exporter  = std::make_unique<config_sdk::ExtensionPushMetricExporterConfiguration>();
  exporter->name = "noop";

  config_sdk::PeriodicMetricReaderConfiguration model;
  model.exporter           = std::move(exporter);
  model.interval           = 12345;
  model.timeout            = 678;
  model.cardinality_limits = std::make_unique<config_sdk::CardinalityLimitsConfiguration>();
  model.cardinality_limits->default_limit              = 100;
  model.cardinality_limits->counter                    = 200;
  model.cardinality_limits->gauge                      = 300;
  model.cardinality_limits->histogram                  = 400;
  model.cardinality_limits->observable_counter         = 500;
  model.cardinality_limits->observable_gauge           = 600;
  model.cardinality_limits->observable_up_down_counter = 700;
  model.cardinality_limits->up_down_counter            = 800;

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
  EXPECT_EQ(reader->GetCardinalityLimit(opentelemetry::sdk::metrics::InstrumentType::kCounter),
            200u);
  EXPECT_EQ(reader->GetCardinalityLimit(opentelemetry::sdk::metrics::InstrumentType::kGauge), 300u);
  EXPECT_EQ(reader->GetCardinalityLimit(opentelemetry::sdk::metrics::InstrumentType::kHistogram),
            400u);
  EXPECT_EQ(
      reader->GetCardinalityLimit(opentelemetry::sdk::metrics::InstrumentType::kObservableCounter),
      500u);
  EXPECT_EQ(
      reader->GetCardinalityLimit(opentelemetry::sdk::metrics::InstrumentType::kObservableGauge),
      600u);
  EXPECT_EQ(reader->GetCardinalityLimit(
                opentelemetry::sdk::metrics::InstrumentType::kObservableUpDownCounter),
            700u);
  EXPECT_EQ(
      reader->GetCardinalityLimit(opentelemetry::sdk::metrics::InstrumentType::kUpDownCounter),
      800u);
}

TEST(SdkBuilder, CreateAttributesProcessor)
{
  std::map<std::string, int> attributes = {{"included", 1}, {"excluded", 2}, {"unlisted", 3}};
  opentelemetry::common::KeyValueIterableView<std::map<std::string, int>> iterable(attributes);
  config_sdk::SdkBuilder builder(std::make_shared<config_sdk::Registry>());

  // When both lists are configured, exclusion takes precedence over inclusion.
  {
    auto model                    = std::make_unique<config_sdk::IncludeExcludeConfiguration>();
    model->included               = std::make_unique<config_sdk::StringArrayConfiguration>();
    model->included->string_array = {"included", "excluded"};
    model->excluded               = std::make_unique<config_sdk::StringArrayConfiguration>();
    model->excluded->string_array = {"excluded"};

    auto processor = builder.CreateAttributesProcessor(model);
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

    auto processor = builder.CreateAttributesProcessor(model);
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

    auto processor = builder.CreateAttributesProcessor(model);
    ASSERT_NE(processor, nullptr);
    auto filtered = processor->process(iterable);

    EXPECT_EQ(filtered.size(), 2u);
    EXPECT_EQ(filtered.find("excluded"), filtered.end());
  }

  // An empty include/exclude block leaves attributes unchanged.
  {
    auto model     = std::make_unique<config_sdk::IncludeExcludeConfiguration>();
    auto processor = builder.CreateAttributesProcessor(model);
    ASSERT_NE(processor, nullptr);
    auto filtered = processor->process(iterable);

    EXPECT_EQ(filtered.size(), attributes.size());
  }
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

#if OPENTELEMETRY_ABI_VERSION_NO < 2
TEST(SdkBuilder, AddViewGaugeUnsupportedWithABIv1)
{
  auto model = MakeCardinalityOnlyViewConfig(config_sdk::InstrumentType::gauge, 42);

  auto registry = std::make_shared<config_sdk::Registry>();
  config_sdk::SdkBuilder builder(registry);
  opentelemetry::sdk::metrics::ViewRegistry view_registry;

  EXPECT_THROW(builder.AddView(&view_registry, model), config_sdk::UnsupportedException);
}
#endif

TEST(SdkBuilder, AddViewEmptySelectorMatchesAllSupportedInstrumentTypes)
{
  namespace metrics_sdk = opentelemetry::sdk::metrics;

  auto model = MakeCardinalityOnlyViewConfig(config_sdk::InstrumentType::none, 42);

  auto registry = std::make_shared<config_sdk::Registry>();
  config_sdk::SdkBuilder builder(registry);
  metrics_sdk::ViewRegistry view_registry;
  builder.AddView(&view_registry, model);

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
    int matched = 0;
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

TEST(SdkBuilder, AddViewHistogramCardinalityLimitOnly)
{
  namespace metrics_sdk = opentelemetry::sdk::metrics;

  auto model = MakeCardinalityOnlyViewConfig(config_sdk::InstrumentType::histogram, 42);

  auto registry = std::make_shared<config_sdk::Registry>();
  config_sdk::SdkBuilder builder(registry);

  metrics_sdk::ViewRegistry view_registry;
  builder.AddView(&view_registry, model);

  metrics_sdk::InstrumentDescriptor instrument_descriptor{
      "test.instrument", "test description", "units", metrics_sdk::InstrumentType::kHistogram,
      metrics_sdk::InstrumentValueType::kLong};
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
      "test.instrument", "test description", "units", metrics_sdk::InstrumentType::kCounter,
      metrics_sdk::InstrumentValueType::kLong};
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

TEST(SdkBuilder, AddViewWithCardinalityLimitPreservesExplicitAggregation)
{
  namespace metrics_sdk = opentelemetry::sdk::metrics;

  auto model = MakeCardinalityOnlyViewConfig(config_sdk::InstrumentType::histogram, 42);
  auto aggregation =
      std::make_unique<config_sdk::ExplicitBucketHistogramAggregationConfiguration>();
  aggregation->boundaries    = {1.0, 2.0};
  model->stream->aggregation = std::move(aggregation);

  auto registry = std::make_shared<config_sdk::Registry>();
  config_sdk::SdkBuilder builder(registry);

  metrics_sdk::ViewRegistry view_registry;
  builder.AddView(&view_registry, model);

  metrics_sdk::InstrumentDescriptor instrument_descriptor{
      "test.instrument", "test description", "units", metrics_sdk::InstrumentType::kHistogram,
      metrics_sdk::InstrumentValueType::kLong};
  auto instrumentation_scope = scope_sdk::InstrumentationScope::Create("");

  int matched = 0;
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
