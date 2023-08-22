// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/trace/tracer_provider_factory.h"
#include "opentelemetry/sdk/trace/processor.h"
#include "opentelemetry/sdk/trace/random_id_generator_factory.h"
#include "opentelemetry/sdk/trace/samplers/always_on_factory.h"
#include "opentelemetry/sdk/trace/tracer_context.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"

namespace trace_api = opentelemetry::trace;
namespace trace_sdk = opentelemetry::sdk::trace;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{

std::unique_ptr<trace_api::TracerProvider> TracerProviderFactory::Create(
    std::unique_ptr<SpanProcessor> processor)
{
  auto resource = opentelemetry::sdk::resource::Resource::Create({});
  return Create(std::move(processor), resource);
}

std::unique_ptr<trace_api::TracerProvider> TracerProviderFactory::Create(
    std::unique_ptr<SpanProcessor> processor,
    const opentelemetry::sdk::resource::Resource &resource)
{
  auto sampler = AlwaysOnSamplerFactory::Create();
  return Create(std::move(processor), resource, std::move(sampler));
}

std::unique_ptr<opentelemetry::trace::TracerProvider> TracerProviderFactory::Create(
    std::unique_ptr<SpanProcessor> processor,
    const opentelemetry::sdk::resource::Resource &resource,
    std::unique_ptr<Sampler> sampler)
{
  auto id_generator = RandomIdGeneratorFactory::Create();
  return Create(std::move(processor), resource, std::move(sampler), std::move(id_generator));
}

std::unique_ptr<opentelemetry::trace::TracerProvider> TracerProviderFactory::Create(
    std::unique_ptr<SpanProcessor> processor,
    const opentelemetry::sdk::resource::Resource &resource,
    std::unique_ptr<Sampler> sampler,
    std::unique_ptr<IdGenerator> id_generator)
{
  std::unique_ptr<trace_api::TracerProvider> provider(new trace_sdk::TracerProvider(
      std::move(processor), resource, std::move(sampler), std::move(id_generator)));
  return provider;
}

std::unique_ptr<opentelemetry::trace::TracerProvider> TracerProviderFactory::Create(
    std::vector<std::unique_ptr<SpanProcessor>> &&processors)
{
  auto resource = opentelemetry::sdk::resource::Resource::Create({});
  return Create(std::move(processors), resource);
}

std::unique_ptr<opentelemetry::trace::TracerProvider> TracerProviderFactory::Create(
    std::vector<std::unique_ptr<SpanProcessor>> &&processors,
    const opentelemetry::sdk::resource::Resource &resource)
{
  auto sampler = AlwaysOnSamplerFactory::Create();
  return Create(std::move(processors), resource, std::move(sampler));
}

std::unique_ptr<opentelemetry::trace::TracerProvider> TracerProviderFactory::Create(
    std::vector<std::unique_ptr<SpanProcessor>> &&processors,
    const opentelemetry::sdk::resource::Resource &resource,
    std::unique_ptr<Sampler> sampler)
{
  auto id_generator = RandomIdGeneratorFactory::Create();
  return Create(std::move(processors), resource, std::move(sampler), std::move(id_generator));
}

std::unique_ptr<opentelemetry::trace::TracerProvider> TracerProviderFactory::Create(
    std::vector<std::unique_ptr<SpanProcessor>> &&processors,
    const opentelemetry::sdk::resource::Resource &resource,
    std::unique_ptr<Sampler> sampler,
    std::unique_ptr<IdGenerator> id_generator)
{
  std::unique_ptr<trace_api::TracerProvider> provider(new trace_sdk::TracerProvider(
      std::move(processors), resource, std::move(sampler), std::move(id_generator)));
  return provider;
}

std::unique_ptr<trace_api::TracerProvider> TracerProviderFactory::Create(
    std::unique_ptr<TracerContext> context)
{
  std::unique_ptr<trace_api::TracerProvider> provider(
      new trace_sdk::TracerProvider(std::move(context)));
  return provider;
}

}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
