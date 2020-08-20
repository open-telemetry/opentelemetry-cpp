#include "opentelemetry/trace/span_context.h"
#include "opentelemetry/trace/trace_id.h"
#include "opentelemetry/trace/span_id.h"

#include <gtest/gtest.h>

using opentelemetry::trace::SpanContext;

TEST(SpanContextTest, IsSampled)
{
  SpanContext s1(true, true);

  ASSERT_EQ(s1.IsSampled(), true);

  SpanContext s2(false, true);

  ASSERT_EQ(s2.IsSampled(), false);
}

TEST(SpanContextTest, HasRemoteParent)
{
  SpanContext s1(true, true);

  ASSERT_EQ(s1.HasRemoteParent(), true);

  SpanContext s2(true, false);

  ASSERT_EQ(s2.HasRemoteParent(), false);
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
  SpanContext s1(false, false);
  ASSERT_FALSE(s1.IsValid());

  // Test that trace id and span id are invalid
  ASSERT_EQ(s1.trace_id(), opentelemetry::trace::TraceId());
  ASSERT_EQ(s1.span_id(), opentelemetry::trace::SpanId());
}
