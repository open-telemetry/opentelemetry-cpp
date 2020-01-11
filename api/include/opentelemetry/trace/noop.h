#pragma once

#include "opentelemetry/trace/tracer.h"

#include <iostream>
#include <memory>

namespace opentelemetry
{
namespace trace
{
/**
 * Noop implementation of Span.
 */
class NoopSpan final : public Span
{
public:
  explicit NoopSpan(const std::shared_ptr<Tracer>& tracer) noexcept : tracer_{tracer} {}

  // Span
  Tracer &tracer() const noexcept override { return *tracer_; }

private:
  std::shared_ptr<Tracer> tracer_;
};

/**
 * Noop implementation of Tracer
 */
class NoopTracer final : public Tracer, public std::enable_shared_from_this<NoopTracer>
{
public:
  // Tracer
  nostd::unique_ptr<Span> StartSpan(nostd::string_view name,
                                    const StartSpanOptions & /*options*/) noexcept override
  {
    return nostd::unique_ptr<Span>{new (std::nothrow) NoopSpan{this->shared_from_this()}};
  }
};
}  // namespace trace
}  // namespace opentelemetry
