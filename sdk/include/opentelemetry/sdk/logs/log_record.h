/*
 * Copyright The OpenTelemetry Authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <map>
#include <unordered_map>
#include "opentelemetry/sdk/common/attribute_utils.h"  // same as traces/attribute_utils
#include "opentelemetry/sdk/logs/recordable.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{

/**
 * A default Recordable implemenation to be passed in log statements,
 * matching the 10 fields of the Log Data Model.
 * (https://github.com/open-telemetry/opentelemetry-specification/blob/master/specification/logs/data-model.md#log-and-event-record-definition)
 *
 */
class LogRecord final : public Recordable
{
private:
  // Default values are set by the respective data structures' constructors for all fields,
  // except the severity field, which must be set manually (an enum with no default value).
  opentelemetry::logs::Severity severity_ = opentelemetry::logs::Severity::kInvalid;
  common::AttributeMap resource_map_;
  common::AttributeMap attributes_map_;
  std::string name_;
  std::string body_;  // Currently a simple string, but should be changed to "Any" type
  opentelemetry::trace::TraceId trace_id_;
  opentelemetry::trace::SpanId span_id_;
  opentelemetry::trace::TraceFlags trace_flags_;
  core::SystemTimestamp timestamp_;  // uint64 nanoseconds since Unix epoch

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
   * Set a resource for this log.
   * @param name the name of the resource
   * @param value the resource value
   */
  void SetResource(nostd::string_view key,
                   const opentelemetry::common::AttributeValue &value) noexcept override
  {
    resource_map_.SetAttribute(key, value);
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
  void SetTimestamp(core::SystemTimestamp timestamp) noexcept override { timestamp_ = timestamp; }

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
   * Get the resource field for this log
   * @return the resource field for this log
   */
  const std::unordered_map<std::string, common::OwnedAttributeValue> &GetResource() const noexcept
  {
    return resource_map_.GetAttributes();
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
  core::SystemTimestamp GetTimestamp() const noexcept { return timestamp_; }
};
}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
