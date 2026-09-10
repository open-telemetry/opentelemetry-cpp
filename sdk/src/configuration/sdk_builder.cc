// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <cstddef>
#include <cstdint>
#include <limits>
#include <map>
#include <memory>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/common/kv_properties.h"
#include "opentelemetry/context/propagation/composite_propagator.h"
#include "opentelemetry/context/propagation/text_map_propagator.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/configuration/always_off_sampler_builder.h"
#include "opentelemetry/sdk/configuration/always_off_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/always_on_sampler_builder.h"
#include "opentelemetry/sdk/configuration/always_on_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/attribute_limits_configuration.h"
#include "opentelemetry/sdk/configuration/attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/attribute_value_configuration_visitor.h"
#include "opentelemetry/sdk/configuration/attributes_configuration.h"
#include "opentelemetry/sdk/configuration/batch_span_processor_builder.h"
#include "opentelemetry/sdk/configuration/batch_span_processor_configuration.h"
#include "opentelemetry/sdk/configuration/boolean_array_attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/boolean_attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/composable_always_off_sampler_builder.h"
#include "opentelemetry/sdk/configuration/composable_always_on_sampler_builder.h"
#include "opentelemetry/sdk/configuration/composable_always_on_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/composable_parent_threshold_sampler_builder.h"
#include "opentelemetry/sdk/configuration/composable_parent_threshold_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/composable_probability_sampler_builder.h"
#include "opentelemetry/sdk/configuration/composable_rule_based_sampler_builder.h"
#include "opentelemetry/sdk/configuration/composable_rule_based_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/composable_rule_based_sampler_rule_configuration.h"
#include "opentelemetry/sdk/configuration/composable_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/composable_sampler_configuration_visitor.h"
#include "opentelemetry/sdk/configuration/composite_sampler_builder.h"
#include "opentelemetry/sdk/configuration/composite_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/configuration.h"
#include "opentelemetry/sdk/configuration/configured_sdk.h"
#include "opentelemetry/sdk/configuration/console_span_exporter_builder.h"
#include "opentelemetry/sdk/configuration/console_span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/container_resource_detector_builder.h"
#include "opentelemetry/sdk/configuration/container_resource_detector_configuration.h"
#include "opentelemetry/sdk/configuration/double_array_attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/double_attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/extension_composable_sampler_builder.h"
#include "opentelemetry/sdk/configuration/extension_composable_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/extension_resource_detector_builder.h"
#include "opentelemetry/sdk/configuration/extension_resource_detector_configuration.h"
#include "opentelemetry/sdk/configuration/extension_sampler_builder.h"
#include "opentelemetry/sdk/configuration/extension_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/extension_span_exporter_builder.h"
#include "opentelemetry/sdk/configuration/extension_span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/extension_span_processor_builder.h"
#include "opentelemetry/sdk/configuration/extension_span_processor_configuration.h"
#include "opentelemetry/sdk/configuration/host_resource_detector_builder.h"
#include "opentelemetry/sdk/configuration/host_resource_detector_configuration.h"
#include "opentelemetry/sdk/configuration/include_exclude_configuration.h"
#include "opentelemetry/sdk/configuration/integer_array_attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/integer_attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/jaeger_remote_sampler_builder.h"
#include "opentelemetry/sdk/configuration/jaeger_remote_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/logger_provider_builder.h"
#include "opentelemetry/sdk/configuration/logger_provider_builder_context.h"
#include "opentelemetry/sdk/configuration/meter_provider_builder.h"
#include "opentelemetry/sdk/configuration/meter_provider_builder_context.h"
#include "opentelemetry/sdk/configuration/otlp_file_span_exporter_builder.h"
#include "opentelemetry/sdk/configuration/otlp_grpc_span_exporter_builder.h"
#include "opentelemetry/sdk/configuration/otlp_http_span_exporter_builder.h"
#include "opentelemetry/sdk/configuration/otlp_http_span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/parent_based_sampler_builder.h"
#include "opentelemetry/sdk/configuration/parent_based_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/probability_sampler_builder.h"
#include "opentelemetry/sdk/configuration/probability_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/process_resource_detector_builder.h"
#include "opentelemetry/sdk/configuration/process_resource_detector_configuration.h"
#include "opentelemetry/sdk/configuration/propagator_configuration.h"
#include "opentelemetry/sdk/configuration/registry.h"
#include "opentelemetry/sdk/configuration/resource_configuration.h"
#include "opentelemetry/sdk/configuration/resource_detection_configuration.h"
#include "opentelemetry/sdk/configuration/resource_detector_configuration.h"
#include "opentelemetry/sdk/configuration/resource_detector_configuration_visitor.h"
#include "opentelemetry/sdk/configuration/sampler_configuration.h"
#include "opentelemetry/sdk/configuration/sampler_configuration_visitor.h"
#include "opentelemetry/sdk/configuration/sdk_builder.h"
#include "opentelemetry/sdk/configuration/service_resource_detector_builder.h"
#include "opentelemetry/sdk/configuration/service_resource_detector_configuration.h"
#include "opentelemetry/sdk/configuration/severity_number.h"
#include "opentelemetry/sdk/configuration/simple_span_processor_builder.h"
#include "opentelemetry/sdk/configuration/simple_span_processor_configuration.h"
#include "opentelemetry/sdk/configuration/span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/span_exporter_configuration_visitor.h"
#include "opentelemetry/sdk/configuration/span_limits_configuration.h"
#include "opentelemetry/sdk/configuration/span_processor_configuration.h"
#include "opentelemetry/sdk/configuration/span_processor_configuration_visitor.h"
#include "opentelemetry/sdk/configuration/string_array_attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/string_array_configuration.h"
#include "opentelemetry/sdk/configuration/string_attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/text_map_propagator_builder.h"
#include "opentelemetry/sdk/configuration/trace_id_ratio_based_sampler_builder.h"
#include "opentelemetry/sdk/configuration/trace_id_ratio_based_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/tracer_configurator_builder.h"
#include "opentelemetry/sdk/configuration/tracer_configurator_configuration.h"
#include "opentelemetry/sdk/configuration/tracer_provider_configuration.h"
#include "opentelemetry/sdk/configuration/unsupported_exception.h"
#include "opentelemetry/sdk/instrumentationscope/scope_configurator.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/sdk/resource/resource_detector.h"
#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/id_generator.h"
#include "opentelemetry/sdk/trace/processor.h"
#include "opentelemetry/sdk/trace/random_id_generator_factory.h"
#include "opentelemetry/sdk/trace/sampler.h"
#include "opentelemetry/sdk/trace/samplers/composable_sampler.h"
#include "opentelemetry/sdk/trace/span_limits.h"
#include "opentelemetry/sdk/trace/tracer_config.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/sdk/trace/tracer_provider_factory.h"
#include "opentelemetry/version.h"
#include "src/common/wildcard_match.h"

#include "src/resource/detail/percent_decode.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

