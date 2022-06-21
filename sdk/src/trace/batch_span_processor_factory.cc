// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/trace/batch_span_processor_factory.h"
#include "opentelemetry/sdk/trace/batch_span_processor.h"
#include "opentelemetry/sdk/trace/batch_span_processor_options.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
nostd::unique_ptr<SpanProcessor> BatchSpanProcessorFactory::Build(
    nostd::unique_ptr<SpanExporter> &&exporter,
    const BatchSpanProcessorOptions &options)
{
  nostd::unique_ptr<SpanProcessor> processor(new BatchSpanProcessor(std::move(exporter), options));
  return processor;
};

}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
