// Copyright 2021, OpenTelemetry Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <cstdint>

#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/common/key_value_iterable_view.h"
#include "opentelemetry/common/timestamp.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/type_traits.h"
#include "opentelemetry/nostd/unique_ptr.h"
#include "opentelemetry/trace/canonical_code.h"
#include "opentelemetry/trace/span_context.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace trace
{

// The key identifies the active span in the current context.
constexpr char kSpanKey[] = "active_span";

enum class SpanKind
{
  kInternal,
  kServer,
  kClient,
  kProducer,
  kConsumer,
};

// StatusCode - Represents the canonical set of status codes of a finished Span.

enum class StatusCode
{
  kUnset,  // default status
  kOk,     // Operation has completed successfully.
  kError   // The operation contains an error
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
  common::SystemTimestamp start_system_time;
  common::SteadyTimestamp start_steady_time;

  // Explicitly set the parent of a Span.
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
  common::SteadyTimestamp end_steady_time;
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
  virtual void AddEvent(nostd::string_view name, common::SystemTimestamp timestamp) noexcept = 0;

  // Adds an event to the Span, with a custom timestamp, and attributes.
  virtual void AddEvent(nostd::string_view name,
                        common::SystemTimestamp timestamp,
                        const common::KeyValueIterable &attributes) noexcept = 0;

  virtual void AddEvent(nostd::string_view name,
                        const common::KeyValueIterable &attributes) noexcept
  {
    this->AddEvent(name, std::chrono::system_clock::now(), attributes);
  }

  template <class T,
            nostd::enable_if_t<common::detail::is_key_value_iterable<T>::value> * = nullptr>
  void AddEvent(nostd::string_view name,
                common::SystemTimestamp timestamp,
                const T &attributes) noexcept
  {
    this->AddEvent(name, timestamp, common::KeyValueIterableView<T>{attributes});
  }

  template <class T,
            nostd::enable_if_t<common::detail::is_key_value_iterable<T>::value> * = nullptr>
  void AddEvent(nostd::string_view name, const T &attributes) noexcept
  {
    this->AddEvent(name, common::KeyValueIterableView<T>{attributes});
  }

  void AddEvent(nostd::string_view name,
                common::SystemTimestamp timestamp,
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

  // Sets the status of the span. The default status is Unset. Only the value of
  // the last call will be
  // recorded, and implementations are free to ignore previous calls.
  virtual void SetStatus(StatusCode code, nostd::string_view description = "") noexcept = 0;

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

template <class SpanType, class TracerType>
nostd::shared_ptr<trace::Span> to_span_ptr(TracerType *objPtr,
                                           nostd::string_view name,
                                           const trace::StartSpanOptions &options)
{
  return nostd::shared_ptr<trace::Span>{new (std::nothrow) SpanType{*objPtr, name, options}};
}

}  // namespace trace
OPENTELEMETRY_END_NAMESPACE
