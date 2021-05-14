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

#include "opentelemetry/trace/scope.h"
#include "opentelemetry/context/context.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/trace/noop.h"

#include <gtest/gtest.h>

using opentelemetry::trace::kSpanKey;
using opentelemetry::trace::NoopSpan;
using opentelemetry::trace::Scope;
using opentelemetry::trace::Span;
namespace nostd   = opentelemetry::nostd;
namespace context = opentelemetry::context;

TEST(ScopeTest, Construct)
{
  nostd::shared_ptr<Span> span(new NoopSpan(nullptr));
  Scope scope(span);

  context::ContextValue active_span_value = context::RuntimeContext::GetValue(kSpanKey);
  ASSERT_TRUE(nostd::holds_alternative<nostd::shared_ptr<Span>>(active_span_value));

  auto active_span = nostd::get<nostd::shared_ptr<Span>>(active_span_value);
  ASSERT_EQ(active_span, span);
}

TEST(ScopeTest, Destruct)
{
  nostd::shared_ptr<Span> span(new NoopSpan(nullptr));
  Scope scope(span);

  {
    nostd::shared_ptr<Span> span_nested(new NoopSpan(nullptr));
    Scope scope_nested(span_nested);

    context::ContextValue active_span_value = context::RuntimeContext::GetValue(kSpanKey);
    ASSERT_TRUE(nostd::holds_alternative<nostd::shared_ptr<Span>>(active_span_value));

    auto active_span = nostd::get<nostd::shared_ptr<Span>>(active_span_value);
    ASSERT_EQ(active_span, span_nested);
  }

  context::ContextValue active_span_value = context::RuntimeContext::GetValue(kSpanKey);
  ASSERT_TRUE(nostd::holds_alternative<nostd::shared_ptr<Span>>(active_span_value));

  auto active_span = nostd::get<nostd::shared_ptr<Span>>(active_span_value);
  ASSERT_EQ(active_span, span);
}
