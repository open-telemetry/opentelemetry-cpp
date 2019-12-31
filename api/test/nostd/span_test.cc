#include "opentelemetry/nostd/span.h"

#include <gtest/gtest.h>

using opentelemetry::nostd::span;

TEST(SpanTest, DefaultConstruction) {
  span<int> s;
  EXPECT_EQ(s.data(), nullptr);
  EXPECT_EQ(s.size(), 0);
  EXPECT_TRUE(s.empty());
}
