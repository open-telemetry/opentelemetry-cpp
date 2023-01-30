// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0/

#pragma once

#ifdef ENABLE_LOGS_PREVIEW

#  include "opentelemetry/logs/event_logger_provider.h"
#  include "opentelemetry/nostd/shared_ptr.h"

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
  static std::unique_ptr<opentelemetry::logs::EventLoggerProvider> Create();
};

}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif /* ENABLE_LOGS_PREVIEW */
