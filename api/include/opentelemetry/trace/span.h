#pragma once

#include <cstdint>

#include "opentelemetry/core/timestamp.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/trace/canonical_code.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
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
 */
class Span
{
public:
  // Note that Spans should be created using the Tracer class. Please refer to
  // tracer.h for documentation.
  Span() = default;

  // The Span destructor End()s the Span, if it hasn't been ended already.
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
  virtual void AddEvent(nostd::string_view name) noexcept = 0;

  // Adds an event to the Span, with a custom timestamp.
  virtual void AddEvent(nostd::string_view name, core::SystemTimestamp timestamp) noexcept = 0;

  // TODO
  // Adds an event to the Span, with a custom timestamp, and attributes.
  // virtual void AddEvent(nostd::string_view name, core::SteadyTimestamp
  // timestamp, nostd::span<const std::pair<nostd::string_view name, AttributeValue
  // value>> attributes) noexcept = 0;

  // Sets the status of the span. The default status is OK. Only the value of the last call will be
  // recorded, and implementations are free to ignore previous calls.
  virtual void SetStatus(CanonicalCode code, nostd::string_view description) noexcept = 0;

  // Updates the name of the Span. If used, this will override the name provided
  // during creation.
  virtual void UpdateName(nostd::string_view name) noexcept = 0;

  // Mark the end of the Span. Only the timing of the first End call for a given Span will
  // be recorded, and implementations are free to ignore all further calls.
  virtual void End() noexcept = 0;

  // TODO
  // virtual void End(EndSpanOptions&& opts) noexcept = 0;

  // TODO
  // SpanContext context() const noexcept = 0;

  // Returns true if this Span is recording tracing events (e.g. SetAttribute,
  // AddEvent).
  virtual bool IsRecording() const noexcept = 0;

  virtual Tracer &tracer() const noexcept = 0;
};
}  // namespace trace
OPENTELEMETRY_END_NAMESPACE
