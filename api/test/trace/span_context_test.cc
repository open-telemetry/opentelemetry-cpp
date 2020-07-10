#include "opentelemetry/trace/span_context.h"

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