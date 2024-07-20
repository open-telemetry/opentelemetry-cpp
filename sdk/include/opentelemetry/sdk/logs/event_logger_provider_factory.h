// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0/

#pragma once

#include <memory>

#include "opentelemetry/version.h"
#include "opentelemetry/sdk/logs/event_logger_provider.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{

/**
 * Factory class for EventLoggerProvider.
 */
class OPENTELEMETRY_EXPORT_TYPE EventLoggerProviderFactory
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
