#pragma once

#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/unique_ptr.h"
#include "opentelemetry/trace/span.h"
#include "opentelemetry/version.h"

#include <chrono>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace trace
{
/**
 * Handles span creation and in-process context propagation.
 *
 * This class provides methods for manipulating the context, creating spans, and controlling spans'
 * lifecycles.
 */
class Tracer
{
public:
  virtual ~Tracer() = default;
  /**
   * Starts a span.
   *
   * Optionally sets attributes at Span creation from the given key/value pairs.
   *
   * Attributes will be processed in order, previous attributes with the same
   * key will be overwritten.
   */
  virtual nostd::unique_ptr<Span> StartSpan(nostd::string_view name,
                                            const KeyValueIterable &attributes,
                                            const StartSpanOptions &options = {}) noexcept = 0;

  nostd::unique_ptr<Span> StartSpan(nostd::string_view name,
                                    const StartSpanOptions &options = {}) noexcept
  {
    return this->StartSpan(name, {}, options);
  }

  template <class T, nostd::enable_if_t<detail::is_key_value_iterable<T>::value> * = nullptr>
  nostd::unique_ptr<Span> StartSpan(nostd::string_view name,
                                    const T &attributes,
                                    const StartSpanOptions &options = {}) noexcept
  {
    return this->StartSpan(name, KeyValueIterableView<T>(attributes), options);
  }

  nostd::unique_ptr<Span> StartSpan(
      nostd::string_view name,
      std::initializer_list<std::pair<nostd::string_view, common::AttributeValue>> attributes,
      const StartSpanOptions &options = {}) noexcept
  {
    return this->StartSpan(name,
                           nostd::span<const std::pair<nostd::string_view, common::AttributeValue>>{
                               attributes.begin(), attributes.end()},
                           options);
  }

  /**
   * Force any buffered spans to flush.
   * @param timeout to complete the flush
   */
  template <class Rep, class Period>
  void ForceFlush(std::chrono::duration<Rep, Period> timeout) noexcept
  {
    this->ForceFlushWithMicroseconds(
        static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::microseconds>(timeout)));
  }

  virtual void ForceFlushWithMicroseconds(uint64_t timeout) noexcept = 0;

  /**
   * ForceFlush any buffered spans and stop reporting spans.
   * @param timeout to complete the flush
   */
  template <class Rep, class Period>
  void Close(std::chrono::duration<Rep, Period> timeout) noexcept
  {
    this->CloseWithMicroseconds(
        static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::microseconds>(timeout)));
  }

  virtual void CloseWithMicroseconds(uint64_t timeout) noexcept = 0;
};
}  // namespace trace
OPENTELEMETRY_END_NAMESPACE