namespace
{

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

class ResourceAttributeValueSetter
    : public opentelemetry::sdk::configuration::AttributeValueConfigurationVisitor
{
public:
  ResourceAttributeValueSetter(
      opentelemetry::sdk::resource::ResourceAttributes &resource_attributes,
      const std::string &name)
      : resource_attributes_(resource_attributes), name_(name)
  {}
  ResourceAttributeValueSetter(ResourceAttributeValueSetter &&)                      = delete;
  ResourceAttributeValueSetter(const ResourceAttributeValueSetter &)                 = delete;
  ResourceAttributeValueSetter &operator=(ResourceAttributeValueSetter &&)           = delete;
  ResourceAttributeValueSetter &operator=(const ResourceAttributeValueSetter &other) = delete;
  ~ResourceAttributeValueSetter() override                                           = default;

  void VisitString(
      const opentelemetry::sdk::configuration::StringAttributeValueConfiguration *model) override
  {
    opentelemetry::common::AttributeValue attr_value(model->value);
    resource_attributes_.SetAttribute(name_, attr_value);
  }

  void VisitInteger(
      const opentelemetry::sdk::configuration::IntegerAttributeValueConfiguration *model) override
  {
    /* Provide exact type to opentelemetry::common::AttributeValue variant. */
    int64_t value = model->value;
    opentelemetry::common::AttributeValue attr_value(value);
    resource_attributes_.SetAttribute(name_, attr_value);
  }

  void VisitDouble(
      const opentelemetry::sdk::configuration::DoubleAttributeValueConfiguration *model) override
  {
    opentelemetry::common::AttributeValue attr_value(model->value);
    resource_attributes_.SetAttribute(name_, attr_value);
  }

  void VisitBoolean(
      const opentelemetry::sdk::configuration::BooleanAttributeValueConfiguration *model) override
  {
    opentelemetry::common::AttributeValue attr_value(model->value);
    resource_attributes_.SetAttribute(name_, attr_value);
  }

  void VisitStringArray(
      const opentelemetry::sdk::configuration::StringArrayAttributeValueConfiguration *model)
      override
  {
    size_t length = model->value.size();
    std::vector<nostd::string_view> string_view_array(length);

    // We have: std::vector<std::string>
    // We need: nostd::span<const nostd::string_view>

    for (size_t i = 0; i < length; i++)
    {
      string_view_array[i] = model->value[i];
    }

    nostd::span<const nostd::string_view> span(string_view_array.data(), string_view_array.size());

    opentelemetry::common::AttributeValue attr_value(span);
    resource_attributes_.SetAttribute(name_, attr_value);
  }

  void VisitIntegerArray(
      const opentelemetry::sdk::configuration::IntegerArrayAttributeValueConfiguration *model)
      override
  {
    size_t length = model->value.size();
    std::vector<int64_t> int_array(length);

    // We have: std::vector<size_t>
    // We need: nostd::span<const int64_t>

    for (size_t i = 0; i < length; i++)
    {
      int_array[i] = static_cast<int64_t>(model->value[i]);
    }

    nostd::span<const int64_t> span(int_array.data(), int_array.size());

    opentelemetry::common::AttributeValue attr_value(span);
    resource_attributes_.SetAttribute(name_, attr_value);
  }

  void VisitDoubleArray(
      const opentelemetry::sdk::configuration::DoubleArrayAttributeValueConfiguration *model)
      override
  {
    // We have: std::vector<double>
    // We need: nostd::span<const double>
    // so no data conversion needed

    nostd::span<const double> span(model->value.data(), model->value.size());

    opentelemetry::common::AttributeValue attr_value(span);
    resource_attributes_.SetAttribute(name_, attr_value);
  }

  void VisitBooleanArray(
      const opentelemetry::sdk::configuration::BooleanArrayAttributeValueConfiguration *model)
      override
  {
    size_t length = model->value.size();

    // Can not use std::vector<bool>,
    // it has no data() to convert it to a span
    std::unique_ptr<bool[]> bool_array(new bool[length]);

    // We have: std::vector<bool>
    // We need: nostd::span<const bool>

    for (size_t i = 0; i < length; i++)
    {
      bool_array[i] = model->value[i];
    }

    nostd::span<const bool> span(&bool_array[0], length);

    opentelemetry::common::AttributeValue attr_value(span);
    resource_attributes_.SetAttribute(name_, attr_value);
  }

  opentelemetry::common::AttributeValue attribute_value;

private:
  opentelemetry::sdk::resource::ResourceAttributes &resource_attributes_;
  std::string name_;
};

class ComposableSamplerBuilder
    : public opentelemetry::sdk::configuration::ComposableSamplerConfigurationVisitor
{
public:
  // Maximum nesting depth of composable samplers, root included.
  static constexpr std::size_t kMaxDepth = 10;

  ComposableSamplerBuilder(const Registry *registry, std::size_t depth)
      : registry_(registry), depth_(depth)
  {}
  ComposableSamplerBuilder(ComposableSamplerBuilder &&)                      = delete;
  ComposableSamplerBuilder(const ComposableSamplerBuilder &)                 = delete;
  ComposableSamplerBuilder &operator=(ComposableSamplerBuilder &&)           = delete;
  ComposableSamplerBuilder &operator=(const ComposableSamplerBuilder &other) = delete;
  ~ComposableSamplerBuilder() override                                       = default;

  // The yaml schema supports infinite sampler nesting due to the
  // ExperimentalComposableRuleBasedSampler containing an array of
  // ExperimentalComposableRuleBasedSamplerRule which each may contain a
  // ExperimentalComposableRuleBasedSampler (See: schema/tracer_provider.yaml#L340).
  // Recursion is used to build the nested samplers, but the depth of recursion is limited to
  // NOLINTBEGIN(misc-no-recursion)
  void VisitComposableAlwaysOff(
      const opentelemetry::sdk::configuration::ComposableAlwaysOffSamplerConfiguration *model)
      override
  {
    const ComposableAlwaysOffSamplerBuilder *builder =
        registry_->GetComposableAlwaysOffSamplerBuilder();
    if (builder != nullptr)
    {
      OTEL_INTERNAL_LOG_DEBUG("[SDK Builder] VisitComposableAlwaysOff() using registered builder");
      sampler = builder->Build(model);
      return;
    }
    static const std::string die("No builder for ComposableAlwaysOffSampler");
    throw UnsupportedException(die);
  }

  void VisitComposableAlwaysOn(
      const opentelemetry::sdk::configuration::ComposableAlwaysOnSamplerConfiguration *model)
      override
  {
    const ComposableAlwaysOnSamplerBuilder *builder =
        registry_->GetComposableAlwaysOnSamplerBuilder();
    if (builder != nullptr)
    {
      OTEL_INTERNAL_LOG_DEBUG("[SDK Builder] VisitComposableAlwaysOn() using registered builder");
      sampler = builder->Build(model);
      return;
    }
    static const std::string die("No builder for ComposableAlwaysOnSampler");
    throw UnsupportedException(die);
  }

  void VisitComposableProbability(
      const opentelemetry::sdk::configuration::ComposableProbabilitySamplerConfiguration *model)
      override
  {
    const ComposableProbabilitySamplerBuilder *builder =
        registry_->GetComposableProbabilitySamplerBuilder();
    if (builder != nullptr)
    {
      OTEL_INTERNAL_LOG_DEBUG(
          "[SDK Builder] VisitComposableProbability() using registered builder");
      sampler = builder->Build(model);
      return;
    }
    static const std::string die("No builder for ComposableProbabilitySampler");
    throw UnsupportedException(die);
  }

  void VisitComposableParentThreshold(
      const opentelemetry::sdk::configuration::ComposableParentThresholdSamplerConfiguration *model)
      override
  {
    std::unique_ptr<opentelemetry::sdk::trace::ComposableSampler> root;
    if (model->root != nullptr)
    {
      root = BuildChild(model->root.get());
    }
    else
    {
      static const opentelemetry::sdk::configuration::ComposableAlwaysOnSamplerConfiguration
          kAlwaysOn;
      const ComposableAlwaysOnSamplerBuilder *ao_builder =
          registry_->GetComposableAlwaysOnSamplerBuilder();
      if (ao_builder == nullptr)
      {
        static const std::string die("No builder for ComposableAlwaysOnSampler");
        throw UnsupportedException(die);
      }
      root = ao_builder->Build(&kAlwaysOn);
    }
    const ComposableParentThresholdSamplerBuilder *builder =
        registry_->GetComposableParentThresholdSamplerBuilder();
    if (builder != nullptr)
    {
      OTEL_INTERNAL_LOG_DEBUG(
          "[SDK Builder] VisitComposableParentThreshold() using registered builder");
      sampler = builder->Build(model, std::move(root));
      return;
    }
    static const std::string die("No builder for ComposableParentThresholdSampler");
    throw UnsupportedException(die);
  }

  void VisitComposableRuleBased(
      const opentelemetry::sdk::configuration::ComposableRuleBasedSamplerConfiguration *model)
      override
  {
    // Index-aligned with model->rules, null for rules with no sampler.
    std::vector<std::unique_ptr<opentelemetry::sdk::trace::ComposableSampler>> rule_samplers;
    rule_samplers.reserve(model->rules.size());
    for (const auto &rule : model->rules)
    {
      if (rule == nullptr || rule->sampler == nullptr)
      {
        OTEL_INTERNAL_LOG_WARN("Ignoring a rule with no sampler");
        rule_samplers.push_back(nullptr);
        continue;
      }
      rule_samplers.push_back(BuildChild(rule->sampler.get()));
    }
    const ComposableRuleBasedSamplerBuilder *builder =
        registry_->GetComposableRuleBasedSamplerBuilder();
    if (builder != nullptr)
    {
      OTEL_INTERNAL_LOG_DEBUG("[SDK Builder] VisitComposableRuleBased() using registered builder");
      sampler = builder->Build(model, std::move(rule_samplers));
      return;
    }
    static const std::string die("No builder for ComposableRuleBasedSampler");
    throw UnsupportedException(die);
  }

  void VisitComposableExtension(
      const opentelemetry::sdk::configuration::ExtensionComposableSamplerConfiguration *model)
      override
  {
    const ExtensionComposableSamplerBuilder *builder =
        registry_->GetExtensionComposableSamplerBuilder(model->name);
    if (builder != nullptr)
    {
      OTEL_INTERNAL_LOG_DEBUG("[SDK Builder] VisitComposableExtension() using registered builder "
                              << model->name);
      sampler = builder->Build(model);
      return;
    }
    std::string die("No builder for extension composable sampler ");
    die.append(model->name);
    throw UnsupportedException(die);
  }
  // NOLINTEND(misc-no-recursion)

  std::unique_ptr<opentelemetry::sdk::trace::ComposableSampler> sampler;

private:
  // NOLINTBEGIN(misc-no-recursion)
  std::unique_ptr<opentelemetry::sdk::trace::ComposableSampler> BuildChild(
      const opentelemetry::sdk::configuration::ComposableSamplerConfiguration *child_model)
  {
    if (depth_ + 1 > kMaxDepth)
    {
      std::string die("Composable sampler nesting depth exceeds ");
      die.append(std::to_string(kMaxDepth));
      throw UnsupportedException(die);
    }
    ComposableSamplerBuilder child(registry_, depth_ + 1);
    child_model->Accept(&child);
    return std::move(child.sampler);
  }
  // NOLINTEND(misc-no-recursion)

  const Registry *registry_;
  std::size_t depth_;
};

class SamplerBuilder : public opentelemetry::sdk::configuration::SamplerConfigurationVisitor
{
public:
  SamplerBuilder(const SdkBuilder *b) : sdk_builder_(b) {}
  SamplerBuilder(SamplerBuilder &&)                      = delete;
  SamplerBuilder(const SamplerBuilder &)                 = delete;
  SamplerBuilder &operator=(SamplerBuilder &&)           = delete;
  SamplerBuilder &operator=(const SamplerBuilder &other) = delete;
  ~SamplerBuilder() override                             = default;

