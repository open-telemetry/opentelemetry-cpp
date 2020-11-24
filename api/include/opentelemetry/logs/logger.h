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

  /* Returns the name of the logger */
  // TODO: decide whether this is useful and/or should be kept, as this is not a method required in
  // the specification. virtual nostd::string_view getName() = 0;

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

  /* The below method is a logging statement that takes in a LogRecord.
   * A default LogRecord that will be assigned if no parameters are passed to Logger's .log() method
   * which should at minimum assign the trace_id, span_id, and timestamp
   */
  virtual void Log(nostd::shared_ptr<LogRecord> record) noexcept = 0;

  /** Overloaded methods for unstructured logging **/
  inline void Log(nostd::string_view message) noexcept
  {
    // Set severity to the default then call log(Severity, String message) method
    Log(Severity::kDefault, message);
  }

  inline void Log(Severity severity, nostd::string_view message) noexcept
  {
    // TODO: set default timestamp later (not in API)
    Log(severity, message, core::SystemTimestamp(std::chrono::system_clock::now()));
  }

  inline void Log(Severity severity,
                  nostd::string_view message,
                  core::SystemTimestamp time) noexcept
  {
    // creates a LogRecord object with given parameters, then calls log(LogRecord)
    auto r       = nostd::shared_ptr<LogRecord>(new LogRecord);
    r->severity  = severity;
    r->body      = message;
    r->timestamp = time;

    Log(r);
  }

  /** Overloaded methods for structured logging**/
  // TODO: separate this method into separate methods since it is not useful for user to create
  // empty logs
  template <class T,
            nostd::enable_if_t<common::detail::is_key_value_iterable<T>::value> * = nullptr>
  inline void Log(Severity severity       = Severity::kDefault,
                  nostd::string_view name = "",
                  const T &attributes     = {}) noexcept
  {
    Log(severity, name, common::KeyValueIterableView<T>(attributes));
  }

  inline void Log(Severity severity,
                  nostd::string_view name,
                  const common::KeyValueIterable &attributes) noexcept
  {
    // creates a LogRecord object with given parameters, then calls log(LogRecord)
    auto r        = nostd::shared_ptr<LogRecord>(new LogRecord);
    r->severity   = severity;
    r->name       = name;
    r->attributes = attributes;

    Log(r);
  }

  // TODO: add function aliases such as void debug(), void trace(), void info(), etc. for each
  // severity level

  /** Future enhancement: templated method for objects / custom types (e.g. JSON, XML, custom
   * classes, etc) **/
  // template<class T> virtual void log(T &some_obj) noexcept;
};
}  // namespace logs
OPENTELEMETRY_END_NAMESPACE