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
namespace detail
{
template <class ValueType>
struct LogRecordSetterTrait;

template <>
struct LogRecordSetterTrait<trace::SpanContext>
{
  template <class ArgumentType>
  inline static LogRecord *Set(LogRecord *log_record, ArgumentType &&arg)
  {
    log_record->SetSpanId(arg.span_id());
    log_record->SetTraceId(arg.trace_id());
    log_record->SetTraceFlags(arg.trace_flags());

    return log_record;
  }
};

template <>
struct LogRecordSetterTrait<trace::SpanId>
{
  template <class ArgumentType>
  inline static LogRecord *Set(LogRecord *log_record, ArgumentType &&arg)
  {
    log_record->SetSpanId(std::forward<ArgumentType>(arg));

    return log_record;
  }
};

template <>
struct LogRecordSetterTrait<trace::TraceId>
{
  template <class ArgumentType>
  inline static LogRecord *Set(LogRecord *log_record, ArgumentType &&arg)
  {
    log_record->SetTraceId(std::forward<ArgumentType>(arg));

    return log_record;
  }
};

template <>
struct LogRecordSetterTrait<trace::TraceFlags>
{
  template <class ArgumentType>
  inline static LogRecord *Set(LogRecord *log_record, ArgumentType &&arg)
  {
    log_record->SetTraceFlags(std::forward<ArgumentType>(arg));

    return log_record;
  }
};

template <>
struct LogRecordSetterTrait<common::SystemTimestamp>
{
  template <class ArgumentType>
  inline static LogRecord *Set(LogRecord *log_record, ArgumentType &&arg)
  {
    log_record->SetTimestamp(std::forward<ArgumentType>(arg));

    return log_record;
  }
};

template <>
struct LogRecordSetterTrait<std::chrono::system_clock::time_point>
{
  template <class ArgumentType>
  inline static LogRecord *Set(LogRecord *log_record, ArgumentType &&arg)
  {
    log_record->SetTimestamp(common::SystemTimestamp(std::forward<ArgumentType>(arg)));

    return log_record;
  }
};

template <>
struct LogRecordSetterTrait<common::KeyValueIterable>
{
  template <class ArgumentType>
  inline static LogRecord *Set(LogRecord *log_record, ArgumentType &&arg)
  {
    arg.ForEachKeyValue(
        [&log_record](nostd::string_view key, common::AttributeValue value) noexcept {
          log_record->SetAttribute(key, value);
          return true;
        });

    return log_record;
  }
};

template <class ValueType>
struct LogRecordSetterTrait
{
  template <class ArgumentType,
            nostd::enable_if_t<std::is_convertible<ArgumentType, nostd::string_view>::value ||
                                   std::is_convertible<ArgumentType, common::AttributeValue>::value,
                               void> * = nullptr>
  inline static LogRecord *Set(LogRecord *log_record, ArgumentType &&arg)
  {
    log_record->SetBody(std::forward<ArgumentType>(arg));

    return log_record;
  }

  template <class ArgumentType,
            nostd::enable_if_t<std::is_base_of<common::KeyValueIterable, ArgumentType>::value, bool>
                * = nullptr>
  inline static LogRecord *Set(LogRecord *log_record, ArgumentType &&arg)
  {
    return LogRecordSetterTrait<common::KeyValueIterable>::template Set(
        log_record, std::forward<ArgumentType>(arg));
  }

  template <class ArgumentType,
            nostd::enable_if_t<common::detail::is_key_value_iterable<ArgumentType>::value, int> * =
                nullptr>
  inline static LogRecord *Set(LogRecord *log_record, ArgumentType &&arg)
  {
    for (auto &argv : arg)
    {
      log_record->SetAttribute(argv.first, argv.second);
    }

    return log_record;
  }
};

}  // namespace detail

}  // namespace logs
OPENTELEMETRY_END_NAMESPACE

#endif