  void VisitAlwaysOff(
      const opentelemetry::sdk::configuration::AlwaysOffSamplerConfiguration *model) override
  {
    sampler = sdk_builder_->CreateAlwaysOffSampler(model);
  }

  void VisitAlwaysOn(
      const opentelemetry::sdk::configuration::AlwaysOnSamplerConfiguration *model) override
  {
    sampler = sdk_builder_->CreateAlwaysOnSampler(model);
  }

  void VisitJaegerRemote(
      const opentelemetry::sdk::configuration::JaegerRemoteSamplerConfiguration *model) override
  {
    sampler = sdk_builder_->CreateJaegerRemoteSampler(model);
  }

  void VisitParentBased(
      const opentelemetry::sdk::configuration::ParentBasedSamplerConfiguration *model) override
  {
    sampler = sdk_builder_->CreateParentBasedSampler(model);
  }

  void VisitProbability(
      const opentelemetry::sdk::configuration::ProbabilitySamplerConfiguration *model) override
  {
    sampler = sdk_builder_->CreateProbabilitySampler(model);
  }

  void VisitTraceIdRatioBased(
      const opentelemetry::sdk::configuration::TraceIdRatioBasedSamplerConfiguration *model)
      override
  {
    sampler = sdk_builder_->CreateTraceIdRatioBasedSampler(model);
  }

  void VisitExtension(
      const opentelemetry::sdk::configuration::ExtensionSamplerConfiguration *model) override
  {
    sampler = sdk_builder_->CreateExtensionSampler(model);
  }

  void VisitComposite(
      const opentelemetry::sdk::configuration::CompositeSamplerConfiguration *model) override
  {
    sampler = sdk_builder_->CreateCompositeSampler(model->composable_sampler.get());
  }

