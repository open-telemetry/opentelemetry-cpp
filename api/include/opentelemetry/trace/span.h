#pragma once

#include <cstdint>

#include "opentelemetry/core/timestamp.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/trace/canonical_code.h"

namespace opentelemetry
{
namespace trace
{
enum class SpanKind
{
  kInternal,
  kServer,
  kClient,
  kProducer,
  kConsumer,
};
/**
 * StartSpanOptions provides options to set properties of a Span at the time of its creation
 */
struct StartSpanOptions
{
  // Optionally sets the start time of a Span.
  //
  // If the start time of a Span is set, timestamps from both the system clock and steady clock
  // must be provided.
  //
  // Timestamps from the steady clock can be used to most accurately measure a Span's
  // duration, while timestamps from the system clock can be used to most accurately place a Span's
  // time point relative to other Spans collected across a distributed system.
  core::SystemTimestamp start_system_time;
  core::SteadyTimestamp start_steady_time;

  // TODO:
  // Span(Context?) parent;
  // SpanContext remote_parent;
  // Links
  // Attributes
  SpanKind kind = SpanKind::kInternal;
};

class Tracer;

/**
 * A Span represents a single operation within a Trace.
 *
 * Spans *must* be ended by calling End(). Merely deleting a Span does not end
 * it.
 */
class Span
{
public:
  // Note that Spans should be created using the Tracer class. Please refer to
  // tracer.h for documentation.
  Span()          = default;
  virtual ~Span() = default;

  // Not copiable or movable.
  Span(const Span &) = delete;
  Span(Span &&)      = delete;
  Span &operator=(const Span &) = delete;
  Span &operator=(Span &&) = delete;

  // TODO
  // Sets an attribute on the Span. If the Span previously contained a mapping for
  // the key, the old value is replaced.
  //
  // If an empty string is used as the value, the attribute will be silently
  // dropped. Note: this behavior could change in the future.
  // virtual void SetAttribute(nostd::string_view key, AttributeValue&& value) = 0;

  // Adds an event to the Span.
  virtual void AddEvent(nostd::string_view name) = 0;

  // Adds an event to the Span, with a custom timestamp.
  virtual void AddEvent(nostd::string_view name, core::SystemTimestamp timestamp) = 0;
  virtual void AddEvent(nostd::string_view name, core::SteadyTimestamp timestamp) = 0;

  // TODO
  // Adds an event to the Span, with a custom timestamp, and attributes.
  // virtual void AddEvent(nostd::string_view name, core::SteadyTimestamp
  // timestamp, nostd::span<std::pair<nostd::string_view name, AttributeValue&&
  // value>> attributes);

  // Sets the status of the span. The default status is OK. Only the value of the last call will be
  // recorded, and implementations are free to ignore previous calls.
  virtual void SetStatus(CanonicalCode code, nostd::string_view description) = 0;

  // Updates the name of the Span. If used, this will override the name provided
  // during construction.
  virtual void UpdateName(nostd::string_view name) = 0;

  // Mark the end of the Span. Only the timing of the first end call for a given {@code Span} will
  // be recorded, and implementations are free to ignore all further calls.
  virtual void End() = 0;

  // TODO
  // virtual void End(EndSpanOptions&& opts) = 0;

  // TODO
  // SpanContext GetContext() const = 0;

  // Returns true if this Span is recording tracing events (e.g. SetAttribute,
  // AddEvent).
  virtual bool IsRecording() const = 0;

  virtual Tracer &tracer() const noexcept = 0;
};
}  // namespace trace
}  // namespace opentelemetry
