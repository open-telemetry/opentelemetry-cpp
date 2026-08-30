// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/common/key_value_iterable_view.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/utility.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/trace/provider.h"
#include "opentelemetry/trace/span.h"
#include "opentelemetry/trace/span_context.h"
#include "opentelemetry/trace/span_context_kv_iterable_view.h"
#include "opentelemetry/trace/span_id.h"
#include "opentelemetry/trace/span_metadata.h"
#include "opentelemetry/trace/trace_flags.h"
#include "opentelemetry/trace/trace_id.h"
#include "opentelemetry/trace/tracer.h"
#include "opentelemetry/trace/tracer_provider.h"

#include "opentelemetry/sdk/configuration/always_off_sampler_builder.h"
#include "opentelemetry/sdk/configuration/always_off_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/always_on_sampler_builder.h"
#include "opentelemetry/sdk/configuration/always_on_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/attribute_limits_configuration.h"
#include "opentelemetry/sdk/configuration/batch_span_processor_builder.h"
#include "opentelemetry/sdk/configuration/batch_span_processor_configuration.h"
#include "opentelemetry/sdk/configuration/composable_always_off_sampler_builder.h"
#include "opentelemetry/sdk/configuration/composable_always_off_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/composable_always_on_sampler_builder.h"
#include "opentelemetry/sdk/configuration/composable_always_on_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/composable_parent_threshold_sampler_builder.h"
#include "opentelemetry/sdk/configuration/composable_parent_threshold_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/composable_probability_sampler_builder.h"
#include "opentelemetry/sdk/configuration/composable_probability_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/composable_rule_based_sampler_builder.h"
#include "opentelemetry/sdk/configuration/composable_rule_based_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/composable_rule_based_sampler_rule_attribute_patterns_configuration.h"
#include "opentelemetry/sdk/configuration/composable_rule_based_sampler_rule_attribute_values_configuration.h"
#include "opentelemetry/sdk/configuration/composable_rule_based_sampler_rule_configuration.h"
#include "opentelemetry/sdk/configuration/composable_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/composite_sampler_builder.h"
#include "opentelemetry/sdk/configuration/composite_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/configuration.h"
#include "opentelemetry/sdk/configuration/configured_sdk.h"
#include "opentelemetry/sdk/configuration/console_span_exporter_builder.h"
#include "opentelemetry/sdk/configuration/console_span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/extension_composable_sampler_builder.h"
#include "opentelemetry/sdk/configuration/extension_composable_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/extension_sampler_builder.h"
#include "opentelemetry/sdk/configuration/extension_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/extension_span_exporter_builder.h"
#include "opentelemetry/sdk/configuration/extension_span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/extension_span_processor_builder.h"
#include "opentelemetry/sdk/configuration/extension_span_processor_configuration.h"
#include "opentelemetry/sdk/configuration/jaeger_remote_sampler_builder.h"
#include "opentelemetry/sdk/configuration/jaeger_remote_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/optional_value.h"
#include "opentelemetry/sdk/configuration/otlp_file_span_exporter_builder.h"
#include "opentelemetry/sdk/configuration/otlp_file_span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/otlp_grpc_span_exporter_builder.h"
#include "opentelemetry/sdk/configuration/otlp_grpc_span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/otlp_http_span_exporter_builder.h"
#include "opentelemetry/sdk/configuration/otlp_http_span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/parent_based_sampler_builder.h"
#include "opentelemetry/sdk/configuration/parent_based_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/probability_sampler_builder.h"
#include "opentelemetry/sdk/configuration/probability_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/registry.h"
#include "opentelemetry/sdk/configuration/sampler_configuration.h"
#include "opentelemetry/sdk/configuration/sdk_builder.h"
#include "opentelemetry/sdk/configuration/simple_span_processor_builder.h"
#include "opentelemetry/sdk/configuration/simple_span_processor_configuration.h"
#include "opentelemetry/sdk/configuration/span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/span_limits_configuration.h"
#include "opentelemetry/sdk/configuration/span_processor_configuration.h"
#include "opentelemetry/sdk/configuration/trace_builders.h"
#include "opentelemetry/sdk/configuration/trace_id_ratio_based_sampler_builder.h"
#include "opentelemetry/sdk/configuration/trace_id_ratio_based_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/tracer_configurator_builder.h"
#include "opentelemetry/sdk/configuration/tracer_configurator_configuration.h"
#include "opentelemetry/sdk/configuration/tracer_provider_configuration.h"
#include "opentelemetry/sdk/configuration/unsupported_exception.h"

#include "opentelemetry/sdk/instrumentationscope/scope_configurator.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/processor.h"
#include "opentelemetry/sdk/trace/sampler.h"
#include "opentelemetry/sdk/trace/samplers/always_on.h"
#include "opentelemetry/sdk/trace/samplers/composable_always_on.h"
#include "opentelemetry/sdk/trace/simple_processor.h"
#include "opentelemetry/sdk/trace/span_limits.h"
#include "opentelemetry/sdk/trace/tracer_config.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"

#include "config_test_trace.h"

namespace trace_api  = opentelemetry::trace;
namespace scope_sdk  = opentelemetry::sdk::instrumentationscope;
namespace config_sdk = opentelemetry::sdk::configuration;
namespace trace_sdk  = opentelemetry::sdk::trace;

namespace
{

class TestExtensionComposableSamplerBuilder : public config_sdk::ExtensionComposableSamplerBuilder
{
public:
  std::unique_ptr<trace_sdk::ComposableSampler> Build(
      const config_sdk::ExtensionComposableSamplerConfiguration *model) const override
  {
    called = true;
    name   = model->name;
    depth  = model->depth;
    return std::make_unique<trace_sdk::ComposableAlwaysOnSampler>();
  }

  mutable bool called{false};
  mutable std::string name;
  mutable std::size_t depth{0};
};

class TestExtensionSamplerBuilder : public config_sdk::ExtensionSamplerBuilder
{
public:
  std::unique_ptr<trace_sdk::Sampler> Build(
      const config_sdk::ExtensionSamplerConfiguration *model) const override
  {
    called = true;
    name   = model->name;
    // Return AlwaysOn sampler by constructing through the registered builder
    return std::make_unique<trace_sdk::AlwaysOnSampler>();
  }

  mutable bool called{false};
  mutable std::string name;
};

class TestExtensionSpanProcessorBuilder : public config_sdk::ExtensionSpanProcessorBuilder
{
public:
  std::unique_ptr<trace_sdk::SpanProcessor> Build(
      const config_sdk::ExtensionSpanProcessorConfiguration *model) const override
  {
    called = true;
    name   = model->name;
    return std::make_unique<trace_sdk::SimpleSpanProcessor>(
        std::make_unique<config_test::NoopSpanExporter>());
  }

  mutable bool called{false};
  mutable std::string name;
};

class TraceBuildersTest : public ::testing::Test
{
protected:
  using RuleAttrMap = std::map<std::string, opentelemetry::common::AttributeValue>;