  std::unique_ptr<opentelemetry::sdk::trace::Sampler> sampler;

private:
  const SdkBuilder *sdk_builder_;
};

class SpanProcessorBuilder
    : public opentelemetry::sdk::configuration::SpanProcessorConfigurationVisitor
{
public:
  SpanProcessorBuilder(const SdkBuilder *b) : sdk_builder_(b) {}
  SpanProcessorBuilder(SpanProcessorBuilder &&)                      = delete;
  SpanProcessorBuilder(const SpanProcessorBuilder &)                 = delete;
  SpanProcessorBuilder &operator=(SpanProcessorBuilder &&)           = delete;
  SpanProcessorBuilder &operator=(const SpanProcessorBuilder &other) = delete;
  ~SpanProcessorBuilder() override                                   = default;

  void VisitBatch(
      const opentelemetry::sdk::configuration::BatchSpanProcessorConfiguration *model) override
  {
    processor = sdk_builder_->CreateBatchSpanProcessor(model);
  }

  void VisitSimple(
      const opentelemetry::sdk::configuration::SimpleSpanProcessorConfiguration *model) override
  {
    processor = sdk_builder_->CreateSimpleSpanProcessor(model);
  }

  void VisitExtension(
      const opentelemetry::sdk::configuration::ExtensionSpanProcessorConfiguration *model) override
  {
    processor = sdk_builder_->CreateExtensionSpanProcessor(model);
  }

  std::unique_ptr<opentelemetry::sdk::trace::SpanProcessor> processor;

private:
  const SdkBuilder *sdk_builder_;
};

class SpanExporterBuilder
    : public opentelemetry::sdk::configuration::SpanExporterConfigurationVisitor
{
public:
  SpanExporterBuilder(const SdkBuilder *b) : sdk_builder_(b) {}
  SpanExporterBuilder(SpanExporterBuilder &&)                      = delete;
  SpanExporterBuilder(const SpanExporterBuilder &)                 = delete;
  SpanExporterBuilder &operator=(SpanExporterBuilder &&)           = delete;
  SpanExporterBuilder &operator=(const SpanExporterBuilder &other) = delete;
  ~SpanExporterBuilder() override                                  = default;

  void VisitOtlpHttp(
      const opentelemetry::sdk::configuration::OtlpHttpSpanExporterConfiguration *model) override
  {
    exporter = sdk_builder_->CreateOtlpHttpSpanExporter(model);
  }

  void VisitOtlpGrpc(
      const opentelemetry::sdk::configuration::OtlpGrpcSpanExporterConfiguration *model) override
  {
    exporter = sdk_builder_->CreateOtlpGrpcSpanExporter(model);
  }

  void VisitOtlpFile(
      const opentelemetry::sdk::configuration::OtlpFileSpanExporterConfiguration *model) override
  {
    exporter = sdk_builder_->CreateOtlpFileSpanExporter(model);
  }

  void VisitConsole(
      const opentelemetry::sdk::configuration::ConsoleSpanExporterConfiguration *model) override
  {
    exporter = sdk_builder_->CreateConsoleSpanExporter(model);
  }

  void VisitExtension(
      const opentelemetry::sdk::configuration::ExtensionSpanExporterConfiguration *model) override
  {
    exporter = sdk_builder_->CreateExtensionSpanExporter(model);
  }

  std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> exporter;

private:
  const SdkBuilder *sdk_builder_;
};

class ResourceDetectorBuilder
    : public opentelemetry::sdk::configuration::ResourceDetectorConfigurationVisitor
{
public:
  ResourceDetectorBuilder(const SdkBuilder *b) : sdk_builder_(b) {}
  ResourceDetectorBuilder(ResourceDetectorBuilder &&)                      = delete;
  ResourceDetectorBuilder(const ResourceDetectorBuilder &)                 = delete;
  ResourceDetectorBuilder &operator=(ResourceDetectorBuilder &&)           = delete;
  ResourceDetectorBuilder &operator=(const ResourceDetectorBuilder &other) = delete;
  ~ResourceDetectorBuilder() override                                      = default;

  void VisitContainer(
      const opentelemetry::sdk::configuration::ContainerResourceDetectorConfiguration *model)
      override
  {
    detector = sdk_builder_->CreateContainerResourceDetector(model);
  }

  void VisitHost(
      const opentelemetry::sdk::configuration::HostResourceDetectorConfiguration *model) override
  {
    detector = sdk_builder_->CreateHostResourceDetector(model);
  }

  void VisitProcess(
      const opentelemetry::sdk::configuration::ProcessResourceDetectorConfiguration *model) override
  {
    detector = sdk_builder_->CreateProcessResourceDetector(model);
  }

  void VisitService(
      const opentelemetry::sdk::configuration::ServiceResourceDetectorConfiguration *model) override
  {
    detector = sdk_builder_->CreateServiceResourceDetector(model);
  }

  void VisitExtension(
      const opentelemetry::sdk::configuration::ExtensionResourceDetectorConfiguration *model)
      override
  {
    detector = sdk_builder_->CreateExtensionResourceDetector(model);
  }

  std::unique_ptr<opentelemetry::sdk::resource::ResourceDetector> detector;

private:
  const SdkBuilder *sdk_builder_;
};

bool ResourceAttributeKeyMatches(
    const opentelemetry::sdk::configuration::IncludeExcludeConfiguration *attributes,
    const std::string &key)
{
  using opentelemetry::sdk::common::WildcardMatch;

  bool included = true;

  if (attributes->included != nullptr && !attributes->included->string_array.empty())
  {
    included = false;
    for (const auto &pattern : attributes->included->string_array)
    {
      if (WildcardMatch(pattern, key))
      {
        included = true;
        break;
      }
    }
  }

  if (!included)
  {
    return false;
  }

  // excluded is applied after included, and wins.
  if (attributes->excluded != nullptr)
  {
    for (const auto &pattern : attributes->excluded->string_array)
    {
      if (WildcardMatch(pattern, key))
      {
        return false;
      }
    }
  }

  return true;
}
}  // namespace

std::unique_ptr<opentelemetry::sdk::trace::Sampler> SdkBuilder::CreateAlwaysOffSampler(
    const opentelemetry::sdk::configuration::AlwaysOffSamplerConfiguration *model) const
{
  const AlwaysOffSamplerBuilder *builder = registry_->GetAlwaysOffSamplerBuilder();
  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("[SDK Builder] CreateAlwaysOffSampler() using registered builder");
    return builder->Build(model);
  }
  static const std::string die("No builder for AlwaysOffSampler");
  throw UnsupportedException(die);
}

std::unique_ptr<opentelemetry::sdk::trace::Sampler> SdkBuilder::CreateAlwaysOnSampler(
    const opentelemetry::sdk::configuration::AlwaysOnSamplerConfiguration *model) const
{
  const AlwaysOnSamplerBuilder *builder = registry_->GetAlwaysOnSamplerBuilder();
  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("[SDK Builder] CreateAlwaysOnSampler() using registered builder");
    return builder->Build(model);
  }
  static const std::string die("No builder for AlwaysOnSampler");
  throw UnsupportedException(die);
}

std::unique_ptr<opentelemetry::sdk::trace::Sampler> SdkBuilder::CreateJaegerRemoteSampler(
    const opentelemetry::sdk::configuration::JaegerRemoteSamplerConfiguration *model) const
{
  const JaegerRemoteSamplerBuilder *builder = registry_->GetJaegerRemoteSamplerBuilder();
  if (builder != nullptr)
  {
    static const opentelemetry::sdk::configuration::AlwaysOnSamplerConfiguration kAlwaysOn;
    auto initial_sampler = CreateAlwaysOnSampler(&kAlwaysOn);
    OTEL_INTERNAL_LOG_DEBUG("[SDK Builder] CreateJaegerRemoteSampler() using registered builder");
    return builder->Build(model, std::move(initial_sampler));
  }
  static const std::string die("No builder for JaegerRemoteSampler");
  throw UnsupportedException(die);
}

