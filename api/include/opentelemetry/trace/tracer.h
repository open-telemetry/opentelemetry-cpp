#pragma once

#include <chrono>

#include "opentelemetry/version.h"

#include "opentelemetry/common/stltypes.h"
#include "opentelemetry/trace/span.h"

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
   */
  virtual nostd::unique_ptr<Span> StartSpan(nostd::string_view name,
                                            const StartSpanOptions &options = {}) noexcept = 0;

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

  void Close() noexcept
  {
    /* TODO: respect timeout from TracerOptions? */
    CloseWithMicroseconds(0);
  }

  virtual void CloseWithMicroseconds(uint64_t timeout) noexcept = 0;
};
}  // namespace trace
OPENTELEMETRY_END_NAMESPACE
