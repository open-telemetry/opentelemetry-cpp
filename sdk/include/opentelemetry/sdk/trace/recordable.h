#pragma once

#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/core/timestamp.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/trace/canonical_code.h"
#include "opentelemetry/trace/key_value_iterable.h"
#include "opentelemetry/trace/key_value_iterable_view.h"
#include "opentelemetry/trace/span_context.h"
#include "opentelemetry/trace/span_id.h"
#include "opentelemetry/trace/trace_id.h"
#include "opentelemetry/version.h"

#include <map>

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
   * Set a trace id, span id and parent span id for this span.
   * @param trace_id the trace id to set
   * @param span_id the span id to set
   * @param parent_span_id the parent span id to set
   */
  virtual void SetIds(opentelemetry::trace::TraceId trace_id,
                      opentelemetry::trace::SpanId span_id,
                      opentelemetry::trace::SpanId parent_span_id) noexcept = 0;

  /**
   * Set an attribute of a span.
   * @param name the name of the attribute
   * @param value the attribute value
   */
  virtual void SetAttribute(nostd::string_view key,
                            const opentelemetry::common::AttributeValue &value) noexcept = 0;

  /**
   * Add an event to a span.
   * @param name the name of the event
   * @param timestamp the timestamp of the event
   * @param attributes the attributes associated with the event
   */
  virtual void AddEvent(
      nostd::string_view name,
      core::SystemTimestamp timestamp = core::SystemTimestamp(std::chrono::system_clock::now()),
      const trace_api::KeyValueIterable &attributes =
          trace_api::KeyValueIterableView<std::map<std::string, int>>({})) noexcept = 0;

  /**
   * Add a link to a span.
   * @param span_context the span context of the linked span
   * @param attributes the attributes associated with the link
   */
  virtual void AddLink(
      opentelemetry::trace::SpanContext span_context,
      const trace_api::KeyValueIterable &attributes =
          trace_api::KeyValueIterableView<std::map<std::string, int>>({})) noexcept = 0;

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