std::unique_ptr<opentelemetry::sdk::trace::Sampler> SdkBuilder::CreateParentBasedSampler(
    const opentelemetry::sdk::configuration::ParentBasedSamplerConfiguration *model) const
{
  std::unique_ptr<opentelemetry::sdk::trace::Sampler> sdk;
  std::unique_ptr<opentelemetry::sdk::trace::Sampler> remote_parent_sampled_sdk;
  std::unique_ptr<opentelemetry::sdk::trace::Sampler> remote_parent_not_sampled_sdk;
  std::unique_ptr<opentelemetry::sdk::trace::Sampler> local_parent_sampled_sdk;
  std::unique_ptr<opentelemetry::sdk::trace::Sampler> local_parent_not_sampled_sdk;

  std::unique_ptr<opentelemetry::sdk::trace::Sampler> root_sdk;
  if (model->root)
  {
    root_sdk = SdkBuilder::CreateSampler(model->root);
  }
  else
  {
    static const opentelemetry::sdk::configuration::AlwaysOnSamplerConfiguration kAlwaysOn;
    root_sdk = CreateAlwaysOnSampler(&kAlwaysOn);
  }

  if (model->remote_parent_sampled != nullptr)
  {
    remote_parent_sampled_sdk = SdkBuilder::CreateSampler(model->remote_parent_sampled);
  }
  else
  {
    static const opentelemetry::sdk::configuration::AlwaysOnSamplerConfiguration kAlwaysOn;
    remote_parent_sampled_sdk = CreateAlwaysOnSampler(&kAlwaysOn);
  }

  if (model->remote_parent_not_sampled != nullptr)
  {
    remote_parent_not_sampled_sdk = SdkBuilder::CreateSampler(model->remote_parent_not_sampled);
  }
  else
  {
    static const opentelemetry::sdk::configuration::AlwaysOffSamplerConfiguration kAlwaysOff;
    remote_parent_not_sampled_sdk = CreateAlwaysOffSampler(&kAlwaysOff);
  }

  if (model->local_parent_sampled != nullptr)
  {
    local_parent_sampled_sdk = SdkBuilder::CreateSampler(model->local_parent_sampled);
  }
  else
  {
    static const opentelemetry::sdk::configuration::AlwaysOnSamplerConfiguration kAlwaysOn;
    local_parent_sampled_sdk = CreateAlwaysOnSampler(&kAlwaysOn);
  }

  if (model->local_parent_not_sampled != nullptr)
  {
    local_parent_not_sampled_sdk = SdkBuilder::CreateSampler(model->local_parent_not_sampled);
  }
  else
  {
    static const opentelemetry::sdk::configuration::AlwaysOffSamplerConfiguration kAlwaysOff;
    local_parent_not_sampled_sdk = CreateAlwaysOffSampler(&kAlwaysOff);
  }

  const ParentBasedSamplerBuilder *builder = registry_->GetParentBasedSamplerBuilder();
  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("[SDK Builder] CreateParentBasedSampler() using registered builder");
    return builder->Build(model, std::move(root_sdk), std::move(remote_parent_sampled_sdk),
                          std::move(remote_parent_not_sampled_sdk),
                          std::move(local_parent_sampled_sdk),
                          std::move(local_parent_not_sampled_sdk));
  }
  static const std::string die("No builder for ParentBasedSampler");
  throw UnsupportedException(die);
}

// NOLINTBEGIN(misc-no-recursion)
std::unique_ptr<opentelemetry::sdk::trace::Sampler> SdkBuilder::CreateCompositeSampler(
    const opentelemetry::sdk::configuration::ComposableSamplerConfiguration *model) const
{
  const CompositeSamplerBuilder *builder = registry_->GetCompositeSamplerBuilder();
  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("[SDK Builder] CreateCompositeSampler() using registered builder");
    ComposableSamplerBuilder composable_builder(registry_.get(), 1);
    model->Accept(&composable_builder);
    return builder->Build(std::move(composable_builder.sampler));
  }
  static const std::string die("No builder for CompositeSampler");
  throw UnsupportedException(die);
}
// NOLINTEND(misc-no-recursion)

std::unique_ptr<opentelemetry::sdk::trace::Sampler> SdkBuilder::CreateProbabilitySampler(
    const opentelemetry::sdk::configuration::ProbabilitySamplerConfiguration *model) const
{
  const ProbabilitySamplerBuilder *builder = registry_->GetProbabilitySamplerBuilder();
  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("[SDK Builder] CreateProbabilitySampler() using registered builder");
    return builder->Build(model);
  }
  static const std::string die("No builder for ProbabilitySampler");
  throw UnsupportedException(die);
}

std::unique_ptr<opentelemetry::sdk::trace::Sampler> SdkBuilder::CreateTraceIdRatioBasedSampler(
    const opentelemetry::sdk::configuration::TraceIdRatioBasedSamplerConfiguration *model) const
{
  const TraceIdRatioBasedSamplerBuilder *builder = registry_->GetTraceIdRatioBasedSamplerBuilder();
  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG(
        "[SDK Builder] CreateTraceIdRatioBasedSampler() using registered builder");
    return builder->Build(model);
  }
  static const std::string die("No builder for TraceIdRatioBasedSampler");
  throw UnsupportedException(die);
}

std::unique_ptr<opentelemetry::sdk::trace::Sampler> SdkBuilder::CreateExtensionSampler(
    const opentelemetry::sdk::configuration::ExtensionSamplerConfiguration *model) const
{
  std::unique_ptr<opentelemetry::sdk::trace::Sampler> sdk;
  std::string name = model->name;

  const ExtensionSamplerBuilder *builder = registry_->GetExtensionSamplerBuilder(name);

  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("[SDK Builder] CreateExtensionSampler() using registered builder "
                            << name);
    sdk = builder->Build(model);
    return sdk;
  }

  std::string die("CreateExtensionSampler() no builder for ");
  die.append(name);
  throw UnsupportedException(die);
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

std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> SdkBuilder::CreateOtlpHttpSpanExporter(
    const opentelemetry::sdk::configuration::OtlpHttpSpanExporterConfiguration *model) const
{
  std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> sdk;
  const OtlpHttpSpanExporterBuilder *builder = registry_->GetOtlpHttpSpanBuilder();
  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG(
        "[SDK Builder] CreateOtlpHttpSpanExporter() using registered http builder");
    sdk = builder->Build(model);
    return sdk;
  }

  static const std::string die("No http builder for OtlpHttpSpanExporter");
  throw UnsupportedException(die);
}

