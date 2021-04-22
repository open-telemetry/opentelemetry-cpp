#pragma once

#include "opentelemetry/trace/propagation/text_map_propagator.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace trace
{
namespace propagation
{

/**
 * No-op implementation TextMapPropagator
 */
class NoOpPropagator : public TextMapPropagator
{
public:
  /** Noop extract function does nothing and returns the input context */
  context::Context Extract(const TextMapCarrier & /*carrier*/,
                           context::Context &context) noexcept override
  {
    return context;
  }

  /** Noop inject function does nothing */
  void Inject(TextMapCarrier & /*carrier*/, const context::Context &context) noexcept override {}
};
}  // namespace propagation
}  // namespace trace
OPENTELEMETRY_END_NAMESPACE