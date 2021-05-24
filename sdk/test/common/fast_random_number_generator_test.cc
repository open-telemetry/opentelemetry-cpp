// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "src/common/random.h"

#include <random>

#include <gtest/gtest.h>

using opentelemetry::sdk::common::FastRandomNumberGenerator;

TEST(FastRandomNumberGeneratorTest, GenerateUniqueNumbers)
{
  std::seed_seq seed_sequence{1, 2, 3};
  FastRandomNumberGenerator random_number_generator;
  random_number_generator.seed(seed_sequence);
  std::set<uint64_t> values;
  for (int i = 0; i < 1000; ++i)
  {
    EXPECT_TRUE(values.insert(random_number_generator()).second);
  }
}
