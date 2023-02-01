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
   * Utility function to help to make a attribute view from a KeyValueIterable
   *
   * @param attributes
   * @return common::KeyValueIterable
   */
  inline static const common::KeyValueIterable &MakeAttributes(
      const common::KeyValueIterable &attributes) noexcept
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
   * @tparam args Arguments which can be used to set data of log record by type.
   *  Severity                                -> severity, severity_text
   *  string_view                             -> body
   *  AttributeValue                          -> body
   *  SpanContext                             -> span_id,tace_id and trace_flags
   *  SpanId                                  -> span_id
   *  TraceId                                 -> tace_id
   *  TraceFlags                              -> trace_flags
   *  SystemTimestamp                         -> timestamp
   *  system_clock::time_point                -> timestamp
   *  KeyValueIterable                        -> attributes
   *  Key value iterable container            -> attributes
   *  span<pair<string_view, AttributeValue>> -> attributes(return type of MakeAttributes)
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
   * @tparam args Arguments which can be used to set data of log record by type.
   *  Severity                                -> severity, severity_text
   *  string_view                             -> body
   *  AttributeValue                          -> body
   *  SpanContext                             -> span_id,tace_id and trace_flags
   *  SpanId                                  -> span_id
   *  TraceId                                 -> tace_id
   *  TraceFlags                              -> trace_flags
   *  SystemTimestamp                         -> timestamp
   *  system_clock::time_point                -> timestamp
   *  KeyValueIterable                        -> attributes
   *  Key value iterable container            -> attributes
   *  span<pair<string_view, AttributeValue>> -> attributes(return type of MakeAttributes)
   */
  template <class... ArgumentType>
  void EmitLogRecord(ArgumentType &&... args)
  {
    nostd::unique_ptr<LogRecord> log_record = CreateLogRecord();
    if (!log_record)
    {
      return;
    }

    EmitLogRecord(std::move(log_record), std::forward<ArgumentType>(args)...);
  }

  /**
   * Writes a log with a severity of trace.
   * @tparam args Arguments which can be used to set data of log record by type.
   *  string_view                             -> body
   *  AttributeValue                          -> body
   *  SpanContext                             -> span_id,tace_id and trace_flags
   *  SpanId                                  -> span_id
   *  TraceId                                 -> tace_id
   *  TraceFlags                              -> trace_flags
   *  SystemTimestamp                         -> timestamp
   *  system_clock::time_point                -> timestamp
   *  KeyValueIterable                        -> attributes
   *  Key value iterable container            -> attributes
   *  span<pair<string_view, AttributeValue>> -> attributes(return type of MakeAttributes)
   */
  template <class... ArgumentType>
  void Trace(ArgumentType &&... args) noexcept
  {
    static_assert(
        !detail::LogRecordHasType<Severity, typename std::decay<ArgumentType>::type...>::value,
        "Severity is already set.");
    this->EmitLogRecord(Severity::kTrace, std::forward<ArgumentType>(args)...);
  }

  /**
   * Writes a log with a severity of debug.
   * @tparam args Arguments which can be used to set data of log record by type.
   *  string_view                             -> body
   *  AttributeValue                          -> body
   *  SpanContext                             -> span_id,tace_id and trace_flags
   *  SpanId                                  -> span_id
   *  TraceId                                 -> tace_id
   *  TraceFlags                              -> trace_flags
   *  SystemTimestamp                         -> timestamp
   *  system_clock::time_point                -> timestamp
   *  KeyValueIterable                        -> attributes
   *  Key value iterable container            -> attributes
   *  span<pair<string_view, AttributeValue>> -> attributes(return type of MakeAttributes)
   */
  template <class... ArgumentType>
  void Debug(ArgumentType &&... args) noexcept
  {
    static_assert(
        !detail::LogRecordHasType<Severity, typename std::decay<ArgumentType>::type...>::value,
        "Severity is already set.");
    this->EmitLogRecord(Severity::kDebug, std::forward<ArgumentType>(args)...);
  }

  /**
   * Writes a log with a severity of info.
   * @tparam args Arguments which can be used to set data of log record by type.
   *  string_view                             -> body
   *  AttributeValue                          -> body
   *  SpanContext                             -> span_id,tace_id and trace_flags
   *  SpanId                                  -> span_id
   *  TraceId                                 -> tace_id
   *  TraceFlags                              -> trace_flags
   *  SystemTimestamp                         -> timestamp
   *  system_clock::time_point                -> timestamp
   *  KeyValueIterable                        -> attributes
   *  Key value iterable container            -> attributes
   *  span<pair<string_view, AttributeValue>> -> attributes(return type of MakeAttributes)
   */
  template <class... ArgumentType>
  void Info(ArgumentType &&... args) noexcept
  {
    static_assert(
        !detail::LogRecordHasType<Severity, typename std::decay<ArgumentType>::type...>::value,
        "Severity is already set.");
    this->EmitLogRecord(Severity::kInfo, std::forward<ArgumentType>(args)...);
  }

  /**
   * Writes a log with a severity of warn.
   * @tparam args Arguments which can be used to set data of log record by type.
   *  string_view                             -> body
   *  AttributeValue                          -> body
   *  SpanContext                             -> span_id,tace_id and trace_flags
   *  SpanId                                  -> span_id
   *  TraceId                                 -> tace_id
   *  TraceFlags                              -> trace_flags
   *  SystemTimestamp                         -> timestamp
   *  system_clock::time_point                -> timestamp
   *  KeyValueIterable                        -> attributes
   *  Key value iterable container            -> attributes
   *  span<pair<string_view, AttributeValue>> -> attributes(return type of MakeAttributes)
   */
  template <class... ArgumentType>
  void Warn(ArgumentType &&... args) noexcept
  {
    static_assert(
        !detail::LogRecordHasType<Severity, typename std::decay<ArgumentType>::type...>::value,
        "Severity is already set.");
    this->EmitLogRecord(Severity::kWarn, std::forward<ArgumentType>(args)...);
  }

  /**
   * Writes a log with a severity of error.
   * @tparam args Arguments which can be used to set data of log record by type.
   *  string_view                             -> body
   *  AttributeValue                          -> body
   *  SpanContext                             -> span_id,tace_id and trace_flags
   *  SpanId                                  -> span_id
   *  TraceId                                 -> tace_id
   *  TraceFlags                              -> trace_flags
   *  SystemTimestamp                         -> timestamp
   *  system_clock::time_point                -> timestamp
   *  KeyValueIterable                        -> attributes
   *  Key value iterable container            -> attributes
   *  span<pair<string_view, AttributeValue>> -> attributes(return type of MakeAttributes)
   */
  template <class... ArgumentType>
  void Error(ArgumentType &&... args) noexcept
  {
    static_assert(
        !detail::LogRecordHasType<Severity, typename std::decay<ArgumentType>::type...>::value,
        "Severity is already set.");
    this->EmitLogRecord(Severity::kError, std::forward<ArgumentType>(args)...);
  }

  /**
   * Writes a log with a severity of fatal.
   * @tparam args Arguments which can be used to set data of log record by type.
   *  string_view                             -> body
   *  AttributeValue                          -> body
   *  SpanContext                             -> span_id,tace_id and trace_flags
   *  SpanId                                  -> span_id
   *  TraceId                                 -> tace_id
   *  TraceFlags                              -> trace_flags
   *  SystemTimestamp                         -> timestamp
   *  system_clock::time_point                -> timestamp
   *  KeyValueIterable                        -> attributes
   *  Key value iterable container            -> attributes
   *  span<pair<string_view, AttributeValue>> -> attributes(return type of MakeAttributes)
   */
  template <class... ArgumentType>
  void Fatal(ArgumentType &&... args) noexcept
  {
    static_assert(
        !detail::LogRecordHasType<Severity, typename std::decay<ArgumentType>::type...>::value,
        "Severity is already set.");
    this->EmitLogRecord(Severity::kFatal, std::forward<ArgumentType>(args)...);
  }

private:
  template <class... ValueType>
  void IgnoreTraitResult(ValueType &&...)
  {}
};
}  // namespace logs
OPENTELEMETRY_END_NAMESPACE

#endif
