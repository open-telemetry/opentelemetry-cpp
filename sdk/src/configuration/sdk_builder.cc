// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <cstddef>
#include <cstdint>
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
#include "opentelemetry/sdk/configuration/aggregation_configuration.h"
#include "opentelemetry/sdk/configuration/aggregation_configuration_visitor.h"
#include "opentelemetry/sdk/configuration/always_off_sampler_builder.h"
#include "opentelemetry/sdk/configuration/always_off_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/always_on_sampler_builder.h"
#include "opentelemetry/sdk/configuration/always_on_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/attribute_value_configuration_visitor.h"
#include "opentelemetry/sdk/configuration/attributes_configuration.h"
#include "opentelemetry/sdk/configuration/base2_exponential_bucket_histogram_aggregation_configuration.h"
#include "opentelemetry/sdk/configuration/batch_log_record_processor_builder.h"
#include "opentelemetry/sdk/configuration/batch_log_record_processor_configuration.h"
#include "opentelemetry/sdk/configuration/batch_span_processor_builder.h"
#include "opentelemetry/sdk/configuration/batch_span_processor_configuration.h"
#include "opentelemetry/sdk/configuration/boolean_array_attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/boolean_attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/cardinality_limits_configuration.h"
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
#include "opentelemetry/sdk/configuration/console_log_record_exporter_builder.h"
#include "opentelemetry/sdk/configuration/console_log_record_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/console_push_metric_exporter_builder.h"
#include "opentelemetry/sdk/configuration/console_push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/console_span_exporter_builder.h"
#include "opentelemetry/sdk/configuration/console_span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/container_resource_detector_builder.h"
#include "opentelemetry/sdk/configuration/container_resource_detector_configuration.h"
#include "opentelemetry/sdk/configuration/double_array_attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/double_attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/exemplar_filter.h"
#include "opentelemetry/sdk/configuration/explicit_bucket_histogram_aggregation_configuration.h"
#include "opentelemetry/sdk/configuration/extension_composable_sampler_builder.h"
#include "opentelemetry/sdk/configuration/extension_composable_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/extension_log_record_exporter_builder.h"
#include "opentelemetry/sdk/configuration/extension_log_record_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/extension_log_record_processor_builder.h"
#include "opentelemetry/sdk/configuration/extension_log_record_processor_configuration.h"
#include "opentelemetry/sdk/configuration/extension_pull_metric_exporter_builder.h"
#include "opentelemetry/sdk/configuration/extension_pull_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/extension_push_metric_exporter_builder.h"
#include "opentelemetry/sdk/configuration/extension_push_metric_exporter_configuration.h"
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
#include "opentelemetry/sdk/configuration/instrument_type.h"
#include "opentelemetry/sdk/configuration/integer_array_attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/integer_attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/jaeger_remote_sampler_builder.h"
#include "opentelemetry/sdk/configuration/jaeger_remote_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/log_record_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/log_record_exporter_configuration_visitor.h"
#include "opentelemetry/sdk/configuration/log_record_limits_configuration.h"
#include "opentelemetry/sdk/configuration/log_record_processor_configuration.h"
#include "opentelemetry/sdk/configuration/log_record_processor_configuration_visitor.h"
#include "opentelemetry/sdk/configuration/logger_configurator_builder.h"
#include "opentelemetry/sdk/configuration/logger_configurator_configuration.h"
#include "opentelemetry/sdk/configuration/logger_provider_configuration.h"
#include "opentelemetry/sdk/configuration/meter_configurator_builder.h"
#include "opentelemetry/sdk/configuration/meter_configurator_configuration.h"
#include "opentelemetry/sdk/configuration/meter_provider_configuration.h"
#include "opentelemetry/sdk/configuration/metric_reader_configuration.h"
#include "opentelemetry/sdk/configuration/metric_reader_configuration_visitor.h"
#include "opentelemetry/sdk/configuration/otlp_file_log_record_exporter_builder.h"
#include "opentelemetry/sdk/configuration/otlp_file_log_record_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/otlp_file_push_metric_exporter_builder.h"
#include "opentelemetry/sdk/configuration/otlp_file_push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/otlp_file_span_exporter_builder.h"
#include "opentelemetry/sdk/configuration/otlp_file_span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/otlp_grpc_log_record_exporter_builder.h"
#include "opentelemetry/sdk/configuration/otlp_grpc_log_record_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/otlp_grpc_push_metric_exporter_builder.h"
#include "opentelemetry/sdk/configuration/otlp_grpc_push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/otlp_grpc_span_exporter_builder.h"
#include "opentelemetry/sdk/configuration/otlp_grpc_span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/otlp_http_log_record_exporter_builder.h"
#include "opentelemetry/sdk/configuration/otlp_http_log_record_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/otlp_http_push_metric_exporter_builder.h"
#include "opentelemetry/sdk/configuration/otlp_http_push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/otlp_http_span_exporter_builder.h"
#include "opentelemetry/sdk/configuration/otlp_http_span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/parent_based_sampler_builder.h"
#include "opentelemetry/sdk/configuration/parent_based_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/periodic_metric_reader_builder.h"
#include "opentelemetry/sdk/configuration/periodic_metric_reader_configuration.h"
#include "opentelemetry/sdk/configuration/probability_sampler_builder.h"
#include "opentelemetry/sdk/configuration/probability_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/process_resource_detector_builder.h"
#include "opentelemetry/sdk/configuration/process_resource_detector_configuration.h"
#include "opentelemetry/sdk/configuration/prometheus_pull_metric_exporter_builder.h"
#include "opentelemetry/sdk/configuration/prometheus_pull_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/propagator_configuration.h"
#include "opentelemetry/sdk/configuration/pull_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/pull_metric_exporter_configuration_visitor.h"
#include "opentelemetry/sdk/configuration/pull_metric_reader_configuration.h"
#include "opentelemetry/sdk/configuration/push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/push_metric_exporter_configuration_visitor.h"
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
#include "opentelemetry/sdk/configuration/simple_log_record_processor_builder.h"
#include "opentelemetry/sdk/configuration/simple_log_record_processor_configuration.h"
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
#include "opentelemetry/sdk/configuration/view_configuration.h"
#include "opentelemetry/sdk/configuration/view_selector_configuration.h"
#include "opentelemetry/sdk/configuration/view_stream_configuration.h"
#include "opentelemetry/sdk/instrumentationscope/scope_configurator.h"
#include "opentelemetry/sdk/logs/exporter.h"
#include "opentelemetry/sdk/logs/log_record_limits.h"
#include "opentelemetry/sdk/logs/logger_config.h"
#include "opentelemetry/sdk/logs/logger_provider.h"
#include "opentelemetry/sdk/logs/logger_provider_factory.h"
#include "opentelemetry/sdk/logs/processor.h"
#include "opentelemetry/sdk/metrics/aggregation/aggregation_config.h"
#include "opentelemetry/sdk/metrics/aggregation/default_aggregation.h"
#include "opentelemetry/sdk/metrics/cardinality_limits.h"
#include "opentelemetry/sdk/metrics/instruments.h"
#include "opentelemetry/sdk/metrics/meter_config.h"
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

