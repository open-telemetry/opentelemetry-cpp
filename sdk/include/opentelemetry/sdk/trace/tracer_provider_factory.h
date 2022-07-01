// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "opentelemetry/sdk/trace/processor.h"
#include "opentelemetry/sdk/trace/tracer_context.h"
#include "opentelemetry/trace/tracer_provider.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{

class TracerProviderFactory
{
public:
  /* Serie of builders with a single processor. */

  static std::shared_ptr<opentelemetry::trace::TracerProvider> Build(
      std::unique_ptr<SpanProcessor> processor);

  static std::shared_ptr<opentelemetry::trace::TracerProvider> Build(
      std::unique_ptr<SpanProcessor> processor,
      opentelemetry::sdk::resource::Resource resource);

  static std::shared_ptr<opentelemetry::trace::TracerProvider> Build(
      std::unique_ptr<SpanProcessor> processor,
      opentelemetry::sdk::resource::Resource resource,
      std::unique_ptr<Sampler> sampler);

  static std::shared_ptr<opentelemetry::trace::TracerProvider> Build(
      std::unique_ptr<SpanProcessor> processor,
      opentelemetry::sdk::resource::Resource resource,
      std::unique_ptr<Sampler> sampler,
      std::unique_ptr<IdGenerator> id_generator);

  /* Serie of builders with a vector of processor. */

  static std::shared_ptr<opentelemetry::trace::TracerProvider> Build(
      std::vector<std::unique_ptr<SpanProcessor>> &&processors);

  static std::shared_ptr<opentelemetry::trace::TracerProvider> Build(
      std::vector<std::unique_ptr<SpanProcessor>> &&processors,
      opentelemetry::sdk::resource::Resource resource);

  static std::shared_ptr<opentelemetry::trace::TracerProvider> Build(
      std::vector<std::unique_ptr<SpanProcessor>> &&processors,
      opentelemetry::sdk::resource::Resource resource,
      std::unique_ptr<Sampler> sampler);

  static std::shared_ptr<opentelemetry::trace::TracerProvider> Build(
      std::vector<std::unique_ptr<SpanProcessor>> &&processors,
      opentelemetry::sdk::resource::Resource resource,
      std::unique_ptr<Sampler> sampler,
      std::unique_ptr<IdGenerator> id_generator);

  /* Builder with a tracer context. */

  static std::shared_ptr<opentelemetry::trace::TracerProvider> Build(
      std::shared_ptr<sdk::trace::TracerContext> context);
};

}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
