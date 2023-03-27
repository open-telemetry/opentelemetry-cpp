// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifdef ENABLE_LOGS_PREVIEW

#  include "opentelemetry/common/attribute_value.h"
#  include "opentelemetry/common/timestamp.h"
#  include "opentelemetry/logs/severity.h"
#  include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace trace
{
class SpanId;
class TraceId;
class TraceFlags;
}  // namespace trace

namespace logs
{
/**
 * Maintains a representation of a log in a format that can be processed by a recorder.
 *
 * This class is thread-compatible.
 */
class LogRecord
{
public:
  virtual ~LogRecord() = default;

  /**
   * Set the timestamp for this log.
   * @param timestamp the timestamp to set
   */
  virtual void SetTimestamp(opentelemetry::common::SystemTimestamp timestamp) noexcept = 0;

  /**
   * Set the observed timestamp for this log.
   * @param timestamp the timestamp to set
   */
  virtual void SetObservedTimestamp(opentelemetry::common::SystemTimestamp timestamp) noexcept = 0;

  /**
   * Set the severity for this log.
   * @param severity the severity of the event
   */
  virtual void SetSeverity(opentelemetry::logs::Severity severity) noexcept = 0;

  /**
   * Set body field for this log.
   * @param message the body to set
   */
  virtual void SetBody(const opentelemetry::common::AttributeValue &message) noexcept = 0;

  /**
   * Set an attribute of a log.
   * @param key the name of the attribute
   * @param value the attribute value
   */
  virtual void SetAttribute(nostd::string_view key,
                            const opentelemetry::common::AttributeValue &value) noexcept = 0;

  /**
   * Set the trace id for this log.
   * @param trace_id the trace id to set
   */
  virtual void SetTraceId(const opentelemetry::trace::TraceId &trace_id) noexcept = 0;

  /**
   * Set the span id for this log.
   * @param span_id the span id to set
   */
  virtual void SetSpanId(const opentelemetry::trace::SpanId &span_id) noexcept = 0;

  /**
   * Inject trace_flags for this log.
   * @param trace_flags the trace flags to set
   */
  virtual void SetTraceFlags(const opentelemetry::trace::TraceFlags &trace_flags) noexcept = 0;
};
}  // namespace logs
OPENTELEMETRY_END_NAMESPACE
#endif
