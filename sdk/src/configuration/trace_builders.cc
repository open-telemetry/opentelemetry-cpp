// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/configuration/trace_builders.h"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/configuration/always_off_sampler_builder.h"
#include "opentelemetry/sdk/configuration/always_on_sampler_builder.h"
#include "opentelemetry/sdk/configuration/attribute_limits_configuration.h"
#include "opentelemetry/sdk/configuration/batch_span_processor_builder.h"
#include "opentelemetry/sdk/configuration/batch_span_processor_configuration.h"
#include "opentelemetry/sdk/configuration/composable_always_off_sampler_builder.h"
#include "opentelemetry/sdk/configuration/composable_always_on_sampler_builder.h"
#include "opentelemetry/sdk/configuration/composable_parent_threshold_sampler_builder.h"
#include "opentelemetry/sdk/configuration/composable_probability_sampler_builder.h"
#include "opentelemetry/sdk/configuration/composable_probability_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/composable_rule_based_sampler_builder.h"
#include "opentelemetry/sdk/configuration/composable_rule_based_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/composable_rule_based_sampler_rule_attribute_patterns_configuration.h"
#include "opentelemetry/sdk/configuration/composable_rule_based_sampler_rule_attribute_values_configuration.h"
#include "opentelemetry/sdk/configuration/composable_rule_based_sampler_rule_configuration.h"
#include "opentelemetry/sdk/configuration/composite_sampler_builder.h"
#include "opentelemetry/sdk/configuration/jaeger_remote_sampler_builder.h"
#include "opentelemetry/sdk/configuration/optional_value.h"
#include "opentelemetry/sdk/configuration/parent_based_sampler_builder.h"
#include "opentelemetry/sdk/configuration/parent_based_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/probability_sampler_builder.h"
#include "opentelemetry/sdk/configuration/probability_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/registry.h"
#include "opentelemetry/sdk/configuration/sampler_configuration.h"
#include "opentelemetry/sdk/configuration/simple_span_processor_builder.h"
#include "opentelemetry/sdk/configuration/span_limits_configuration.h"
#include "opentelemetry/sdk/configuration/trace_builder_utils.h"
#include "opentelemetry/sdk/configuration/trace_id_ratio_based_sampler_builder.h"
#include "opentelemetry/sdk/configuration/trace_id_ratio_based_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/tracer_config_configuration.h"
#include "opentelemetry/sdk/configuration/tracer_configurator_builder.h"
#include "opentelemetry/sdk/configuration/tracer_configurator_configuration.h"
#include "opentelemetry/sdk/configuration/tracer_matcher_and_config_configuration.h"
#include "opentelemetry/sdk/configuration/tracer_provider_builder.h"
#include "opentelemetry/sdk/configuration/tracer_provider_builder_context.h"
#include "opentelemetry/sdk/configuration/tracer_provider_configuration.h"
#include "opentelemetry/sdk/configuration/unsupported_exception.h"
#include "opentelemetry/sdk/instrumentationscope/instrumentation_scope.h"
#include "opentelemetry/sdk/instrumentationscope/scope_configurator.h"
#include "opentelemetry/sdk/trace/batch_span_processor_factory.h"
#include "opentelemetry/sdk/trace/batch_span_processor_options.h"
#include "opentelemetry/sdk/trace/id_generator.h"
#include "opentelemetry/sdk/trace/processor.h"
#include "opentelemetry/sdk/trace/random_id_generator_factory.h"
#include "opentelemetry/sdk/trace/sampler.h"
#include "opentelemetry/sdk/trace/samplers/always_off_factory.h"
#include "opentelemetry/sdk/trace/samplers/always_on_factory.h"
#include "opentelemetry/sdk/trace/samplers/composable_always_off.h"
#include "opentelemetry/sdk/trace/samplers/composable_always_on.h"
#include "opentelemetry/sdk/trace/samplers/composable_parent_threshold.h"
#include "opentelemetry/sdk/trace/samplers/composable_probability.h"
#include "opentelemetry/sdk/trace/samplers/composable_rule_based.h"
#include "opentelemetry/sdk/trace/samplers/composable_sampler.h"
#include "opentelemetry/sdk/trace/samplers/composite_sampler_factory.h"
#include "opentelemetry/sdk/trace/samplers/parent_factory.h"
#include "opentelemetry/sdk/trace/samplers/probability_factory.h"
#include "opentelemetry/sdk/trace/samplers/rule_based_predicate.h"
#include "opentelemetry/sdk/trace/samplers/trace_id_ratio_factory.h"
#include "opentelemetry/sdk/trace/simple_processor_factory.h"
#include "opentelemetry/sdk/trace/span_limits.h"
#include "opentelemetry/sdk/trace/tracer_config.h"
#include "opentelemetry/sdk/trace/tracer_context.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/sdk/trace/tracer_provider_factory.h"
#include "opentelemetry/version.h"
#include "src/common/wildcard_match.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

