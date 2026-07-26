// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "opentelemetry/sdk/logs/processor.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{

/**
 * Factory class for EventToSpanEventBridgeProcessor.
 */
class OPENTELEMETRY_EXPORT EventToSpanEventBridgeProcessorFactory
{
public:
  /**
   * Create an EventToSpanEventBridgeProcessor.
   */
  static std::unique_ptr<LogRecordProcessor> Create();
};

}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