  trace_sdk::Decision SampleWith(trace_sdk::Sampler &sampler,
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
  void SetUp() override
  {
    registry_ = std::make_shared<config_sdk::Registry>();
    config_sdk::RegisterDefaultTraceBuilders(registry_.get());
    registry_->SetConsoleSpanBuilder(
        std::make_unique<config_test::NoopConsoleSpanExporterBuilder>());
    registry_->SetOtlpHttpSpanBuilder(
        std::make_unique<config_test::RecordingOtlpHttpSpanExporterBuilder>(nullptr));
    registry_->SetOtlpGrpcSpanBuilder(
        std::make_unique<config_test::RecordingOtlpGrpcSpanExporterBuilder>(nullptr));
    registry_->SetOtlpFileSpanBuilder(
        std::make_unique<config_test::RecordingOtlpFileSpanExporterBuilder>(nullptr));
  }

  std::shared_ptr<trace_sdk::TracerProvider> MakeTracerProvider(
      std::unique_ptr<config_sdk::TracerProviderConfiguration> model)
  {
    auto resource = opentelemetry::sdk::resource::Resource::Create({});
    config_sdk::SdkBuilder builder(registry_);
    return builder.CreateTracerProvider(model, resource);
  }

  std::unique_ptr<trace_sdk::SpanProcessor> MakeSpanProcessor(
      std::unique_ptr<config_sdk::SpanProcessorConfiguration> model)
  {
    config_sdk::SdkBuilder builder(registry_);
    return builder.CreateSpanProcessor(model);
  }

  std::unique_ptr<trace_sdk::SpanExporter> MakeSpanExporter(
      std::unique_ptr<config_sdk::SpanExporterConfiguration> model)
  {
    config_sdk::SdkBuilder builder(registry_);
    return builder.CreateSpanExporter(model);
  }

  std::unique_ptr<trace_sdk::Sampler> MakeSampler(
      std::unique_ptr<config_sdk::SamplerConfiguration> model)
  {
    config_sdk::SdkBuilder builder(registry_);
    return builder.CreateSampler(model);
  }

  std::unique_ptr<scope_sdk::ScopeConfigurator<trace_sdk::TracerConfig>> MakeTracerConfigurator(
      std::unique_ptr<config_sdk::TracerConfiguratorConfiguration> model)
  {
    config_sdk::SdkBuilder builder(registry_);
    return builder.CreateTracerConfigurator(model);
  }

  std::unique_ptr<trace_sdk::Sampler> MakeComposableSampler(
      std::unique_ptr<config_sdk::SamplerConfiguration> model)
  {
    return MakeSampler(std::move(model));
  }

  // Builds composite(rule_based{[rule]}) where the rule maps to always_on.
  std::unique_ptr<trace_sdk::Sampler> MakeRuleSampler(
      std::unique_ptr<config_sdk::ComposableRuleBasedSamplerRuleConfiguration> rule)
  {
    rule->sampler = std::make_unique<config_sdk::ComposableAlwaysOnSamplerConfiguration>();
    auto rule_based_config =
        std::make_unique<config_sdk::ComposableRuleBasedSamplerConfiguration>();
    rule_based_config->rules.push_back(std::move(rule));
    auto composite                = std::make_unique<config_sdk::CompositeSamplerConfiguration>();
    composite->composable_sampler = std::move(rule_based_config);
    std::unique_ptr<config_sdk::SamplerConfiguration> sampler_config = std::move(composite);
    return MakeSampler(std::move(sampler_config));
  }

  std::shared_ptr<config_sdk::Registry> registry_;
};

}  // namespace

TEST_F(TraceBuildersTest, DefaultRegistry)
{
  auto registry = std::make_shared<config_sdk::Registry>();

  EXPECT_EQ(registry->GetAlwaysOnSamplerBuilder(), nullptr);
  EXPECT_EQ(registry->GetAlwaysOffSamplerBuilder(), nullptr);
  EXPECT_EQ(registry->GetTraceIdRatioBasedSamplerBuilder(), nullptr);
  EXPECT_EQ(registry->GetProbabilitySamplerBuilder(), nullptr);
  EXPECT_EQ(registry->GetParentBasedSamplerBuilder(), nullptr);
  EXPECT_EQ(registry->GetJaegerRemoteSamplerBuilder(), nullptr);
  EXPECT_EQ(registry->GetBatchSpanProcessorBuilder(), nullptr);
  EXPECT_EQ(registry->GetSimpleSpanProcessorBuilder(), nullptr);
  EXPECT_EQ(registry->GetTracerConfiguratorBuilder(), nullptr);
  EXPECT_EQ(registry->GetComposableAlwaysOnSamplerBuilder(), nullptr);
  EXPECT_EQ(registry->GetComposableAlwaysOffSamplerBuilder(), nullptr);
  EXPECT_EQ(registry->GetComposableProbabilitySamplerBuilder(), nullptr);
  EXPECT_EQ(registry->GetComposableParentThresholdSamplerBuilder(), nullptr);
  EXPECT_EQ(registry->GetComposableRuleBasedSamplerBuilder(), nullptr);
  EXPECT_EQ(registry->GetCompositeSamplerBuilder(), nullptr);
}

TEST_F(TraceBuildersTest, RegisterDefaultTraceBuilders)
{
  auto registry = std::make_shared<config_sdk::Registry>();
  config_sdk::RegisterDefaultTraceBuilders(registry.get());

  EXPECT_NE(registry->GetAlwaysOnSamplerBuilder(), nullptr);
  EXPECT_NE(registry->GetAlwaysOffSamplerBuilder(), nullptr);
  EXPECT_NE(registry->GetTraceIdRatioBasedSamplerBuilder(), nullptr);
  EXPECT_NE(registry->GetProbabilitySamplerBuilder(), nullptr);
  EXPECT_NE(registry->GetParentBasedSamplerBuilder(), nullptr);
  EXPECT_NE(registry->GetJaegerRemoteSamplerBuilder(), nullptr);
  EXPECT_NE(registry->GetBatchSpanProcessorBuilder(), nullptr);
  EXPECT_NE(registry->GetSimpleSpanProcessorBuilder(), nullptr);
  EXPECT_NE(registry->GetTracerConfiguratorBuilder(), nullptr);
  EXPECT_NE(registry->GetComposableAlwaysOnSamplerBuilder(), nullptr);
  EXPECT_NE(registry->GetComposableAlwaysOffSamplerBuilder(), nullptr);
  EXPECT_NE(registry->GetComposableProbabilitySamplerBuilder(), nullptr);
  EXPECT_NE(registry->GetComposableParentThresholdSamplerBuilder(), nullptr);
  EXPECT_NE(registry->GetComposableRuleBasedSamplerBuilder(), nullptr);
  EXPECT_NE(registry->GetCompositeSamplerBuilder(), nullptr);
}

TEST_F(TraceBuildersTest, SpanLimitsDefaults)
{
  auto model    = std::make_unique<config_sdk::TracerProviderConfiguration>();
  model->limits = nullptr;
  auto provider = MakeTracerProvider(std::move(model));
  ASSERT_NE(provider, nullptr);

  const auto limits         = provider->GetSpanLimits();
  const auto default_limits = trace_sdk::SpanLimits{};

  EXPECT_EQ(limits.attribute_count_limit, default_limits.attribute_count_limit);
  EXPECT_EQ(limits.event_count_limit, default_limits.event_count_limit);
  EXPECT_EQ(limits.link_count_limit, default_limits.link_count_limit);
  EXPECT_EQ(limits.event_attribute_count_limit, default_limits.event_attribute_count_limit);
  EXPECT_EQ(limits.link_attribute_count_limit, default_limits.link_attribute_count_limit);
  EXPECT_EQ(limits.attribute_value_length_limit, default_limits.attribute_value_length_limit);
}

TEST_F(TraceBuildersTest, SpanLimitsConfiguration)
{
  auto model    = std::make_unique<config_sdk::TracerProviderConfiguration>();
  model->limits = std::make_unique<config_sdk::SpanLimitsConfiguration>();
  model->limits->attribute_value_length_limit = 1111;
  model->limits->attribute_count_limit        = 2222;
  model->limits->event_count_limit            = 3333;
  model->limits->link_count_limit             = 4444;
  model->limits->event_attribute_count_limit  = 5555;
  model->limits->link_attribute_count_limit   = 6666;

  auto provider = MakeTracerProvider(std::move(model));
  ASSERT_NE(provider, nullptr);

  auto limits = provider->GetSpanLimits();
  EXPECT_EQ(limits.attribute_value_length_limit, 1111);
  EXPECT_EQ(limits.attribute_count_limit, 2222);
  EXPECT_EQ(limits.event_count_limit, 3333);
  EXPECT_EQ(limits.link_count_limit, 4444);
  EXPECT_EQ(limits.event_attribute_count_limit, 5555);
  EXPECT_EQ(limits.link_attribute_count_limit, 6666);
}

TEST_F(TraceBuildersTest, SpanLimitsFromAttributeLimits)
{
  auto model    = std::make_unique<config_sdk::TracerProviderConfiguration>();
  model->limits = nullptr;

  config_sdk::AttributeLimitsConfiguration attribute_limits;
  attribute_limits.attribute_count_limit        = 7;
  attribute_limits.attribute_value_length_limit = 9;

  auto provider = MakeTracerProvider(std::move(model));
  ASSERT_NE(provider, nullptr);

  const auto limits         = provider->GetSpanLimits();
  const auto default_limits = opentelemetry::sdk::trace::SpanLimits{};

  EXPECT_EQ(limits.attribute_count_limit, attribute_limits.attribute_count_limit.Value());
  EXPECT_EQ(limits.attribute_value_length_limit,
            attribute_limits.attribute_value_length_limit.Value());
  EXPECT_EQ(limits.event_count_limit, default_limits.event_count_limit);
  EXPECT_EQ(limits.link_count_limit, default_limits.link_count_limit);
  EXPECT_EQ(limits.event_attribute_count_limit, default_limits.event_attribute_count_limit);
  EXPECT_EQ(limits.link_attribute_count_limit, default_limits.link_attribute_count_limit);
}

TEST_F(TraceBuildersTest, SpanLimitsOverrideAttributeLimits)
{
  auto model    = std::make_unique<config_sdk::TracerProviderConfiguration>();
  model->limits = std::make_unique<config_sdk::SpanLimitsConfiguration>();
  model->limits->attribute_value_length_limit = 1111;
  model->limits->attribute_count_limit        = 2222;
  model->limits->event_count_limit            = 3333;

  config_sdk::AttributeLimitsConfiguration attribute_limits;
  attribute_limits.attribute_count_limit        = 7;
  attribute_limits.attribute_value_length_limit = 9;

  auto provider = MakeTracerProvider(std::move(model));
  ASSERT_NE(provider, nullptr);

  const auto limits = provider->GetSpanLimits();
  EXPECT_EQ(limits.attribute_value_length_limit,
            model->limits->attribute_value_length_limit.Value());
  EXPECT_EQ(limits.attribute_count_limit, model->limits->attribute_count_limit.Value());
  EXPECT_EQ(limits.event_count_limit, model->limits->event_count_limit.Value());
}

TEST_F(TraceBuildersTest, SpanLimitsPerFieldAttributeLimits)
{
  auto model                       = std::make_unique<config_sdk::TracerProviderConfiguration>();
  model->limits                    = std::make_unique<config_sdk::SpanLimitsConfiguration>();
  model->limits->event_count_limit = 64;
  model->limits->link_count_limit  = 64;
  model->limits->event_attribute_count_limit = 8;
  model->limits->link_attribute_count_limit  = 8;

  config_sdk::AttributeLimitsConfiguration attribute_limits;
  attribute_limits.attribute_value_length_limit = 4096;

  auto provider = MakeTracerProvider(std::move(model));
  ASSERT_NE(provider, nullptr);

  const auto limits         = provider->GetSpanLimits();
  const auto default_limits = opentelemetry::sdk::trace::SpanLimits{};
  EXPECT_EQ(limits.attribute_value_length_limit, 4096);
  EXPECT_EQ(limits.attribute_count_limit, default_limits.attribute_count_limit);
  EXPECT_EQ(limits.event_count_limit, 64);
  EXPECT_EQ(limits.link_count_limit, 64);
  EXPECT_EQ(limits.event_attribute_count_limit, 8);
  EXPECT_EQ(limits.link_attribute_count_limit, 8);
}

TEST_F(TraceBuildersTest, SpanLimitsPartialOverrideAttributeLimits)
{
  auto model    = std::make_unique<config_sdk::TracerProviderConfiguration>();
  model->limits = std::make_unique<config_sdk::SpanLimitsConfiguration>();
  model->limits->attribute_count_limit = 22;

  config_sdk::AttributeLimitsConfiguration attribute_limits;
  attribute_limits.attribute_count_limit        = 7;
  attribute_limits.attribute_value_length_limit = 9;

  auto provider = MakeTracerProvider(std::move(model));
  ASSERT_NE(provider, nullptr);

  const auto limits = provider->GetSpanLimits();
  EXPECT_EQ(limits.attribute_count_limit, 22);
  EXPECT_EQ(limits.attribute_value_length_limit, 9);
}

TEST_F(TraceBuildersTest, BatchSpanProcessorDispatch)
{
  auto exporter       = std::make_unique<config_sdk::ConsoleSpanExporterConfiguration>();
  auto processor      = std::make_unique<config_sdk::BatchSpanProcessorConfiguration>();
  processor->exporter = std::move(exporter);
  std::unique_ptr<config_sdk::SpanProcessorConfiguration> model = std::move(processor);

  auto result = MakeSpanProcessor(std::move(model));
  EXPECT_NE(result, nullptr);
}

TEST_F(TraceBuildersTest, SimpleSpanProcessorDispatch)
{
  auto exporter       = std::make_unique<config_sdk::ConsoleSpanExporterConfiguration>();
  auto processor      = std::make_unique<config_sdk::SimpleSpanProcessorConfiguration>();
  processor->exporter = std::move(exporter);
  std::unique_ptr<config_sdk::SpanProcessorConfiguration> model = std::move(processor);

  auto result = MakeSpanProcessor(std::move(model));
  EXPECT_NE(result, nullptr);
}

TEST_F(TraceBuildersTest, CreateParentBasedSampler)
{
  // parent based with no root configured should default to always on
  {
    auto parent_based_sampler_config =
        std::make_unique<config_sdk::ParentBasedSamplerConfiguration>();
    parent_based_sampler_config->root = nullptr;

    std::unique_ptr<config_sdk::SamplerConfiguration> sampler_config =
        std::move(parent_based_sampler_config);
    auto sampler = MakeSampler(std::move(sampler_config));
    ASSERT_NE(sampler, nullptr);
    EXPECT_EQ(std::string{sampler->GetDescription()}, R"(ParentBased{AlwaysOnSampler})");
  }

  // parent based with root always on
  {
    auto parent_based_sampler_config =
        std::make_unique<config_sdk::ParentBasedSamplerConfiguration>();
    parent_based_sampler_config->root =
        std::make_unique<config_sdk::AlwaysOnSamplerConfiguration>();
    std::unique_ptr<config_sdk::SamplerConfiguration> sampler_config =
        std::move(parent_based_sampler_config);
    auto sampler = MakeSampler(std::move(sampler_config));
    ASSERT_NE(sampler, nullptr);
    EXPECT_EQ(std::string{sampler->GetDescription()}, R"(ParentBased{AlwaysOnSampler})");
  }

  // parent based with root always off
  {
    auto parent_based_sampler_config =
        std::make_unique<config_sdk::ParentBasedSamplerConfiguration>();
    parent_based_sampler_config->root =
        std::make_unique<config_sdk::AlwaysOffSamplerConfiguration>();
    std::unique_ptr<config_sdk::SamplerConfiguration> sampler_config =
        std::move(parent_based_sampler_config);
    auto sampler = MakeSampler(std::move(sampler_config));
    ASSERT_NE(sampler, nullptr);
    EXPECT_EQ(std::string{sampler->GetDescription()}, R"(ParentBased{AlwaysOffSampler})");
  }

  // parent based with a custom root sampler
  {
    auto parent_based_sampler_config =
        std::make_unique<config_sdk::ParentBasedSamplerConfiguration>();
    auto trace_id_ratio_based_sampler_config =
        std::make_unique<config_sdk::TraceIdRatioBasedSamplerConfiguration>();
    trace_id_ratio_based_sampler_config->ratio = 0.5;
    parent_based_sampler_config->root          = std::move(trace_id_ratio_based_sampler_config);
    std::unique_ptr<config_sdk::SamplerConfiguration> sampler_config =
        std::move(parent_based_sampler_config);
    auto sampler = MakeSampler(std::move(sampler_config));
    ASSERT_NE(sampler, nullptr);
    EXPECT_EQ(std::string{sampler->GetDescription()},
              R"(ParentBased{TraceIdRatioBasedSampler{0.500000}})");
  }

  // parent based with all sub samplers set
  {
    auto parent_based_sampler_config =
        std::make_unique<config_sdk::ParentBasedSamplerConfiguration>();
    auto trace_id_ratio_based_sampler_config =
        std::make_unique<config_sdk::TraceIdRatioBasedSamplerConfiguration>();
    trace_id_ratio_based_sampler_config->ratio = 0.25;
    parent_based_sampler_config->root          = std::move(trace_id_ratio_based_sampler_config);

    auto always_off_sampler_config = std::make_unique<config_sdk::AlwaysOffSamplerConfiguration>();
    parent_based_sampler_config->remote_parent_sampled = std::move(always_off_sampler_config);

    auto always_on_sampler_config = std::make_unique<config_sdk::AlwaysOnSamplerConfiguration>();
    parent_based_sampler_config->remote_parent_not_sampled = std::move(always_on_sampler_config);

    auto trace_id_ratio_based_sampler_config_2 =
        std::make_unique<config_sdk::TraceIdRatioBasedSamplerConfiguration>();
    trace_id_ratio_based_sampler_config_2->ratio = 0.35;
    parent_based_sampler_config->local_parent_sampled =
        std::move(trace_id_ratio_based_sampler_config_2);

    auto always_off_sampler_config_2 =
        std::make_unique<config_sdk::AlwaysOffSamplerConfiguration>();
    parent_based_sampler_config->local_parent_not_sampled = std::move(always_off_sampler_config_2);

    std::unique_ptr<config_sdk::SamplerConfiguration> sampler_config =
        std::move(parent_based_sampler_config);
    auto sampler = MakeSampler(std::move(sampler_config));
    ASSERT_NE(sampler, nullptr);
    EXPECT_EQ(std::string{sampler->GetDescription()},
              R"(ParentBased{TraceIdRatioBasedSampler{0.250000}})");
  }
}

TEST_F(TraceBuildersTest, CreateProbabilitySampler)
{
  // default ratio is 1.0
  {
    auto probability_sampler_config =
        std::make_unique<config_sdk::ProbabilitySamplerConfiguration>();
    std::unique_ptr<config_sdk::SamplerConfiguration> sampler_config =
        std::move(probability_sampler_config);
    auto sampler = MakeSampler(std::move(sampler_config));
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
    auto sampler = MakeSampler(std::move(sampler_config));
    ASSERT_NE(sampler, nullptr);
    EXPECT_EQ(std::string{sampler->GetDescription()}, R"(ProbabilitySampler{0.500000})");
  }
}

TEST_F(TraceBuildersTest, CreateComposableAlwaysOnSampler)
{
  auto composable_config = std::make_unique<config_sdk::ComposableAlwaysOnSamplerConfiguration>();
  auto composite         = std::make_unique<config_sdk::CompositeSamplerConfiguration>();
  composite->composable_sampler                                    = std::move(composable_config);
  std::unique_ptr<config_sdk::SamplerConfiguration> sampler_config = std::move(composite);
  auto sampler = MakeSampler(std::move(sampler_config));
  ASSERT_NE(sampler, nullptr);
  EXPECT_EQ(std::string{sampler->GetDescription()},
            R"(CompositeSampler{ComposableAlwaysOnSampler})");
}

TEST_F(TraceBuildersTest, CreateComposableAlwaysOffSampler)
{
  auto composable_config = std::make_unique<config_sdk::ComposableAlwaysOffSamplerConfiguration>();
  auto composite         = std::make_unique<config_sdk::CompositeSamplerConfiguration>();
  composite->composable_sampler                                    = std::move(composable_config);
  std::unique_ptr<config_sdk::SamplerConfiguration> sampler_config = std::move(composite);
  auto sampler = MakeSampler(std::move(sampler_config));
  ASSERT_NE(sampler, nullptr);
  EXPECT_EQ(std::string{sampler->GetDescription()},
            R"(CompositeSampler{ComposableAlwaysOffSampler})");
}

TEST_F(TraceBuildersTest, CreateExtensionComposableSampler)
{
  auto extension_builder      = std::make_unique<TestExtensionComposableSamplerBuilder>();
  auto *extension_builder_ptr = extension_builder.get();
  registry_->SetExtensionComposableSamplerBuilder("custom_composable",
                                                  std::move(extension_builder));

  auto extension_config   = std::make_unique<config_sdk::ExtensionComposableSamplerConfiguration>();
  extension_config->name  = "custom_composable";
  extension_config->depth = 2;
  auto composite          = std::make_unique<config_sdk::CompositeSamplerConfiguration>();
  composite->composable_sampler                                    = std::move(extension_config);
  std::unique_ptr<config_sdk::SamplerConfiguration> sampler_config = std::move(composite);

  auto sampler = MakeSampler(std::move(sampler_config));

  ASSERT_NE(sampler, nullptr);
  EXPECT_TRUE(extension_builder_ptr->called);
  EXPECT_EQ(extension_builder_ptr->name, "custom_composable");
  EXPECT_EQ(extension_builder_ptr->depth, 2);
  EXPECT_EQ(std::string{sampler->GetDescription()},
            R"(CompositeSampler{ComposableAlwaysOnSampler})");
}

TEST_F(TraceBuildersTest, CreateUnregisteredExtensionComposableSampler)
{
  auto extension_config  = std::make_unique<config_sdk::ExtensionComposableSamplerConfiguration>();
  extension_config->name = "missing_composable";
  auto composite         = std::make_unique<config_sdk::CompositeSamplerConfiguration>();
  composite->composable_sampler                                    = std::move(extension_config);
  std::unique_ptr<config_sdk::SamplerConfiguration> sampler_config = std::move(composite);

  EXPECT_THROW(MakeSampler(std::move(sampler_config)), config_sdk::UnsupportedException);
}

TEST_F(TraceBuildersTest, CreateComposableProbabilitySampler)
{
  auto composable_probability_sampler_config =
      std::make_unique<config_sdk::ComposableProbabilitySamplerConfiguration>();
  composable_probability_sampler_config->ratio = 0.25;
  auto composite                = std::make_unique<config_sdk::CompositeSamplerConfiguration>();
  composite->composable_sampler = std::move(composable_probability_sampler_config);
  std::unique_ptr<config_sdk::SamplerConfiguration> sampler_config = std::move(composite);
  auto sampler = MakeSampler(std::move(sampler_config));
  ASSERT_NE(sampler, nullptr);
  EXPECT_EQ(std::string{sampler->GetDescription()},
            R"(CompositeSampler{ComposableProbabilitySampler{0.250000}})");
}

TEST_F(TraceBuildersTest, CreateComposableParentThresholdSampler)
{
  auto root_config   = std::make_unique<config_sdk::ComposableProbabilitySamplerConfiguration>();
  root_config->ratio = 0.25;
  auto parent_config =
      std::make_unique<config_sdk::ComposableParentThresholdSamplerConfiguration>();
  parent_config->root           = std::move(root_config);
  auto composite                = std::make_unique<config_sdk::CompositeSamplerConfiguration>();
  composite->composable_sampler = std::move(parent_config);
  std::unique_ptr<config_sdk::SamplerConfiguration> sampler_config = std::move(composite);
  auto sampler = MakeSampler(std::move(sampler_config));
  ASSERT_NE(sampler, nullptr);
  EXPECT_EQ(
      std::string{sampler->GetDescription()},
      R"(CompositeSampler{ComposableParentThresholdSampler{ComposableProbabilitySampler{0.250000}}})");
}

TEST_F(TraceBuildersTest, CreateComposableParentThresholdSamplerNullRoot)
{
  auto parent_config =
      std::make_unique<config_sdk::ComposableParentThresholdSamplerConfiguration>();
  parent_config->root           = nullptr;
  auto composite                = std::make_unique<config_sdk::CompositeSamplerConfiguration>();
  composite->composable_sampler = std::move(parent_config);
  std::unique_ptr<config_sdk::SamplerConfiguration> sampler_config = std::move(composite);
  auto sampler = MakeSampler(std::move(sampler_config));
  ASSERT_NE(sampler, nullptr);
  EXPECT_EQ(std::string{sampler->GetDescription()},
            R"(CompositeSampler{ComposableParentThresholdSampler{ComposableAlwaysOnSampler}})");
}

TEST_F(TraceBuildersTest, CreateComposableParentThresholdSamplerNestedDepth3)
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
  auto sampler = MakeSampler(std::move(sampler_config));
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

TEST_F(TraceBuildersTest, CreateComposableSamplerAtMaxDepth)
{
  // 9 parent nodes + leaf = depth 10, the default maximum.
  auto sampler_config = MakeNestedComposableConfig(9);
  auto sampler        = MakeSampler(std::move(sampler_config));
  ASSERT_NE(sampler, nullptr);
}

TEST_F(TraceBuildersTest, CreateComposableSamplerBeyondMaxDepth)
{
  // 10 parent nodes + leaf = depth 11, exceeding the default maximum.
  auto sampler_config = MakeNestedComposableConfig(10);
  EXPECT_THROW(MakeSampler(std::move(sampler_config)), config_sdk::UnsupportedException);
}

TEST_F(TraceBuildersTest, CreateComposableRuleBasedSampler)
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
  auto sampler = MakeSampler(std::move(sampler_config));
  ASSERT_NE(sampler, nullptr);
  EXPECT_EQ(
      std::string{sampler->GetDescription()},
      R"(CompositeSampler{ComposableRuleBasedSampler{ComposableAlwaysOffSampler,ComposableAlwaysOnSampler}})");
}

