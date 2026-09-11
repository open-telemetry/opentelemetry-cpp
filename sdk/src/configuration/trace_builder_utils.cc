// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/configuration/trace_builder_utils.h"

#include <cstddef>
#include <memory>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/configuration/always_off_sampler_builder.h"
#include "opentelemetry/sdk/configuration/always_off_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/always_on_sampler_builder.h"
#include "opentelemetry/sdk/configuration/always_on_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/batch_span_processor_builder.h"
#include "opentelemetry/sdk/configuration/batch_span_processor_configuration.h"
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
#include "opentelemetry/sdk/configuration/sampler_configuration_visitor.h"
#include "opentelemetry/sdk/configuration/simple_span_processor_builder.h"
#include "opentelemetry/sdk/configuration/simple_span_processor_configuration.h"
#include "opentelemetry/sdk/configuration/span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/span_exporter_configuration_visitor.h"
#include "opentelemetry/sdk/configuration/span_processor_configuration.h"
#include "opentelemetry/sdk/configuration/span_processor_configuration_visitor.h"
#include "opentelemetry/sdk/configuration/trace_id_ratio_based_sampler_builder.h"
#include "opentelemetry/sdk/configuration/trace_id_ratio_based_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/tracer_configurator_builder.h"
#include "opentelemetry/sdk/configuration/tracer_configurator_configuration.h"
#include "opentelemetry/sdk/configuration/unsupported_exception.h"
#include "opentelemetry/sdk/instrumentationscope/scope_configurator.h"
#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/processor.h"
#include "opentelemetry/sdk/trace/sampler.h"
#include "opentelemetry/sdk/trace/samplers/composable_sampler.h"
#include "opentelemetry/sdk/trace/tracer_config.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

namespace
{

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
  // Recursion is used to build the nested samplers; depth is limited to kMaxDepth.
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
    static const std::string message("No builder for ComposableAlwaysOffSampler");
    throw UnsupportedException(message);
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
    static const std::string message("No builder for ComposableAlwaysOnSampler");
    throw UnsupportedException(message);
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
    static const std::string message("No builder for ComposableProbabilitySampler");
    throw UnsupportedException(message);
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
        static const std::string message("No builder for ComposableAlwaysOnSampler");
        throw UnsupportedException(message);
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
    static const std::string message("No builder for ComposableParentThresholdSampler");
    throw UnsupportedException(message);
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
    static const std::string message("No builder for ComposableRuleBasedSampler");
    throw UnsupportedException(message);
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
    std::string message("No builder for extension composable sampler ");
    message.append(model->name);
    throw UnsupportedException(message);
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
      std::string message("Composable sampler nesting depth exceeds ");
      message.append(std::to_string(kMaxDepth));
      throw UnsupportedException(message);
    }
    ComposableSamplerBuilder child(registry_, depth_ + 1);
    child_model->Accept(&child);
    return std::move(child.sampler);
  }
  // NOLINTEND(misc-no-recursion)

  const Registry *registry_;
  std::size_t depth_;
};

// NOLINTBEGIN(misc-no-recursion)
static std::unique_ptr<opentelemetry::sdk::trace::Sampler> CreateAlwaysOffSampler(
    const Registry *registry,
    const opentelemetry::sdk::configuration::AlwaysOffSamplerConfiguration *model)
{
  const AlwaysOffSamplerBuilder *builder = registry->GetAlwaysOffSamplerBuilder();
  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateAlwaysOffSampler() using registered builder");
    return builder->Build(model);
  }
  static const std::string message("No builder for AlwaysOffSampler");
  throw UnsupportedException(message);
}

static std::unique_ptr<opentelemetry::sdk::trace::Sampler> CreateAlwaysOnSampler(
    const Registry *registry,
    const opentelemetry::sdk::configuration::AlwaysOnSamplerConfiguration *model)
{
  const AlwaysOnSamplerBuilder *builder = registry->GetAlwaysOnSamplerBuilder();
  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateAlwaysOnSampler() using registered builder");
    return builder->Build(model);
  }
  static const std::string message("No builder for AlwaysOnSampler");
  throw UnsupportedException(message);
}

static std::unique_ptr<opentelemetry::sdk::trace::Sampler> CreateProbabilitySampler(
    const Registry *registry,
    const opentelemetry::sdk::configuration::ProbabilitySamplerConfiguration *model)
{
  const ProbabilitySamplerBuilder *builder = registry->GetProbabilitySamplerBuilder();
  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateProbabilitySampler() using registered builder");
    return builder->Build(model);
  }
  static const std::string message("No builder for ProbabilitySampler");
  throw UnsupportedException(message);
}