std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> SdkBuilder::CreateOtlpGrpcSpanExporter(
    const opentelemetry::sdk::configuration::OtlpGrpcSpanExporterConfiguration *model) const
{
  std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> sdk;
  const OtlpGrpcSpanExporterBuilder *builder = registry_->GetOtlpGrpcSpanBuilder();
  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG(
        "[SDK Builder] CreateOtlpGrpcSpanExporter() using registered grpc builder");
    sdk = builder->Build(model);
    return sdk;
  }

  static const std::string die("No builder for OtlpGrpcSpanExporter");
  throw UnsupportedException(die);
}

std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> SdkBuilder::CreateOtlpFileSpanExporter(
    const opentelemetry::sdk::configuration::OtlpFileSpanExporterConfiguration *model) const
{
  std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> sdk;
  const OtlpFileSpanExporterBuilder *builder = registry_->GetOtlpFileSpanBuilder();
  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG(
        "[SDK Builder] CreateOtlpFileSpanExporter() using registered file builder");
    sdk = builder->Build(model);
    return sdk;
  }

  static const std::string die("No builder for OtlpFileSpanExporter");
  throw UnsupportedException(die);
}

std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> SdkBuilder::CreateConsoleSpanExporter(
    const opentelemetry::sdk::configuration::ConsoleSpanExporterConfiguration *model) const
{
  std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> sdk;
  const ConsoleSpanExporterBuilder *builder = registry_->GetConsoleSpanBuilder();

  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("[SDK Builder] CreateConsoleSpanExporter() using registered builder");
    sdk = builder->Build(model);
    return sdk;
  }

  static const std::string die("No builder for ConsoleSpanExporter");
  throw UnsupportedException(die);
}

std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> SdkBuilder::CreateExtensionSpanExporter(
    const opentelemetry::sdk::configuration::ExtensionSpanExporterConfiguration *model) const
{
  std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> sdk;
  std::string name = model->name;

  const ExtensionSpanExporterBuilder *builder = registry_->GetExtensionSpanExporterBuilder(name);

  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("[SDK Builder] CreateExtensionSpanExporter() using registered builder "
                            << name);
    sdk = builder->Build(model);
    return sdk;
  }

  std::string die("CreateExtensionSpanExporter() no builder for ");
  die.append(name);
  throw UnsupportedException(die);
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
  auto exporter_sdk = CreateSpanExporter(model->exporter);

  const BatchSpanProcessorBuilder *builder = registry_->GetBatchSpanProcessorBuilder();
  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("[SDK Builder] CreateBatchSpanProcessor() using registered builder");
    return builder->Build(model, std::move(exporter_sdk));
  }
  static const std::string die("No builder for BatchSpanProcessor");
  throw UnsupportedException(die);
}

std::unique_ptr<opentelemetry::sdk::trace::SpanProcessor> SdkBuilder::CreateSimpleSpanProcessor(
    const opentelemetry::sdk::configuration::SimpleSpanProcessorConfiguration *model) const
{
  auto exporter_sdk = CreateSpanExporter(model->exporter);

  const SimpleSpanProcessorBuilder *builder = registry_->GetSimpleSpanProcessorBuilder();
  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("[SDK Builder] CreateSimpleSpanProcessor() using registered builder");
    return builder->Build(model, std::move(exporter_sdk));
  }
  static const std::string die("No builder for SimpleSpanProcessor");
  throw UnsupportedException(die);
}

std::unique_ptr<opentelemetry::sdk::trace::SpanProcessor> SdkBuilder::CreateExtensionSpanProcessor(
    const opentelemetry::sdk::configuration::ExtensionSpanProcessorConfiguration *model) const
{
  std::unique_ptr<opentelemetry::sdk::trace::SpanProcessor> sdk;
  std::string name = model->name;

  const ExtensionSpanProcessorBuilder *builder = registry_->GetExtensionSpanProcessorBuilder(name);

  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("[SDK Builder] CreateExtensionSpanProcessor() using registered builder "
                            << name);
    sdk = builder->Build(model);
    return sdk;
  }

  std::string die("CreateExtensionSpanProcessor() no builder for ");
  die.append(name);
  throw UnsupportedException(die);
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

std::unique_ptr<opentelemetry::sdk::instrumentationscope::ScopeConfigurator<
    opentelemetry::sdk::trace::TracerConfig>>
SdkBuilder::CreateTracerConfigurator(
    const std::unique_ptr<TracerConfiguratorConfiguration> &model) const
{
  const TracerConfiguratorBuilder *builder = registry_->GetTracerConfiguratorBuilder();
  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("[SDK Builder] CreateTracerConfigurator() using registered builder");
    return builder->Build(model.get());
  }
  static const std::string die("No builder for TracerConfigurator");
  throw UnsupportedException(die);
}

