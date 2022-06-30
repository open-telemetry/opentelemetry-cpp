// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/trace/simple_processor_factory.h"
#include "opentelemetry/sdk/trace/simple_processor.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
std::unique_ptr<SpanProcessor> SimpleSpanProcessorFactory::Build(
    std::unique_ptr<SpanExporter> &&exporter)
{
  std::unique_ptr<SpanProcessor> processor(new SimpleSpanProcessor(std::move(exporter)));
  return std::move(processor);
};

}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
