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
  uint8_t buf1[8] = {0};
  uint8_t buf2[8] = {0};
  Random::GenerateRandomBuffer(buf1);
  Random::GenerateRandomBuffer(buf2);
  EXPECT_FALSE(std::equal(std::begin(buf1), std::end(buf1), std::begin(buf2)));

  // Edge cases.
  for (auto size : {7, 8, 9, 16, 17})
  {
    std::vector<uint8_t> buf1(size);
    std::vector<uint8_t> buf2(size);

    Random::GenerateRandomBuffer(buf1);
    Random::GenerateRandomBuffer(buf2);
    EXPECT_FALSE(std::equal(std::begin(buf1), std::end(buf1), std::begin(buf2)));
  }
}