namespace
{

class DefaultAlwaysOnSamplerBuilder : public AlwaysOnSamplerBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::trace::Sampler> Build(
      const AlwaysOnSamplerConfiguration * /* model */) const override
  {
    return opentelemetry::sdk::trace::AlwaysOnSamplerFactory::Create();
  }
};

class DefaultAlwaysOffSamplerBuilder : public AlwaysOffSamplerBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::trace::Sampler> Build(
      const AlwaysOffSamplerConfiguration * /* model */) const override
  {
    return opentelemetry::sdk::trace::AlwaysOffSamplerFactory::Create();
  }
};

class DefaultTraceIdRatioBasedSamplerBuilder : public TraceIdRatioBasedSamplerBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::trace::Sampler> Build(
      const TraceIdRatioBasedSamplerConfiguration *model) const override
  {
    return opentelemetry::sdk::trace::TraceIdRatioBasedSamplerFactory::Create(model->ratio);
  }
};

class DefaultProbabilitySamplerBuilder : public ProbabilitySamplerBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::trace::Sampler> Build(
      const ProbabilitySamplerConfiguration *model) const override
  {
    return opentelemetry::sdk::trace::ProbabilitySamplerFactory::Create(model->ratio);
  }
};

class DefaultParentBasedSamplerBuilder : public ParentBasedSamplerBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::trace::Sampler> Build(
      const ParentBasedSamplerConfiguration * /* model */,
      std::unique_ptr<opentelemetry::sdk::trace::Sampler> &&root,
      std::unique_ptr<opentelemetry::sdk::trace::Sampler> &&remote_parent_sampled,
      std::unique_ptr<opentelemetry::sdk::trace::Sampler> &&remote_parent_not_sampled,
      std::unique_ptr<opentelemetry::sdk::trace::Sampler> &&local_parent_sampled,
      std::unique_ptr<opentelemetry::sdk::trace::Sampler> &&local_parent_not_sampled) const override
  {
    return opentelemetry::sdk::trace::ParentBasedSamplerFactory::Create(
        std::shared_ptr<opentelemetry::sdk::trace::Sampler>(std::move(root)),
        std::shared_ptr<opentelemetry::sdk::trace::Sampler>(std::move(remote_parent_sampled)),
        std::shared_ptr<opentelemetry::sdk::trace::Sampler>(std::move(remote_parent_not_sampled)),
        std::shared_ptr<opentelemetry::sdk::trace::Sampler>(std::move(local_parent_sampled)),
        std::shared_ptr<opentelemetry::sdk::trace::Sampler>(std::move(local_parent_not_sampled)));
  }
};

class DefaultJaegerRemoteSamplerBuilder : public JaegerRemoteSamplerBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::trace::Sampler> Build(
      const JaegerRemoteSamplerConfiguration * /* model */,
      std::unique_ptr<opentelemetry::sdk::trace::Sampler> &&initial_sampler) const override
  {
    auto unused = std::move(initial_sampler);
    static const std::string die("JaegerRemoteSampler not supported");
    throw UnsupportedException(die);
  }
};

