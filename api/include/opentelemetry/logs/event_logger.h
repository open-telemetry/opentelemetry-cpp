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
#  include "opentelemetry/logs/logger.h"
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
 * Handles event log record creation.
 **/
class EventLogger
{
public:
  virtual ~EventLogger() = default;

  /* Returns the name of the logger */
  virtual const nostd::string_view GetName() noexcept = 0;

  /* Returns the delegate logger of this event logger */
  virtual nostd::shared_ptr<Logger> GetDelegateLogger() noexcept = 0;

  /**
   * Create a Log Record object
   *
   * @return nostd::unique_ptr<LogRecord>
   */
  virtual nostd::unique_ptr<LogRecord> CreateLogRecord() noexcept = 0;

  /**
   * Emit a event Log Record object
   *
   * @param event_name Event name
   * @param log_record Log record
   */
  virtual void EmitEvent(nostd::string_view event_name,
                         nostd::unique_ptr<LogRecord> &&log_record) noexcept = 0;

  /**
   * Emit a event Log Record object with arguments
   *
   * @param event_name Event name
   * @param log_record Log record
   * @tparam args Arguments
   */
  template <class... ArgumentType>
  void EmitEvent(nostd::string_view event_name,
                 nostd::unique_ptr<LogRecord> &&log_record,
                 ArgumentType &&... args)
  {
    if (!log_record)
    {
      return;
    }

    IgnoreTraitResult(
        detail::LogRecordSetterTrait<typename std::decay<ArgumentType>::type>::template Set(
            log_record.get(), std::forward<ArgumentType>(args))...);

    EmitEvent(event_name, std::move(log_record));
  }

  /**
   * Emit a event Log Record object with arguments
   *
   * @param event_name Event name
   * @param log_record Log record
   */
  template <class... ArgumentType>
  void EmitEvent(Severity severity, nostd::string_view event_name, ArgumentType &&... args)
  {
    nostd::unique_ptr<LogRecord> log_record = CreateLogRecord();
    if (!log_record)
    {
      return;
    }

    log_record->SetSeverity(severity);
    EmitEvent(event_name, std::move(log_record), std::forward<ArgumentType>(args)...);
  }

private:
  template <class... ValueType>
  void IgnoreTraitResult(ValueType &&...)
  {}
};
}  // namespace logs
OPENTELEMETRY_END_NAMESPACE

#endif
