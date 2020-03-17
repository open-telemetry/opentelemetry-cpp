#pragma once

#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/unique_ptr.h"
#include "opentelemetry/trace/span.h"
#include "opentelemetry/version.h"

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
};
}  // namespace trace
OPENTELEMETRY_END_NAMESPACE
