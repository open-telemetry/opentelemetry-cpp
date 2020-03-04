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

  template <class Rep, class Period>
  bool Flush(std::chrono::duration<Rep, Period> timeout) noexcept
  {
    return this->FlushWithMicroseconds(
        static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::microseconds>(timeout)));
  }

  virtual bool FlushWithMicroseconds(uint64_t timeout) noexcept = 0;

  virtual bool CloseWithMicroseconds(uint64_t timeout) noexcept = 0;

  template <class Rep, class Period>
  bool Close(std::chrono::duration<Rep, Period> timeout) noexcept
  {
    return this->CloseWithMicroseconds(
        static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::microseconds>(timeout)));
  }
};
}  // namespace trace
}  // namespace opentelemetry
