// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifdef ENABLE_LOGS_PREVIEW

#  include <chrono>
#  include <map>
#  include <vector>

#  include "opentelemetry/common/attribute_value.h"
#  include "opentelemetry/common/key_value_iterable.h"
#  include "opentelemetry/common/key_value_iterable_view.h"
#  include "opentelemetry/common/timestamp.h"
#  include "opentelemetry/logs/severity.h"
#  include "opentelemetry/nostd/shared_ptr.h"
#  include "opentelemetry/nostd/span.h"
#  include "opentelemetry/nostd/string_view.h"
#  include "opentelemetry/nostd/type_traits.h"
#  include "opentelemetry/trace/span_id.h"
#  include "opentelemetry/trace/trace_flags.h"
#  include "opentelemetry/trace/trace_id.h"
#  include "opentelemetry/version.h"

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
  virtual const nostd::string_view GetName() noexcept = 0;

  /**
   * Each of the following overloaded Log(...) methods
   * creates a log message with the specific parameters passed.
   *
   * @param severity the severity level of the log event.
   * @param name the name of the log event.
   * @param message the string message of the log (perhaps support std::fmt or fmt-lib format).
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
   */
  virtual void Log(Severity severity,
                   nostd::string_view name,
                   nostd::string_view body,
                   const common::KeyValueIterable &attributes,
                   trace::TraceId trace_id,
                   trace::SpanId span_id,
                   trace::TraceFlags trace_flags,
                   common::SystemTimestamp timestamp) noexcept = 0;

  /*** Overloaded methods for KeyValueIterables ***/
  /**
   * The secondary base Log(...) method that all other Log(...) overloaded methods except the one
   * above will eventually call,  in order to create a log record.
   */
  template <class T,
            nostd::enable_if_t<common::detail::is_key_value_iterable<T>::value> * = nullptr>
  void Log(Severity severity,
           nostd::string_view name,
           nostd::string_view body,
           const T &attributes,
           trace::TraceId trace_id,
           trace::SpanId span_id,
           trace::TraceFlags trace_flags,
           common::SystemTimestamp timestamp) noexcept
  {
    Log(severity, name, body, common::KeyValueIterableView<T>(attributes), trace_id, span_id,
        trace_flags, timestamp);
  }

  void Log(Severity severity,
           nostd::string_view name,
           nostd::string_view body,
           std::initializer_list<std::pair<nostd::string_view, common::AttributeValue>> attributes,
           trace::TraceId trace_id,
           trace::SpanId span_id,
           trace::TraceFlags trace_flags,
           common::SystemTimestamp timestamp) noexcept
  {
    return this->Log(severity, name, body,
                     nostd::span<const std::pair<nostd::string_view, common::AttributeValue>>{
                         attributes.begin(), attributes.end()},
                     trace_id, span_id, trace_flags, timestamp);
  }

  /** Wrapper methods that the user could call for convenience when logging **/

  /**
   * Writes a log.
   * @param severity The severity of the log
   * @param message The message to log
   */
  void Log(Severity severity, nostd::string_view message) noexcept
  {
    this->Log(severity, "", message, {}, {}, {}, {}, std::chrono::system_clock::now());
  }

  /**
   * Writes a log.
   * @param severity The severity of the log
   * @param name The name of the log
   * @param message The message to log
   */
  void Log(Severity severity, nostd::string_view name, nostd::string_view message) noexcept
  {
    this->Log(severity, name, message, {}, {}, {}, {}, std::chrono::system_clock::now());
  }

  /**
   * Writes a log.
   * @param severity The severity of the log
   * @param attributes The attributes of the log as a key/value object
   */
  template <class T,
            nostd::enable_if_t<common::detail::is_key_value_iterable<T>::value> * = nullptr>
  void Log(Severity severity, const T &attributes) noexcept
  {
    this->Log(severity, "", "", attributes, {}, {}, {}, std::chrono::system_clock::now());
  }

  /**
   * Writes a log.
   * @param severity The severity of the log
   * @param name The name of the log
   * @param attributes The attributes of the log as a key/value object
   */
  template <class T,
            nostd::enable_if_t<common::detail::is_key_value_iterable<T>::value> * = nullptr>
  void Log(Severity severity, nostd::string_view name, const T &attributes) noexcept
  {
    this->Log(severity, name, "", attributes, {}, {}, {}, std::chrono::system_clock::now());
  }

  /**
   * Writes a log.
   * @param severity The severity of the log
   * @param attributes The attributes of the log as an initializer list
   */
  void Log(Severity severity,
           std::initializer_list<std::pair<nostd::string_view, common::AttributeValue>>
               attributes) noexcept
  {
    this->Log(severity, "", "", attributes, {}, {}, {}, std::chrono::system_clock::now());
  }

  /**
   * Writes a log.
   * @param severity The severity of the log
   * @param name The name of the log
   * @param attributes The attributes of the log as an initializer list
   */
  void Log(Severity severity,
           nostd::string_view name,
           std::initializer_list<std::pair<nostd::string_view, common::AttributeValue>>
               attributes) noexcept
  {
    this->Log(severity, name, "", attributes, {}, {}, {}, std::chrono::system_clock::now());
  }

  /**
   * Writes a log.
   * @param severity The severity of the log
   * @param name The name of the log
   * @param attributes The attributes, stored as a 2D list of key/value pairs, that are associated
   * with the log event
   */
  void Log(Severity severity,
           nostd::string_view name,
           common::KeyValueIterable &attributes) noexcept
  {
    this->Log(severity, name, {}, attributes, {}, {}, {}, std::chrono::system_clock::now());
  }

  /** Trace severity overloads **/

  /**
   * Writes a log with a severity of trace.
   * @param message The message to log
   */
  void Trace(nostd::string_view message) noexcept { this->Log(Severity::kTrace, message); }

  /**
   * Writes a log with a severity of trace.
   * @param name The name of the log
   * @param message The message to log
   */
  void Trace(nostd::string_view name, nostd::string_view message) noexcept
  {
    this->Log(Severity::kTrace, name, message);
  }

  /**
   * Writes a log with a severity of trace.
   * @param attributes The attributes of the log as a key/value object
   */
  template <class T,
            nostd::enable_if_t<common::detail::is_key_value_iterable<T>::value> * = nullptr>
  void Trace(const T &attributes) noexcept
  {
    this->Log(Severity::kTrace, attributes);
  }

  /**
   * Writes a log with a severity of trace.
   * @param name The name of the log
   * @param attributes The attributes of the log as a key/value object
   */
  template <class T,
            nostd::enable_if_t<common::detail::is_key_value_iterable<T>::value> * = nullptr>
  void Trace(nostd::string_view name, const T &attributes) noexcept
  {
    this->Log(Severity::kTrace, name, attributes);
  }

  /**
   * Writes a log with a severity of trace.
   * @param attributes The attributes of the log as an initializer list
   */
  void Trace(std::initializer_list<std::pair<nostd::string_view, common::AttributeValue>>
                 attributes) noexcept
  {
    this->Log(Severity::kTrace, attributes);
  }

  /**
   * Writes a log with a severity of trace.
   * @param name The name of the log
   * @param attributes The attributes of the log as an initializer list
   */
  void Trace(nostd::string_view name,
             std::initializer_list<std::pair<nostd::string_view, common::AttributeValue>>
                 attributes) noexcept
  {
    this->Log(Severity::kTrace, name, attributes);
  }

  /** Debug severity overloads **/

  /**
   * Writes a log with a severity of debug.
   * @param message The message to log
   */
  void Debug(nostd::string_view message) noexcept { this->Log(Severity::kDebug, message); }

  /**
   * Writes a log with a severity of debug.
   * @param name The name of the log
   * @param message The message to log
   */
  void Debug(nostd::string_view name, nostd::string_view message) noexcept
  {
    this->Log(Severity::kDebug, name, message);
  }

  /**
   * Writes a log with a severity of debug.
   * @param attributes The attributes of the log as a key/value object
   */
  template <class T,
            nostd::enable_if_t<common::detail::is_key_value_iterable<T>::value> * = nullptr>
  void Debug(const T &attributes) noexcept
  {
    this->Log(Severity::kDebug, attributes);
  }

  /**
   * Writes a log with a severity of debug.
   * @param name The name of the log
   * @param attributes The attributes of the log as a key/value object
   */
  template <class T,
            nostd::enable_if_t<common::detail::is_key_value_iterable<T>::value> * = nullptr>
  void Debug(nostd::string_view name, const T &attributes) noexcept
  {
    this->Log(Severity::kDebug, name, attributes);
  }

  /**
   * Writes a log with a severity of debug.
   * @param attributes The attributes of the log as an initializer list
   */
  void Debug(std::initializer_list<std::pair<nostd::string_view, common::AttributeValue>>
                 attributes) noexcept
  {
    this->Log(Severity::kDebug, attributes);
  }

  /**
   * Writes a log with a severity of debug.
   * @param name The name of the log
   * @param attributes The attributes of the log as an initializer list
   */
  void Debug(nostd::string_view name,
             std::initializer_list<std::pair<nostd::string_view, common::AttributeValue>>
                 attributes) noexcept
  {
    this->Log(Severity::kDebug, name, attributes);
  }

  /** Info severity overloads **/

  /**
   * Writes a log with a severity of info.
   * @param message The message to log
   */
  void Info(nostd::string_view message) noexcept { this->Log(Severity::kInfo, message); }

  /**
   * Writes a log with a severity of info.
   * @param name The name of the log
   * @param message The message to log
   */
  void Info(nostd::string_view name, nostd::string_view message) noexcept
  {
    this->Log(Severity::kInfo, name, message);
  }

  /**
   * Writes a log with a severity of info.
   * @param attributes The attributes of the log as a key/value object
   */
  template <class T,
            nostd::enable_if_t<common::detail::is_key_value_iterable<T>::value> * = nullptr>
  void Info(const T &attributes) noexcept
  {
    this->Log(Severity::kInfo, attributes);
  }

  /**
   * Writes a log with a severity of info.
   * @param name The name of the log
   * @param attributes The attributes of the log as a key/value object
   */
  template <class T,
            nostd::enable_if_t<common::detail::is_key_value_iterable<T>::value> * = nullptr>
  void Info(nostd::string_view name, const T &attributes) noexcept
  {
    this->Log(Severity::kInfo, name, attributes);
  }

  /**
   * Writes a log with a severity of info.
   * @param attributes The attributes of the log as an initializer list
   */
  void Info(std::initializer_list<std::pair<nostd::string_view, common::AttributeValue>>
                attributes) noexcept
  {
    this->Log(Severity::kInfo, attributes);
  }

  /**
   * Writes a log with a severity of info.
   * @param name The name of the log
   * @param attributes The attributes of the log as an initializer list
   */
  void Info(nostd::string_view name,
            std::initializer_list<std::pair<nostd::string_view, common::AttributeValue>>
                attributes) noexcept
  {
    this->Log(Severity::kInfo, name, attributes);
  }

  /** Warn severity overloads **/

  /**
   * Writes a log with a severity of warn.
   * @param message The message to log
   */
  void Warn(nostd::string_view message) noexcept { this->Log(Severity::kWarn, message); }

  /**
   * Writes a log with a severity of warn.
   * @param name The name of the log
   * @param message The message to log
   */
  void Warn(nostd::string_view name, nostd::string_view message) noexcept
  {
    this->Log(Severity::kWarn, name, message);
  }

  /**
   * Writes a log with a severity of warn.
   * @param attributes The attributes of the log as a key/value object
   */
  template <class T,
            nostd::enable_if_t<common::detail::is_key_value_iterable<T>::value> * = nullptr>
  void Warn(const T &attributes) noexcept
  {
    this->Log(Severity::kWarn, attributes);
  }

  /**
   * Writes a log with a severity of warn.
   * @param name The name of the log
   * @param attributes The attributes of the log as a key/value object
   */
  template <class T,
            nostd::enable_if_t<common::detail::is_key_value_iterable<T>::value> * = nullptr>
  void Warn(nostd::string_view name, const T &attributes) noexcept
  {
    this->Log(Severity::kWarn, name, attributes);
  }

  /**
   * Writes a log with a severity of warn.
   * @param attributes The attributes of the log as an initializer list
   */
  void Warn(std::initializer_list<std::pair<nostd::string_view, common::AttributeValue>>
                attributes) noexcept
  {
    this->Log(Severity::kWarn, attributes);
  }

  /**
   * Writes a log with a severity of warn.
   * @param name The name of the log
   * @param attributes The attributes of the log as an initializer list
   */
  void Warn(nostd::string_view name,
            std::initializer_list<std::pair<nostd::string_view, common::AttributeValue>>
                attributes) noexcept
  {
    this->Log(Severity::kWarn, name, attributes);
  }

  /** Error severity overloads **/

  /**
   * Writes a log with a severity of error.
   * @param message The message to log
   */
  void Error(nostd::string_view message) noexcept { this->Log(Severity::kError, message); }

  /**
   * Writes a log with a severity of error.
   * @param name The name of the log
   * @param message The message to log
   */
  void Error(nostd::string_view name, nostd::string_view message) noexcept
  {
    this->Log(Severity::kError, name, message);
  }

  /**
   * Writes a log with a severity of error.
   * @param attributes The attributes of the log as a key/value object
   */
  template <class T,
            nostd::enable_if_t<common::detail::is_key_value_iterable<T>::value> * = nullptr>
  void Error(const T &attributes) noexcept
  {
    this->Log(Severity::kError, attributes);
  }

  /**
   * Writes a log with a severity of error.
   * @param name The name of the log
   * @param attributes The attributes of the log as a key/value object
   */
  template <class T,
            nostd::enable_if_t<common::detail::is_key_value_iterable<T>::value> * = nullptr>
  void Error(nostd::string_view name, const T &attributes) noexcept
  {
    this->Log(Severity::kError, name, attributes);
  }

  /**
   * Writes a log with a severity of error.
   * @param attributes The attributes of the log as an initializer list
   */
  void Error(std::initializer_list<std::pair<nostd::string_view, common::AttributeValue>>
                 attributes) noexcept
  {
    this->Log(Severity::kError, attributes);
  }

  /**
   * Writes a log with a severity of error.
   * @param name The name of the log
   * @param attributes The attributes of the log as an initializer list
   */
  void Error(nostd::string_view name,
             std::initializer_list<std::pair<nostd::string_view, common::AttributeValue>>
                 attributes) noexcept
  {
    this->Log(Severity::kError, name, attributes);
  }

  /** Fatal severity overloads **/

  /**
   * Writes a log with a severity of fatal.
   * @param message The message to log
   */
  void Fatal(nostd::string_view message) noexcept { this->Log(Severity::kFatal, message); }

  /**
   * Writes a log with a severity of fatal.
   * @param name The name of the log
   * @param message The message to log
   */
  void Fatal(nostd::string_view name, nostd::string_view message) noexcept
  {
    this->Log(Severity::kFatal, name, message);
  }

  /**
   * Writes a log with a severity of fatal.
   * @param attributes The attributes of the log as a key/value object
   */
  template <class T,
            nostd::enable_if_t<common::detail::is_key_value_iterable<T>::value> * = nullptr>
  void Fatal(const T &attributes) noexcept
  {
    this->Log(Severity::kFatal, attributes);
  }

  /**
   * Writes a log with a severity of fatal.
   * @param name The name of the log
   * @param attributes The attributes of the log as a key/value object
   */
  template <class T,
            nostd::enable_if_t<common::detail::is_key_value_iterable<T>::value> * = nullptr>
  void Fatal(nostd::string_view name, const T &attributes) noexcept
  {
    this->Log(Severity::kFatal, name, attributes);
  }

  /**
   * Writes a log with a severity of fatal.
   * @param attributes The attributes of the log as an initializer list
   */
  void Fatal(std::initializer_list<std::pair<nostd::string_view, common::AttributeValue>>
                 attributes) noexcept
  {
    this->Log(Severity::kFatal, attributes);
  }

  /**
   * Writes a log with a severity of fatal.
   * @param name The name of the log
   * @param attributes The attributes of the log as an initializer list
   */
  void Fatal(nostd::string_view name,
             std::initializer_list<std::pair<nostd::string_view, common::AttributeValue>>
                 attributes) noexcept
  {
    this->Log(Severity::kFatal, name, attributes);
  }
};
}  // namespace logs
OPENTELEMETRY_END_NAMESPACE
#endif