std::unique_ptr<opentelemetry::sdk::trace::TracerProvider> SdkBuilder::CreateTracerProvider(
    const std::unique_ptr<opentelemetry::sdk::configuration::TracerProviderConfiguration> &model,
    const opentelemetry::sdk::resource::Resource &resource,
    const AttributeLimitsConfiguration *attribute_limits) const
{
  std::unique_ptr<opentelemetry::sdk::trace::TracerProvider> sdk;

  // FIXME-CONFIG: https://github.com/open-telemetry/opentelemetry-configuration/issues/70
  // FIXME-CONFIG: Add support for IdGenerator

  std::unique_ptr<opentelemetry::sdk::trace::Sampler> sampler;

  if (model->sampler)
  {
    sampler = CreateSampler(model->sampler);
  }
  else
  {
    // Spec default: parentbased_always_on
    static const opentelemetry::sdk::configuration::ParentBasedSamplerConfiguration kDefault;
    sampler = CreateParentBasedSampler(&kDefault);
  }

  std::vector<std::unique_ptr<opentelemetry::sdk::trace::SpanProcessor>> sdk_processors;

  for (const auto &processor_model : model->processors)
  {
    sdk_processors.push_back(CreateSpanProcessor(processor_model));
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
  if (attribute_limits)
  {
    general_attribute_value_length_limit = attribute_limits->attribute_value_length_limit;
    general_attribute_count_limit        = attribute_limits->attribute_count_limit;
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

  if (model->tracer_configurator)
  {
    auto tracer_configurator = CreateTracerConfigurator(model->tracer_configurator);
    auto id_generator        = opentelemetry::sdk::trace::RandomIdGeneratorFactory::Create();
    sdk                      = opentelemetry::sdk::trace::TracerProviderFactory::Create(
        std::move(sdk_processors), resource, std::move(sampler), std::move(id_generator),
        std::move(tracer_configurator), span_limits);
  }
  else
  {
    auto default_model       = std::make_unique<TracerConfiguratorConfiguration>();
    auto tracer_configurator = CreateTracerConfigurator(default_model);
    auto id_generator        = opentelemetry::sdk::trace::RandomIdGeneratorFactory::Create();
    sdk                      = opentelemetry::sdk::trace::TracerProviderFactory::Create(
        std::move(sdk_processors), resource, std::move(sampler), std::move(id_generator),
        std::move(tracer_configurator), span_limits);
  }

  return sdk;
}

std::unique_ptr<opentelemetry::context::propagation::TextMapPropagator>
SdkBuilder::CreateTextMapPropagator(const std::string &name) const
{
  std::unique_ptr<opentelemetry::context::propagation::TextMapPropagator> sdk;

  const TextMapPropagatorBuilder *builder = registry_->GetTextMapPropagatorBuilder(name);

  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("[SDK Builder] CreateTextMapPropagator() using registered builder "
                            << name);
    sdk = builder->Build();
    return sdk;
  }

  std::string die("CreateTextMapPropagator() no builder for ");
  die.append(name);
  throw UnsupportedException(die);
}

static bool IsDuplicate(const std::vector<std::string> &propagator_seen, const std::string &name)
{
  bool duplicate = false;
  for (const auto &seen : propagator_seen)
  {
    if (name == seen)
    {
      duplicate = true;
    }
  }

  return duplicate;
}

std::unique_ptr<opentelemetry::context::propagation::TextMapPropagator>
SdkBuilder::CreatePropagator(
    const std::unique_ptr<opentelemetry::sdk::configuration::PropagatorConfiguration> &model) const
{
  std::unique_ptr<opentelemetry::context::propagation::CompositePropagator> sdk;
  std::vector<std::unique_ptr<opentelemetry::context::propagation::TextMapPropagator>> propagators;
  std::unique_ptr<opentelemetry::context::propagation::TextMapPropagator> propagator;
  std::vector<std::string> propagator_seen;
  bool duplicate = false;

  /*
   * Note that the spec only requires to check duplicates between
   * composite and composite_list.
   * Here we check for duplicates globally, for ease of use.
   */

  for (const auto &name : model->composite)
  {
    duplicate = IsDuplicate(propagator_seen, name);

    if (!duplicate)
    {
      propagator = CreateTextMapPropagator(name);
      propagators.push_back(std::move(propagator));
      propagator_seen.push_back(name);
    }
  }

  if (model->composite_list.size() > 0)
  {
    std::string str_list = model->composite_list;
    size_t start_pos     = 0;
    size_t end_pos       = 0;
    char separator       = ',';
    std::string name;

    while ((end_pos = str_list.find(separator, start_pos)) != std::string::npos)
    {
      name = str_list.substr(start_pos, end_pos - start_pos);

      duplicate = IsDuplicate(propagator_seen, name);

      if (!duplicate)
      {
        propagator = CreateTextMapPropagator(name);
        propagators.push_back(std::move(propagator));
        propagator_seen.push_back(name);
      }
      start_pos = end_pos + 1;
    }

    name = str_list.substr(start_pos);

    duplicate = IsDuplicate(propagator_seen, name);

    if (!duplicate)
    {
      propagator = CreateTextMapPropagator(name);
      propagators.push_back(std::move(propagator));
    }
  }

  if (propagators.size() > 0)
  {
    sdk = std::make_unique<opentelemetry::context::propagation::CompositePropagator>(
        std::move(propagators));
  }

  return sdk;
}

std::unique_ptr<opentelemetry::sdk::resource::ResourceDetector>
SdkBuilder::CreateContainerResourceDetector(
    const opentelemetry::sdk::configuration::ContainerResourceDetectorConfiguration *model) const
{
  const ContainerResourceDetectorBuilder *builder =
      registry_->GetContainerResourceDetectorBuilder();

  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG(
        "[SDK Builder] CreateContainerResourceDetector() using registered builder");
    return builder->Build(model);
  }

  static const std::string die("No builder for ContainerResourceDetector");
  throw UnsupportedException(die);
}

std::unique_ptr<opentelemetry::sdk::resource::ResourceDetector>
SdkBuilder::CreateHostResourceDetector(
    const opentelemetry::sdk::configuration::HostResourceDetectorConfiguration *model) const
{
  const HostResourceDetectorBuilder *builder = registry_->GetHostResourceDetectorBuilder();

  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("[SDK Builder] CreateHostResourceDetector() using registered builder");
    return builder->Build(model);
  }

  static const std::string die("No builder for HostResourceDetector");
  throw UnsupportedException(die);
}

std::unique_ptr<opentelemetry::sdk::resource::ResourceDetector>
SdkBuilder::CreateProcessResourceDetector(
    const opentelemetry::sdk::configuration::ProcessResourceDetectorConfiguration *model) const
{
  const ProcessResourceDetectorBuilder *builder = registry_->GetProcessResourceDetectorBuilder();

  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG(
        "[SDK Builder] CreateProcessResourceDetector() using registered builder");
    return builder->Build(model);
  }

  static const std::string die("No builder for ProcessResourceDetector");
  throw UnsupportedException(die);
}

std::unique_ptr<opentelemetry::sdk::resource::ResourceDetector>
SdkBuilder::CreateServiceResourceDetector(
    const opentelemetry::sdk::configuration::ServiceResourceDetectorConfiguration *model) const
{
  const ServiceResourceDetectorBuilder *builder = registry_->GetServiceResourceDetectorBuilder();

  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG(
        "[SDK Builder] CreateServiceResourceDetector() using registered builder");
    return builder->Build(model);
  }

  static const std::string die("No builder for ServiceResourceDetector");
  throw UnsupportedException(die);
}

std::unique_ptr<opentelemetry::sdk::resource::ResourceDetector>
SdkBuilder::CreateExtensionResourceDetector(
    const opentelemetry::sdk::configuration::ExtensionResourceDetectorConfiguration *model) const
{
  std::string name = model->name;

  const ExtensionResourceDetectorBuilder *builder =
      registry_->GetExtensionResourceDetectorBuilder(name);

  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG(
        "[SDK Builder] CreateExtensionResourceDetector() using registered builder " << name);
    return builder->Build(model);
  }

  std::string die("CreateExtensionResourceDetector() no builder for ");
  die.append(name);
  throw UnsupportedException(die);
}

std::unique_ptr<opentelemetry::sdk::resource::ResourceDetector> SdkBuilder::CreateResourceDetector(
    const std::unique_ptr<opentelemetry::sdk::configuration::ResourceDetectorConfiguration> &model)
    const
{
  ResourceDetectorBuilder builder(this);
  model->Accept(&builder);
  return std::move(builder.detector);
}

opentelemetry::sdk::resource::Resource SdkBuilder::CreateDetectedResource(
    const std::unique_ptr<opentelemetry::sdk::configuration::ResourceDetectionConfiguration> &model)
    const
{
  opentelemetry::sdk::resource::Resource detected;

  for (const auto &detector_model : model->detectors)
  {
    auto detector = CreateResourceDetector(detector_model);
    detected      = detected.Merge(detector->Detect());
  }

  // The filter applies to detected attributes only.
  if (model->attributes != nullptr)
  {
    opentelemetry::sdk::resource::ResourceAttributes filtered_attributes;

    for (const auto &kv : detected.GetAttributes())
    {
      if (ResourceAttributeKeyMatches(model->attributes.get(), kv.first))
      {
        filtered_attributes[kv.first] = kv.second;
      }
    }

    detected = opentelemetry::sdk::resource::Resource(filtered_attributes, detected.GetSchemaURL());
  }

  return detected;
}

