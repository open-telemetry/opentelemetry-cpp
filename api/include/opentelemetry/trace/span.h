#pragma once

#include <cstdint>

#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/core/timestamp.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/unique_ptr.h"
#include "opentelemetry/trace/canonical_code.h"
#include "opentelemetry/trace/key_value_iterable_view.h"
#include "opentelemetry/trace/span_context.h"
#include "opentelemetry/version.h"

constexpr char SpanKey[] = "span_key";

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
 * StartSpanOptions provides options to set properties of a Span at the time of
 * its creation
 */
struct StartSpanOptions
{
  // Optionally sets the start time of a Span.
  //
  // If the start time of a Span is set, timestamps from both the system clock
  // and steady clock must be provided.
  //
  // Timestamps from the steady clock can be used to most accurately measure a
  // Span's duration, while timestamps from the system clock can be used to most
  // accurately place a Span's
  // time point relative to other Spans collected across a distributed system.
  core::SystemTimestamp start_system_time;
  core::SteadyTimestamp start_steady_time;

  // Explicitely set the parent of a Span.
  //
  // This defaults to an invalid span context. In this case, the Span is
  // automatically parented to the currently active span.
  SpanContext parent = SpanContext::GetInvalid();

  // TODO:
  // SpanContext remote_parent;
  // Links
  SpanKind kind = SpanKind::kInternal;
};
/**
 * StartEndOptions provides options to set properties of a Span when it is
 * ended.
 */
struct EndSpanOptions
{
  // Optionally sets the end time of a Span.
  core::SteadyTimestamp end_steady_time;
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

  // Sets an attribute on the Span. If the Span previously contained a mapping
  // for
  // the key, the old value is replaced.
  virtual void SetAttribute(nostd::string_view key,
                            const common::AttributeValue &value) noexcept = 0;

  // Adds an event to the Span.
  virtual void AddEvent(nostd::string_view name) noexcept = 0;

  // Adds an event to the Span, with a custom timestamp.
  virtual void AddEvent(nostd::string_view name, core::SystemTimestamp timestamp) noexcept = 0;

  // Adds an event to the Span, with a custom timestamp, and attributes.
  virtual void AddEvent(nostd::string_view name,
                        core::SystemTimestamp timestamp,
                        const KeyValueIterable &attributes) noexcept = 0;

  virtual void AddEvent(nostd::string_view name, const KeyValueIterable &attributes) noexcept
  {
    this->AddEvent(name, std::chrono::system_clock::now(), attributes);
  }

  template <class T, nostd::enable_if_t<detail::is_key_value_iterable<T>::value> * = nullptr>
  void AddEvent(nostd::string_view name,
                core::SystemTimestamp timestamp,
                const T &attributes) noexcept
  {
    this->AddEvent(name, timestamp, KeyValueIterableView<T>{attributes});
  }

  template <class T, nostd::enable_if_t<detail::is_key_value_iterable<T>::value> * = nullptr>
  void AddEvent(nostd::string_view name, const T &attributes) noexcept
  {
    this->AddEvent(name, KeyValueIterableView<T>{attributes});
  }

  void AddEvent(nostd::string_view name,
                core::SystemTimestamp timestamp,
                std::initializer_list<std::pair<nostd::string_view, common::AttributeValue>>
                    attributes) noexcept
  {
    this->AddEvent(name, timestamp,
                   nostd::span<const std::pair<nostd::string_view, common::AttributeValue>>{
                       attributes.begin(), attributes.end()});
  }

  void AddEvent(nostd::string_view name,
                std::initializer_list<std::pair<nostd::string_view, common::AttributeValue>>
                    attributes) noexcept
  {
    this->AddEvent(name, std::chrono::system_clock::now(),
                   nostd::span<const std::pair<nostd::string_view, common::AttributeValue>>{
                       attributes.begin(), attributes.end()});
  }

  // Sets the status of the span. The default status is OK. Only the value of
  // the last call will be
  // recorded, and implementations are free to ignore previous calls.
  virtual void SetStatus(CanonicalCode code, nostd::string_view description) noexcept = 0;

  // Updates the name of the Span. If used, this will override the name provided
  // during creation.
  virtual void UpdateName(nostd::string_view name) noexcept = 0;

  /**
   * Mark the end of the Span.
   * Only the timing of the first End call for a given Span will be recorded,
   * and implementations are free to ignore all further calls.
   * @param options can be used to manually define span properties like the end
   * timestamp
   */
  virtual void End(const EndSpanOptions &options = {}) noexcept = 0;

  virtual trace::SpanContext GetContext() const noexcept = 0;

  // Returns true if this Span is recording tracing events (e.g. SetAttribute,
  // AddEvent).
  virtual bool IsRecording() const noexcept = 0;
};
}  // namespace trace
OPENTELEMETRY_END_NAMESPACE
