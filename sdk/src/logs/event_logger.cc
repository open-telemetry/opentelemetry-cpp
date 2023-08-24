// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/logs/event_logger.h"
#include "opentelemetry/sdk_config.h"
#include "opentelemetry/trace/provider.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{
namespace nostd = opentelemetry::nostd;

EventLogger::EventLogger(nostd::shared_ptr<opentelemetry::logs::Logger> delegate_logger,
                         nostd::string_view event_domain) noexcept
    : delegate_logger_(delegate_logger), event_domain_(event_domain)
{}

const nostd::string_view EventLogger::GetName() noexcept
{
  if (delegate_logger_)
  {
    return delegate_logger_->GetName();
  }

  return {};
}

nostd::shared_ptr<opentelemetry::logs::Logger> EventLogger::GetDelegateLogger() noexcept
{
  return delegate_logger_;
}

void EventLogger::EmitEvent(nostd::string_view event_name,
                            nostd::unique_ptr<opentelemetry::logs::LogRecord> &&log_record) noexcept
{
  if (!delegate_logger_ || !log_record)
  {
    return;
  }

  if (!event_domain_.empty() && !event_name.empty())
  {
    log_record->SetAttribute("event.domain", event_domain_);
    log_record->SetAttribute("event.name", event_name);
  }

  delegate_logger_->EmitLogRecord(std::move(log_record));
}

}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
