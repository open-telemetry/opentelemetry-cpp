// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstdint>

#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/common/key_value_iterable_view.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/type_traits.h"
#include "opentelemetry/trace/span_context.h"
#include "opentelemetry/trace/span_context_kv_iterable.h"
#include "opentelemetry/trace/span_context_kv_iterable_view.h"
#include "opentelemetry/trace/span_metadata.h"

#include "opentelemetry/version.h"

#if OPENTELEMETRY_ABI_VERSION_NO >= 2
#  include <exception>

#  include "opentelemetry/semconv/exception_attributes.h"
#  include "opentelemetry/semconv/exception_events.h"
#endif

OPENTELEMETRY_BEGIN_NAMESPACE
namespace trace
{

class Tracer;

#if OPENTELEMETRY_ABI_VERSION_NO >= 2
namespace detail
{

/**
 * Internal helper for Span::RecordException(): presents `exception.message` (derived from
 * std::exception::what()) followed by the caller-provided attributes, as a single
 * KeyValueIterable suitable for Span::AddEvent().
 *
 * Per the OpenTelemetry specification, caller-provided attributes take precedence: when the
 * caller already supplies an `exception.message`, the default derived from what() is omitted.
 */
class ExceptionAttributes final : public common::KeyValueIterable
{
public:
  ExceptionAttributes(nostd::string_view message,
                      const common::KeyValueIterable &additional) noexcept
      : message_{message},
        additional_{additional},
        caller_has_message_{CallerHasMessage(additional)}
  {}

  bool ForEachKeyValue(nostd::function_ref<bool(nostd::string_view, common::AttributeValue)>
                           callback) const noexcept override
  {
    const bool keep_going = additional_.ForEachKeyValue(callback);
    if (keep_going && !caller_has_message_)
    {
      return callback(semconv::exception::kExceptionMessage, message_);
    }
    return keep_going;
  }

  size_t size() const noexcept override
  {
    return additional_.size() + (caller_has_message_ ? 0 : 1);
  }

private:
  static bool CallerHasMessage(const common::KeyValueIterable &additional) noexcept
  {
    bool found = false;
    additional.ForEachKeyValue([&found](nostd::string_view key, common::AttributeValue) noexcept {
      if (key == semconv::exception::kExceptionMessage)
      {
        found = true;
        return false;
      }
      return true;
    });
    return found;
  }

  nostd::string_view message_;
  const common::KeyValueIterable &additional_;
  bool caller_has_message_;
};
}  // namespace detail
#endif /* OPENTELEMETRY_ABI_VERSION_NO */

/**
 * A Span represents a single operation within a Trace.
 *
 * Span attributes can be provided:
 * - at span creation time, using Tracer::StartSpan(),
 * - during the span lifetime, using Span::SetAttribute()
 *
 * Please note that head samplers,
 * in the SDK (@ref opentelemetry::sdk::trace::Sampler),
 * can only make sampling decisions based on data known
 * at span creation time.
 *
 * When attributes are known early, adding attributes
 * with @ref opentelemetry::trace::Tracer::StartSpan() is preferable.
 *
 * Attributes added or changed with Span::SetAttribute()
 * can not change a sampler decision.
 *
 * Likewise, links can be provided:
 * - at span creation time, using Tracer::StartSpan(),
 * - during the span lifetime, using Span::AddLink() or Span::AddLinks().
 *
 * When links are known early, adding links
 * with @ref opentelemetry::trace::Tracer::StartSpan() is preferable.
 *
 * Links added with Span::AddLink() or Span::AddLinks()
 * can not change a sampler decision.
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
  Span(const Span &)            = delete;
  Span(Span &&)                 = delete;
  Span &operator=(const Span &) = delete;
  Span &operator=(Span &&)      = delete;

  /**
   * Sets an attribute on the Span (ABI).
   *
   * If the Span previously contained a mapping for the key,
   * the old value is replaced.
   *
   * See comments about sampling in @ref opentelemetry::trace::Span
   */
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

#if OPENTELEMETRY_ABI_VERSION_NO >= 2

  /**
   * Add link (ABI).
   *
   * See comments about sampling in @ref opentelemetry::trace::Span
   *
   * @since ABI_VERSION 2
   */
  virtual void AddLink(const SpanContext &target,
                       const common::KeyValueIterable &attrs) noexcept = 0;

