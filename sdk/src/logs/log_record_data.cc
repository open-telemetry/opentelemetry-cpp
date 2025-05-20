// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <stdint.h>
#include <chrono>
#include <memory>
#include <string>
#include <unordered_map>

#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/common/timestamp.h"
#include "opentelemetry/logs/severity.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/sdk/common/attribute_utils.h"
#include "opentelemetry/sdk/instrumentationscope/instrumentation_scope.h"
#include "opentelemetry/sdk/logs/log_record_data.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/trace/span_id.h"
#include "opentelemetry/trace/trace_flags.h"
#include "opentelemetry/trace/trace_id.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{

LogRecordData::LogRecordData()
    : severity_(opentelemetry::logs::Severity::kInvalid),
      resource_(nullptr),
      instrumentation_scope_(nullptr),
#if !defined(OPENTELEMETRY_DEPRECATED_SDK_LOG_RECORD)
      body_(std::string()),
#endif
      observed_timestamp_(std::chrono::system_clock::now()),
      event_id_(0),
      event_name_("")
{
#if defined(OPENTELEMETRY_DEPRECATED_SDK_LOG_RECORD)
  body_.SetAttribute("", nostd::string_view());
#endif
}

LogRecordData::~LogRecordData() {}

void LogRecordData::SetTimestamp(opentelemetry::common::SystemTimestamp timestamp) noexcept
{
  timestamp_ = timestamp;
}

opentelemetry::common::SystemTimestamp LogRecordData::GetTimestamp() const noexcept
{
  return timestamp_;
}

void LogRecordData::SetObservedTimestamp(opentelemetry::common::SystemTimestamp timestamp) noexcept
{
  observed_timestamp_ = timestamp;
}

opentelemetry::common::SystemTimestamp LogRecordData::GetObservedTimestamp() const noexcept
{
  return observed_timestamp_;
}

void LogRecordData::SetSeverity(opentelemetry::logs::Severity severity) noexcept
{
  severity_ = severity;
}

opentelemetry::logs::Severity LogRecordData::GetSeverity() const noexcept
{
  return severity_;
}

void LogRecordData::SetBody(const opentelemetry::common::AttributeValue &message) noexcept
{
#if !defined(OPENTELEMETRY_DEPRECATED_SDK_LOG_RECORD)
  body_ = nostd::visit(attribute_converter_, message);
#else
  body_.SetAttribute("", message);
#endif
}

#if !defined(OPENTELEMETRY_DEPRECATED_SDK_LOG_RECORD)
const common::OwnedAttributeValue &
#else
const opentelemetry::common::AttributeValue &
#endif
LogRecordData::GetBody() const noexcept
{
#if !defined(OPENTELEMETRY_DEPRECATED_SDK_LOG_RECORD)
  return body_;
#else
  return body_.GetAttributes().begin()->second;
#endif
}

void LogRecordData::SetEventId(int64_t id, nostd::string_view name) noexcept
{
  event_id_   = id;
  event_name_ = std::string{name};
}

int64_t LogRecordData::GetEventId() const noexcept
{
  return event_id_;
}

nostd::string_view LogRecordData::GetEventName() const noexcept
{
  return nostd::string_view{event_name_};
}

void LogRecordData::SetTraceId(const opentelemetry::trace::TraceId &trace_id) noexcept
{
  if (!trace_state_)
  {
    trace_state_ = std::unique_ptr<TraceState>(new TraceState());
  }

  trace_state_->trace_id = trace_id;
}

const opentelemetry::trace::TraceId &LogRecordData::GetTraceId() const noexcept
{
  if (trace_state_)
  {
    return trace_state_->trace_id;
  }

  static opentelemetry::trace::TraceId empty;
  return empty;
}

void LogRecordData::SetSpanId(const opentelemetry::trace::SpanId &span_id) noexcept
{
  if (!trace_state_)
  {
    trace_state_ = std::unique_ptr<TraceState>(new TraceState());
  }

  trace_state_->span_id = span_id;
}

const opentelemetry::trace::SpanId &LogRecordData::GetSpanId() const noexcept
{
  if (trace_state_)
  {
    return trace_state_->span_id;
  }

  static opentelemetry::trace::SpanId empty;
  return empty;
}

void LogRecordData::SetTraceFlags(const opentelemetry::trace::TraceFlags &trace_flags) noexcept
{
  if (!trace_state_)
  {
    trace_state_ = std::unique_ptr<TraceState>(new TraceState());
  }

  trace_state_->trace_flags = trace_flags;
}

const opentelemetry::trace::TraceFlags &LogRecordData::GetTraceFlags() const noexcept
{
  if (trace_state_)
  {
    return trace_state_->trace_flags;
  }

  static opentelemetry::trace::TraceFlags empty;
  return empty;
}

void LogRecordData::SetAttribute(nostd::string_view key,
                                 const opentelemetry::common::AttributeValue &value) noexcept
{
  attributes_map_.SetAttribute(key, value);
}

#if !defined(OPENTELEMETRY_DEPRECATED_SDK_LOG_RECORD)
const std::unordered_map<std::string, opentelemetry::sdk::common::OwnedAttributeValue> &
#else
const std::unordered_map<std::string, opentelemetry::common::AttributeValue> &
#endif
LogRecordData::GetAttributes() const noexcept
{
  return attributes_map_.GetAttributes();
}

const opentelemetry::sdk::resource::Resource &LogRecordData::GetResource() const noexcept
{
  if OPENTELEMETRY_LIKELY_CONDITION (nullptr != resource_)
  {
    return *resource_;
  }

  return GetDefaultResource();
}

void LogRecordData::SetResource(const opentelemetry::sdk::resource::Resource &resource) noexcept
{
  resource_ = &resource;
}

const opentelemetry::sdk::instrumentationscope::InstrumentationScope &
LogRecordData::GetInstrumentationScope() const noexcept
{
  if OPENTELEMETRY_LIKELY_CONDITION (nullptr != instrumentation_scope_)
  {
    return *instrumentation_scope_;
  }

  return GetDefaultInstrumentationScope();
}

void LogRecordData::SetInstrumentationScope(
    const opentelemetry::sdk::instrumentationscope::InstrumentationScope
        &instrumentation_scope) noexcept
{
  instrumentation_scope_ = &instrumentation_scope;
}
}  // namespace logs
}  // namespace sdk

OPENTELEMETRY_END_NAMESPACE
