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
public:
  virtual ~Logger() = default;

  /**
   * Each of the following overloaded Log(...) methods
   * creates a log message with the specific parameters passed.
   *
   * @param severity the severity level of the log event.
   * @param name the name of the log event.
   * @param message the string message of the log (perhaps support std::fmt or fmt-lib format).
   * @param resource the resources, stored as a 2D list of key/value pairs, that are associated
   * with the log event.
   * @param attributes the attributes, stored as a 2D list of key/value pairs, that are associated
   * with the log event.
   * @param trace_id the trace id associated with the log event.
   * @param span_id the span id associate with the log event.
   * @param trace_flags the trace flags associated with the log event.
   * @param timestamp the timestamp the log record was created.
   * @throws No exceptions under any circumstances.
   */

  /**
   * The base Log(...) method that all other Log(...) overloaded methods will eventually call,
   * in order to create a log record.
   *
   * Note this takes in a KeyValueIterable for the resource and attributes fields.
   */
  virtual void Log(Severity severity,
                   nostd::string_view name,
                   nostd::string_view body,
                   const common::KeyValueIterable &resource,
                   const common::KeyValueIterable &attributes,
                   trace::TraceId trace_id,
                   trace::SpanId span_id,
                   trace::TraceFlags trace_flags,
                   core::SystemTimestamp timestamp) noexcept = 0;

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
  void Log(Severity severity,
           nostd::string_view name,
           nostd::string_view body,
           const T &resource,
           const U &attributes,
           trace::TraceId trace_id,
           trace::SpanId span_id,
           trace::TraceFlags trace_flags,
           core::SystemTimestamp timestamp) noexcept
  {
    Log(severity, name, body, common::KeyValueIterableView<T>(resource),
        common::KeyValueIterableView<U>(attributes), trace_id, span_id, trace_flags, timestamp);
  }

  void Log(Severity severity,
           nostd::string_view name,
           nostd::string_view body,
           std::initializer_list<std::pair<nostd::string_view, common::AttributeValue>> resource,
           std::initializer_list<std::pair<nostd::string_view, common::AttributeValue>> attributes,
           trace::TraceId trace_id,
           trace::SpanId span_id,
           trace::TraceFlags trace_flags,
           core::SystemTimestamp timestamp) noexcept
  {
    return this->Log(severity, name, body,
                     nostd::span<const std::pair<nostd::string_view, common::AttributeValue>>{
                         resource.begin(), resource.end()},
                     nostd::span<const std::pair<nostd::string_view, common::AttributeValue>>{
                         attributes.begin(), attributes.end()},
                     trace_id, span_id, trace_flags, timestamp);
  }

  /** Wrapper methods that the user could call for convenience when logging **/

  // Set default values for unspecified fields, then call the base Log() method
  void Log(Severity severity, nostd::string_view message, core::SystemTimestamp timestamp) noexcept
  {
    this->Log(severity, "", message, {}, {}, {}, {}, {}, timestamp);
  }

  // Set default time, and call base Log(severity, message, time) method
  void Log(Severity severity, nostd::string_view message) noexcept
  {
    this->Log(severity, message, std::chrono::system_clock::now());
  }

  // Set default severity then call Log(Severity, String message) method
  void Log(nostd::string_view message) noexcept { this->Log(Severity::kInfo, message); }

  // TODO: Add more overloaded Log(...) methods with different combiantions of parameters.

  // TODO: Add function aliases such as void debug(), void trace(), void info(), etc. for each
  // severity level
};
}  // namespace logs
OPENTELEMETRY_END_NAMESPACE
