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

#include "opentelemetry/common/key_value_iterable.h"
#include "opentelemetry/logs/log_record.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
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
   * Each of the following overloaded log(...) methods
   * creates a log message with the specific parameters passed.
   *
   * @param name the name of the log event.
   * @param severity the severity level of the log event.
   * @param message the string message of the log (perhaps support std::fmt or fmt-lib format).
   * @param record the log record (object type LogRecord) that is logged.
   * @param attributes the attributes, stored as a 2D list of key/value pairs, that are associated
   * with this log.
   * @throws No exceptions under any circumstances.
   */

  /**
   * Logs a LogRecord, which contains all the fields of the Log Data Model. Normally called
   * indirectly from other log() Methods, but can be called directly for high detail.
   * @param record A log record filled with information from the user.
   */
  virtual void log(const LogRecord &record) noexcept = 0;

  /// Overloaded Log methods, which are simpler than calling a LogRecord directly

  /**
   * Writes a log.
   * @param severity The log's severity
   * @param message The message to log
   */
  inline void log(Severity severity, nostd::string_view message) noexcept
  {
    // Create a LogRecord to hold this information
    LogRecord r;
    r.severity = severity;
    r.body     = message;

    // Call the main log(LogRecord) method
    log(r);
  }

  /**
   * Writes a log.
   * @param severity The log's severity
   * @param name The name of the log
   * @param message The message to log
   */
  inline void log(Severity severity, nostd::string_view name, nostd::string_view message) noexcept
  {
    // Create a LogRecord to hold this information
    LogRecord r;
    r.severity = severity;
    r.name     = name;
    r.body     = message;

    // Call the main log(LogRecord) method
    log(r);
  }

  /**
   * Writes a log.
   * @param severity The severity of the log, from 1 to 24
   * @param attributes A key/value object
   */
  template <class T,
            nostd::enable_if_t<common::detail::is_key_value_iterable<T>::value> * = nullptr>
  inline void log(Severity severity, const T &attributes) noexcept
  {
    // Create a LogRecord to hold this information
    LogRecord r;
    r.severity   = severity;
    r.attributes = nostd::shared_ptr<common::KeyValueIterable>(
        new common::KeyValueIterableView<T>{attributes});

    // Call the main log(LogRecord) method
    log(r);
  }

  /**
   * Writes a log.
   * @param severity The severity of the log, from 1 to 24
   * @param name The name of the log
   * @param attributes A key/value object
   */
  template <class T,
            nostd::enable_if_t<common::detail::is_key_value_iterable<T>::value> * = nullptr>
  inline void log(Severity severity, nostd::string_view name, const T &attributes) noexcept
  {
    // Create a LogRecord to hold this information
    LogRecord r;
    r.severity   = severity;
    r.name       = name;
    r.attributes = nostd::shared_ptr<common::KeyValueIterable>(
        new common::KeyValueIterableView<T>{attributes});

    // Call the main log(LogRecord) method
    log(r);
  }

  /** Future enhancement: templated method for objects / custom types (e.g. JSON, XML, custom
   * classes, etc) **/
  // template<class T> virtual void log(T &some_obj) noexcept;
};
}  // namespace logs
OPENTELEMETRY_END_NAMESPACE
