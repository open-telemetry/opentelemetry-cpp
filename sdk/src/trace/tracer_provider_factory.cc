// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/trace/tracer_provider_factory.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"

namespace trace_api = opentelemetry::trace;
namespace trace_sdk = opentelemetry::sdk::trace;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{

nostd::shared_ptr<trace_api::TracerProvider> TracerProviderFactory::Build(
    std::unique_ptr<SpanProcessor> processor)
{
  auto resource = opentelemetry::sdk::resource::Resource::Create({});
  return Build(std::move(processor), resource);
}

nostd::shared_ptr<trace_api::TracerProvider> TracerProviderFactory::Build(
    std::unique_ptr<SpanProcessor> processor,
    opentelemetry::sdk::resource::Resource resource)
{
  nostd::shared_ptr<trace_api::TracerProvider> provider(
      new trace_sdk::TracerProvider(std::move(processor), resource));
  return provider;
}

nostd::shared_ptr<trace_api::TracerProvider> TracerProviderFactory::Build(
    std::shared_ptr<sdk::trace::TracerContext> context)
{
  nostd::shared_ptr<trace_api::TracerProvider> provider(new trace_sdk::TracerProvider(context));
  return provider;
}

}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
