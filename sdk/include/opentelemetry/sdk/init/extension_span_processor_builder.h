// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/sdk/configuration/extension_span_processor_configuration.h"
#include "opentelemetry/sdk/trace/processor.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace init
{

class ExtensionSpanProcessorBuilder
{
public:
  ExtensionSpanProcessorBuilder()          = default;
  virtual ~ExtensionSpanProcessorBuilder() = default;

  virtual std::unique_ptr<opentelemetry::sdk::trace::SpanProcessor> Build(
      const opentelemetry::sdk::configuration::ExtensionSpanProcessorConfiguration *model)
      const = 0;
};

}  // namespace init
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