void SdkBuilder::SetResourceAttribute(
    opentelemetry::sdk::resource::ResourceAttributes &resource_attributes,
    const std::string &name,
    const opentelemetry::sdk::configuration::AttributeValueConfiguration *model) const
{
  ResourceAttributeValueSetter setter(resource_attributes, name);
  // Invokes resource_attributes.SetAttribute(name, <proper value from model>)
  model->Accept(&setter);
}

void SdkBuilder::SetResource(
    opentelemetry::sdk::resource::Resource &resource,
    const std::unique_ptr<opentelemetry::sdk::configuration::ResourceConfiguration> &opt_model)
    const
{
  // Lowest priority: the default resource, with telemetry.sdk.* attributes only.
  // Resource::Create() is not usable here: it also runs OTELResourceDetector,
  // which is not part of the configuration model.
  resource = opentelemetry::sdk::resource::Resource::GetDefault();

  if (opt_model)
  {
    // Detected attributes, filtered by detection.attributes, win over the default.
    if (opt_model->detection != nullptr)
    {
      resource = resource.Merge(CreateDetectedResource(opt_model->detection));
    }

    // attributes_list wins over detected attributes.
    if (opt_model->attributes_list.size() != 0)
    {
      opentelemetry::sdk::resource::ResourceAttributes list_attributes;

      opentelemetry::common::KeyValueStringTokenizer tokenizer{opt_model->attributes_list};

      opentelemetry::nostd::string_view attribute_key;
      opentelemetry::nostd::string_view attribute_value;
      bool attribute_valid = true;

      while (tokenizer.next(attribute_valid, attribute_key, attribute_value))
      {
        if (attribute_valid)
        {
          std::string decoded_value = opentelemetry::sdk::resource::detail::PercentDecode(
              std::string{attribute_value.data(), attribute_value.size()});

          opentelemetry::common::AttributeValue wrapped_attribute_value(decoded_value);
          list_attributes.SetAttribute(attribute_key, wrapped_attribute_value);
        }
        else
        {
          OTEL_INTERNAL_LOG_WARN("[SDK Builder] Found invalid key/value pair in attributes_list");
        }
      }

      resource = resource.Merge(opentelemetry::sdk::resource::Resource(list_attributes));
    }

    // Highest priority: attributes and schema_url from the model.
    opentelemetry::sdk::resource::ResourceAttributes sdk_attributes;

    if (opt_model->attributes)
    {
      for (const auto &kv : opt_model->attributes->kv_map)
      {
        // kv_map values may be nullptr for programmatically-set null entries.
        // The default behavior for resource.attributes is to ignore null values.
        if (kv.second != nullptr)
        {
          SetResourceAttribute(sdk_attributes, kv.first, kv.second.get());
        }
      }
    }

    resource = resource.Merge(
        opentelemetry::sdk::resource::Resource(sdk_attributes, opt_model->schema_url));
  }
}

void SdkBuilder::SetLogLevel(
    opentelemetry::sdk::common::internal_log::LogLevel &sdk_log_level,
    opentelemetry::sdk::configuration::SeverityNumber model_log_level) const
{
  sdk_log_level = opentelemetry::sdk::common::internal_log::LogLevel::Info;

  switch (model_log_level)
  {
    case SeverityNumber::trace:
    case SeverityNumber::trace2:
    case SeverityNumber::trace3:
    case SeverityNumber::trace4:
    case SeverityNumber::debug:
    case SeverityNumber::debug2:
    case SeverityNumber::debug3:
    case SeverityNumber::debug4:
      sdk_log_level = opentelemetry::sdk::common::internal_log::LogLevel::Debug;
      break;
    case SeverityNumber::info:
    case SeverityNumber::info2:
    case SeverityNumber::info3:
    case SeverityNumber::info4:
      sdk_log_level = opentelemetry::sdk::common::internal_log::LogLevel::Info;
      break;
    case SeverityNumber::warn:
    case SeverityNumber::warn2:
    case SeverityNumber::warn3:
    case SeverityNumber::warn4:
      sdk_log_level = opentelemetry::sdk::common::internal_log::LogLevel::Warning;
      break;
    case SeverityNumber::error:
    case SeverityNumber::error2:
    case SeverityNumber::error3:
    case SeverityNumber::error4:
    case SeverityNumber::fatal:
    case SeverityNumber::fatal2:
    case SeverityNumber::fatal3:
    case SeverityNumber::fatal4:
      sdk_log_level = opentelemetry::sdk::common::internal_log::LogLevel::Error;
      break;
  }
}

std::unique_ptr<ConfiguredSdk> SdkBuilder::CreateConfiguredSdk(
    const std::unique_ptr<opentelemetry::sdk::configuration::Configuration> &model) const
{
  auto sdk = std::make_unique<ConfiguredSdk>();

  SetLogLevel(sdk->log_level, model->log_level);

  if (!model->disabled)
  {
    SetResource(sdk->resource, model->resource);

    if (model->tracer_provider)
    {
      sdk->tracer_provider = CreateTracerProvider(model->tracer_provider, sdk->resource,
                                                  model->attribute_limits.get());
    }

    if (model->propagator)
    {
      sdk->propagator = CreatePropagator(model->propagator);
    }

    if (model->meter_provider)
    {
      const MeterProviderBuilder *mp_builder = registry_->GetMeterProviderBuilder();
      if (mp_builder == nullptr)
      {
        static const std::string message("No builder for MeterProvider");
        throw UnsupportedException(message);
      }
      MeterProviderBuilderContext mp_context{registry_.get(), &sdk->resource};
      sdk->meter_provider = mp_builder->Build(mp_context, model->meter_provider.get());
      if (sdk->meter_provider == nullptr)
      {
        static const std::string message("MeterProviderBuilder returned null provider");
        throw UnsupportedException(message);
      }
    }

    if (model->logger_provider)
    {
      const LoggerProviderBuilder *lp_builder = registry_->GetLoggerProviderBuilder();
      if (lp_builder == nullptr)
      {
        static const std::string message("No builder for LoggerProvider");
        throw UnsupportedException(message);
      }
      LoggerProviderBuilderContext lp_context{registry_.get(), &sdk->resource,
                                              model->attribute_limits.get()};
      sdk->logger_provider = lp_builder->Build(lp_context, model->logger_provider.get());
      if (sdk->logger_provider == nullptr)
      {
        static const std::string message("LoggerProviderBuilder returned null provider");
        throw UnsupportedException(message);
      }
    }

    if (model->distribution)
    {
      // FIXME-CONFIG: Implement distribution configuration support
      OTEL_INTERNAL_LOG_WARN("[SDK Builder] the distribution model is not yet supported, ignoring");
    }

    // FIXME-CONFIG: Implement instrumentation/development support
    // if(model->instrumentation)
    // {
    //   OTEL_INTERNAL_LOG_WARN("[SDK Builder] instrumentation is not yet supported, ignoring");
    // }
  }

  // Set the log level if the SDK has been created successfully.
  opentelemetry::sdk::common::internal_log::GlobalLogHandler::SetLogLevel(sdk->log_level);

  OTEL_INTERNAL_LOG_DEBUG("[SDK Builder] Configured SDK is "
                          << (model->disabled ? "disabled" : "enabled"));

  return sdk;
}

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
