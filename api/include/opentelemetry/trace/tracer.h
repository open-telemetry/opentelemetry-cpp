#pragma once

#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/unique_ptr.h"
#include "opentelemetry/trace/span.h"

#include <chrono>

namespace opentelemetry
{
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
   */
  virtual nostd::unique_ptr<Span> StartSpan(nostd::string_view name,
                                            const StartSpanOptions &options = {}) noexcept = 0;

  /**
   * Flush any buffered spans.
   * @param timeout to complete the flush
   * @return true if the flush was completed before the timeout
   */
  template <class Rep, class Period>
  void Flush(std::chrono::duration<Rep, Period> timeout) noexcept
  {
    this->FlushWithMicroseconds(
        static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::microseconds>(timeout)));
  }

  virtual void FlushWithMicroseconds(uint64_t timeout) noexcept = 0;

  /**
   * Flush any buffered spans and stop reporting spans.
   * @param timeout to complete the flush
   * @return true if the flush was completed before the timeout
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
}  // namespace opentelemetry
