#include "opentelemetry/sdk/common/circular_buffer_range.h"

#include <iterator>

#include <gtest/gtest.h>
using opentelemetry::sdk::common::CircularBufferRange;

TEST(CircularBufferRangeTest, ForEach)
{
  int array1[] = {1, 2, 3, 4};
  int array2[] = {5, 6, 7};
  CircularBufferRange<int> range{array1, array2};

  int x = 0;
  range.ForEach([&](int y) {
    EXPECT_EQ(++x, y);
    return true;
  });
  EXPECT_EQ(x, 7);
}

TEST(CircularBufferRangeTest, ForEachWithExit)
{
  int array1[] = {1, 2, 3, 4};
  int array2[] = {5, 6, 7};
  CircularBufferRange<int> range{array1, array2};

  int x = 0;
  range.ForEach([&](int y) {
    EXPECT_EQ(++x, y);
    return false;
  });
  EXPECT_EQ(x, 1);

  x = 0;
  range.ForEach([&](int y) {
    EXPECT_EQ(++x, y);
    return y != 5;
  });
  EXPECT_EQ(x, 5);
}

TEST(CircularBufferRangeTest, Conversion)
{
  int array1[] = {1, 2, 3, 4};
  int array2[] = {5, 6, 7};
  CircularBufferRange<int> range{array1, array2};

  CircularBufferRange<const int> range2{range};
  int x = 0;
  range2.ForEach([&](int y) {
    EXPECT_EQ(++x, y);
    return true;
  });
  EXPECT_EQ(x, 7);
}
