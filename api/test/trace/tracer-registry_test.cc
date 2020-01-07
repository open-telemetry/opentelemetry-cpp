#include "opentelemetry/trace/tracer-registry.h"

#include <gtest/gtest.h>

using opentelemetry::trace::TracerRegistry;

TEST(TracerRegistry, Constructor){
  auto t = TracerRegistry::getInstance();
  ASSERT_NE(t, nullptr);
}

TEST(TracerRegistry, get){
  auto tf = TracerRegistry::getInstance();
  auto t1 = tf->get("test");
  auto t2 = tf->get("test");
  auto t3 = tf->get("different");
  ASSERT_NE(t1, nullptr);
  ASSERT_NE(t2, nullptr);
  ASSERT_NE(t3, nullptr);
  ASSERT_EQ(t1, t1);
  ASSERT_NE(t2, t3);
}
