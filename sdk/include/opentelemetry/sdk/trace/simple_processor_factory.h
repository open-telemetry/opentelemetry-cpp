// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/nostd/unique_ptr.h"
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
  static nostd::unique_ptr<SpanProcessor> Build(nostd::unique_ptr<SpanExporter> &&exporter);
};

}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