#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
#  include "opentelemetry/sdk/metrics/exemplar/filter_type.h"
#endif

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

namespace
{

/// Convert a CardinalityLimitsConfiguration (where 0 means "unset, use
/// default_limit") into a fully-resolved CardinalityLimits struct ready for
/// MetricReader::SetCardinalityLimits().
static opentelemetry::sdk::metrics::CardinalityLimits ToCardinalityLimits(
    const opentelemetry::sdk::configuration::CardinalityLimitsConfiguration &cfg)
{
  opentelemetry::sdk::metrics::CardinalityLimits limits;
  // If the caller supplied a non-zero default, use it; otherwise keep the
  // SDK default (kDefaultCardinalityLimit = 2000) already in the struct.
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
      OTEL_INTERNAL_LOG_DEBUG("VisitComposableAlwaysOff() using registered builder");
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
      OTEL_INTERNAL_LOG_DEBUG("VisitComposableAlwaysOn() using registered builder");
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
      OTEL_INTERNAL_LOG_DEBUG("VisitComposableProbability() using registered builder");
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
      OTEL_INTERNAL_LOG_DEBUG("VisitComposableParentThreshold() using registered builder");
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
      OTEL_INTERNAL_LOG_DEBUG("VisitComposableRuleBased() using registered builder");
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
      OTEL_INTERNAL_LOG_DEBUG("VisitComposableExtension() using registered builder "
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

class MetricReaderBuilder
    : public opentelemetry::sdk::configuration::MetricReaderConfigurationVisitor
{
public:
  MetricReaderBuilder(const SdkBuilder *b) : sdk_builder_(b) {}
  MetricReaderBuilder(MetricReaderBuilder &&)                      = delete;
  MetricReaderBuilder(const MetricReaderBuilder &)                 = delete;
  MetricReaderBuilder &operator=(MetricReaderBuilder &&)           = delete;
  MetricReaderBuilder &operator=(const MetricReaderBuilder &other) = delete;
  ~MetricReaderBuilder() override                                  = default;

  void VisitPeriodic(
      const opentelemetry::sdk::configuration::PeriodicMetricReaderConfiguration *model) override
  {
    metric_reader = sdk_builder_->CreatePeriodicMetricReader(model);
  }

  void VisitPull(
      const opentelemetry::sdk::configuration::PullMetricReaderConfiguration *model) override
  {
    metric_reader = sdk_builder_->CreatePullMetricReader(model);
  }

  std::unique_ptr<opentelemetry::sdk::metrics::MetricReader> metric_reader;

private:
  const SdkBuilder *sdk_builder_;
};

class PushMetricExporterBuilder
    : public opentelemetry::sdk::configuration::PushMetricExporterConfigurationVisitor
{
public:
  PushMetricExporterBuilder(const SdkBuilder *b) : sdk_builder_(b) {}
  PushMetricExporterBuilder(PushMetricExporterBuilder &&)                      = delete;
  PushMetricExporterBuilder(const PushMetricExporterBuilder &)                 = delete;
  PushMetricExporterBuilder &operator=(PushMetricExporterBuilder &&)           = delete;
  PushMetricExporterBuilder &operator=(const PushMetricExporterBuilder &other) = delete;
  ~PushMetricExporterBuilder() override                                        = default;

  void VisitOtlpHttp(
      const opentelemetry::sdk::configuration::OtlpHttpPushMetricExporterConfiguration *model)
      override
  {
    exporter = sdk_builder_->CreateOtlpHttpPushMetricExporter(model);
  }

  void VisitOtlpGrpc(
      const opentelemetry::sdk::configuration::OtlpGrpcPushMetricExporterConfiguration *model)
      override
  {
    exporter = sdk_builder_->CreateOtlpGrpcPushMetricExporter(model);
  }

  void VisitOtlpFile(
      const opentelemetry::sdk::configuration::OtlpFilePushMetricExporterConfiguration *model)
      override
  {
    exporter = sdk_builder_->CreateOtlpFilePushMetricExporter(model);
  }

  void VisitConsole(const opentelemetry::sdk::configuration::ConsolePushMetricExporterConfiguration
                        *model) override
  {
    exporter = sdk_builder_->CreateConsolePushMetricExporter(model);
  }

  void VisitExtension(
      const opentelemetry::sdk::configuration::ExtensionPushMetricExporterConfiguration *model)
      override
  {
    exporter = sdk_builder_->CreateExtensionPushMetricExporter(model);
  }

  std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter> exporter;

private:
  const SdkBuilder *sdk_builder_;
};

class PullMetricExporterBuilder
    : public opentelemetry::sdk::configuration::PullMetricExporterConfigurationVisitor
{
public:
  PullMetricExporterBuilder(const SdkBuilder *b) : sdk_builder_(b) {}
  PullMetricExporterBuilder(PullMetricExporterBuilder &&)                      = delete;
  PullMetricExporterBuilder(const PullMetricExporterBuilder &)                 = delete;
  PullMetricExporterBuilder &operator=(PullMetricExporterBuilder &&)           = delete;
  PullMetricExporterBuilder &operator=(const PullMetricExporterBuilder &other) = delete;
  ~PullMetricExporterBuilder() override                                        = default;

  void VisitPrometheus(
      const opentelemetry::sdk::configuration::PrometheusPullMetricExporterConfiguration *model)
      override
  {
    exporter = sdk_builder_->CreatePrometheusPullMetricExporter(model);
  }

  void VisitExtension(
      const opentelemetry::sdk::configuration::ExtensionPullMetricExporterConfiguration *model)
      override
  {
    exporter = sdk_builder_->CreateExtensionPullMetricExporter(model);
  }

  std::unique_ptr<opentelemetry::sdk::metrics::MetricReader> exporter;

private:
  const SdkBuilder *sdk_builder_;
};

class AggregationConfigBuilder
    : public opentelemetry::sdk::configuration::AggregationConfigurationVisitor
{
public:
  AggregationConfigBuilder(const SdkBuilder *b) : sdk_builder_(b) {}
  AggregationConfigBuilder(AggregationConfigBuilder &&)                      = delete;
  AggregationConfigBuilder(const AggregationConfigBuilder &)                 = delete;
  AggregationConfigBuilder &operator=(AggregationConfigBuilder &&)           = delete;
  AggregationConfigBuilder &operator=(const AggregationConfigBuilder &other) = delete;
  ~AggregationConfigBuilder() override                                       = default;

  void VisitBase2ExponentialBucketHistogram(
      const opentelemetry::sdk::configuration::
          Base2ExponentialBucketHistogramAggregationConfiguration *model) override
  {
    aggregation_type   = opentelemetry::sdk::metrics::AggregationType::kBase2ExponentialHistogram;
    aggregation_config = sdk_builder_->CreateBase2ExponentialBucketHistogramAggregation(model);
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
    aggregation_config = sdk_builder_->CreateExplicitBucketHistogramAggregation(model);
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

private:
  const SdkBuilder *sdk_builder_;
};

class LogRecordProcessorBuilder
    : public opentelemetry::sdk::configuration::LogRecordProcessorConfigurationVisitor
{
public:
  LogRecordProcessorBuilder(const SdkBuilder *b) : sdk_builder_(b) {}
  LogRecordProcessorBuilder(LogRecordProcessorBuilder &&)                      = delete;
  LogRecordProcessorBuilder(const LogRecordProcessorBuilder &)                 = delete;
  LogRecordProcessorBuilder &operator=(LogRecordProcessorBuilder &&)           = delete;
  LogRecordProcessorBuilder &operator=(const LogRecordProcessorBuilder &other) = delete;
  ~LogRecordProcessorBuilder() override                                        = default;

  void VisitBatch(
      const opentelemetry::sdk::configuration::BatchLogRecordProcessorConfiguration *model) override
  {
    processor = sdk_builder_->CreateBatchLogRecordProcessor(model);
  }

  void VisitSimple(const opentelemetry::sdk::configuration::SimpleLogRecordProcessorConfiguration
                       *model) override
  {
    processor = sdk_builder_->CreateSimpleLogRecordProcessor(model);
  }

  void VisitExtension(
      const opentelemetry::sdk::configuration::ExtensionLogRecordProcessorConfiguration *model)
      override
  {
    processor = sdk_builder_->CreateExtensionLogRecordProcessor(model);
  }

  std::unique_ptr<opentelemetry::sdk::logs::LogRecordProcessor> processor;

private:
  const SdkBuilder *sdk_builder_;
};

class LogRecordExporterBuilder
    : public opentelemetry::sdk::configuration::LogRecordExporterConfigurationVisitor
{
public:
  LogRecordExporterBuilder(const SdkBuilder *b) : sdk_builder_(b) {}
  LogRecordExporterBuilder(LogRecordExporterBuilder &&)                      = delete;
  LogRecordExporterBuilder(const LogRecordExporterBuilder &)                 = delete;
  LogRecordExporterBuilder &operator=(LogRecordExporterBuilder &&)           = delete;
  LogRecordExporterBuilder &operator=(const LogRecordExporterBuilder &other) = delete;
  ~LogRecordExporterBuilder() override                                       = default;

  void VisitOtlpHttp(const opentelemetry::sdk::configuration::OtlpHttpLogRecordExporterConfiguration
                         *model) override
  {
    exporter = sdk_builder_->CreateOtlpHttpLogRecordExporter(model);
  }

  void VisitOtlpGrpc(const opentelemetry::sdk::configuration::OtlpGrpcLogRecordExporterConfiguration
                         *model) override
  {
    exporter = sdk_builder_->CreateOtlpGrpcLogRecordExporter(model);
  }

  void VisitOtlpFile(const opentelemetry::sdk::configuration::OtlpFileLogRecordExporterConfiguration
                         *model) override
  {
    exporter = sdk_builder_->CreateOtlpFileLogRecordExporter(model);
  }

  void VisitConsole(const opentelemetry::sdk::configuration::ConsoleLogRecordExporterConfiguration
                        *model) override
  {
    exporter = sdk_builder_->CreateConsoleLogRecordExporter(model);
  }

  void VisitExtension(
      const opentelemetry::sdk::configuration::ExtensionLogRecordExporterConfiguration *model)
      override
  {
    exporter = sdk_builder_->CreateExtensionLogRecordExporter(model);
  }

  std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> exporter;

private:
  const SdkBuilder *sdk_builder_;
};

}  // namespace

std::unique_ptr<opentelemetry::sdk::trace::Sampler> SdkBuilder::CreateAlwaysOffSampler(
    const opentelemetry::sdk::configuration::AlwaysOffSamplerConfiguration *model) const
{
  const AlwaysOffSamplerBuilder *builder = registry_->GetAlwaysOffSamplerBuilder();
  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateAlwaysOffSampler() using registered builder");
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
    OTEL_INTERNAL_LOG_DEBUG("CreateAlwaysOnSampler() using registered builder");
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
    OTEL_INTERNAL_LOG_DEBUG("CreateJaegerRemoteSampler() using registered builder");
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
    OTEL_INTERNAL_LOG_DEBUG("CreateParentBasedSampler() using registered builder");
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
    OTEL_INTERNAL_LOG_DEBUG("CreateCompositeSampler() using registered builder");
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
    OTEL_INTERNAL_LOG_DEBUG("CreateProbabilitySampler() using registered builder");
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
    OTEL_INTERNAL_LOG_DEBUG("CreateTraceIdRatioBasedSampler() using registered builder");
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
    OTEL_INTERNAL_LOG_DEBUG("CreateExtensionSampler() using registered builder " << name);
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
    OTEL_INTERNAL_LOG_DEBUG("CreateOtlpHttpSpanExporter() using registered http builder");
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
    OTEL_INTERNAL_LOG_DEBUG("CreateOtlpGrpcSpanExporter() using registered grpc builder");
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
    OTEL_INTERNAL_LOG_DEBUG("CreateOtlpFileSpanExporter() using registered file builder");
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
    OTEL_INTERNAL_LOG_DEBUG("CreateConsoleSpanExporter() using registered builder");
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
    OTEL_INTERNAL_LOG_DEBUG("CreateExtensionSpanExporter() using registered builder " << name);
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
    OTEL_INTERNAL_LOG_DEBUG("CreateBatchSpanProcessor() using registered builder");
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
    OTEL_INTERNAL_LOG_DEBUG("CreateSimpleSpanProcessor() using registered builder");
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
    OTEL_INTERNAL_LOG_DEBUG("CreateExtensionSpanProcessor() using registered builder " << name);
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
    OTEL_INTERNAL_LOG_DEBUG("CreateTracerConfigurator() using registered builder");
    return builder->Build(model.get());
  }
  static const std::string die("No builder for TracerConfigurator");
  throw UnsupportedException(die);
}

std::unique_ptr<opentelemetry::sdk::trace::TracerProvider> SdkBuilder::CreateTracerProvider(
    const std::unique_ptr<opentelemetry::sdk::configuration::TracerProviderConfiguration> &model,
    const opentelemetry::sdk::resource::Resource &resource) const
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
  if (model->limits)
  {
    span_limits.attribute_value_length_limit = model->limits->attribute_value_length_limit;
    span_limits.attribute_count_limit        = model->limits->attribute_count_limit;
    span_limits.event_count_limit            = model->limits->event_count_limit;
    span_limits.link_count_limit             = model->limits->link_count_limit;
    span_limits.event_attribute_count_limit  = model->limits->event_attribute_count_limit;
    span_limits.link_attribute_count_limit   = model->limits->link_attribute_count_limit;
  }

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
    OTEL_INTERNAL_LOG_DEBUG("CreateTextMapPropagator() using registered builder " << name);
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

#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW

static opentelemetry::sdk::metrics::ExemplarFilterType ConvertExemplarFilter(
    enum opentelemetry::sdk::configuration::ExemplarFilter config)
{
  opentelemetry::sdk::metrics::ExemplarFilterType sdk{
      opentelemetry::sdk::metrics::ExemplarFilterType::kTraceBased};

  switch (config)
  {
    case opentelemetry::sdk::configuration::ExemplarFilter::always_on:
      sdk = opentelemetry::sdk::metrics::ExemplarFilterType::kAlwaysOn;
      break;
    case opentelemetry::sdk::configuration::ExemplarFilter::always_off:
      sdk = opentelemetry::sdk::metrics::ExemplarFilterType::kAlwaysOff;
      break;
    case opentelemetry::sdk::configuration::ExemplarFilter::trace_based:
      sdk = opentelemetry::sdk::metrics::ExemplarFilterType::kTraceBased;
      break;
    default:
      break;
  }

  return sdk;
}
#endif /* ENABLE_METRICS_EXEMPLAR_PREVIEW */

std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter>
SdkBuilder::CreateOtlpHttpPushMetricExporter(
    const opentelemetry::sdk::configuration::OtlpHttpPushMetricExporterConfiguration *model) const
{
  std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter> sdk;
  const OtlpHttpPushMetricExporterBuilder *builder =
      registry_->GetOtlpHttpPushMetricExporterBuilder();
  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateOtlpHttpPushMetricExporter() using registered http builder");
    sdk = builder->Build(model);
    return sdk;
  }

  static const std::string die("No http builder for OtlpPushMetricExporter");
  throw UnsupportedException(die);
}

std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter>
SdkBuilder::CreateOtlpGrpcPushMetricExporter(
    const opentelemetry::sdk::configuration::OtlpGrpcPushMetricExporterConfiguration *model) const
{
  std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter> sdk;
  const OtlpGrpcPushMetricExporterBuilder *builder =
      registry_->GetOtlpGrpcPushMetricExporterBuilder();
  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateOtlpGrpcPushMetricExporter() using registered grpc builder");
    sdk = builder->Build(model);
    return sdk;
  }

