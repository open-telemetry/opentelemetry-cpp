// Copyright 2021, OpenTelemetry Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