TEST_F(TraceBuildersTest, RuleBasedPredicateAttributeValues)
{
  using config_sdk::ComposableRuleBasedSamplerRuleAttributeValuesConfiguration;
  using config_sdk::ComposableRuleBasedSamplerRuleConfiguration;
  using trace_sdk::Decision;

  auto parent = MakeRuleParent(true, true);

  // string attribute, exact match
  {
    auto rule      = std::make_unique<ComposableRuleBasedSamplerRuleConfiguration>();
    auto values    = std::make_unique<ComposableRuleBasedSamplerRuleAttributeValuesConfiguration>();
    values->key    = "http.route";
    values->values = {"/health", "/metrics"};
    rule->attribute_values = std::move(values);
    auto sampler           = MakeRuleSampler(std::move(rule));
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
    auto sampler           = MakeRuleSampler(std::move(rule));
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
    auto sampler           = MakeRuleSampler(std::move(rule));
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

TEST_F(TraceBuildersTest, RuleBasedPredicateAttributePatterns)
{
  using config_sdk::ComposableRuleBasedSamplerRuleAttributePatternsConfiguration;
  using config_sdk::ComposableRuleBasedSamplerRuleConfiguration;
  using trace_sdk::Decision;

  auto parent = MakeRuleParent(true, true);

  // included glob, excluded overrides
  auto rule     = std::make_unique<ComposableRuleBasedSamplerRuleConfiguration>();
  auto patterns = std::make_unique<ComposableRuleBasedSamplerRuleAttributePatternsConfiguration>();
  patterns->key = "url.path";
  patterns->included       = {"/api/*"};
  patterns->excluded       = {"/api/health?"};
  rule->attribute_patterns = std::move(patterns);
  auto sampler             = MakeRuleSampler(std::move(rule));
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
    auto excluded_only_sampler             = MakeRuleSampler(std::move(excluded_only_rule));
    ASSERT_NE(excluded_only_sampler, nullptr);

    EXPECT_EQ(SampleWith(*excluded_only_sampler, parent, trace_api::SpanKind::kServer,
                         {{"url.path", "/internal/x"}}),
              Decision::DROP);
    EXPECT_EQ(SampleWith(*excluded_only_sampler, parent, trace_api::SpanKind::kServer,
                         {{"url.path", "/public/x"}}),
              Decision::RECORD_AND_SAMPLE);
  }
}

TEST_F(TraceBuildersTest, RuleBasedPredicateSpanKindAndParent)
{
  using config_sdk::ComposableRuleBasedSamplerRuleConfiguration;
  using trace_sdk::Decision;

  // span kind
  {
    auto rule                    = std::make_unique<ComposableRuleBasedSamplerRuleConfiguration>();
    rule->match_span_kind_server = true;
    auto sampler                 = MakeRuleSampler(std::move(rule));
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
    auto sampler              = MakeRuleSampler(std::move(rule));
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
    auto sampler             = MakeRuleSampler(std::move(rule));
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
    auto sampler            = MakeRuleSampler(std::move(rule));
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
    auto sampler           = MakeRuleSampler(std::move(rule));
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

TEST_F(TraceBuildersTest, RuleBasedFirstMatchAndDefaults)
{
  using config_sdk::ComposableRuleBasedSamplerRuleConfiguration;
  using trace_sdk::Decision;

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

    auto sampler = MakeSampler(std::move(sampler_config));
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

    auto sampler = MakeSampler(std::move(sampler_config));
    ASSERT_NE(sampler, nullptr);

    EXPECT_EQ(SampleWith(*sampler, parent, trace_api::SpanKind::kServer, {}), Decision::DROP);
  }
}

TEST_F(TraceBuildersTest, RuleBasedSamplerIgnoresRuleWithNullSampler)
{
  auto rule       = std::make_unique<config_sdk::ComposableRuleBasedSamplerRuleConfiguration>();
  rule->sampler   = nullptr;
  auto rule_based = std::make_unique<config_sdk::ComposableRuleBasedSamplerConfiguration>();
  rule_based->rules.push_back(std::move(rule));
  auto composite                = std::make_unique<config_sdk::CompositeSamplerConfiguration>();
  composite->composable_sampler = std::move(rule_based);
  std::unique_ptr<config_sdk::SamplerConfiguration> model = std::move(composite);

  auto sampler = MakeSampler(std::move(model));
  EXPECT_NE(sampler, nullptr);
}

TEST_F(TraceBuildersTest, RegisteredExtensionSamplerBuilder)
{
  auto builder    = std::make_unique<TestExtensionSamplerBuilder>();
  auto *builder_p = builder.get();
  registry_->SetExtensionSamplerBuilder("custom_sampler", std::move(builder));

  auto ext  = std::make_unique<config_sdk::ExtensionSamplerConfiguration>();
  ext->name = "custom_sampler";
  std::unique_ptr<config_sdk::SamplerConfiguration> model = std::move(ext);
  auto sampler                                            = MakeSampler(std::move(model));

  ASSERT_NE(sampler, nullptr);
  EXPECT_TRUE(builder_p->called);
  EXPECT_EQ(builder_p->name, "custom_sampler");
}

TEST_F(TraceBuildersTest, RegisteredExtensionSpanProcessorBuilder)
{
  auto builder    = std::make_unique<TestExtensionSpanProcessorBuilder>();
  auto *builder_p = builder.get();
  registry_->SetExtensionSpanProcessorBuilder("custom_processor", std::move(builder));

  auto ext  = std::make_unique<config_sdk::ExtensionSpanProcessorConfiguration>();
  ext->name = "custom_processor";
  std::unique_ptr<config_sdk::SpanProcessorConfiguration> model = std::move(ext);
  auto processor = MakeSpanProcessor(std::move(model));

  ASSERT_NE(processor, nullptr);
  EXPECT_TRUE(builder_p->called);
  EXPECT_EQ(builder_p->name, "custom_processor");
}

// ---------------------------------------------------------------------------
// Unregistered builder slots: verify each sampler and configurator slot throws.

TEST_F(TraceBuildersTest, UnregisteredAlwaysOnSamplerBuilder)
{
  registry_->SetAlwaysOnSamplerBuilder(nullptr);
  std::unique_ptr<config_sdk::SamplerConfiguration> model =
      std::make_unique<config_sdk::AlwaysOnSamplerConfiguration>();
  EXPECT_THROW(MakeSampler(std::move(model)), config_sdk::UnsupportedException);
}

TEST_F(TraceBuildersTest, UnregisteredAlwaysOffSamplerBuilder)
{
  registry_->SetAlwaysOffSamplerBuilder(nullptr);
  std::unique_ptr<config_sdk::SamplerConfiguration> model =
      std::make_unique<config_sdk::AlwaysOffSamplerConfiguration>();
  EXPECT_THROW(MakeSampler(std::move(model)), config_sdk::UnsupportedException);
}

TEST_F(TraceBuildersTest, UnregisteredProbabilitySamplerBuilder)
{
  registry_->SetProbabilitySamplerBuilder(nullptr);
  std::unique_ptr<config_sdk::SamplerConfiguration> model =
      std::make_unique<config_sdk::ProbabilitySamplerConfiguration>();
  EXPECT_THROW(MakeSampler(std::move(model)), config_sdk::UnsupportedException);
}

TEST_F(TraceBuildersTest, UnregisteredTraceIdRatioBasedSamplerBuilder)
{
  registry_->SetTraceIdRatioBasedSamplerBuilder(nullptr);
  std::unique_ptr<config_sdk::SamplerConfiguration> model =
      std::make_unique<config_sdk::TraceIdRatioBasedSamplerConfiguration>();
  EXPECT_THROW(MakeSampler(std::move(model)), config_sdk::UnsupportedException);
}

TEST_F(TraceBuildersTest, UnregisteredParentBasedSamplerBuilder)
{
  registry_->SetParentBasedSamplerBuilder(nullptr);
  std::unique_ptr<config_sdk::SamplerConfiguration> model =
      std::make_unique<config_sdk::ParentBasedSamplerConfiguration>();
  EXPECT_THROW(MakeSampler(std::move(model)), config_sdk::UnsupportedException);
}

TEST_F(TraceBuildersTest, UnregisteredJaegerRemoteSamplerBuilder)
{
  registry_->SetJaegerRemoteSamplerBuilder(nullptr);
  std::unique_ptr<config_sdk::SamplerConfiguration> model =
      std::make_unique<config_sdk::JaegerRemoteSamplerConfiguration>();
  EXPECT_THROW(MakeSampler(std::move(model)), config_sdk::UnsupportedException);
}

TEST_F(TraceBuildersTest, UnregisteredCompositeSamplerBuilder)
{
  registry_->SetCompositeSamplerBuilder(nullptr);

  auto composite = std::make_unique<config_sdk::CompositeSamplerConfiguration>();
  composite->composable_sampler =
      std::make_unique<config_sdk::ComposableAlwaysOnSamplerConfiguration>();
  std::unique_ptr<config_sdk::SamplerConfiguration> model = std::move(composite);

  EXPECT_THROW(MakeSampler(std::move(model)), config_sdk::UnsupportedException);
}

TEST_F(TraceBuildersTest, UnregisteredExtensionSamplerBuilder)
{
  auto ext  = std::make_unique<config_sdk::ExtensionSamplerConfiguration>();
  ext->name = "unregistered";
  std::unique_ptr<config_sdk::SamplerConfiguration> model = std::move(ext);
  EXPECT_THROW(MakeSampler(std::move(model)), config_sdk::UnsupportedException);
}

TEST_F(TraceBuildersTest, UnregisteredTracerConfiguratorBuilder)
{
  registry_->SetTracerConfiguratorBuilder(nullptr);
  auto model = std::make_unique<config_sdk::TracerConfiguratorConfiguration>();
  EXPECT_THROW(MakeTracerConfigurator(std::move(model)), config_sdk::UnsupportedException);
}

TEST_F(TraceBuildersTest, UnregisteredExtensionSpanExporterBuilder)
{
  auto ext  = std::make_unique<config_sdk::ExtensionSpanExporterConfiguration>();
  ext->name = "unregistered";
  std::unique_ptr<config_sdk::SpanExporterConfiguration> model = std::move(ext);
  EXPECT_THROW(MakeSpanExporter(std::move(model)), config_sdk::UnsupportedException);
}

TEST_F(TraceBuildersTest, UnregisteredBatchSpanProcessorBuilder)
{
  registry_->SetBatchSpanProcessorBuilder(nullptr);

  auto exporter       = std::make_unique<config_sdk::ConsoleSpanExporterConfiguration>();
  auto processor      = std::make_unique<config_sdk::BatchSpanProcessorConfiguration>();
  processor->exporter = std::move(exporter);
  std::unique_ptr<config_sdk::SpanProcessorConfiguration> model = std::move(processor);

  EXPECT_THROW(MakeSpanProcessor(std::move(model)), config_sdk::UnsupportedException);
}

TEST_F(TraceBuildersTest, UnregisteredSimpleSpanProcessorBuilder)
{
  registry_->SetSimpleSpanProcessorBuilder(nullptr);

  auto exporter       = std::make_unique<config_sdk::ConsoleSpanExporterConfiguration>();
  auto processor      = std::make_unique<config_sdk::SimpleSpanProcessorConfiguration>();
  processor->exporter = std::move(exporter);
  std::unique_ptr<config_sdk::SpanProcessorConfiguration> model = std::move(processor);

  EXPECT_THROW(MakeSpanProcessor(std::move(model)), config_sdk::UnsupportedException);
}

TEST_F(TraceBuildersTest, UnregisteredExtensionSpanProcessorBuilder)
{
  auto ext  = std::make_unique<config_sdk::ExtensionSpanProcessorConfiguration>();
  ext->name = "unregistered";
  std::unique_ptr<config_sdk::SpanProcessorConfiguration> model = std::move(ext);
  EXPECT_THROW(MakeSpanProcessor(std::move(model)), config_sdk::UnsupportedException);
}

TEST_F(TraceBuildersTest, UnregisteredConsoleSpanExporterBuilder)
{
  registry_->SetConsoleSpanBuilder(nullptr);
  EXPECT_THROW(MakeSpanExporter(std::make_unique<config_sdk::ConsoleSpanExporterConfiguration>()),
               config_sdk::UnsupportedException);
}

TEST_F(TraceBuildersTest, UnregisteredOtlpHttpSpanExporterBuilder)
{
  registry_->SetOtlpHttpSpanBuilder(nullptr);
  EXPECT_THROW(MakeSpanExporter(std::make_unique<config_sdk::OtlpHttpSpanExporterConfiguration>()),
               config_sdk::UnsupportedException);
}

TEST_F(TraceBuildersTest, UnregisteredOtlpGrpcSpanExporterBuilder)
{
  registry_->SetOtlpGrpcSpanBuilder(nullptr);
  EXPECT_THROW(MakeSpanExporter(std::make_unique<config_sdk::OtlpGrpcSpanExporterConfiguration>()),
               config_sdk::UnsupportedException);
}

TEST_F(TraceBuildersTest, UnregisteredOtlpFileSpanExporterBuilder)
{
  registry_->SetOtlpFileSpanBuilder(nullptr);
  EXPECT_THROW(MakeSpanExporter(std::make_unique<config_sdk::OtlpFileSpanExporterConfiguration>()),
               config_sdk::UnsupportedException);
}

TEST_F(TraceBuildersTest, UnregisteredComposableAlwaysOnSamplerBuilder)
{
  registry_->SetComposableAlwaysOnSamplerBuilder(nullptr);

  auto composite = std::make_unique<config_sdk::CompositeSamplerConfiguration>();
  composite->composable_sampler =
      std::make_unique<config_sdk::ComposableAlwaysOnSamplerConfiguration>();
  std::unique_ptr<config_sdk::SamplerConfiguration> model = std::move(composite);

  EXPECT_THROW(MakeComposableSampler(std::move(model)), config_sdk::UnsupportedException);
}

TEST_F(TraceBuildersTest, UnregisteredComposableAlwaysOffSamplerBuilder)
{
  registry_->SetComposableAlwaysOffSamplerBuilder(nullptr);

  auto composite = std::make_unique<config_sdk::CompositeSamplerConfiguration>();
  composite->composable_sampler =
      std::make_unique<config_sdk::ComposableAlwaysOffSamplerConfiguration>();
  std::unique_ptr<config_sdk::SamplerConfiguration> model = std::move(composite);

  EXPECT_THROW(MakeComposableSampler(std::move(model)), config_sdk::UnsupportedException);
}

TEST_F(TraceBuildersTest, UnregisteredComposableProbabilitySamplerBuilder)
{
  registry_->SetComposableProbabilitySamplerBuilder(nullptr);

  auto composite = std::make_unique<config_sdk::CompositeSamplerConfiguration>();
  composite->composable_sampler =
      std::make_unique<config_sdk::ComposableProbabilitySamplerConfiguration>();
  std::unique_ptr<config_sdk::SamplerConfiguration> model = std::move(composite);

  EXPECT_THROW(MakeComposableSampler(std::move(model)), config_sdk::UnsupportedException);
}

TEST_F(TraceBuildersTest, UnregisteredComposableParentThresholdSamplerBuilder)
{
  registry_->SetComposableParentThresholdSamplerBuilder(nullptr);

  auto parent_config =
      std::make_unique<config_sdk::ComposableParentThresholdSamplerConfiguration>();
  parent_config->root = std::make_unique<config_sdk::ComposableAlwaysOnSamplerConfiguration>();
  auto composite      = std::make_unique<config_sdk::CompositeSamplerConfiguration>();
  composite->composable_sampler                           = std::move(parent_config);
  std::unique_ptr<config_sdk::SamplerConfiguration> model = std::move(composite);

  EXPECT_THROW(MakeComposableSampler(std::move(model)), config_sdk::UnsupportedException);
}

TEST_F(TraceBuildersTest, UnregisteredComposableRuleBasedSamplerBuilder)
{
  registry_->SetComposableRuleBasedSamplerBuilder(nullptr);

  auto rule_based = std::make_unique<config_sdk::ComposableRuleBasedSamplerConfiguration>();
  auto composite  = std::make_unique<config_sdk::CompositeSamplerConfiguration>();
  composite->composable_sampler                           = std::move(rule_based);
  std::unique_ptr<config_sdk::SamplerConfiguration> model = std::move(composite);

  EXPECT_THROW(MakeComposableSampler(std::move(model)), config_sdk::UnsupportedException);
}

TEST_F(TraceBuildersTest, UnregisteredComposableAlwaysOnBuilderInParentThreshold)
{
  // null root triggers a default AlwaysOn child; clearing that builder must throw.
  registry_->SetComposableAlwaysOnSamplerBuilder(nullptr);

  auto parent    = std::make_unique<config_sdk::ComposableParentThresholdSamplerConfiguration>();
  parent->root   = nullptr;
  auto composite = std::make_unique<config_sdk::CompositeSamplerConfiguration>();
  composite->composable_sampler                           = std::move(parent);
  std::unique_ptr<config_sdk::SamplerConfiguration> model = std::move(composite);

  EXPECT_THROW(MakeComposableSampler(std::move(model)), config_sdk::UnsupportedException);
}

// ---------------------------------------------------------------------------
// TracerProviderConfigTest: Use ConfiguredSdk::Create to verify tracer provider configuration and
// installation.

namespace
{

class TracerProviderConfigTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    registry_    = std::make_shared<config_sdk::Registry>();
    span_buffer_ = std::make_shared<config_test::SpanBuffer>();
    config_sdk::RegisterDefaultTraceBuilders(registry_.get());
  }

  void TearDown() override
  {
    if (sdk_)
    {
      sdk_->UnInstall();
    }
  }

  static std::unique_ptr<config_sdk::Configuration> MakeTracerProviderConfig(
      std::unique_ptr<config_sdk::SpanExporterConfiguration> exporter)
  {
    auto processor      = std::make_unique<config_sdk::SimpleSpanProcessorConfiguration>();
    processor->exporter = std::move(exporter);
    return MakeTracerProviderConfig(std::move(processor));
  }

  static std::unique_ptr<config_sdk::Configuration> MakeTracerProviderConfig(
      std::unique_ptr<config_sdk::SpanProcessorConfiguration> processor)
  {
    auto tp = std::make_unique<config_sdk::TracerProviderConfiguration>();
    tp->processors.emplace_back(std::move(processor));
    auto model             = std::make_unique<config_sdk::Configuration>();
    model->tracer_provider = std::move(tp);
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
    ASSERT_NE(sdk_->tracer_provider, nullptr);
    ASSERT_EQ(sdk_->meter_provider, nullptr);
    ASSERT_EQ(sdk_->logger_provider, nullptr);
    auto provider = trace_api::Provider::GetTracerProvider();
    ASSERT_NE(provider, nullptr);
    provider->GetTracer("tracer")->StartSpan("span")->End();
    ASSERT_TRUE(sdk_->tracer_provider->ForceFlush(std::chrono::milliseconds(5000)));
    ASSERT_TRUE(sdk_->tracer_provider->Shutdown(std::chrono::milliseconds(5000)));
    EXPECT_EQ(span_buffer_->size(), 1u);
  }

  std::shared_ptr<config_sdk::Registry> registry_;
  std::shared_ptr<config_test::SpanBuffer> span_buffer_;
  std::unique_ptr<config_sdk::ConfiguredSdk> sdk_;
};

}  // namespace

