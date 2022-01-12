// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifdef ENABLE_LOGS_PREVIEW

#  include <map>
#  include <unordered_map>
#  include "opentelemetry/sdk/common/attribute_utils.h"
#  include "opentelemetry/sdk/logs/recordable.h"
#  include "opentelemetry/sdk/resource/resource.h"
#  include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{

/**
 * A default Recordable implemenation to be passed in log statements,
 * matching the 10 fields of the Log Data Model.
 * (https://github.com/open-telemetry/opentelemetry-specification/blob/main/specification/logs/data-model.md#log-and-event-record-definition)
 *
 */
class LogRecord final : public Recordable
{
private:
  // Default values are set by the respective data structures' constructors for all fields,
  // except the severity field, which must be set manually (an enum with no default value).
  opentelemetry::logs::Severity severity_                 = opentelemetry::logs::Severity::kInvalid;
  const opentelemetry::sdk::resource::Resource *resource_ = nullptr;
  common::AttributeMap attributes_map_;
  std::string name_;
  std::string body_;  // Currently a simple string, but should be changed to "Any" type
  opentelemetry::trace::TraceId trace_id_;
  opentelemetry::trace::SpanId span_id_;
  opentelemetry::trace::TraceFlags trace_flags_;
  opentelemetry::common::SystemTimestamp timestamp_;  // uint64 nanoseconds since Unix epoch

public:
  /********** Setters for each field (overrides methods from the Recordable interface) ************/

  /**
   * Set the severity for this log.
   * @param severity the severity of the event
   */
  void SetSeverity(opentelemetry::logs::Severity severity) noexcept override
  {
    severity_ = severity;
  }

  /**
   * Set name for this log
   * @param name the name to set
   */
  void SetName(nostd::string_view name) noexcept override { name_ = std::string(name); }

  /**
   * Set body field for this log.
   * @param message the body to set
   */
  void SetBody(nostd::string_view message) noexcept override { body_ = std::string(message); }

  /**
   * Set Resource of this log
   * @param Resource the resource to set
   */
  void SetResource(const opentelemetry::sdk::resource::Resource &resource) noexcept override
  {
    resource_ = &resource;
  }

  /**
   * Set an attribute of a log.
   * @param name the name of the attribute
   * @param value the attribute value
   */

  void SetAttribute(nostd::string_view key,
                    const opentelemetry::common::AttributeValue &value) noexcept override
  {
    attributes_map_.SetAttribute(key, value);
  }

  /**
   * Set trace id for this log.
   * @param trace_id the trace id to set
   */
  void SetTraceId(opentelemetry::trace::TraceId trace_id) noexcept override
  {
    trace_id_ = trace_id;
  }

  /**
   * Set span id for this log.
   * @param span_id the span id to set
   */
  virtual void SetSpanId(opentelemetry::trace::SpanId span_id) noexcept override
  {
    span_id_ = span_id;
  }

  /**
   * Inject a trace_flags  for this log.
   * @param trace_flags the span id to set
   */
  void SetTraceFlags(opentelemetry::trace::TraceFlags trace_flags) noexcept override
  {
    trace_flags_ = trace_flags;
  }

  /**
   * Set the timestamp for this log.
   * @param timestamp the timestamp of the event
   */
  void SetTimestamp(opentelemetry::common::SystemTimestamp timestamp) noexcept override
  {
    timestamp_ = timestamp;
  }

  /************************** Getters for each field ****************************/

  /**
   * Get the severity for this log
   * @return the severity for this log
   */
  opentelemetry::logs::Severity GetSeverity() const noexcept { return severity_; }

  /**
   * Get the name of this log
   * @return the name of  this log
   */
  std::string GetName() const noexcept { return name_; }

  /**
   * Get the body of this log
   * @return the body of this log
   */
  std::string GetBody() const noexcept { return body_; }

  /**
   * Get the resource for this log
   * @return the resource for this log
   */
  const opentelemetry::sdk::resource::Resource &GetResource() const noexcept
  {
    if (nullptr == resource_)
    {
      return sdk::resource::Resource::GetDefault();
    }
    return *resource_;
  }

  /**
   * Get the attributes for this log
   * @return the attributes for this log
   */
  const std::unordered_map<std::string, common::OwnedAttributeValue> &GetAttributes() const noexcept
  {
    return attributes_map_.GetAttributes();
  }

  /**
   * Get the trace id for this log
   * @return the trace id for this log
   */
  opentelemetry::trace::TraceId GetTraceId() const noexcept { return trace_id_; }

  /**
   * Get the span id for this log
   * @return the span id for this log
   */
  opentelemetry::trace::SpanId GetSpanId() const noexcept { return span_id_; }

  /**
   * Get the trace flags for this log
   * @return the trace flags for this log
   */
  opentelemetry::trace::TraceFlags GetTraceFlags() const noexcept { return trace_flags_; }

  /**
   * Get the timestamp for this log
   * @return the timestamp for this log
   */
  opentelemetry::common::SystemTimestamp GetTimestamp() const noexcept { return timestamp_; }

  /**
   * Set instrumentation_library for this log.
   * @param instrumentation_library the instrumentation library to set
   */
  void SetInstrumentationLibrary(
      const opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary
          &instrumentation_library) noexcept
  {
    instrumentation_library_ = &instrumentation_library;
  }

private:
  const opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary
      *instrumentation_library_ = nullptr;
};
}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
