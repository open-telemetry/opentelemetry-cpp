// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/trace/tracer_provider_factory.h"
#include "opentelemetry/sdk/trace/random_id_generator_factory.h"
#include "opentelemetry/sdk/trace/samplers/always_on_factory.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"

namespace trace_api = opentelemetry::trace;
namespace trace_sdk = opentelemetry::sdk::trace;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{

std::shared_ptr<trace_api::TracerProvider> TracerProviderFactory::Build(
    std::unique_ptr<SpanProcessor> processor)
{
  auto resource = opentelemetry::sdk::resource::Resource::Create({});
  return Build(std::move(processor), resource);
}

std::shared_ptr<trace_api::TracerProvider> TracerProviderFactory::Build(
    std::unique_ptr<SpanProcessor> processor,
    opentelemetry::sdk::resource::Resource resource)
{
  auto sampler = AlwaysOnSamplerFactory::Build();
  return Build(std::move(processor), resource, std::move(sampler));
}

std::shared_ptr<opentelemetry::trace::TracerProvider> TracerProviderFactory::Build(
    std::unique_ptr<SpanProcessor> processor,
    opentelemetry::sdk::resource::Resource resource,
    std::unique_ptr<Sampler> sampler)
{
  auto id_generator = RandomIdGeneratorFactory::Build();
  return Build(std::move(processor), resource, std::move(sampler), std::move(id_generator));
}

std::shared_ptr<opentelemetry::trace::TracerProvider> TracerProviderFactory::Build(
    std::unique_ptr<SpanProcessor> processor,
    opentelemetry::sdk::resource::Resource resource,
    std::unique_ptr<Sampler> sampler,
    std::unique_ptr<IdGenerator> id_generator)
{
  std::shared_ptr<trace_api::TracerProvider> provider(new trace_sdk::TracerProvider(
      std::move(processor), resource, std::move(sampler), std::move(id_generator)));
  return provider;
}

std::shared_ptr<opentelemetry::trace::TracerProvider> TracerProviderFactory::Build(
    std::vector<std::unique_ptr<SpanProcessor>> &&processors)
{
  auto resource = opentelemetry::sdk::resource::Resource::Create({});
  return Build(std::move(processors), resource);
}

std::shared_ptr<opentelemetry::trace::TracerProvider> TracerProviderFactory::Build(
    std::vector<std::unique_ptr<SpanProcessor>> &&processors,
    opentelemetry::sdk::resource::Resource resource)
{
  auto sampler = AlwaysOnSamplerFactory::Build();
  return Build(std::move(processors), resource, std::move(sampler));
}

std::shared_ptr<opentelemetry::trace::TracerProvider> TracerProviderFactory::Build(
    std::vector<std::unique_ptr<SpanProcessor>> &&processors,
    opentelemetry::sdk::resource::Resource resource,
    std::unique_ptr<Sampler> sampler)
{
  auto id_generator = RandomIdGeneratorFactory::Build();
  return Build(std::move(processors), resource, std::move(sampler), std::move(id_generator));
}

std::shared_ptr<opentelemetry::trace::TracerProvider> TracerProviderFactory::Build(
    std::vector<std::unique_ptr<SpanProcessor>> &&processors,
    opentelemetry::sdk::resource::Resource resource,
    std::unique_ptr<Sampler> sampler,
    std::unique_ptr<IdGenerator> id_generator)
{
  std::shared_ptr<trace_api::TracerProvider> provider(new trace_sdk::TracerProvider(
      std::move(processors), resource, std::move(sampler), std::move(id_generator)));
  return provider;
}

std::shared_ptr<trace_api::TracerProvider> TracerProviderFactory::Build(
    std::shared_ptr<sdk::trace::TracerContext> context)
{
  std::shared_ptr<trace_api::TracerProvider> provider(new trace_sdk::TracerProvider(context));
  return provider;
}

}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