TEST_F(TracerProviderConfigTest, TracerProviderWithExtensionSpanExporter)
{
  registry_->SetExtensionSpanExporterBuilder(
      "recording", std::make_unique<config_test::RecordingSpanExporterBuilder>(span_buffer_));

  auto exporter  = std::make_unique<config_sdk::ExtensionSpanExporterConfiguration>();
  exporter->name = "recording";
  auto model     = MakeTracerProviderConfig(std::move(exporter));
  BuildAndInstall(model);
  EmitAndVerify();
}

TEST_F(TracerProviderConfigTest, TracerProviderWithConsoleSpanExporter)
{
  registry_->SetConsoleSpanBuilder(
      std::make_unique<config_test::RecordingConsoleSpanExporterBuilder>(span_buffer_));
  auto model =
      MakeTracerProviderConfig(std::make_unique<config_sdk::ConsoleSpanExporterConfiguration>());
  BuildAndInstall(model);
  EmitAndVerify();
}

TEST_F(TracerProviderConfigTest, TracerProviderWithOtlpHttpSpanExporter)
{
  registry_->SetOtlpHttpSpanBuilder(
      std::make_unique<config_test::RecordingOtlpHttpSpanExporterBuilder>(span_buffer_));
  auto model =
      MakeTracerProviderConfig(std::make_unique<config_sdk::OtlpHttpSpanExporterConfiguration>());
  BuildAndInstall(model);
  EmitAndVerify();
}