class DefaultBatchSpanProcessorBuilder : public BatchSpanProcessorBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::trace::SpanProcessor> Build(
      const BatchSpanProcessorConfiguration *model,
      std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> &&exporter) const override
  {
    opentelemetry::sdk::trace::BatchSpanProcessorOptions options;
    options.schedule_delay_millis = std::chrono::milliseconds(model->schedule_delay);
    options.export_timeout        = std::chrono::milliseconds(model->export_timeout);
    options.max_queue_size        = model->max_queue_size;
    options.max_export_batch_size = model->max_export_batch_size;
    return opentelemetry::sdk::trace::BatchSpanProcessorFactory::Create(std::move(exporter),
                                                                        options);
  }
};

class DefaultSimpleSpanProcessorBuilder : public SimpleSpanProcessorBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::trace::SpanProcessor> Build(
      const SimpleSpanProcessorConfiguration * /* model */,
      std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> &&exporter) const override
  {
    return opentelemetry::sdk::trace::SimpleSpanProcessorFactory::Create(std::move(exporter));
  }
};

class DefaultComposableAlwaysOnSamplerBuilder : public ComposableAlwaysOnSamplerBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::trace::ComposableSampler> Build(
      const ComposableAlwaysOnSamplerConfiguration * /* model */) const override
  {
    return std::make_unique<opentelemetry::sdk::trace::ComposableAlwaysOnSampler>();
  }
};

class DefaultComposableAlwaysOffSamplerBuilder : public ComposableAlwaysOffSamplerBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::trace::ComposableSampler> Build(
      const ComposableAlwaysOffSamplerConfiguration * /* model */) const override
  {
    return std::make_unique<opentelemetry::sdk::trace::ComposableAlwaysOffSampler>();
  }
};

class DefaultComposableProbabilitySamplerBuilder : public ComposableProbabilitySamplerBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::trace::ComposableSampler> Build(
      const ComposableProbabilitySamplerConfiguration *model) const override
  {
    double ratio = model ? model->ratio : ComposableProbabilitySamplerConfiguration::kDefaultRatio;
    return std::make_unique<opentelemetry::sdk::trace::ComposableProbabilitySampler>(ratio);
  }
};

class DefaultComposableParentThresholdSamplerBuilder
    : public ComposableParentThresholdSamplerBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::trace::ComposableSampler> Build(
      const ComposableParentThresholdSamplerConfiguration * /* model */,
      std::unique_ptr<opentelemetry::sdk::trace::ComposableSampler> &&root) const override
  {
    return std::make_unique<opentelemetry::sdk::trace::ComposableParentThresholdSampler>(
        std::shared_ptr<opentelemetry::sdk::trace::ComposableSampler>(std::move(root)));
  }
};

class DefaultComposableRuleBasedSamplerBuilder : public ComposableRuleBasedSamplerBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::trace::ComposableSampler> Build(
      const ComposableRuleBasedSamplerConfiguration *model,
      std::vector<std::unique_ptr<opentelemetry::sdk::trace::ComposableSampler>> &&rule_samplers)
      const override
  {
    auto owned_samplers = std::move(rule_samplers);
    if (owned_samplers.size() != model->rules.size())
    {
      throw UnsupportedException("rule_samplers size does not match rules size");
    }
    std::vector<opentelemetry::sdk::trace::PredicatedSampler> rules;
    rules.reserve(model->rules.size());
    for (std::size_t i = 0; i < owned_samplers.size(); ++i)
    {
      // rule_samplers is index-aligned with model->rules; null means no sampler.
      if (owned_samplers[i] == nullptr)
      {
        continue;
      }
      opentelemetry::sdk::trace::PredicatedSampler predicated;
      predicated.predicate = std::make_shared<opentelemetry::sdk::trace::RuleBasedPredicate>(
          MakePredicateOptions(model->rules[i].get()));
      predicated.sampler = std::move(owned_samplers[i]);
      rules.push_back(std::move(predicated));
    }
    return std::make_unique<opentelemetry::sdk::trace::ComposableRuleBasedSampler>(
        std::move(rules));
  }