  static const std::string die("No grpc builder for OtlpPushMetricExporter");
  throw UnsupportedException(die);
}

std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter>
SdkBuilder::CreateOtlpFilePushMetricExporter(
    const opentelemetry::sdk::configuration::OtlpFilePushMetricExporterConfiguration *model) const
{
  std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter> sdk;
  const OtlpFilePushMetricExporterBuilder *builder =
      registry_->GetOtlpFilePushMetricExporterBuilder();
  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateOtlpFilePushMetricExporter() using registered file builder");
    sdk = builder->Build(model);
    return sdk;
  }

  static const std::string die("No file builder for OtlpPushMetricExporter");
  throw UnsupportedException(die);
}

std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter>
SdkBuilder::CreateConsolePushMetricExporter(
    const opentelemetry::sdk::configuration::ConsolePushMetricExporterConfiguration *model) const
{
  std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter> sdk;

  const ConsolePushMetricExporterBuilder *builder =
      registry_->GetConsolePushMetricExporterBuilder();

  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateConsolePushMetricExporter() using registered builder");
    sdk = builder->Build(model);
    return sdk;
  }

  static const std::string die("No builder for ConsolePushMetricExporter");
  throw UnsupportedException(die);
}

std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter>
SdkBuilder::CreateExtensionPushMetricExporter(
    const opentelemetry::sdk::configuration::ExtensionPushMetricExporterConfiguration *model) const
{
  std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter> sdk;
  std::string name = model->name;

  const ExtensionPushMetricExporterBuilder *builder =
      registry_->GetExtensionPushMetricExporterBuilder(name);

  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateExtensionPushMetricExporter() using registered builder "
                            << name);
    sdk = builder->Build(model);
    return sdk;
  }

  std::string die("No builder for ExtensionPushMetricExporter ");
  die.append(name);
  throw UnsupportedException(die);
}

