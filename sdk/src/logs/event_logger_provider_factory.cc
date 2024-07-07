// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/version.h"
#include "opentelemetry/sdk/logs/event_logger_provider_factory.h"
#include "opentelemetry/sdk/logs/event_logger_provider.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{

#ifdef OPENTELEMETRY_DEPRECATED_SDK_FACTORY

std::unique_ptr<opentelemetry::logs::EventLoggerProvider> EventLoggerProviderFactory::Create()
{
  return std::unique_ptr<opentelemetry::logs::EventLoggerProvider>(new EventLoggerProvider());
}

#else

std::unique_ptr<opentelemetry::sdk::logs::EventLoggerProvider> EventLoggerProviderFactory::Create()
{
  return std::unique_ptr<opentelemetry::sdk::logs::EventLoggerProvider>(new EventLoggerProvider());
}

#endif /* OPENTELEMETRY_DEPRECATED_SDK_FACTORY */

}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
