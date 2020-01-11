#pragma once

namespace opentelemetry
{
namespace trace
{
/**
 * StartConfig provides options to set properties of span at the time of starting a new span.
 */
struct StartSpanOptions
{};

class Tracer;

/**
 * A span represents a single operation within a trace.
 */
class Span
{
public:
  virtual ~Span() = default;

  virtual Tracer &tracer() const noexcept = 0;
};
}  // namespace trace
}  // namespace opentelemetry