std::unique_ptr<opentelemetry::sdk::metrics::MetricReader>
SdkBuilder::CreatePrometheusPullMetricExporter(
    const opentelemetry::sdk::configuration::PrometheusPullMetricExporterConfiguration *model) const
{
  std::unique_ptr<opentelemetry::sdk::metrics::MetricReader> sdk;

  const PrometheusPullMetricExporterBuilder *builder =
      registry_->GetPrometheusPullMetricExporterBuilder();

  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreatePrometheusPullMetricExporter() using registered builder");
    sdk = builder->Build(model);
    return sdk;
  }

  static const std::string die("No builder for PrometheusMetricExporter");
  throw UnsupportedException(die);
}

std::unique_ptr<opentelemetry::sdk::metrics::MetricReader>
SdkBuilder::CreateExtensionPullMetricExporter(
    const opentelemetry::sdk::configuration::ExtensionPullMetricExporterConfiguration *model) const
{
  std::unique_ptr<opentelemetry::sdk::metrics::MetricReader> sdk;
  std::string name = model->name;

  const ExtensionPullMetricExporterBuilder *builder =
      registry_->GetExtensionPullMetricExporterBuilder(name);

  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateExtensionPullMetricExporter() using registered builder "
                            << name);
    sdk = builder->Build(model);
    return sdk;
  }

  std::string die("No builder for ExtensionPullMetricExporter ");
  die.append(name);
  throw UnsupportedException(die);
}

