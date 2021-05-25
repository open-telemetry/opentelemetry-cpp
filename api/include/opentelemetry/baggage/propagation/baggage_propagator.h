#pragma once

#include "opentelemetry/baggage/baggage.h"
#include "opentelemetry/context/propagation/text_map_propagator.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace baggage
{
namespace propagation
{

static const nostd::string_view kBaggageHeader = "baggage";

inline nostd::shared_ptr<baggage::Baggage> GetBaggage(const context::Context &context)
{
  context::ContextValue context_value = context.GetValue(kBaggageHeader);
  if (nostd::holds_alternative<nostd::shared_ptr<baggage::Baggage>>(context_value))
  {
    return nostd::get<nostd::shared_ptr<baggage::Baggage>>(context_value);
  }
  static nostd::shared_ptr<baggage::Baggage> empty_baggage{new baggage::Baggage()};
  return empty_baggage;
}

inline context::Context SetBaggage(context::Context &context,
                                   nostd::shared_ptr<baggage::Baggage> baggage)
{
  return context.SetValue(kBaggageHeader, baggage);
}

class BaggagePropagator : public context::propagation::TextMapPropagator
{
public:
  void Inject(context::propagation::TextMapCarrier &carrier,
              const context::Context &context) noexcept override
  {
    auto baggage = GetBaggage(context);
    carrier.Set(kBaggageHeader, baggage->ToHeader());
  }

  context::Context Extract(const context::propagation::TextMapCarrier &carrier,
                           context::Context &context) noexcept override
  {
    nostd::string_view baggage_str = carrier.Get(kBaggageHeader);
    auto baggage                   = Baggage::FromHeader(baggage_str);
    return SetBaggage(context, baggage);
  }

  bool Fields(nostd::function_ref<bool(nostd::string_view)> callback) const noexcept override
  {
    if (callback(kBaggageHeader))
    {
      return true;
    }
    return false;
  }
};
}  // namespace propagation
}  // namespace baggage
OPENTELEMETRY_END_NAMESPACE