static std::unique_ptr<opentelemetry::sdk::trace::Sampler> CreateTraceIdRatioBasedSampler(
    const Registry *registry,
    const opentelemetry::sdk::configuration::TraceIdRatioBasedSamplerConfiguration *model)
{
  const TraceIdRatioBasedSamplerBuilder *builder = registry->GetTraceIdRatioBasedSamplerBuilder();
  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateTraceIdRatioBasedSampler() using registered builder");
    return builder->Build(model);
  }
  static const std::string message("No builder for TraceIdRatioBasedSampler");
  throw UnsupportedException(message);
}

static std::unique_ptr<opentelemetry::sdk::trace::Sampler> CreateExtensionSampler(
    const Registry *registry,
    const opentelemetry::sdk::configuration::ExtensionSamplerConfiguration *model)
{
  std::string name                       = model->name;
  const ExtensionSamplerBuilder *builder = registry->GetExtensionSamplerBuilder(name);
  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateExtensionSampler() using registered builder " << name);
    return builder->Build(model);
  }
  std::string message("CreateExtensionSampler() no builder for ");
  message.append(name);
  throw UnsupportedException(message);
}

static std::unique_ptr<opentelemetry::sdk::trace::Sampler> CreateJaegerRemoteSampler(
    const Registry *registry,
    const opentelemetry::sdk::configuration::JaegerRemoteSamplerConfiguration *model)
{
  const JaegerRemoteSamplerBuilder *builder = registry->GetJaegerRemoteSamplerBuilder();
  if (builder != nullptr)
  {
    static const opentelemetry::sdk::configuration::AlwaysOnSamplerConfiguration kAlwaysOn;
    auto initial_sampler = CreateAlwaysOnSampler(registry, &kAlwaysOn);
    OTEL_INTERNAL_LOG_DEBUG("CreateJaegerRemoteSampler() using registered builder");
    return builder->Build(model, std::move(initial_sampler));
  }
  static const std::string message("No builder for JaegerRemoteSampler");
  throw UnsupportedException(message);
}

static std::unique_ptr<opentelemetry::sdk::trace::Sampler> CreateParentBasedSampler(
    const Registry *registry,
    const opentelemetry::sdk::configuration::ParentBasedSamplerConfiguration *model)
{
  std::unique_ptr<opentelemetry::sdk::trace::Sampler> root_sdk;
  if (model->root)
  {
    root_sdk = TraceBuilderUtils::CreateSampler(registry, model->root);
  }
  else
  {
    static const opentelemetry::sdk::configuration::AlwaysOnSamplerConfiguration kAlwaysOn;
    root_sdk = CreateAlwaysOnSampler(registry, &kAlwaysOn);
  }

  std::unique_ptr<opentelemetry::sdk::trace::Sampler> remote_parent_sampled_sdk;
  if (model->remote_parent_sampled != nullptr)
  {
    remote_parent_sampled_sdk =
        TraceBuilderUtils::CreateSampler(registry, model->remote_parent_sampled);
  }
  else
  {
    static const opentelemetry::sdk::configuration::AlwaysOnSamplerConfiguration kAlwaysOn;
    remote_parent_sampled_sdk = CreateAlwaysOnSampler(registry, &kAlwaysOn);
  }

  std::unique_ptr<opentelemetry::sdk::trace::Sampler> remote_parent_not_sampled_sdk;
  if (model->remote_parent_not_sampled != nullptr)
  {
    remote_parent_not_sampled_sdk =
        TraceBuilderUtils::CreateSampler(registry, model->remote_parent_not_sampled);
  }
  else
  {
    static const opentelemetry::sdk::configuration::AlwaysOffSamplerConfiguration kAlwaysOff;
    remote_parent_not_sampled_sdk = CreateAlwaysOffSampler(registry, &kAlwaysOff);
  }

  std::unique_ptr<opentelemetry::sdk::trace::Sampler> local_parent_sampled_sdk;
  if (model->local_parent_sampled != nullptr)
  {
    local_parent_sampled_sdk =
        TraceBuilderUtils::CreateSampler(registry, model->local_parent_sampled);
  }
  else
  {
    static const opentelemetry::sdk::configuration::AlwaysOnSamplerConfiguration kAlwaysOn;
    local_parent_sampled_sdk = CreateAlwaysOnSampler(registry, &kAlwaysOn);
  }

  std::unique_ptr<opentelemetry::sdk::trace::Sampler> local_parent_not_sampled_sdk;
  if (model->local_parent_not_sampled != nullptr)
  {
    local_parent_not_sampled_sdk =
        TraceBuilderUtils::CreateSampler(registry, model->local_parent_not_sampled);
  }
  else
  {
    static const opentelemetry::sdk::configuration::AlwaysOffSamplerConfiguration kAlwaysOff;
    local_parent_not_sampled_sdk = CreateAlwaysOffSampler(registry, &kAlwaysOff);
  }

  const ParentBasedSamplerBuilder *builder = registry->GetParentBasedSamplerBuilder();
  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateParentBasedSampler() using registered builder");
    return builder->Build(model, std::move(root_sdk), std::move(remote_parent_sampled_sdk),
                          std::move(remote_parent_not_sampled_sdk),
                          std::move(local_parent_sampled_sdk),
                          std::move(local_parent_not_sampled_sdk));
  }
  static const std::string message("No builder for ParentBasedSampler");
  throw UnsupportedException(message);
}

