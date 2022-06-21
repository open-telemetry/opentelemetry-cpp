// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/trace/simple_processor.h"
#include "opentelemetry/sdk/trace/simple_processor_factory.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
nostd::unique_ptr<SpanProcessor> SimpleSpanProcessorFactory::Build(
    nostd::unique_ptr<SpanExporter> &&exporter)
{
  nostd::unique_ptr<SpanProcessor> processor(new SimpleSpanProcessor(std::move(exporter)));
  return processor;
};

}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