TEST_F(TracerProviderConfigTest, TracerProviderWithOtlpGrpcSpanExporter)
{
  registry_->SetOtlpGrpcSpanBuilder(
      std::make_unique<config_test::RecordingOtlpGrpcSpanExporterBuilder>(span_buffer_));
  auto model =
      MakeTracerProviderConfig(std::make_unique<config_sdk::OtlpGrpcSpanExporterConfiguration>());
  BuildAndInstall(model);
  EmitAndVerify();
}

TEST_F(TracerProviderConfigTest, TracerProviderWithOtlpFileSpanExporter)
{
  registry_->SetOtlpFileSpanBuilder(
      std::make_unique<config_test::RecordingOtlpFileSpanExporterBuilder>(span_buffer_));
  auto model =
      MakeTracerProviderConfig(std::make_unique<config_sdk::OtlpFileSpanExporterConfiguration>());
  BuildAndInstall(model);
  EmitAndVerify();
}

TEST_F(TracerProviderConfigTest, TracerProviderWithBatchSpanProcessor)
{
  registry_->SetBatchSpanProcessorBuilder(
      std::make_unique<config_test::MockBatchSpanProcessorBuilder>());
  registry_->SetExtensionSpanExporterBuilder(
      "recording", std::make_unique<config_test::RecordingSpanExporterBuilder>(span_buffer_));

  auto exporter       = std::make_unique<config_sdk::ExtensionSpanExporterConfiguration>();
  exporter->name      = "recording";
  auto processor      = std::make_unique<config_sdk::BatchSpanProcessorConfiguration>();
  processor->exporter = std::move(exporter);
  auto model          = MakeTracerProviderConfig(std::move(processor));
  BuildAndInstall(model);
  EmitAndVerify();
}
