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
#include "opentelemetry/sdk/logs/attribute_utils.h" // same as traces/attribute_utils 
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/sdk/logs/recordable.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{

/**
 * A default Event object to be passed in log statements,
 * matching the 10 fields of the Log Data Model.
 * (https://github.com/open-telemetry/opentelemetry-specification/blob/master/specification/logs/data-model.md#log-and-event-record-definition)
 *
 */
class LogRecord final : public Recordable
{
private: 
  // Log Data Model fields
  core::SystemTimestamp timestamp_;                            // uint64 nanoseconds since Unix epoch
  opentelemetry::trace::TraceId trace_id_;                 // byte sequence
  opentelemetry::trace::SpanId span_id_;                   // byte sequence
  opentelemetry::trace::TraceFlags trace_flags_;            // byte
  opentelemetry::logs::Severity severity_;              // Severity enum 
  nostd::string_view name_;                                 // string
  nostd::string_view body_;                             // currently a simple string, but should be changed "Any" type
  AttributeMap resource_map_;
  AttributeMap attributes_map_;

public:

  /********** Setters for each field (overrides methods from the Recordable interface) ************/
  /**
   * Set the timestamp for this log. 
   * @param timestamp the timestamp of the event
   */
  void SetTimestamp(core::SystemTimestamp timestamp) noexcept override
  {
    timestamp_ = timestamp;
  }

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
  void SetName(nostd::string_view name) noexcept override
  {
    name_ = name;
  }

  /**
   * Set body field for this log.
   * @param message the body to set
   */ 
  void SetBody(nostd::string_view message) noexcept override
  {
    body_ = message;
  }

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
  void SetTraceId(opentelemetry::trace::TraceId trace_id) noexcept  override
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

  /************************** Getters for each field ****************************/

  /**
   * Get the timestamp for this log
   * @return the timestamp for this log
   */
  core::SystemTimestamp GetTimestamp() const noexcept { return timestamp_; }

  /**
   * Get the severity for this log
   * @return the severity for this log
   */
  opentelemetry::logs::Severity GetSeverity() const noexcept { return severity_; }

  /**
   * Get the name of this log
   * @return the name of  this log
   */
  nostd::string_view GetName() const noexcept { return name_; }

  /**
   * Get the body of this log
   * @return the body of this log
   */
  nostd::string_view GetBody() const noexcept { return body_; }

  /**
   * Get the resource field for this log
   * @return the resource field for this log
   */
  const std::unordered_map<std::string, LogRecordAttributeValue> &GetResource() const noexcept
  {
    return resource_map_.GetAttributes();
  }

  /**
   * Get the attributes for this log
   * @return the attributes for this log
   */
  const std::unordered_map<std::string, LogRecordAttributeValue> &GetAttributes() const noexcept
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


};
}  // namespace logs
} // namespace sdk
OPENTELEMETRY_END_NAMESPACE