static std::unique_ptr<opentelemetry::sdk::trace::Sampler> CreateCompositeSampler(
    const Registry *registry,
    const opentelemetry::sdk::configuration::ComposableSamplerConfiguration *model)
{
  const CompositeSamplerBuilder *builder = registry->GetCompositeSamplerBuilder();
  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateCompositeSampler() using registered builder");
    ComposableSamplerBuilder composable_builder(registry, 1);
    model->Accept(&composable_builder);
    return builder->Build(std::move(composable_builder.sampler));
  }
  static const std::string message("No builder for CompositeSampler");
  throw UnsupportedException(message);
}

class SamplerBuilder : public opentelemetry::sdk::configuration::SamplerConfigurationVisitor
{
public:
  explicit SamplerBuilder(const Registry *registry) : registry_(registry) {}
  SamplerBuilder(SamplerBuilder &&)                      = delete;
  SamplerBuilder(const SamplerBuilder &)                 = delete;
  SamplerBuilder &operator=(SamplerBuilder &&)           = delete;
  SamplerBuilder &operator=(const SamplerBuilder &other) = delete;
  ~SamplerBuilder() override                             = default;

  void VisitAlwaysOff(
      const opentelemetry::sdk::configuration::AlwaysOffSamplerConfiguration *model) override
  {
    sampler = CreateAlwaysOffSampler(registry_, model);
  }

  void VisitAlwaysOn(
      const opentelemetry::sdk::configuration::AlwaysOnSamplerConfiguration *model) override
  {
    sampler = CreateAlwaysOnSampler(registry_, model);
  }

  void VisitJaegerRemote(
      const opentelemetry::sdk::configuration::JaegerRemoteSamplerConfiguration *model) override
  {
    sampler = CreateJaegerRemoteSampler(registry_, model);
  }

  void VisitParentBased(
      const opentelemetry::sdk::configuration::ParentBasedSamplerConfiguration *model) override
  {
    sampler = CreateParentBasedSampler(registry_, model);
  }

  void VisitProbability(
      const opentelemetry::sdk::configuration::ProbabilitySamplerConfiguration *model) override
  {
    sampler = CreateProbabilitySampler(registry_, model);
  }

  void VisitTraceIdRatioBased(
      const opentelemetry::sdk::configuration::TraceIdRatioBasedSamplerConfiguration *model)
      override
  {
    sampler = CreateTraceIdRatioBasedSampler(registry_, model);
  }

  void VisitExtension(
      const opentelemetry::sdk::configuration::ExtensionSamplerConfiguration *model) override
  {
    sampler = CreateExtensionSampler(registry_, model);
  }

  void VisitComposite(
      const opentelemetry::sdk::configuration::CompositeSamplerConfiguration *model) override
  {
    sampler = CreateCompositeSampler(registry_, model->composable_sampler.get());
  }

  std::unique_ptr<opentelemetry::sdk::trace::Sampler> sampler;

private:
  const Registry *registry_;
};
// NOLINTEND(misc-no-recursion)

