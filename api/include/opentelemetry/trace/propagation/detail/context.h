#pragma once
#include "opentelemetry/context/context.h"
#include "opentelemetry/trace/default_span.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace trace
{
namespace propagation
{
namespace detail
{

inline nostd::shared_ptr<trace::Span> GetSpanFromContext(const context::Context &context)
{
  context::ContextValue span = context.GetValue(trace::kSpanKey);
  if (nostd::holds_alternative<nostd::shared_ptr<trace::Span>>(span))
  {
    return nostd::get<nostd::shared_ptr<trace::Span>>(span);
  }
  static nostd::shared_ptr<Span> invalid_span{new DefaultSpan(SpanContext::GetInvalid())};
  return invalid_span;
}

inline context::Context SetSpanToContext(context::Context &context,
                                         nostd::shared_ptr<trace::Span> span)
{

  return context.SetValue(kSpanKey, span);
}

}  // namespace detail
}  // namespace propagation
}  // namespace trace
OPENTELEMETRY_END_NAMESPACE
