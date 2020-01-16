#pragma once

#include <memory>

#include "opentelemetry/trace/span.h"

using opentelemetry::trace::Tracer;

namespace opentelemetry
{
namespace sdk
{
namespace trace
{
class Span final : public opentelemetry::trace::Span
{
public:
  explicit Span(const std::shared_ptr<Tracer> &tracer) noexcept : tracer_{tracer} {}
  Tracer &tracer() const noexcept override;

private:
  std::shared_ptr<Tracer> tracer_;
};
}  // namespace trace
}  // namespace sdk
}  // namespace opentelemetry
