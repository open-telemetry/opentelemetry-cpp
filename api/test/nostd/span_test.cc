#include "opentelemetry/nostd/span.h"

#include <array>
#include <type_traits>

#include <gtest/gtest.h>

using opentelemetry::nostd::span;

TEST(SpanTest, DefaultConstruction) {
  span<int> s1;
  EXPECT_EQ(s1.data(), nullptr);
  EXPECT_EQ(s1.size(), 0);
  EXPECT_TRUE(s1.empty());

  span<int, 0> s2;
  EXPECT_EQ(s2.data(), nullptr);
  EXPECT_EQ(s2.size(), 0);
  EXPECT_TRUE(s2.empty());

  EXPECT_FALSE((std::is_default_constructible<span<int, 1>>::value));
}

TEST(SpanTest, PointerCountConstruction) {
  std::array<int, 3> array = {1, 2, 3};

  span<int> s1(array.data(), array.size());
  EXPECT_EQ(s1.data(), array.data());
  EXPECT_EQ(s1.size(), array.size());

  span<int, 3> s2(array.data(), array.size());
  EXPECT_EQ(s2.data(), array.data());
  EXPECT_EQ(s2.size(), array.size());
}
