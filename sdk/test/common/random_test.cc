// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "src/common/random.h"

#include <algorithm>
#include <iterator>

#include <gtest/gtest.h>
using opentelemetry::sdk::common::Random;

TEST(RandomTest, GenerateRandom64)
{
  EXPECT_NE(Random::GenerateRandom64(), Random::GenerateRandom64());
}

TEST(RandomTest, GenerateRandomBuffer)
{
  uint8_t buf1_array[8] = {0};
  uint8_t buf2_array[8] = {0};
  Random::GenerateRandomBuffer(buf1_array);
  Random::GenerateRandomBuffer(buf2_array);
  EXPECT_FALSE(std::equal(std::begin(buf1_array), std::end(buf1_array), std::begin(buf2_array)));

  // Edge cases.
  for (auto size : {7, 8, 9, 16, 17})
  {
    std::vector<uint8_t> buf1_vector(size);
    std::vector<uint8_t> buf2_vector(size);

    Random::GenerateRandomBuffer(buf1_vector);
    Random::GenerateRandomBuffer(buf2_vector);
    EXPECT_FALSE(
        std::equal(std::begin(buf1_vector), std::end(buf1_vector), std::begin(buf2_vector)));
  }
}
