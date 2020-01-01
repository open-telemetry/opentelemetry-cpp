#include "opentelemetry/nostd/span.h"

#include <array>
#include <type_traits>

#include <gtest/gtest.h>

using opentelemetry::nostd::span;

TEST(SpanTest, DefaultConstruction)
{
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

TEST(SpanTest, PointerCountConstruction)
{
  std::array<int, 3> array = {1, 2, 3};

  span<int> s1{array.data(), array.size()};
  EXPECT_EQ(s1.data(), array.data());
  EXPECT_EQ(s1.size(), array.size());

  span<int, 3> s2{array.data(), array.size()};
  EXPECT_EQ(s2.data(), array.data());
  EXPECT_EQ(s2.size(), array.size());

  EXPECT_DEATH((span<int, 2>{array.data(), array.size()}), ".*");
}

TEST(SpanTest, OtherSpanConstruction)
{
  std::array<int, 3> array = {1, 2, 3};
  span<int> s1{array.data(), array.size()};
  span<int, 3> s2{array.data(), array.size()};

  span<int> s3{s1};
  EXPECT_EQ(s3.data(), array.data());
  EXPECT_EQ(s3.size(), array.size());

  span<int> s4{s2};
  EXPECT_EQ(s4.data(), array.data());
  EXPECT_EQ(s4.size(), array.size());

  span<const int> s5{s1};
  EXPECT_EQ(s5.data(), array.data());
  EXPECT_EQ(s5.size(), array.size());

  EXPECT_FALSE((std::is_constructible<span<int>, span<const int>>::value));
  EXPECT_FALSE((std::is_constructible<span<int>, span<double>>::value));

  span<int, 3> s6{s2};
  EXPECT_EQ(s6.data(), array.data());
  EXPECT_EQ(s6.size(), array.size());

  span<const int, 3> s7{s2};
  EXPECT_EQ(s7.data(), array.data());
  EXPECT_EQ(s7.size(), array.size());

  EXPECT_FALSE((std::is_constructible<span<int, 3>, span<int, 4>>::value));
  EXPECT_FALSE((std::is_constructible<span<int, 3>, span<double, 3>>::value));
}

TEST(SpanTest, BracketOperator)
{
  std::array<int, 2> array = {1, 2};

  span<int> s1{array.data(), array.size()};
  EXPECT_EQ(s1[0], 1);
  EXPECT_EQ(s1[1], 2);

  span<int, 2> s2{array.data(), array.size()};
  EXPECT_EQ(s2[0], 1);
  EXPECT_EQ(s2[1], 2);
}
