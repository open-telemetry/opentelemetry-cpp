// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/sdk/configuration/always_off_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/configuration.h"
#include "opentelemetry/sdk/trace/processor.h"
#include "opentelemetry/sdk/trace/sampler.h"
#include "opentelemetry/trace/tracer_provider.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace init
{

class SdkBuilder
{
public:
  static std::unique_ptr<opentelemetry::sdk::trace::Sampler> CreateAlwaysOffSampler(
      const opentelemetry::sdk::configuration::AlwaysOffSamplerConfiguration *model);

  static std::unique_ptr<opentelemetry::sdk::trace::Sampler> CreateAlwaysOnSampler(
      const opentelemetry::sdk::configuration::AlwaysOnSamplerConfiguration *model);

  static std::unique_ptr<opentelemetry::sdk::trace::Sampler> CreateJaegerRemoteSampler(
      const opentelemetry::sdk::configuration::JaegerRemoteSamplerConfiguration *model);

  static std::unique_ptr<opentelemetry::sdk::trace::Sampler> CreateParentBasedSampler(
      const opentelemetry::sdk::configuration::ParentBasedSamplerConfiguration *model);

  static std::unique_ptr<opentelemetry::sdk::trace::Sampler> CreateTraceIdRatioBasedSampler(
      const opentelemetry::sdk::configuration::TraceIdRatioBasedSamplerConfiguration *model);

  static std::unique_ptr<opentelemetry::sdk::trace::Sampler> CreateExtensionSampler(
      const opentelemetry::sdk::configuration::ExtensionSamplerConfiguration *model);

  static std::unique_ptr<opentelemetry::sdk::trace::Sampler> CreateSampler(
      const std::unique_ptr<opentelemetry::sdk::configuration::SamplerConfiguration> &model);

  static std::unique_ptr<opentelemetry::sdk::trace::SpanProcessor> CreateProcessor(
      const std::unique_ptr<opentelemetry::sdk::configuration::SpanProcessorConfiguration> &model);

  static std::unique_ptr<opentelemetry::trace::TracerProvider> CreateTracerProvider(
      const std::unique_ptr<opentelemetry::sdk::configuration::TracerProviderConfiguration> &model);

  static std::unique_ptr<ConfiguredSdk> CreateConfiguredSdk(
      const std::unique_ptr<opentelemetry::sdk::configuration::Configuration> &model);

#if 0
  static std::unique_ptr<opentelemetry::trace::XXX> CreateXXX(
      const std::unique_ptr<opentelemetry::sdk::configuration::XXX> &model);
#endif
};

}  // namespace init
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