private:
  static opentelemetry::sdk::trace::RuleBasedPredicateOptions MakePredicateOptions(
      const opentelemetry::sdk::configuration::ComposableRuleBasedSamplerRuleConfiguration *rule)
  {
    opentelemetry::sdk::trace::RuleBasedPredicateOptions options;
    if (rule->attribute_values != nullptr)
    {
      options.match_values = true;
      options.values_key   = rule->attribute_values->key;
      options.values       = rule->attribute_values->values;
    }
    if (rule->attribute_patterns != nullptr)
    {
      options.match_patterns = true;
      options.patterns_key   = rule->attribute_patterns->key;
      options.included       = rule->attribute_patterns->included;
      options.excluded       = rule->attribute_patterns->excluded;
    }
    options.match_parent_none   = rule->match_parent_none;
    options.match_parent_remote = rule->match_parent_remote;
    options.match_parent_local  = rule->match_parent_local;

    options.match_span_kind_internal = rule->match_span_kind_internal;
    options.match_span_kind_server   = rule->match_span_kind_server;
    options.match_span_kind_client   = rule->match_span_kind_client;
    options.match_span_kind_producer = rule->match_span_kind_producer;
    options.match_span_kind_consumer = rule->match_span_kind_consumer;
    return options;
  }
};

class DefaultCompositeSamplerBuilder : public CompositeSamplerBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::trace::Sampler> Build(
      std::unique_ptr<opentelemetry::sdk::trace::ComposableSampler> &&sampler) const override
  {
    return opentelemetry::sdk::trace::CompositeSamplerFactory::Create(
        std::shared_ptr<opentelemetry::sdk::trace::ComposableSampler>(std::move(sampler)));
  }
};

class DefaultTracerConfiguratorBuilder : public TracerConfiguratorBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::instrumentationscope::ScopeConfigurator<
      opentelemetry::sdk::trace::TracerConfig>>
  Build(const TracerConfiguratorConfiguration *model) const override
  {
    using common::WildcardMatch;
    using opentelemetry::sdk::instrumentationscope::InstrumentationScope;
    using opentelemetry::sdk::instrumentationscope::ScopeConfigurator;
    using opentelemetry::sdk::trace::TracerConfig;

    TracerConfig default_config =
        model->default_config.enabled ? TracerConfig::Enabled() : TracerConfig::Disabled();
    auto builder = ScopeConfigurator<TracerConfig>::Builder(default_config);

    for (const auto &entry : model->tracers)
    {
      TracerConfig entry_config =
          entry.config.enabled ? TracerConfig::Enabled() : TracerConfig::Disabled();
      std::string pattern = entry.name;
      builder.AddCondition(
          [pattern](const InstrumentationScope &scope) {
            return WildcardMatch(pattern, scope.GetName());
          },
          entry_config);
    }

    return std::make_unique<ScopeConfigurator<TracerConfig>>(builder.Build());
  }
};

struct TracerProviderComponents
{
  std::vector<std::unique_ptr<opentelemetry::sdk::trace::SpanProcessor>> processors;
  std::unique_ptr<opentelemetry::sdk::trace::Sampler> sampler;
  std::unique_ptr<opentelemetry::sdk::trace::IdGenerator> id_generator;
  std::unique_ptr<opentelemetry::sdk::instrumentationscope::ScopeConfigurator<
      opentelemetry::sdk::trace::TracerConfig>>
      tracer_configurator;
  opentelemetry::sdk::trace::SpanLimits span_limits;
};

class DefaultTracerProviderBuilder : public TracerProviderBuilder
{
public:
  std::shared_ptr<opentelemetry::sdk::trace::TracerProvider> Build(
      const TracerProviderBuilderContext &context,
      const opentelemetry::sdk::configuration::TracerProviderConfiguration *model) const override
  {
    if (context.registry == nullptr || context.resource == nullptr)
    {
      static std::string message =
          "TracerProviderBuilderContext must have non-null registry and resource.";
      throw UnsupportedException(message);
    }

    auto components = BuildComponents(context, model);
    auto ctx        = CreateContext(context, std::move(components));
    return CreateProvider(std::move(ctx));
  }

private:
  static std::uint32_t ToUint32Limit(std::size_t value)
  {
    constexpr auto kMax = (std::numeric_limits<std::uint32_t>::max)();
    if (value > kMax)
    {
      return kMax;
    }
    return static_cast<std::uint32_t>(value);
  }

