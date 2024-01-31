// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/common/global_log_handler.h"

#include "opentelemetry/sdk/configuration/always_off_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/always_on_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/batch_span_processor_configuration.h"
#include "opentelemetry/sdk/configuration/configuration.h"
#include "opentelemetry/sdk/configuration/configuration_factory.h"
#include "opentelemetry/sdk/configuration/document.h"
#include "opentelemetry/sdk/configuration/document_node.h"
#include "opentelemetry/sdk/configuration/jaeger_remote_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/otlp_span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/parent_based_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/sampler_configuration_visitor.h"
#include "opentelemetry/sdk/configuration/simple_span_processor_configuration.h"
#include "opentelemetry/sdk/configuration/trace_id_ratio_based_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/yaml_document.h"
#include "opentelemetry/sdk/configuration/zipkin_span_exporter_configuration.h"
#include "opentelemetry/sdk/init/configured_sdk.h"
#include "opentelemetry/sdk/init/sdk_builder.h"
#include "opentelemetry/sdk/trace/processor.h"
#include "opentelemetry/sdk/trace/sampler.h"
#include "opentelemetry/sdk/trace/samplers/always_off_factory.h"
#include "opentelemetry/sdk/trace/samplers/always_on_factory.h"
#include "opentelemetry/sdk/trace/samplers/parent_factory.h"
#include "opentelemetry/sdk/trace/samplers/trace_id_ratio_factory.h"
#include "opentelemetry/sdk/trace/tracer_provider_factory.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace init
{

class SamplerBuilder : public opentelemetry::sdk::configuration::SamplerConfigurationVisitor
{
public:
  void VisitAlwaysOff(
      const opentelemetry::sdk::configuration::AlwaysOffSamplerConfiguration *model) override
  {
    sampler = SdkBuilder::CreateAlwaysOffSampler(model);
  }

  void VisitAlwaysOn(
      const opentelemetry::sdk::configuration::AlwaysOnSamplerConfiguration *model) override
  {
    sampler = SdkBuilder::CreateAlwaysOnSampler(model);
  }

  void VisitJaegerRemote(
      const opentelemetry::sdk::configuration::JaegerRemoteSamplerConfiguration *model) override
  {
    sampler = SdkBuilder::CreateJaegerRemoteSampler(model);
  }

  void VisitParentBased(
      const opentelemetry::sdk::configuration::ParentBasedSamplerConfiguration *model) override
  {
    sampler = SdkBuilder::CreateParentBasedSampler(model);
  }

  void VisitTraceIdRatioBased(
      const opentelemetry::sdk::configuration::TraceIdRatioBasedSamplerConfiguration *model)
      override
  {
    sampler = SdkBuilder::CreateTraceIdRatioBasedSampler(model);
  }

  void VisitExtension(
      const opentelemetry::sdk::configuration::ExtensionSamplerConfiguration *model) override
  {
    sampler = SdkBuilder::CreateExtensionSampler(model);
  }

  std::unique_ptr<opentelemetry::sdk::trace::Sampler> sampler;
};

std::unique_ptr<opentelemetry::sdk::trace::Sampler> SdkBuilder::CreateAlwaysOffSampler(
    const opentelemetry::sdk::configuration::AlwaysOffSamplerConfiguration *model)
{
  std::unique_ptr<opentelemetry::sdk::trace::Sampler> sdk;

  sdk = opentelemetry::sdk::trace::AlwaysOffSamplerFactory::Create();

  return sdk;
}

std::unique_ptr<opentelemetry::sdk::trace::Sampler> SdkBuilder::CreateAlwaysOnSampler(
    const opentelemetry::sdk::configuration::AlwaysOnSamplerConfiguration *model)
{
  std::unique_ptr<opentelemetry::sdk::trace::Sampler> sdk;

  sdk = opentelemetry::sdk::trace::AlwaysOnSamplerFactory::Create();

  return sdk;
}

std::unique_ptr<opentelemetry::sdk::trace::Sampler> SdkBuilder::CreateJaegerRemoteSampler(
    const opentelemetry::sdk::configuration::JaegerRemoteSamplerConfiguration *model)
{
  std::unique_ptr<opentelemetry::sdk::trace::Sampler> sdk;

  OTEL_INTERNAL_LOG_ERROR("CreateJaegerRemoteSampler: FIXME");

  return sdk;
}

std::unique_ptr<opentelemetry::sdk::trace::Sampler> SdkBuilder::CreateParentBasedSampler(
    const opentelemetry::sdk::configuration::ParentBasedSamplerConfiguration *model)
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
    const opentelemetry::sdk::configuration::TraceIdRatioBasedSamplerConfiguration *model)
{
  std::unique_ptr<opentelemetry::sdk::trace::Sampler> sdk;

  sdk = opentelemetry::sdk::trace::TraceIdRatioBasedSamplerFactory::Create(model->ratio);

  return sdk;
}

std::unique_ptr<opentelemetry::sdk::trace::Sampler> SdkBuilder::CreateExtensionSampler(
    const opentelemetry::sdk::configuration::ExtensionSamplerConfiguration *model)
{
  std::unique_ptr<opentelemetry::sdk::trace::Sampler> sdk;

  OTEL_INTERNAL_LOG_ERROR("CreateExtensionSampler: FIXME");

  return sdk;
}

std::unique_ptr<opentelemetry::sdk::trace::Sampler> SdkBuilder::CreateSampler(
    const std::unique_ptr<opentelemetry::sdk::configuration::SamplerConfiguration> &model)
{
  std::unique_ptr<opentelemetry::sdk::trace::Sampler> sdk;

  SamplerBuilder builder;
  model->Accept(&builder);
  sdk = std::move(builder.sampler);

  return sdk;
}

std::unique_ptr<opentelemetry::sdk::trace::SpanProcessor> SdkBuilder::CreateProcessor(
    const std::unique_ptr<opentelemetry::sdk::configuration::SpanProcessorConfiguration> &model)
{
  std::unique_ptr<opentelemetry::sdk::trace::SpanProcessor> sdk;

  OTEL_INTERNAL_LOG_ERROR("CreateProcessor: FIXME");

  // sdk = xxx;

  return sdk;
}

std::unique_ptr<opentelemetry::trace::TracerProvider> SdkBuilder::CreateTracerProvider(
    const std::unique_ptr<opentelemetry::sdk::configuration::TracerProviderConfiguration> &model)
{
  std::unique_ptr<opentelemetry::trace::TracerProvider> sdk;

  OTEL_INTERNAL_LOG_ERROR("CreateTracerProvider: FIXME");

  auto sampler = CreateSampler(model->sampler);

  std::vector<std::unique_ptr<opentelemetry::sdk::trace::SpanProcessor>> sdk_processors;

  for (const auto &processor_model : model->processors)
  {
    sdk_processors.push_back(CreateProcessor(processor_model));
  }

  sdk = opentelemetry::sdk::trace::TracerProviderFactory::Create(std::move(sdk_processors));

  return sdk;
}

std::unique_ptr<ConfiguredSdk> SdkBuilder::CreateConfiguredSdk(
    const std::unique_ptr<opentelemetry::sdk::configuration::Configuration> &model)
{
  std::unique_ptr<ConfiguredSdk> sdk(new ConfiguredSdk);

  if (!model->disabled)
  {
    sdk->m_tracer_provider = CreateTracerProvider(model->tracer_provider);
  }

  return sdk;
}

}  // namespace init
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