std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter>
SdkBuilder::CreatePushMetricExporter(
    const std::unique_ptr<opentelemetry::sdk::configuration::PushMetricExporterConfiguration>
        &model) const
{
  std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter> sdk;

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

  PullMetricExporterBuilder builder(this);
  model->Accept(&builder);
  sdk = std::move(builder.exporter);

  return sdk;
}

std::unique_ptr<opentelemetry::sdk::metrics::MetricReader> SdkBuilder::CreatePeriodicMetricReader(
    const opentelemetry::sdk::configuration::PeriodicMetricReaderConfiguration *model) const
{
  std::unique_ptr<opentelemetry::sdk::metrics::MetricReader> sdk;

  auto exporter_sdk = CreatePushMetricExporter(model->exporter);

  if (model->producers.size() > 0)
  {
    OTEL_INTERNAL_LOG_WARN("metric producer not supported, ignoring");
  }

  const PeriodicMetricReaderBuilder *builder = registry_->GetPeriodicMetricReaderBuilder();

  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreatePeriodicMetricReader() using registered builder");
    sdk = builder->Build(model, std::move(exporter_sdk));
  }
  else
  {
    static const std::string die("No builder for PeriodicMetricReader");
    throw UnsupportedException(die);
  }

  if (model->cardinality_limits != nullptr)
  {
    sdk->SetCardinalityLimits(ToCardinalityLimits(*model->cardinality_limits));
  }

  return sdk;
}

