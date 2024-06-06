// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/logs/event_logger_provider.h"
#include "opentelemetry/nostd/unique_ptr.h"
#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/logs/event_logger.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{
namespace nostd = opentelemetry::nostd;

EventLoggerProvider::EventLoggerProvider() noexcept
{
  OTEL_INTERNAL_LOG_DEBUG("[EventLoggerProvider] EventLoggerProvider created.");
}

EventLoggerProvider::~EventLoggerProvider() {}

nostd::shared_ptr<opentelemetry::logs::EventLogger> EventLoggerProvider::CreateEventLogger(
    nostd::shared_ptr<opentelemetry::logs::Logger> delegate_logger,
    nostd::string_view event_domain) noexcept
{
  return nostd::shared_ptr<opentelemetry::logs::EventLogger>{
      new EventLogger(delegate_logger, event_domain)};
}

}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
