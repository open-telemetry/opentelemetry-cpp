// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/sdk/configuration/always_off_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/batch_span_processor_configuration.h"
#include "opentelemetry/sdk/configuration/composite_propagator_configuration.h"
#include "opentelemetry/sdk/configuration/configuration.h"
#include "opentelemetry/sdk/configuration/extension_span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/extension_span_processor_configuration.h"
#include "opentelemetry/sdk/configuration/otlp_span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/simple_propagator_configuration.h"
#include "opentelemetry/sdk/configuration/simple_span_processor_configuration.h"
#include "opentelemetry/sdk/configuration/span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/zipkin_span_exporter_configuration.h"
#include "opentelemetry/sdk/init/registry.h"
#include "opentelemetry/sdk/trace/exporter.h"
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
  SdkBuilder(std::shared_ptr<Registry> registry) : m_registry(registry) {}
  ~SdkBuilder() = default;

  std::unique_ptr<opentelemetry::sdk::trace::Sampler> CreateAlwaysOffSampler(
      const opentelemetry::sdk::configuration::AlwaysOffSamplerConfiguration *model) const;

  std::unique_ptr<opentelemetry::sdk::trace::Sampler> CreateAlwaysOnSampler(
      const opentelemetry::sdk::configuration::AlwaysOnSamplerConfiguration *model) const;

  std::unique_ptr<opentelemetry::sdk::trace::Sampler> CreateJaegerRemoteSampler(
      const opentelemetry::sdk::configuration::JaegerRemoteSamplerConfiguration *model) const;

  std::unique_ptr<opentelemetry::sdk::trace::Sampler> CreateParentBasedSampler(
      const opentelemetry::sdk::configuration::ParentBasedSamplerConfiguration *model) const;

  std::unique_ptr<opentelemetry::sdk::trace::Sampler> CreateTraceIdRatioBasedSampler(
      const opentelemetry::sdk::configuration::TraceIdRatioBasedSamplerConfiguration *model) const;

  std::unique_ptr<opentelemetry::sdk::trace::Sampler> CreateExtensionSampler(
      const opentelemetry::sdk::configuration::ExtensionSamplerConfiguration *model) const;

  std::unique_ptr<opentelemetry::sdk::trace::Sampler> CreateSampler(
      const std::unique_ptr<opentelemetry::sdk::configuration::SamplerConfiguration> &model) const;

  std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> CreateOtlpSpanExporter(
      const opentelemetry::sdk::configuration::OtlpSpanExporterConfiguration *model) const;

  std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> CreateConsoleSpanExporter(
      const opentelemetry::sdk::configuration::ConsoleSpanExporterConfiguration *model) const;

  std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> CreateZipkinSpanExporter(
      const opentelemetry::sdk::configuration::ZipkinSpanExporterConfiguration *model) const;

  std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> CreateExtensionSpanExporter(
      const opentelemetry::sdk::configuration::ExtensionSpanExporterConfiguration *model) const;

  std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> CreateSpanExporter(
      const std::unique_ptr<opentelemetry::sdk::configuration::SpanExporterConfiguration> &model)
      const;

  std::unique_ptr<opentelemetry::sdk::trace::SpanProcessor> CreateBatchSpanProcessor(
      const opentelemetry::sdk::configuration::BatchSpanProcessorConfiguration *model) const;

  std::unique_ptr<opentelemetry::sdk::trace::SpanProcessor> CreateSimpleSpanProcessor(
      const opentelemetry::sdk::configuration::SimpleSpanProcessorConfiguration *model) const;

  std::unique_ptr<opentelemetry::sdk::trace::SpanProcessor> CreateExtensionSpanProcessor(
      const opentelemetry::sdk::configuration::ExtensionSpanProcessorConfiguration *model) const;

  std::unique_ptr<opentelemetry::sdk::trace::SpanProcessor> CreateProcessor(
      const std::unique_ptr<opentelemetry::sdk::configuration::SpanProcessorConfiguration> &model)
      const;

  std::unique_ptr<opentelemetry::trace::TracerProvider> CreateTracerProvider(
      const std::unique_ptr<opentelemetry::sdk::configuration::TracerProviderConfiguration> &model)
      const;

  std::unique_ptr<opentelemetry::context::propagation::TextMapPropagator> CreateTextMapPropagator(
      const std::string &name) const;

  std::unique_ptr<opentelemetry::context::propagation::TextMapPropagator> CreateSimplePropagator(
      const opentelemetry::sdk::configuration::SimplePropagatorConfiguration *model) const;

  std::unique_ptr<opentelemetry::context::propagation::TextMapPropagator> CreateCompositePropagator(
      const opentelemetry::sdk::configuration::CompositePropagatorConfiguration *model) const;

  std::unique_ptr<opentelemetry::context::propagation::TextMapPropagator> CreatePropagator(
      const std::unique_ptr<opentelemetry::sdk::configuration::PropagatorConfiguration> &model)
      const;

  std::unique_ptr<ConfiguredSdk> CreateConfiguredSdk(
      const std::unique_ptr<opentelemetry::sdk::configuration::Configuration> &model) const;

private:
  std::shared_ptr<Registry> m_registry;
};

}  // namespace init
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