  /**
   * Add links (ABI).
   *
   * See comments about sampling in @ref opentelemetry::trace::Span
   *
   * @since ABI_VERSION 2
   */
  virtual void AddLinks(const SpanContextKeyValueIterable &links) noexcept = 0;

  /**
   * Add link (API helper).
   *
   * See comments about sampling in @ref opentelemetry::trace::Span
   *
   * @since ABI_VERSION 2
   */
  template <class U,
            nostd::enable_if_t<common::detail::is_key_value_iterable<U>::value> * = nullptr>
  void AddLink(const SpanContext &target, const U &attrs)
  {
    common::KeyValueIterableView<U> view(attrs);
    this->AddLink(target, view);
  }

  /**
   * Add link (API helper).
   *
   * See comments about sampling in @ref opentelemetry::trace::Span
   *
   * @since ABI_VERSION 2
   */
  void AddLink(const SpanContext &target,
               std::initializer_list<std::pair<nostd::string_view, common::AttributeValue>> attrs)
  {
    /* Build a container from std::initializer_list. */
    nostd::span<const std::pair<nostd::string_view, common::AttributeValue>> container{
        attrs.begin(), attrs.end()};

    /* Build a view on the container. */
    common::KeyValueIterableView<
        nostd::span<const std::pair<nostd::string_view, common::AttributeValue>>>
        view(container);

    return this->AddLink(target, view);
  }

  /**
   * Add links (API helper).
   *
   * See comments about sampling in @ref opentelemetry::trace::Span
   *
   * @since ABI_VERSION 2
   */
  template <class U, nostd::enable_if_t<detail::is_span_context_kv_iterable<U>::value> * = nullptr>
  void AddLinks(const U &links)
  {
    SpanContextKeyValueIterableView<U> view(links);
    this->AddLinks(view);
  }

  /**
   * Add links (API helper).
   *
   * See comments about sampling in @ref opentelemetry::trace::Span
   *
   * @since ABI_VERSION 2
   */
  void AddLinks(
      std::initializer_list<
          std::pair<SpanContext,
                    std::initializer_list<std::pair<nostd::string_view, common::AttributeValue>>>>
          links)
  {
    /* Build a container from std::initializer_list. */
    nostd::span<const std::pair<
        SpanContext, std::initializer_list<std::pair<nostd::string_view, common::AttributeValue>>>>
        container{links.begin(), links.end()};

    /* Build a view on the container. */
    SpanContextKeyValueIterableView<nostd::span<const std::pair<
        SpanContext, std::initializer_list<std::pair<nostd::string_view, common::AttributeValue>>>>>
        view(container);

    return this->AddLinks(view);
  }

  /**
   * Records an exception as an event named "exception" on the Span.
   *
   * `exception.message` is set from `exc.what()`. Additional attributes - such as
   * `exception.type` and `exception.stacktrace`, keyed by the
   * `opentelemetry::semconv::exception::*` constants - may be supplied by the caller and take
   * precedence over the defaults on key collision.
   *
   * @since ABI_VERSION 2
   */
  void RecordException(const std::exception &exc) noexcept { this->RecordException(exc, {}); }

  void RecordException(const std::exception &exc,
                       const common::KeyValueIterable &attributes) noexcept
  {
    detail::ExceptionAttributes exception_attributes{exc.what(), attributes};
    this->AddEvent(semconv::exception::kException, exception_attributes);
  }

  template <class T,
            nostd::enable_if_t<common::detail::is_key_value_iterable<T>::value> * = nullptr>
  void RecordException(const std::exception &exc, const T &attributes) noexcept
  {
    this->RecordException(exc, common::KeyValueIterableView<T>{attributes});
  }

  void RecordException(const std::exception &exc,
                       std::initializer_list<std::pair<nostd::string_view, common::AttributeValue>>
                           attributes) noexcept
  {
    this->RecordException(exc,
                          nostd::span<const std::pair<nostd::string_view, common::AttributeValue>>{
                              attributes.begin(), attributes.end()});
  }

#endif /* OPENTELEMETRY_ABI_VERSION_NO */

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
  virtual void End(const trace::EndSpanOptions &options = {}) noexcept = 0;

  virtual trace::SpanContext GetContext() const noexcept = 0;

  // Returns true if this Span is recording tracing events (e.g. SetAttribute,
  // AddEvent).
  virtual bool IsRecording() const noexcept = 0;
};

}  // namespace trace
OPENTELEMETRY_END_NAMESPACE