  /// Resolve one limit per
  /// https://opentelemetry.io/docs/specs/otel/common/#attribute-limits
  /// model-specific if set, else general if set, else the model-specific default.
  template <typename T>
  static T ResolveLimit(const OptionalValue<T> &model_specific,
                        const OptionalValue<T> &general,
                        T model_default)
  {
    if (model_specific.HasValue())
    {
      return model_specific.Value();
    }
    if (general.HasValue())
    {
      return general.Value();
    }
    return model_default;
  }

  static OptionalValue<std::uint32_t> ToOptionalUint32(const OptionalValue<std::size_t> &value)
  {
    if (!value.HasValue())
    {
      return OptionalValue<std::uint32_t>{};
    }
    return OptionalValue<std::uint32_t>{ToUint32Limit(value.Value())};
  }

  static TracerProviderComponents BuildComponents(
      const TracerProviderBuilderContext &context,
      const opentelemetry::sdk::configuration::TracerProviderConfiguration *model)
  {
    TracerProviderComponents components;

    if (model->sampler)
    {
      components.sampler = TraceBuilderUtils::CreateSampler(context.registry, model->sampler);
    }
    else
    {
      // Spec default: parentbased_always_on
      auto pb_config =
          std::make_unique<opentelemetry::sdk::configuration::ParentBasedSamplerConfiguration>();
      std::unique_ptr<opentelemetry::sdk::configuration::SamplerConfiguration> default_sampler =
          std::move(pb_config);
      components.sampler = TraceBuilderUtils::CreateSampler(context.registry, default_sampler);
    }

    for (const auto &processor_model : model->processors)
    {
      components.processors.push_back(
          TraceBuilderUtils::CreateSpanProcessor(context.registry, processor_model));
    }

    opentelemetry::sdk::trace::SpanLimits span_limits;
    OptionalValue<std::size_t> model_attribute_value_length_limit;
    OptionalValue<std::uint32_t> model_attribute_count_limit;
    OptionalValue<std::uint32_t> model_event_count_limit;
    OptionalValue<std::uint32_t> model_link_count_limit;
    OptionalValue<std::uint32_t> model_event_attribute_count_limit;
    OptionalValue<std::uint32_t> model_link_attribute_count_limit;
    if (model->limits)
    {
      model_attribute_value_length_limit = model->limits->attribute_value_length_limit;
      model_attribute_count_limit        = model->limits->attribute_count_limit;
      model_event_count_limit            = model->limits->event_count_limit;
      model_link_count_limit             = model->limits->link_count_limit;
      model_event_attribute_count_limit  = model->limits->event_attribute_count_limit;
      model_link_attribute_count_limit   = model->limits->link_attribute_count_limit;
    }

    OptionalValue<std::size_t> general_attribute_value_length_limit;
    OptionalValue<std::size_t> general_attribute_count_limit;
    if (context.attribute_limits)
    {
      general_attribute_value_length_limit = context.attribute_limits->attribute_value_length_limit;
      general_attribute_count_limit        = context.attribute_limits->attribute_count_limit;
    }

    using SpanLimitDefaults = SpanLimitsConfiguration;
    span_limits.attribute_value_length_limit =
        ResolveLimit(model_attribute_value_length_limit, general_attribute_value_length_limit,
                     SpanLimitDefaults::kDefaultAttributeValueLengthLimit);
    span_limits.attribute_count_limit =
        ResolveLimit(model_attribute_count_limit, ToOptionalUint32(general_attribute_count_limit),
                     SpanLimitDefaults::kDefaultAttributeCountLimit);
    span_limits.event_count_limit =
        ResolveLimit(model_event_count_limit, OptionalValue<std::uint32_t>{},
                     SpanLimitDefaults::kDefaultEventCountLimit);
    span_limits.link_count_limit =
        ResolveLimit(model_link_count_limit, OptionalValue<std::uint32_t>{},
                     SpanLimitDefaults::kDefaultLinkCountLimit);
    span_limits.event_attribute_count_limit =
        ResolveLimit(model_event_attribute_count_limit, OptionalValue<std::uint32_t>{},
                     SpanLimitDefaults::kDefaultEventAttributeCountLimit);
    span_limits.link_attribute_count_limit =
        ResolveLimit(model_link_attribute_count_limit, OptionalValue<std::uint32_t>{},
                     SpanLimitDefaults::kDefaultLinkAttributeCountLimit);

    components.span_limits = span_limits;

    if (model->tracer_configurator)
    {
      components.tracer_configurator =
          TraceBuilderUtils::CreateTracerConfigurator(context.registry, model->tracer_configurator);
    }
    else
    {
      auto default_model =
          std::make_unique<opentelemetry::sdk::configuration::TracerConfiguratorConfiguration>();
      components.tracer_configurator =
          TraceBuilderUtils::CreateTracerConfigurator(context.registry, default_model);
    }

    // FIXME-CONFIG: https://github.com/open-telemetry/opentelemetry-configuration/issues/70
    // FIXME-CONFIG: Add support for IdGenerator schema node
    components.id_generator = opentelemetry::sdk::trace::RandomIdGeneratorFactory::Create();

    return components;
  }

