#pragma once
#include "opentelemetry/context/context.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace trace
{
namespace propagation
{
namespace detail
{

trace::SpanContext GetCurrentSpan(const context::Context& context)
{
  context::ContextValue span = context.GetValue(trace::kSpanKey);
  if (nostd::holds_alternative<nostd::shared_ptr<trace::Span>>(span))
  {
    return nostd::get<nostd::shared_ptr<trace::Span>>(span).get()->GetContext();
  }

  return trace::SpanContext::GetInvalid();
}

}  // namespace detail
}  // namespace propagation
}  // namespace trace
OPENTELEMETRY_END_NAMESPACE
