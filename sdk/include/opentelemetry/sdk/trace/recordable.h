#pragma once

#include "opentelemetry/core/timestamp.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/trace/canonical_code.h"
#include "opentelemetry/trace/span_id.h"
#include "opentelemetry/trace/trace_id.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace_api = opentelemetry::trace;
namespace trace
{
/**
 * Maintains a representation of a span in a format that can be processed by a recorder.
 *
 * This class is thread-compatible.
 */
class Recordable
{
public:
  virtual ~Recordable() = default;

  /**
   * Set a trace id for this span.
   * @param trace_id the trace id to set
   */
  virtual void SetTraceId(opentelemetry::trace::TraceId trace_id) noexcept = 0;

  /**
   * Set a span id for this span.
   * @param span_id the span id to set
   */
  virtual void SetSpanId(opentelemetry::trace::SpanId span_id) noexcept = 0;

  /**
   * Set a parent span id for this span.
   * @param parent_span_id the parent span id to set
   */
  virtual void SetParentSpanId(opentelemetry::trace::SpanId parent_span_id) noexcept = 0;

  /**
   * Add an event to a span.
   * @param name the name of the event
   * @param timestamp the timestamp of the event
   */
  virtual void AddEvent(nostd::string_view name, core::SystemTimestamp timestamp) noexcept = 0;

  /**
   * Set the status of the span.
   * @param code the status code
   * @param description a description of the status
   */
  virtual void SetStatus(trace_api::CanonicalCode code,
                         nostd::string_view description) noexcept = 0;

  /**
   * Set the name of the span.
   * @param name the name to set
   */
  virtual void SetName(nostd::string_view name) noexcept = 0;

  /**
   * Set the start time of the span.
   * @param start_time the start time to set
   */
  virtual void SetStartTime(opentelemetry::core::SystemTimestamp start_time) noexcept = 0;

  /**
   * Set the duration of the span.
   * @param duration the duration to set
   */
  virtual void SetDuration(std::chrono::nanoseconds duration) noexcept = 0;
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
