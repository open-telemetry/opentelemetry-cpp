// Copyright 2021, OpenTelemetry Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
