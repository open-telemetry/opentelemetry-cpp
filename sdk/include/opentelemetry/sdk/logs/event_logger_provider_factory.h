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
class OPENTELEMETRY_EXPORT EventLoggerProviderFactory
{
public:
  /**
   * Create a EventLoggerProvider.
   */

#ifdef OPENTELEMETRY_DEPRECATED_SDK_FACTORY

#  ifndef OPENTELEMETRY_NO_DEPRECATED_CODE
  OPENTELEMETRY_DEPRECATED
  static std::unique_ptr<opentelemetry::logs::EventLoggerProvider> Create();
#  endif /* OPENTELEMETRY_NO_DEPRECATED_CODE */

#else
  static std::unique_ptr<opentelemetry::sdk::logs::EventLoggerProvider> Create();
#endif /* OPENTELEMETRY_DEPRECATED_SDK_FACTORY */
};

}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
