#include "opentelemetry/sdk/trace/default_tracer_provider.h"

#include <gtest/gtest.h>

using opentelemetry::sdk::trace::DefaultTracerProvider;

TEST(TracerProvider, GetTracer)
{
  auto tf = DefaultTracerProvider();
  auto t1 = tf.GetTracer("test");
  auto t2 = tf.GetTracer("test");
  auto t3 = tf.GetTracer("different", "1.0.0");
  ASSERT_NE(nullptr, t1);
  ASSERT_NE(nullptr, t2);
  ASSERT_NE(nullptr, t3);

  // Should return the same instance each time.
  ASSERT_EQ(t1, t2);
  ASSERT_EQ(t1, t3);
}
