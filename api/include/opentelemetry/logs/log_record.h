// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/common/timestamp.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace trace
{
class SpanId;
class TraceId;
class TraceFlags;
}  // namespace trace

namespace logs
{

enum class Severity : uint8_t;

/**
 * Maintains a representation of a log in a format that can be processed by a recorder.
 *
 * This class is thread-compatible.
 */
class LogRecord
{
public:
  LogRecord()                                 = default;
  LogRecord(const LogRecord &)                = default;
  LogRecord(LogRecord &&) noexcept            = default;
  LogRecord &operator=(const LogRecord &)     = default;
  LogRecord &operator=(LogRecord &&) noexcept = default;
  virtual ~LogRecord()                        = default;

  /**
   * Set the timestamp for this log.
   * @param timestamp the timestamp to set
   */
  virtual void SetTimestamp(common::SystemTimestamp timestamp) noexcept = 0;

  /**
   * Set the observed timestamp for this log.
   * @param timestamp the timestamp to set
   */
  virtual void SetObservedTimestamp(common::SystemTimestamp timestamp) noexcept = 0;

  /**
   * Set the severity for this log.
   * @param severity the severity of the event
   */
  virtual void SetSeverity(logs::Severity severity) noexcept = 0;

  /**
   * Set body field for this log.
   * @param message the body to set
   */
  virtual void SetBody(const common::AttributeValue &message) noexcept = 0;

  /**
   * Set an attribute of a log.
   * @param key the name of the attribute
   * @param value the attribute value
   */
  virtual void SetAttribute(nostd::string_view key,
                            const common::AttributeValue &value) noexcept = 0;

  /**
   * Set the Event Id.
   * @param id The event id to set
   * @param name Optional event name to set
   */
  // TODO: mark this as pure virtual once all exporters have been updated
  virtual void SetEventId(int64_t id, nostd::string_view name = {}) noexcept = 0;

  /**
   * Set the trace id for this log.
   * @param trace_id the trace id to set
   */
  virtual void SetTraceId(const trace::TraceId &trace_id) noexcept = 0;

  /**
   * Set the span id for this log.
   * @param span_id the span id to set
   */
  virtual void SetSpanId(const trace::SpanId &span_id) noexcept = 0;

  /**
   * Inject trace_flags for this log.
   * @param trace_flags the trace flags to set
   */
  virtual void SetTraceFlags(const trace::TraceFlags &trace_flags) noexcept = 0;

  /**
   * Whether this LogRecord is also an opentelemetry::sdk::logs::Recordable. The SDK's
   * Logger::EmitLogRecord() accepts a LogRecord and needs to downcast it to the SDK's own
   * Recordable to attach resource/scope and hand it to a processor, but a caller (or another
   * SDK/wrapper built on the API) can supply a LogRecord implementation that is not a
   * Recordable, and this project supports building with RTTI disabled, so a checked
   * dynamic_cast is not available as a guard. Overriding this to return true is how a
   * Recordable implementation asserts that such a downcast is safe. Do not override this in
   * any type that is not actually a Recordable.
   */
  virtual bool IsRecordable() const noexcept { return false; }
};
}  // namespace logs
OPENTELEMETRY_END_NAMESPACE
