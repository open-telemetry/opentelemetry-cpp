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

#include "opentelemetry/trace/span_context.h"
#include "opentelemetry/trace/span_id.h"
#include "opentelemetry/trace/trace_id.h"

#include <gtest/gtest.h>

using opentelemetry::trace::SpanContext;

TEST(SpanContextTest, IsSampled)
{
  SpanContext s1(true, true);

  ASSERT_EQ(s1.IsSampled(), true);

  SpanContext s2(false, true);

  ASSERT_EQ(s2.IsSampled(), false);
}

TEST(SpanContextTest, IsRemote)
{
  SpanContext s1(true, true);

  ASSERT_EQ(s1.IsRemote(), true);

  SpanContext s2(true, false);

  ASSERT_EQ(s2.IsRemote(), false);
}

TEST(SpanContextTest, TraceFlags)
{
  SpanContext s1(true, true);

  ASSERT_EQ(s1.trace_flags().flags(), 1);

  SpanContext s2(false, true);

  ASSERT_EQ(s2.trace_flags().flags(), 0);
}

// Test that SpanContext is invalid
TEST(SpanContextTest, Invalid)
{
  SpanContext s1 = SpanContext::GetInvalid();
  EXPECT_FALSE(s1.IsValid());

  // Test that trace id and span id are invalid
  EXPECT_EQ(s1.trace_id(), opentelemetry::trace::TraceId());
  EXPECT_EQ(s1.span_id(), opentelemetry::trace::SpanId());
}
