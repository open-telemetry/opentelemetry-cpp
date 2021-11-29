// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/baggage/baggage.h"
#include "opentelemetry/baggage/baggage_context.h"
#include "opentelemetry/context/propagation/text_map_propagator.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace baggage
{
namespace propagation
{

class BaggagePropagator : public opentelemetry::context::propagation::TextMapPropagator
{
public:
  void Inject(opentelemetry::context::propagation::TextMapCarrier &carrier,
              const opentelemetry::context::Context &context) noexcept override
  {
    auto baggage = opentelemetry::baggage::GetBaggage(context);
    carrier.Set(kBaggageHeader, baggage->ToHeader());
  }

  context::Context Extract(const opentelemetry::context::propagation::TextMapCarrier &carrier,
                           opentelemetry::context::Context &context) noexcept override
  {
    nostd::string_view baggage_str = carrier.Get(opentelemetry::baggage::kBaggageHeader);
    auto baggage                   = opentelemetry::baggage::Baggage::FromHeader(baggage_str);
    return opentelemetry::baggage::SetBaggage(context, baggage);
  }

  bool Fields(nostd::function_ref<bool(nostd::string_view)> callback) const noexcept override
  {
    return callback(kBaggageHeader);
  }
};
}  // namespace propagation
}  // namespace baggage
OPENTELEMETRY_END_NAMESPACE