static std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> CreateOtlpHttpSpanExporter(
    const Registry *registry,
    const opentelemetry::sdk::configuration::OtlpHttpSpanExporterConfiguration *model)
{
  const OtlpHttpSpanExporterBuilder *builder = registry->GetOtlpHttpSpanBuilder();
  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateOtlpHttpSpanExporter() using registered http builder");
    return builder->Build(model);
  }
  static const std::string message("No http builder for OtlpHttpSpanExporter");
  throw UnsupportedException(message);
}

static std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> CreateOtlpGrpcSpanExporter(
    const Registry *registry,
    const opentelemetry::sdk::configuration::OtlpGrpcSpanExporterConfiguration *model)
{
  const OtlpGrpcSpanExporterBuilder *builder = registry->GetOtlpGrpcSpanBuilder();
  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateOtlpGrpcSpanExporter() using registered grpc builder");
    return builder->Build(model);
  }
  static const std::string message("No builder for OtlpGrpcSpanExporter");
  throw UnsupportedException(message);
}

static std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> CreateOtlpFileSpanExporter(
    const Registry *registry,
    const opentelemetry::sdk::configuration::OtlpFileSpanExporterConfiguration *model)
{
  const OtlpFileSpanExporterBuilder *builder = registry->GetOtlpFileSpanBuilder();
  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateOtlpFileSpanExporter() using registered file builder");
    return builder->Build(model);
  }
  static const std::string message("No builder for OtlpFileSpanExporter");
  throw UnsupportedException(message);
}

static std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> CreateConsoleSpanExporter(
    const Registry *registry,
    const opentelemetry::sdk::configuration::ConsoleSpanExporterConfiguration *model)
{
  const ConsoleSpanExporterBuilder *builder = registry->GetConsoleSpanBuilder();
  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateConsoleSpanExporter() using registered builder");
    return builder->Build(model);
  }
  static const std::string message("No builder for ConsoleSpanExporter");
  throw UnsupportedException(message);
}

static std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> CreateExtensionSpanExporter(
    const Registry *registry,
    const opentelemetry::sdk::configuration::ExtensionSpanExporterConfiguration *model)
{
  std::string name                            = model->name;
  const ExtensionSpanExporterBuilder *builder = registry->GetExtensionSpanExporterBuilder(name);
  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateExtensionSpanExporter() using registered builder " << name);
    return builder->Build(model);
  }
  std::string message("CreateExtensionSpanExporter() no builder for ");
  message.append(name);
  throw UnsupportedException(message);
}

class SpanExporterBuilder
    : public opentelemetry::sdk::configuration::SpanExporterConfigurationVisitor
{
public:
  explicit SpanExporterBuilder(const Registry *registry) : registry_(registry) {}
  SpanExporterBuilder(SpanExporterBuilder &&)                      = delete;
  SpanExporterBuilder(const SpanExporterBuilder &)                 = delete;
  SpanExporterBuilder &operator=(SpanExporterBuilder &&)           = delete;
  SpanExporterBuilder &operator=(const SpanExporterBuilder &other) = delete;
  ~SpanExporterBuilder() override                                  = default;

  void VisitOtlpHttp(
      const opentelemetry::sdk::configuration::OtlpHttpSpanExporterConfiguration *model) override
  {
    exporter = CreateOtlpHttpSpanExporter(registry_, model);
  }

  void VisitOtlpGrpc(
      const opentelemetry::sdk::configuration::OtlpGrpcSpanExporterConfiguration *model) override
  {
    exporter = CreateOtlpGrpcSpanExporter(registry_, model);
  }

  void VisitOtlpFile(
      const opentelemetry::sdk::configuration::OtlpFileSpanExporterConfiguration *model) override
  {
    exporter = CreateOtlpFileSpanExporter(registry_, model);
  }

  void VisitConsole(
      const opentelemetry::sdk::configuration::ConsoleSpanExporterConfiguration *model) override
  {
    exporter = CreateConsoleSpanExporter(registry_, model);
  }

  void VisitExtension(
      const opentelemetry::sdk::configuration::ExtensionSpanExporterConfiguration *model) override
  {
    exporter = CreateExtensionSpanExporter(registry_, model);
  }

  std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> exporter;

private:
  const Registry *registry_;
};

