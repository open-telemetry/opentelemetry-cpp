#include "opentelemetry/sdk/trace/tracer_provider.h"

#include <gtest/gtest.h>

using opentelemetry::sdk::trace::TracerProvider;

TEST(TracerProvider, GetTracer)
{
  auto tf = new TracerProvider();
  auto t1 = tf->GetTracer("test");
  auto t2 = tf->GetTracer("test");
  auto t3 = tf->GetTracer("different");
  ASSERT_NE(t1, nullptr);
  ASSERT_NE(t2, nullptr);
  ASSERT_NE(t3, nullptr);

  // Should return a new instance each time
  ASSERT_NE(t1, t2);
  ASSERT_NE(t1, t3);
  ASSERT_NE(t2, t3);
}
