#include "opentelemetry/sdk/trace/tracer_factory.h"

#include <gtest/gtest.h>

using opentelemetry::sdk::trace::TracerFactory;

TEST(TracerFactory, getTracer)
{
  auto tf = new TracerFactory();
  auto t1 = tf->getTracer("test");
  auto t2 = tf->getTracer("test");
  auto t3 = tf->getTracer("different");
  ASSERT_NE(t1, nullptr);
  ASSERT_NE(t2, nullptr);
  ASSERT_NE(t3, nullptr);

  // Should return a new instance each time
  ASSERT_NE(t1, t2);
  ASSERT_NE(t2, t3);
  ASSERT_NE(t1, t3);
}
