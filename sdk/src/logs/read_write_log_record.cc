// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <cstddef>
#include <type_traits>

#include "opentelemetry/sdk/logs/read_write_log_record.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{

ReadWriteLogRecord::ReadWriteLogRecord()
    : severity_(opentelemetry::logs::Severity::kInvalid),
      resource_(nullptr),
      instrumentation_scope_(nullptr),
      body_(nostd::string_view()),
      observed_timestamp_(std::chrono::system_clock::now()),
      event_id_(0),
      event_name_("")
{}

ReadWriteLogRecord::~ReadWriteLogRecord() {}

void ReadWriteLogRecord::SetTimestamp(opentelemetry::common::SystemTimestamp timestamp) noexcept
{
  timestamp_ = timestamp;
}

opentelemetry::common::SystemTimestamp ReadWriteLogRecord::GetTimestamp() const noexcept
{
  return timestamp_;
}

void ReadWriteLogRecord::SetObservedTimestamp(
    opentelemetry::common::SystemTimestamp timestamp) noexcept
{
  observed_timestamp_ = timestamp;
}

opentelemetry::common::SystemTimestamp ReadWriteLogRecord::GetObservedTimestamp() const noexcept
{
  return observed_timestamp_;
}

void ReadWriteLogRecord::SetSeverity(opentelemetry::logs::Severity severity) noexcept
{
  severity_ = severity;
}

opentelemetry::logs::Severity ReadWriteLogRecord::GetSeverity() const noexcept
{
  return severity_;
}

void ReadWriteLogRecord::SetBody(const opentelemetry::common::AttributeValue &message) noexcept
{
  body_ = message;
}

const opentelemetry::common::AttributeValue &ReadWriteLogRecord::GetBody() const noexcept
{
  return body_;
}

void ReadWriteLogRecord::SetEventId(int64_t id, nostd::string_view name) noexcept
{
  event_id_   = id;
  event_name_ = std::string{name};
}

int64_t ReadWriteLogRecord::GetEventId() const noexcept
{
  return event_id_;
}

nostd::string_view ReadWriteLogRecord::GetEventName() const noexcept
{
  return nostd::string_view{event_name_};
}

void ReadWriteLogRecord::SetTraceId(const opentelemetry::trace::TraceId &trace_id) noexcept
{
  if (!trace_state_)
  {
    trace_state_ = std::unique_ptr<TraceState>(new TraceState());
  }

  trace_state_->trace_id = trace_id;
}

const opentelemetry::trace::TraceId &ReadWriteLogRecord::GetTraceId() const noexcept
{
  if (trace_state_)
  {
    return trace_state_->trace_id;
  }

  static opentelemetry::trace::TraceId empty;
  return empty;
}

void ReadWriteLogRecord::SetSpanId(const opentelemetry::trace::SpanId &span_id) noexcept
{
  if (!trace_state_)
  {
    trace_state_ = std::unique_ptr<TraceState>(new TraceState());
  }

  trace_state_->span_id = span_id;
}

const opentelemetry::trace::SpanId &ReadWriteLogRecord::GetSpanId() const noexcept
{
  if (trace_state_)
  {
    return trace_state_->span_id;
  }

  static opentelemetry::trace::SpanId empty;
  return empty;
}

void ReadWriteLogRecord::SetTraceFlags(const opentelemetry::trace::TraceFlags &trace_flags) noexcept
{
  if (!trace_state_)
  {
    trace_state_ = std::unique_ptr<TraceState>(new TraceState());
  }

  trace_state_->trace_flags = trace_flags;
}

const opentelemetry::trace::TraceFlags &ReadWriteLogRecord::GetTraceFlags() const noexcept
{
  if (trace_state_)
  {
    return trace_state_->trace_flags;
  }

  static opentelemetry::trace::TraceFlags empty;
  return empty;
}

void ReadWriteLogRecord::SetAttribute(nostd::string_view key,
                                      const opentelemetry::common::AttributeValue &value) noexcept
{
  attributes_map_[static_cast<std::string>(key)] = value;
}

const std::unordered_map<std::string, opentelemetry::common::AttributeValue> &
ReadWriteLogRecord::GetAttributes() const noexcept
{
  return attributes_map_;
}

const opentelemetry::sdk::resource::Resource &ReadWriteLogRecord::GetResource() const noexcept
{
  if OPENTELEMETRY_LIKELY_CONDITION (nullptr != resource_)
  {
    return *resource_;
  }

  return GetDefaultResource();
}

void ReadWriteLogRecord::SetResource(
    const opentelemetry::sdk::resource::Resource &resource) noexcept
{
  resource_ = &resource;
}

const opentelemetry::sdk::instrumentationscope::InstrumentationScope &
ReadWriteLogRecord::GetInstrumentationScope() const noexcept
{
  if OPENTELEMETRY_LIKELY_CONDITION (nullptr != instrumentation_scope_)
  {
    return *instrumentation_scope_;
  }

  return GetDefaultInstrumentationScope();
}

void ReadWriteLogRecord::SetInstrumentationScope(
    const opentelemetry::sdk::instrumentationscope::InstrumentationScope
        &instrumentation_scope) noexcept
{
  instrumentation_scope_ = &instrumentation_scope;
}
}  // namespace logs
}  // namespace sdk

OPENTELEMETRY_END_NAMESPACE
