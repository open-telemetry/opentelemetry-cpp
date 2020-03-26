#include "src/common/random.h"

#include <gtest/gtest.h>
using opentelemetry::sdk::common::GetRandomNumberGenerator;

TEST(RandomTest, GenerateRandomNumbers) {
  auto& random_number_generator = GetRandomNumberGenerator();
  EXPECT_NE(random_number_generator(), random_number_generator());
}
