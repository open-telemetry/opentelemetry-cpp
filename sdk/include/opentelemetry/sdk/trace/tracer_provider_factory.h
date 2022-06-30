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
  static std::shared_ptr<opentelemetry::trace::TracerProvider> Build(
      std::unique_ptr<SpanProcessor> processor);

  static std::shared_ptr<opentelemetry::trace::TracerProvider> Build(
      std::unique_ptr<SpanProcessor> processor,
      opentelemetry::sdk::resource::Resource resource);

  // TODO: full list of Build() apis

  static std::shared_ptr<opentelemetry::trace::TracerProvider> Build(
      std::shared_ptr<sdk::trace::TracerContext> context);
};

}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
