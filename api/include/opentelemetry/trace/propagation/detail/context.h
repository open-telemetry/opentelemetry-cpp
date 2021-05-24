// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#include "opentelemetry/context/context.h"
#include "opentelemetry/trace/default_span.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace trace
{
namespace propagation
{

inline nostd::shared_ptr<trace::Span> GetSpan(const context::Context &context)
{
  context::ContextValue span = context.GetValue(trace::kSpanKey);
  if (nostd::holds_alternative<nostd::shared_ptr<trace::Span>>(span))
  {
    return nostd::get<nostd::shared_ptr<trace::Span>>(span);
  }
  return nostd::shared_ptr<trace::Span>(new DefaultSpan(SpanContext::GetInvalid()));
}

inline context::Context SetSpan(context::Context &context, nostd::shared_ptr<trace::Span> span)
{

  return context.SetValue(kSpanKey, span);
}

}  // namespace propagation
}  // namespace trace
OPENTELEMETRY_END_NAMESPACE