  static std::unique_ptr<opentelemetry::sdk::trace::TracerContext> CreateContext(
      const TracerProviderBuilderContext &context,
      TracerProviderComponents &&components)
  {
    auto owned_components = std::move(components);
    return std::make_unique<opentelemetry::sdk::trace::TracerContext>(
        std::move(owned_components.processors), *context.resource,
        std::move(owned_components.sampler), std::move(owned_components.id_generator),
        std::move(owned_components.tracer_configurator), owned_components.span_limits);
  }

  static std::shared_ptr<opentelemetry::sdk::trace::TracerProvider> CreateProvider(
      std::unique_ptr<opentelemetry::sdk::trace::TracerContext> &&ctx)
  {
    return opentelemetry::sdk::trace::TracerProviderFactory::Create(std::move(ctx));
  }
};

}  // namespace

void RegisterDefaultTraceBuilders(Registry *registry)
{
  registry->SetAlwaysOnSamplerBuilder(std::make_unique<DefaultAlwaysOnSamplerBuilder>());
  registry->SetAlwaysOffSamplerBuilder(std::make_unique<DefaultAlwaysOffSamplerBuilder>());
  registry->SetTraceIdRatioBasedSamplerBuilder(
      std::make_unique<DefaultTraceIdRatioBasedSamplerBuilder>());
  registry->SetProbabilitySamplerBuilder(std::make_unique<DefaultProbabilitySamplerBuilder>());
  registry->SetParentBasedSamplerBuilder(std::make_unique<DefaultParentBasedSamplerBuilder>());
  registry->SetJaegerRemoteSamplerBuilder(std::make_unique<DefaultJaegerRemoteSamplerBuilder>());
  registry->SetBatchSpanProcessorBuilder(std::make_unique<DefaultBatchSpanProcessorBuilder>());
  registry->SetSimpleSpanProcessorBuilder(std::make_unique<DefaultSimpleSpanProcessorBuilder>());
  registry->SetTracerConfiguratorBuilder(std::make_unique<DefaultTracerConfiguratorBuilder>());
  registry->SetComposableAlwaysOnSamplerBuilder(
      std::make_unique<DefaultComposableAlwaysOnSamplerBuilder>());
  registry->SetComposableAlwaysOffSamplerBuilder(
      std::make_unique<DefaultComposableAlwaysOffSamplerBuilder>());
  registry->SetComposableProbabilitySamplerBuilder(
      std::make_unique<DefaultComposableProbabilitySamplerBuilder>());
  registry->SetComposableParentThresholdSamplerBuilder(
      std::make_unique<DefaultComposableParentThresholdSamplerBuilder>());
  registry->SetComposableRuleBasedSamplerBuilder(
      std::make_unique<DefaultComposableRuleBasedSamplerBuilder>());
  registry->SetCompositeSamplerBuilder(std::make_unique<DefaultCompositeSamplerBuilder>());
  registry->SetTracerProviderBuilder(std::make_unique<DefaultTracerProviderBuilder>());
}

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
