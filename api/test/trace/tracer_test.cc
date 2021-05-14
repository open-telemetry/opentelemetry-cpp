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

#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/trace/noop.h"
#include "opentelemetry/trace/scope.h"

#include <gtest/gtest.h>

namespace trace_api = opentelemetry::trace;
namespace nostd     = opentelemetry::nostd;
namespace context   = opentelemetry::context;

TEST(TracerTest, GetCurrentSpan)
{
  std::unique_ptr<trace_api::Tracer> tracer(new trace_api::NoopTracer());
  nostd::shared_ptr<trace_api::Span> span_first(new trace_api::NoopSpan(nullptr));
  nostd::shared_ptr<trace_api::Span> span_second(new trace_api::NoopSpan(nullptr));

  auto current = tracer->GetCurrentSpan();
  ASSERT_FALSE(current->GetContext().IsValid());

  auto scope_first = tracer->WithActiveSpan(span_first);
  current          = tracer->GetCurrentSpan();
  ASSERT_EQ(current, span_first);

  auto scope_second = tracer->WithActiveSpan(span_second);
  current           = tracer->GetCurrentSpan();
  ASSERT_EQ(current, span_second);

  scope_second.reset(nullptr);
  current = tracer->GetCurrentSpan();
  ASSERT_EQ(current, span_first);

  scope_first.reset(nullptr);
  current = tracer->GetCurrentSpan();
  ASSERT_FALSE(current->GetContext().IsValid());
}