std::unique_ptr<opentelemetry::sdk::metrics::MetricReader> SdkBuilder::CreatePullMetricReader(
    const opentelemetry::sdk::configuration::PullMetricReaderConfiguration *model) const
{
  std::unique_ptr<opentelemetry::sdk::metrics::MetricReader> sdk;

  sdk = CreatePullMetricExporter(model->exporter);

  if (model->producers.size() > 0)
  {
    OTEL_INTERNAL_LOG_WARN("metric producer not supported, ignoring");
  }

  if (model->cardinality_limits != nullptr)
  {
    sdk->SetCardinalityLimits(ToCardinalityLimits(*model->cardinality_limits));
  }

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

std::unique_ptr<opentelemetry::sdk::metrics::Base2ExponentialHistogramAggregationConfig>
SdkBuilder::CreateBase2ExponentialBucketHistogramAggregation(
    const opentelemetry::sdk::configuration::Base2ExponentialBucketHistogramAggregationConfiguration
        *model) const
{
  auto sdk =
      std::make_unique<opentelemetry::sdk::metrics::Base2ExponentialHistogramAggregationConfig>();

  sdk->max_size_       = model->max_size;
  sdk->max_scale_      = model->max_scale;
  sdk->record_min_max_ = model->record_min_max;

  return sdk;
}

std::unique_ptr<opentelemetry::sdk::metrics::HistogramAggregationConfig>
SdkBuilder::CreateExplicitBucketHistogramAggregation(
    const opentelemetry::sdk::configuration::ExplicitBucketHistogramAggregationConfiguration *model)
    const
{
  auto sdk = std::make_unique<opentelemetry::sdk::metrics::HistogramAggregationConfig>();

  sdk->boundaries_     = model->boundaries;
  sdk->record_min_max_ = model->record_min_max;

  return sdk;
}

std::unique_ptr<opentelemetry::sdk::metrics::AggregationConfig> SdkBuilder::CreateAggregationConfig(
    const std::unique_ptr<opentelemetry::sdk::configuration::AggregationConfiguration> &model,
    opentelemetry::sdk::metrics::AggregationType &aggregation_type) const
{
  std::unique_ptr<opentelemetry::sdk::metrics::AggregationConfig> sdk;

  AggregationConfigBuilder builder(this);
  model->Accept(&builder);
  aggregation_type = builder.aggregation_type;
  sdk              = std::move(builder.aggregation_config);

  return sdk;
}

std::unique_ptr<opentelemetry::sdk::metrics::AttributesProcessor>
SdkBuilder::CreateAttributesProcessor(
    const std::unique_ptr<opentelemetry::sdk::configuration::IncludeExcludeConfiguration> &model)
    const
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

void SdkBuilder::AddView(
    opentelemetry::sdk::metrics::ViewRegistry *view_registry,
    const std::unique_ptr<opentelemetry::sdk::configuration::ViewConfiguration> &model) const
{
  auto *selector = model->selector.get();

  // Synchronous gauge instruments are not supported in ABIv1
#if OPENTELEMETRY_ABI_VERSION_NO < 2
  if (selector->instrument_type == opentelemetry::sdk::configuration::InstrumentType::gauge)
  {
    std::string die("Runtime does not support instrument_type: gauge with ABI version 1");
    throw UnsupportedException(die);
  }
#endif

  auto add_view = [&](opentelemetry::sdk::metrics::InstrumentType sdk_instrument_type) {
    // If the instrument name is empty, use "*" to match all instruments of the given type.
    const std::string instrument_name =
        selector->instrument_name.empty() ? "*" : selector->instrument_name;

    auto sdk_instrument_selector =
        std::make_unique<opentelemetry::sdk::metrics::InstrumentSelector>(
            sdk_instrument_type, instrument_name, selector->unit);

    auto sdk_meter_selector = std::make_unique<opentelemetry::sdk::metrics::MeterSelector>(
        selector->meter_name, selector->meter_version, selector->meter_schema_url);

    auto *stream = model->stream.get();

    opentelemetry::sdk::metrics::AggregationType sdk_aggregation_type =
        opentelemetry::sdk::metrics::AggregationType::kDefault;

    std::shared_ptr<opentelemetry::sdk::metrics::AggregationConfig> sdk_aggregation_config;

    if (stream->aggregation)
    {
      // CreateAggregationConfig() may build a config purely for non-cardinality reasons (e.g.
      // histogram boundaries) via a config's default constructor, which already leaves the
      // cardinality limit not-explicit (see AggregationConfig's constructor) so it doesn't
      // shadow a MetricReader-level fallback; the block below marks it explicit if
      // aggregation_cardinality_limit is also configured.
      sdk_aggregation_config = CreateAggregationConfig(stream->aggregation, sdk_aggregation_type);
    }

    // Apply aggregation_cardinality_limit from the view stream configuration
    if (stream->aggregation_cardinality_limit != 0)
    {
      if (sdk_aggregation_config)
      {
        sdk_aggregation_config->SetCardinalityLimit(stream->aggregation_cardinality_limit);
      }
      else
      {
        // No explicit `aggregation` block was configured, so the view falls back to the
        // instrument's default aggregation. ViewRegistry::AddView() rejects a view whose
        // AggregationConfig type does not match the (possibly instrument-derived) aggregation
        // type, so the config created here must match that same default rather than always
        // being a plain AggregationConfig (which only satisfies kSum/kLastValue/kDrop).
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
            // A default-constructed HistogramAggregationConfig has empty boundaries_, which
            // LongHistogramAggregation/DoubleHistogramAggregation interpret as "use these zero
            // boundaries" rather than "no boundaries configured" (that distinction only exists
            // when the config pointer itself is null). Since this config is synthesized here
            // rather than coming from an explicit `aggregation` block, it must carry the SDK's
            // default boundaries to preserve the instrument's default histogram shape.
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

    // FIXME-SDK: The CreateAttributesProcessor method is not implemented yet.
    if (stream->attribute_keys != nullptr)
    {
      sdk_attribute_processor = CreateAttributesProcessor(stream->attribute_keys);
    }

    auto sdk_view = std::make_unique<opentelemetry::sdk::metrics::View>(
        stream->name, stream->description, sdk_aggregation_type, sdk_aggregation_config,
        std::move(sdk_attribute_processor));

    view_registry->AddView(std::move(sdk_instrument_selector), std::move(sdk_meter_selector),
                           std::move(sdk_view));
  };  // add_view

  // If the instrument type is "none", add views to select all instrument types.
  // FIXME-SDK: register a single view instead. InstrumentSelector must support an optional
  // instrument type (a "match all types" value honored by ViewRegistry::MatchInstrument).
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
SdkBuilder::CreateMeterConfigurator(
    const std::unique_ptr<MeterConfiguratorConfiguration> &model) const
{
  const MeterConfiguratorBuilder *builder = registry_->GetMeterConfiguratorBuilder();
  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateMeterConfigurator() using registered builder");
    return builder->Build(model.get());
  }
  static const std::string die("No builder for MeterConfigurator");
  throw UnsupportedException(die);
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

#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
  auto sdk_exemplar_filter = ConvertExemplarFilter(model->exemplar_filter);
#endif

  std::unique_ptr<opentelemetry::sdk::metrics::MeterContext> meter_context;
  if (model->meter_configurator)
  {
    auto meter_configurator = CreateMeterConfigurator(model->meter_configurator);
#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
    meter_context = opentelemetry::sdk::metrics::MeterContextFactory::Create(
        std::move(view_registry), resource, std::move(meter_configurator), sdk_exemplar_filter);
#else
    meter_context = opentelemetry::sdk::metrics::MeterContextFactory::Create(
        std::move(view_registry), resource, std::move(meter_configurator));
#endif
  }
  else
  {
#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
    auto default_model      = std::make_unique<MeterConfiguratorConfiguration>();
    auto meter_configurator = CreateMeterConfigurator(default_model);
    meter_context           = opentelemetry::sdk::metrics::MeterContextFactory::Create(
        std::move(view_registry), resource, std::move(meter_configurator), sdk_exemplar_filter);
#else
    auto default_model      = std::make_unique<MeterConfiguratorConfiguration>();
    auto meter_configurator = CreateMeterConfigurator(default_model);
    meter_context           = opentelemetry::sdk::metrics::MeterContextFactory::Create(
        std::move(view_registry), resource, std::move(meter_configurator));
#endif
  }

  for (const auto &reader_configuration : model->readers)
  {
    std::shared_ptr<opentelemetry::sdk::metrics::MetricReader> metric_reader;
    metric_reader = CreateMetricReader(reader_configuration);
    meter_context->AddMetricReader(metric_reader);
  }

#ifndef ENABLE_METRICS_EXEMPLAR_PREVIEW
  /* Do not spam with warnings if disabled anyway. */
  if (model->exemplar_filter != ExemplarFilter::always_off)
  {
    OTEL_INTERNAL_LOG_WARN("ENABLE_METRICS_EXEMPLAR_PREVIEW not set, ignoring exemplar filter");
  }
#endif /* ENABLE_METRICS_EXEMPLAR_PREVIEW */

  sdk = opentelemetry::sdk::metrics::MeterProviderFactory::Create(std::move(meter_context));

  return sdk;
}

std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter>
SdkBuilder::CreateOtlpHttpLogRecordExporter(
    const opentelemetry::sdk::configuration::OtlpHttpLogRecordExporterConfiguration *model) const
{
  std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> sdk;
  const OtlpHttpLogRecordExporterBuilder *builder = registry_->GetOtlpHttpLogRecordBuilder();
  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateOtlpHttpLogRecordExporter() using registered http builder");
    sdk = builder->Build(model);
    return sdk;
  }

  static const std::string die("No http builder for OtlpLogRecordExporter");
  throw UnsupportedException(die);
}

std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter>
SdkBuilder::CreateOtlpGrpcLogRecordExporter(
    const opentelemetry::sdk::configuration::OtlpGrpcLogRecordExporterConfiguration *model) const
{
  std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> sdk;
  const OtlpGrpcLogRecordExporterBuilder *builder = registry_->GetOtlpGrpcLogRecordBuilder();
  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateOtlpGrpcLogRecordExporter() using registered grpc builder");
    sdk = builder->Build(model);
    return sdk;
  }

  static const std::string die("No grpc builder for OtlpLogRecordExporter");
  throw UnsupportedException(die);
}

std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter>
SdkBuilder::CreateOtlpFileLogRecordExporter(
    const opentelemetry::sdk::configuration::OtlpFileLogRecordExporterConfiguration *model) const
{
  std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> sdk;
  const OtlpFileLogRecordExporterBuilder *builder = registry_->GetOtlpFileLogRecordBuilder();
  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateOtlpFileLogRecordExporter() using registered file builder");
    sdk = builder->Build(model);
    return sdk;
  }

  static const std::string die("No file builder for OtlpLogRecordExporter");
  throw UnsupportedException(die);
}

