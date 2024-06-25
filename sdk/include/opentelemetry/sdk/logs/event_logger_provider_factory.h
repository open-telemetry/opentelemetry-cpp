// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0/

#pragma once

#include <memory>

#include "opentelemetry/sdk/logs/event_logger_provider.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{

/**
 * Factory class for EventLoggerProvider.
 */
class EventLoggerProviderFactory
{
public:
  /**
   * Create a EventLoggerProvider.
   */

  static std::unique_ptr<opentelemetry::sdk::logs::EventLoggerProvider> Create();
};

}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
