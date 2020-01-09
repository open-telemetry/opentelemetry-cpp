#include "opentelemetry/sdk/trace/tracer-factory.h"

#include <gtest/gtest.h>

using opentelemetry::sdk::trace::TracerFactory;

TEST(TracerFactory, get){
  auto tf = new TracerFactory();
  auto t1 = tf->getTracer("test");
  auto t2 = tf->getTracer("test");
  auto t3 = tf->getTracer("different");
  ASSERT_NE(t1, nullptr);
  ASSERT_NE(t2, nullptr);
  ASSERT_NE(t3, nullptr);
  ASSERT_EQ(t1, t1);
  ASSERT_NE(t2, t3);
}
