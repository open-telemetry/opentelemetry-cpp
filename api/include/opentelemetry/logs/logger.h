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

#include <chrono>
#include <map>
#include <vector>

#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/common/key_value_iterable.h"
#include "opentelemetry/common/key_value_iterable_view.h"
#include "opentelemetry/core/timestamp.h"
#include "opentelemetry/logs/severity.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/trace/span_id.h"
#include "opentelemetry/trace/trace_flags.h"
#include "opentelemetry/trace/trace_id.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace logs
{
/**
 * Handles log record creation.
 **/
class Logger
{
protected:
  // Default values to set for fields that the user doesn't specify
  core::SystemTimestamp default_timestamp_;  // uint64 nanoseconds since Unix epoch, default 0
  Severity default_severity_;                // default 0 (kInvalid)
  nostd::string_view default_name_;          // default 0 length, nullptr data
  nostd::string_view default_body_;  // default 0 length, nullptr data. TODO: currently a simple
                                     // string, but should be changed "Any" type
  trace::TraceId default_trace_id_;  // default 00000000000000000000000000000000
  trace::SpanId default_span_id_;    // default 0000000000000000
  trace::TraceFlags default_trace_flags_;  // default 00

public:
  virtual ~Logger() = default;

  /* Returns the name of the logger */
  // TODO: decide whether this is useful and/or should be kept, as this is not a method required in
  // the specification. virtual nostd::string_view getName() = 0;

  /**
   * Each of the following overloaded Log(...) methods
   * creates a log message with the specific parameters passed.
   *
   * @param timestamp the timestamp the log record was created.
   * @param name the name of the log event.
   * @param severity the severity level of the log event.
   * @param message the string message of the log (perhaps support std::fmt or fmt-lib format).
   * @param record the log record (object type LogRecord) that is logged.
   * @param attributes the attributes, stored as a 2D list of key/value pairs, that are associated
   * wtih the log event.
   * @param trace_id the trace id associated with the log event.
   * @param span_id the span id associate with the log event.
   * @param trace_flags the trace flags associated with the log event.
   * @throws No exceptions under any circumstances.
   */

  /**
   * The base Log(...) method that all other Log(...) overloaded methods will eventually call,
   * in order to create a log record.
   *
   * Note this takes in a KeyValueIterable for the resource and attributes fields.
   */
  virtual void Log(core::SystemTimestamp timestamp,
                   Severity severity,
                   nostd::string_view name,
                   nostd::string_view body,
                   const common::KeyValueIterable &resource,
                   const common::KeyValueIterable &attributes,
                   trace::TraceId trace_id,
                   trace::SpanId span_id,
                   trace::TraceFlags trace_flags) noexcept = 0;

  /*** Overloaded methods for KeyValueIterables ***/
  /**
   * The secondary base Log(...) method that all other Log(...) overloaded methods except the one
   * above will eventually call,  in order to create a log record.
   *
   * Note this takes in template types for the resource and attributes fields.
   */
  template <class T,
            class U,
            nostd::enable_if_t<common::detail::is_key_value_iterable<T>::value> * = nullptr,
            nostd::enable_if_t<common::detail::is_key_value_iterable<U>::value> * = nullptr>
  void Log(core::SystemTimestamp timestamp,
           Severity severity,
           nostd::string_view name,
           nostd::string_view body,
           const T &resources,
           const U &attributes,
           trace::TraceId trace_id,
           trace::SpanId span_id,
           trace::TraceFlags trace_flags) noexcept
  {
    Log(timestamp, severity, name, body, common::KeyValueIterableView<T>(resources),
        common::KeyValueIterableView<U>(attributes), trace_id, span_id, trace_flags);
  }

  void Log(core::SystemTimestamp timestamp,
           Severity severity,
           nostd::string_view name,
           nostd::string_view body,
           std::initializer_list<std::pair<nostd::string_view, common::AttributeValue>> resource,
           std::initializer_list<std::pair<nostd::string_view, common::AttributeValue>> attributes,
           trace::TraceId trace_id,
           trace::SpanId span_id,
           trace::TraceFlags trace_flags) noexcept
  {
    return this->Log(timestamp, severity, name, body,
                     nostd::span<const std::pair<nostd::string_view, common::AttributeValue>>{
                         resource.begin(), resource.end()},
                     nostd::span<const std::pair<nostd::string_view, common::AttributeValue>>{
                         attributes.begin(), attributes.end()},
                     trace_id, span_id, trace_flags);
  }

  /** Wrapper methods that the user could call for convenience when logging **/

  // Set default values for unspecified fields, then call the base Log() method
  inline void Log(Severity severity,
                  nostd::string_view message,
                  core::SystemTimestamp time) noexcept
  {
    this->Log(time, severity, default_name_, message, {}, {}, default_trace_id_, default_span_id_,
              default_trace_flags_);
  }

  // Set default time, and call base Log(severity, message, time) method
  inline void Log(Severity severity, nostd::string_view message) noexcept
  {
    this->Log(severity, message, default_timestamp_);
  }

  // Set default severity then call Log(Severity, String message) method
  inline void Log(nostd::string_view message) noexcept { this->Log(default_severity_, message); }

  // Set default time, body, resources, traceid, spanid, and traceflags,
  // and then call Log(Severity, String message) method
  // inline void Log(Severity severity,
  //                 nostd::string_view name,
  //                 const common::KeyValueIterable &attributes) noexcept
  // {
  //   this->Log(default_timestamp_, severity, name, default_body_, default_resource_, attributes,
  //   default_trace_id_, default_span_id_, default_trace_flags_);
  // }

  // TODO: Add more overloaded Log(...) methods with different combiantions of parameters.

  // TODO: Add function aliases such as void debug(), void trace(), void info(), etc. for each
  // severity level
};
}  // namespace logs
OPENTELEMETRY_END_NAMESPACE