static std::unique_ptr<opentelemetry::sdk::trace::SpanProcessor> CreateExtensionSpanProcessor(
    const Registry *registry,
    const opentelemetry::sdk::configuration::ExtensionSpanProcessorConfiguration *model)
{
  std::string name                             = model->name;
  const ExtensionSpanProcessorBuilder *builder = registry->GetExtensionSpanProcessorBuilder(name);
  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateExtensionSpanProcessor() using registered builder " << name);
    return builder->Build(model);
  }
  std::string message("CreateExtensionSpanProcessor() no builder for ");
  message.append(name);
  throw UnsupportedException(message);
}

class SpanProcessorBuilder
    : public opentelemetry::sdk::configuration::SpanProcessorConfigurationVisitor
{
public:
  explicit SpanProcessorBuilder(const Registry *registry) : registry_(registry) {}
  SpanProcessorBuilder(SpanProcessorBuilder &&)                      = delete;
  SpanProcessorBuilder(const SpanProcessorBuilder &)                 = delete;
  SpanProcessorBuilder &operator=(SpanProcessorBuilder &&)           = delete;
  SpanProcessorBuilder &operator=(const SpanProcessorBuilder &other) = delete;
  ~SpanProcessorBuilder() override                                   = default;

  void VisitBatch(
      const opentelemetry::sdk::configuration::BatchSpanProcessorConfiguration *model) override
  {
    auto exporter_sdk = TraceBuilderUtils::CreateSpanExporter(registry_, model->exporter);
    const BatchSpanProcessorBuilder *builder = registry_->GetBatchSpanProcessorBuilder();
    if (builder != nullptr)
    {
      OTEL_INTERNAL_LOG_DEBUG("CreateBatchSpanProcessor() using registered builder");
      processor = builder->Build(model, std::move(exporter_sdk));
      return;
    }
    static const std::string message("No builder for BatchSpanProcessor");
    throw UnsupportedException(message);
  }

  void VisitSimple(
      const opentelemetry::sdk::configuration::SimpleSpanProcessorConfiguration *model) override
  {
    auto exporter_sdk = TraceBuilderUtils::CreateSpanExporter(registry_, model->exporter);
    const SimpleSpanProcessorBuilder *builder = registry_->GetSimpleSpanProcessorBuilder();
    if (builder != nullptr)
    {
      OTEL_INTERNAL_LOG_DEBUG("CreateSimpleSpanProcessor() using registered builder");
      processor = builder->Build(model, std::move(exporter_sdk));
      return;
    }
    static const std::string message("No builder for SimpleSpanProcessor");
    throw UnsupportedException(message);
  }

  void VisitExtension(
      const opentelemetry::sdk::configuration::ExtensionSpanProcessorConfiguration *model) override
  {
    processor = CreateExtensionSpanProcessor(registry_, model);
  }

  std::unique_ptr<opentelemetry::sdk::trace::SpanProcessor> processor;

private:
  const Registry *registry_;
};

}  // namespace

// NOLINTBEGIN(misc-no-recursion)
std::unique_ptr<opentelemetry::sdk::trace::Sampler> TraceBuilderUtils::CreateSampler(
    const Registry *registry,
    const std::unique_ptr<SamplerConfiguration> &model)
{
  SamplerBuilder builder(registry);
  model->Accept(&builder);
  return std::move(builder.sampler);
}
// NOLINTEND(misc-no-recursion)

std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> TraceBuilderUtils::CreateSpanExporter(
    const Registry *registry,
    const std::unique_ptr<SpanExporterConfiguration> &model)
{
  SpanExporterBuilder builder(registry);
  model->Accept(&builder);
  return std::move(builder.exporter);
}

std::unique_ptr<opentelemetry::sdk::trace::SpanProcessor> TraceBuilderUtils::CreateSpanProcessor(
    const Registry *registry,
    const std::unique_ptr<SpanProcessorConfiguration> &model)
{
  SpanProcessorBuilder builder(registry);
  model->Accept(&builder);
  return std::move(builder.processor);
}

std::unique_ptr<opentelemetry::sdk::instrumentationscope::ScopeConfigurator<
    opentelemetry::sdk::trace::TracerConfig>>
TraceBuilderUtils::CreateTracerConfigurator(
    const Registry *registry,
    const std::unique_ptr<TracerConfiguratorConfiguration> &model)
{
  const TracerConfiguratorBuilder *builder = registry->GetTracerConfiguratorBuilder();
  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateTracerConfigurator() using registered builder");
    return builder->Build(model.get());
  }
  static const std::string message("No builder for TracerConfigurator");
  throw UnsupportedException(message);
}

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
