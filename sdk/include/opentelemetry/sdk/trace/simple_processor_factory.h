// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/processor.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{

class SimpleSpanProcessorFactory
{
public:
  static std::unique_ptr<SpanProcessor> Build(std::unique_ptr<SpanExporter> &&exporter);
};

}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
