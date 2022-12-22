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
#  include "opentelemetry/common/macros.h"
#  include "opentelemetry/common/timestamp.h"
#  include "opentelemetry/logs/log_record.h"
#  include "opentelemetry/logs/logger_type_traits.h"
#  include "opentelemetry/logs/severity.h"
#  include "opentelemetry/nostd/shared_ptr.h"
#  include "opentelemetry/nostd/span.h"
#  include "opentelemetry/nostd/string_view.h"
#  include "opentelemetry/nostd/type_traits.h"
#  include "opentelemetry/nostd/unique_ptr.h"
#  include "opentelemetry/trace/span_context.h"
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
  /**
   * Utility function to help to make a attribute view from initializer_list
   *
   * @param attributes
   * @return nostd::span<const std::pair<nostd::string_view, common::AttributeValue>>
   */
  inline static nostd::span<const std::pair<nostd::string_view, common::AttributeValue>>
  MakeAttributes(std::initializer_list<std::pair<nostd::string_view, common::AttributeValue>>
                     attributes) noexcept
  {
    return nostd::span<const std::pair<nostd::string_view, common::AttributeValue>>{
        attributes.begin(), attributes.end()};
  }

  /**
   * Utility function to help to make a attribute view from a span
   *
   * @param attributes
   * @return nostd::span<const std::pair<nostd::string_view, common::AttributeValue>>
   */
  inline static nostd::span<const std::pair<nostd::string_view, common::AttributeValue>>
  MakeAttributes(
      nostd::span<const std::pair<nostd::string_view, common::AttributeValue>> attributes) noexcept
  {
    return attributes;
  }

  /**
   * Utility function to help to make a attribute view from a key-value iterable object
   *
   * @param attributes
   * @return nostd::span<const std::pair<nostd::string_view, common::AttributeValue>>
   */
  template <
      class ArgumentType,
      nostd::enable_if_t<common::detail::is_key_value_iterable<ArgumentType>::value> * = nullptr>
  inline static common::KeyValueIterableView<ArgumentType> MakeAttributes(
      const ArgumentType &arg) noexcept
  {
    return common::KeyValueIterableView<ArgumentType>(arg);
  }

  virtual ~Logger() = default;

  /* Returns the name of the logger */
  virtual const nostd::string_view GetName() noexcept = 0;

  /**
   * Create a Log Record object
   *
   * @return nostd::unique_ptr<LogRecord>
   */
  virtual nostd::unique_ptr<LogRecord> CreateLogRecord() noexcept = 0;

  /**
   * Emit a Log Record object
   *
   * @param log_record
   */
  virtual void EmitLogRecord(nostd::unique_ptr<LogRecord> &&log_record) noexcept = 0;

  /**
   * Emit a Log Record object with arguments
   *
   * @param log_record Log record
   * @tparam args Arguments
   */
  template <class... ArgumentType>
  void EmitLogRecord(nostd::unique_ptr<LogRecord> &&log_record, ArgumentType &&... args)
  {
    if (!log_record)
    {
      return;
    }

    IgnoreTraitResult(
        detail::LogRecordSetterTrait<typename std::decay<ArgumentType>::type>::template Set(
            log_record.get(), std::forward<ArgumentType>(args))...);

    EmitLogRecord(std::move(log_record));
  }

  /**
   * Emit a Log Record object with arguments
   *
   * @param log_record
   */
  template <class... ArgumentType>
  void EmitLogRecord(Severity severity, ArgumentType &&... args)
  {
    nostd::unique_ptr<LogRecord> log_record = CreateLogRecord();
    if (!log_record)
    {
      return;
    }

    log_record->SetSeverity(severity);
    EmitLogRecord(std::move(log_record), std::forward<ArgumentType>(args)...);
  }

  /**
   *
   * @param severity the severity level of the log event.
   * @param message the string message of the log (perhaps support std::fmt or fmt-lib format).
   * @param attributes the attributes, stored as a 2D list of key/value pairs, that are associated
   * with the log event.
   * @param trace_id the trace id associated with the log event.
   * @param span_id the span id associate with the log event.
   * @param trace_flags the trace flags associated with the log event.
   * @param timestamp the timestamp the log record was created.
   * @throws No exceptions under any circumstances.
   */
  virtual void Log(Severity severity,
                   nostd::string_view body,
                   const common::KeyValueIterable &attributes,
                   trace::TraceId trace_id,
                   trace::SpanId span_id,
                   trace::TraceFlags trace_flags,
                   common::SystemTimestamp timestamp) noexcept
  {
    bool trace_id_valid = trace_id.IsValid();
    bool span_id_valid  = span_id.IsValid();
    if (trace_id_valid && span_id_valid)
    {
      EmitLogRecord(severity, body, attributes, trace_id, trace_flags, span_id, timestamp);
    }
    else if (trace_id_valid)
    {
      EmitLogRecord(severity, body, attributes, trace_id, trace_flags, timestamp);
    }
    else if (span_id_valid)
    {
      EmitLogRecord(severity, body, attributes, span_id, timestamp);
    }
    else
    {
      EmitLogRecord(severity, body, attributes, timestamp);
    }
  }

  /**
   *
   * @param severity the severity level of the log event.
   * @param message the string message of the log (perhaps support std::fmt or fmt-lib format).
   * @param attributes the attributes, stored as a 2D list of key/value pairs, that are associated
   * with the log event.
   * @param timestamp the timestamp the log record was created.
   * @throws No exceptions under any circumstances.
   */
  virtual void Log(Severity severity,
                   nostd::string_view body,
                   const common::KeyValueIterable &attributes,
                   common::SystemTimestamp timestamp) noexcept
  {
    EmitLogRecord(severity, body, attributes, timestamp);
  }

  /*** Overloaded methods for KeyValueIterables ***/
  /**
   * The secondary base Log(...) method that all other Log(...) overloaded methods except the one
   * above will eventually call,  in order to create a log record.
   */
  template <class T,
            nostd::enable_if_t<common::detail::is_key_value_iterable<T>::value> * = nullptr>
  void Log(Severity severity,
           nostd::string_view body,
           const T &attributes,
           trace::TraceId trace_id,
           trace::SpanId span_id,
           trace::TraceFlags trace_flags,
           common::SystemTimestamp timestamp) noexcept
  {
    Log(severity, body, common::KeyValueIterableView<T>(attributes), trace_id, span_id, trace_flags,
        timestamp);
  }

  template <class T,
            nostd::enable_if_t<common::detail::is_key_value_iterable<T>::value> * = nullptr>
  void Log(Severity severity,
           nostd::string_view body,
           const T &attributes,
           common::SystemTimestamp timestamp) noexcept
  {
    Log(severity, body, common::KeyValueIterableView<T>(attributes), timestamp);
  }

  void Log(Severity severity,
           nostd::string_view body,
           std::initializer_list<std::pair<nostd::string_view, common::AttributeValue>> attributes,
           trace::TraceId trace_id,
           trace::SpanId span_id,
           trace::TraceFlags trace_flags,
           common::SystemTimestamp timestamp) noexcept
  {
    return this->Log(severity, body,
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
    this->Log(severity, message,
              nostd::span<const std::pair<nostd::string_view, common::AttributeValue>>{},
              std::chrono::system_clock::now());
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
    this->Log(severity, "", attributes, std::chrono::system_clock::now());
  }

  /**
   * Writes a log.
   * @param severity The severity of the log
   * @param message The message to log
   * @param attributes The attributes of the log as a key/value object
   */
  template <class T,
            nostd::enable_if_t<common::detail::is_key_value_iterable<T>::value> * = nullptr>
  void Log(Severity severity, nostd::string_view message, const T &attributes) noexcept
  {
    this->Log(severity, message, attributes, std::chrono::system_clock::now());
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
    this->Log(severity, "", attributes, std::chrono::system_clock::now());
  }

  /**
   * Writes a log.
   * @param severity The severity of the log
   * @param message The message to log
   * @param attributes The attributes of the log as an initializer list
   */
  void Log(Severity severity,
           nostd::string_view message,
           std::initializer_list<std::pair<nostd::string_view, common::AttributeValue>>
               attributes) noexcept
  {
    this->Log(severity, message, attributes, std::chrono::system_clock::now());
  }

  /**
   * Writes a log.
   * @param severity The severity of the log
   * @param attributes The attributes, stored as a 2D list of key/value pairs, that are associated
   * with the log event
   */
  void Log(Severity severity, const common::KeyValueIterable &attributes) noexcept
  {
    this->Log(severity, "", attributes, std::chrono::system_clock::now());
  }

  /**
   * Writes a log.
   * @param severity The severity of the log
   * @param message The message to log
   * @param attributes The attributes, stored as a 2D list of key/value pairs, that are associated
   * with the log event
   */
  void Log(Severity severity,
           nostd::string_view message,
           const common::KeyValueIterable &attributes) noexcept
  {
    this->Log(severity, message, attributes, std::chrono::system_clock::now());
  }

  /** Trace severity overloads **/

  /**
   * Writes a log with a severity of trace.
   * @param message The message to log
   */
  void Trace(nostd::string_view message) noexcept { this->Log(Severity::kTrace, message); }

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
   * @param message The message of the log
   * @param attributes The attributes of the log as a key/value object
   */
  template <class T,
            nostd::enable_if_t<common::detail::is_key_value_iterable<T>::value> * = nullptr>
  void Trace(nostd::string_view message, const T &attributes) noexcept
  {
    this->Log(Severity::kTrace, message, attributes);
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
   * @param message The message of the log
   * @param attributes The attributes of the log as an initializer list
   */
  void Trace(nostd::string_view message,
             std::initializer_list<std::pair<nostd::string_view, common::AttributeValue>>
                 attributes) noexcept
  {
    this->Log(Severity::kTrace, message, attributes);
  }

  /** Debug severity overloads **/

  /**
   * Writes a log with a severity of debug.
   * @param message The message to log
   */
  void Debug(nostd::string_view message) noexcept { this->Log(Severity::kDebug, message); }

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
   * @param message The message of the log
   * @param attributes The attributes of the log as a key/value object
   */
  template <class T,
            nostd::enable_if_t<common::detail::is_key_value_iterable<T>::value> * = nullptr>
  void Debug(nostd::string_view message, const T &attributes) noexcept
  {
    this->Log(Severity::kDebug, message, attributes);
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
   * @param message The message of the log
   * @param attributes The attributes of the log as an initializer list
   */
  void Debug(nostd::string_view message,
             std::initializer_list<std::pair<nostd::string_view, common::AttributeValue>>
                 attributes) noexcept
  {
    this->Log(Severity::kDebug, message, attributes);
  }

  /** Info severity overloads **/

  /**
   * Writes a log with a severity of info.
   * @param message The message to log
   */
  void Info(nostd::string_view message) noexcept { this->Log(Severity::kInfo, message); }

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
   * @param message The message of the log
   * @param attributes The attributes of the log as a key/value object
   */
  template <class T,
            nostd::enable_if_t<common::detail::is_key_value_iterable<T>::value> * = nullptr>
  void Info(nostd::string_view message, const T &attributes) noexcept
  {
    this->Log(Severity::kInfo, message, attributes);
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
   * @param message The message of the log
   * @param attributes The attributes of the log as an initializer list
   */
  void Info(nostd::string_view message,
            std::initializer_list<std::pair<nostd::string_view, common::AttributeValue>>
                attributes) noexcept
  {
    this->Log(Severity::kInfo, message, attributes);
  }

  /** Warn severity overloads **/

  /**
   * Writes a log with a severity of warn.
   * @param message The message to log
   */
  void Warn(nostd::string_view message) noexcept { this->Log(Severity::kWarn, message); }

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
   * @param message The message of the log
   * @param attributes The attributes of the log as a key/value object
   */
  template <class T,
            nostd::enable_if_t<common::detail::is_key_value_iterable<T>::value> * = nullptr>
  void Warn(nostd::string_view message, const T &attributes) noexcept
  {
    this->Log(Severity::kWarn, message, attributes);
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
   * @param message The message of the log
   * @param attributes The attributes of the log as an initializer list
   */
  void Warn(nostd::string_view message,
            std::initializer_list<std::pair<nostd::string_view, common::AttributeValue>>
                attributes) noexcept
  {
    this->Log(Severity::kWarn, message, attributes);
  }

  /** Error severity overloads **/

  /**
   * Writes a log with a severity of error.
   * @param message The message to log
   */
  void Error(nostd::string_view message) noexcept { this->Log(Severity::kError, message); }

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
   * @param message The message of the log
   * @param attributes The attributes of the log as a key/value object
   */
  template <class T,
            nostd::enable_if_t<common::detail::is_key_value_iterable<T>::value> * = nullptr>
  void Error(nostd::string_view message, const T &attributes) noexcept
  {
    this->Log(Severity::kError, message, attributes);
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
   * @param message The message of the log
   * @param attributes The attributes of the log as an initializer list
   */
  void Error(nostd::string_view message,
             std::initializer_list<std::pair<nostd::string_view, common::AttributeValue>>
                 attributes) noexcept
  {
    this->Log(Severity::kError, message, attributes);
  }

  /** Fatal severity overloads **/

  /**
   * Writes a log with a severity of fatal.
   * @param message The message to log
   */
  void Fatal(nostd::string_view message) noexcept { this->Log(Severity::kFatal, message); }

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
   * @param message The message of the log
   * @param attributes The attributes of the log as a key/value object
   */
  template <class T,
            nostd::enable_if_t<common::detail::is_key_value_iterable<T>::value> * = nullptr>
  void Fatal(nostd::string_view message, const T &attributes) noexcept
  {
    this->Log(Severity::kFatal, message, attributes);
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
   * @param message The message of the log
   * @param attributes The attributes of the log as an initializer list
   */
  void Fatal(nostd::string_view message,
             std::initializer_list<std::pair<nostd::string_view, common::AttributeValue>>
                 attributes) noexcept
  {
    this->Log(Severity::kFatal, message, attributes);
  }

private:
  template <class... ValueType>
  void IgnoreTraitResult(ValueType &&...)
  {}
};
}  // namespace logs
OPENTELEMETRY_END_NAMESPACE

#endif
