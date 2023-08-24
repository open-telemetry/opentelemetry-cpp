// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/logs/event_logger_provider_factory.h"
#include "opentelemetry/sdk/logs/event_logger_provider.h"
#include "opentelemetry/sdk/resource/resource.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{

std::unique_ptr<opentelemetry::logs::EventLoggerProvider> EventLoggerProviderFactory::Create()
{
  return std::unique_ptr<opentelemetry::logs::EventLoggerProvider>(new EventLoggerProvider());
}

}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
