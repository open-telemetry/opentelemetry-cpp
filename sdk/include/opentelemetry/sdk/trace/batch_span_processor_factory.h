// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "opentelemetry/sdk/trace/batch_span_processor_options.h"
#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/processor.h"
#include "opentelemetry/sdk/version/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{

class BatchSpanProcessorFactory
{
public:
  static std::unique_ptr<SpanProcessor> Build(std::unique_ptr<SpanExporter> &&exporter,
                                              const BatchSpanProcessorOptions &options);
};

}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