std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter>
SdkBuilder::CreateConsoleLogRecordExporter(
    const opentelemetry::sdk::configuration::ConsoleLogRecordExporterConfiguration *model) const
{
  std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> sdk;
  const ConsoleLogRecordExporterBuilder *builder = registry_->GetConsoleLogRecordBuilder();

  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateConsoleLogRecordExporter() using registered builder");
    sdk = builder->Build(model);
    return sdk;
  }

  static const std::string die("No builder for ConsoleLogRecordExporter");
  throw UnsupportedException(die);
}

std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter>
SdkBuilder::CreateExtensionLogRecordExporter(
    const opentelemetry::sdk::configuration::ExtensionLogRecordExporterConfiguration *model) const
{
  std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> sdk;
  std::string name = model->name;

  const ExtensionLogRecordExporterBuilder *builder =
      registry_->GetExtensionLogRecordExporterBuilder(name);

  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateExtensionLogRecordExporter() using registered builder " << name);
    sdk = builder->Build(model);
    return sdk;
  }

  std::string die("CreateExtensionLogRecordExporter() no builder for ");
  die.append(name);
  throw UnsupportedException(die);
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
  auto exporter_sdk = CreateLogRecordExporter(model->exporter);

  const BatchLogRecordProcessorBuilder *builder = registry_->GetBatchLogRecordProcessorBuilder();
  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateBatchLogRecordProcessor() using registered builder");
    return builder->Build(model, std::move(exporter_sdk));
  }
  static const std::string die("No builder for BatchLogRecordProcessor");
  throw UnsupportedException(die);
}

