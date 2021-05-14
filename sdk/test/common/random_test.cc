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
