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
template <typename T>
class NoOpPropagator : public TextMapPropagator<T>
{
public:
  using Getter = nostd::string_view (*)(const T &carrier, nostd::string_view trace_type);

  using Setter = void (*)(T &carrier,
                          nostd::string_view trace_type,
                          nostd::string_view trace_description);

  /** Noop extract function does nothing and returns the input context */
  virtual context::Context Extract(Getter get_from_carrier,
                                   const T &carrier,
                                   context::Context &context) noexcept override
  {
    return context;
  }

  /** Noop inject function does nothing */
  virtual void Inject(Setter set_from_carrier,
                      T &carrier,
                      const context::Context &context) noexcept override
  {}
};
}  // namespace propagation
}  // namespace trace
OPENTELEMETRY_END_NAMESPACE