std::unique_ptr<opentelemetry::sdk::logs::LogRecordProcessor>
SdkBuilder::CreateSimpleLogRecordProcessor(
    const opentelemetry::sdk::configuration::SimpleLogRecordProcessorConfiguration *model) const
{
  auto exporter_sdk = CreateLogRecordExporter(model->exporter);

  const SimpleLogRecordProcessorBuilder *builder = registry_->GetSimpleLogRecordProcessorBuilder();
  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateSimpleLogRecordProcessor() using registered builder");
    return builder->Build(model, std::move(exporter_sdk));
  }
  static const std::string die("No builder for SimpleLogRecordProcessor");
  throw UnsupportedException(die);
}

std::unique_ptr<opentelemetry::sdk::logs::LogRecordProcessor>
SdkBuilder::CreateExtensionLogRecordProcessor(
    const opentelemetry::sdk::configuration::ExtensionLogRecordProcessorConfiguration *model) const
{
  std::unique_ptr<opentelemetry::sdk::logs::LogRecordProcessor> sdk;
  std::string name = model->name;

  const ExtensionLogRecordProcessorBuilder *builder =
      registry_->GetExtensionLogRecordProcessorBuilder(name);

  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateExtensionLogRecordProcessor() using registered builder "
                            << name);
    sdk = builder->Build(model);
    return sdk;
  }

  std::string die("CreateExtensionLogRecordProcessor() no builder for ");
  die.append(name);
  throw UnsupportedException(die);
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

std::unique_ptr<opentelemetry::sdk::instrumentationscope::ScopeConfigurator<
    opentelemetry::sdk::logs::LoggerConfig>>
SdkBuilder::CreateLoggerConfigurator(
    const std::unique_ptr<LoggerConfiguratorConfiguration> &model) const
{
  const LoggerConfiguratorBuilder *builder = registry_->GetLoggerConfiguratorBuilder();
  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateLoggerConfigurator() using registered builder");
    return builder->Build(model.get());
  }
  static const std::string die("No builder for LoggerConfigurator");
  throw UnsupportedException(die);
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

  opentelemetry::sdk::logs::LogRecordLimits log_record_limits;
  if (model->limits)
  {
    log_record_limits.attribute_value_length_limit = model->limits->attribute_value_length_limit;
    log_record_limits.attribute_count_limit        = model->limits->attribute_count_limit;
  }

  std::unique_ptr<opentelemetry::sdk::instrumentationscope::ScopeConfigurator<
      opentelemetry::sdk::logs::LoggerConfig>>
      logger_configurator;
  if (model->logger_configurator)
  {
    logger_configurator = CreateLoggerConfigurator(model->logger_configurator);
  }
  else
  {
    auto default_model  = std::make_unique<LoggerConfiguratorConfiguration>();
    logger_configurator = CreateLoggerConfigurator(default_model);
  }

  sdk = opentelemetry::sdk::logs::LoggerProviderFactory::Create(
      std::move(sdk_processors), resource, std::move(logger_configurator), log_record_limits);

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
    OTEL_INTERNAL_LOG_DEBUG("CreateContainerResourceDetector() using registered builder");
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
    OTEL_INTERNAL_LOG_DEBUG("CreateHostResourceDetector() using registered builder");
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
    OTEL_INTERNAL_LOG_DEBUG("CreateProcessResourceDetector() using registered builder");
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
    OTEL_INTERNAL_LOG_DEBUG("CreateServiceResourceDetector() using registered builder");
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
    OTEL_INTERNAL_LOG_DEBUG("CreateExtensionResourceDetector() using registered builder " << name);
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
          opentelemetry::common::AttributeValue wrapped_attribute_value(attribute_value);
          list_attributes.SetAttribute(attribute_key, wrapped_attribute_value);
        }
        else
        {
          OTEL_INTERNAL_LOG_WARN("Found invalid key/value pair in attributes_list");
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
        SetResourceAttribute(sdk_attributes, kv.first, kv.second.get());
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

    if (model->attribute_limits)
    {
      // FIXME-SDK: https://github.com/open-telemetry/opentelemetry-cpp/issues/3303
      // FIXME-SDK: Implement attribute limits
      OTEL_INTERNAL_LOG_WARN("attribute_limits not supported, ignoring");
    }

    if (model->tracer_provider)
    {
      sdk->tracer_provider = CreateTracerProvider(model->tracer_provider, sdk->resource);
    }

    if (model->propagator)
    {
      sdk->propagator = CreatePropagator(model->propagator);
    }

    if (model->meter_provider)
    {
      sdk->meter_provider = CreateMeterProvider(model->meter_provider, sdk->resource);
    }

    if (model->logger_provider)
    {
      sdk->logger_provider = CreateLoggerProvider(model->logger_provider, sdk->resource);
    }
  }

  return sdk;
}

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
