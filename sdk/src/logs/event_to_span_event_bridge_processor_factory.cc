// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <memory>

#include "opentelemetry/sdk/logs/event_to_span_event_bridge_processor.h"
#include "opentelemetry/sdk/logs/event_to_span_event_bridge_processor_factory.h"
#include "opentelemetry/sdk/logs/processor.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{

std::unique_ptr<LogRecordProcessor> EventToSpanEventBridgeProcessorFactory::Create()
{
  std::unique_ptr<LogRecordProcessor> processor(new EventToSpanEventBridgeProcessor());
  return processor;
}

}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
