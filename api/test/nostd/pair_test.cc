#include "opentelemetry/nostd/pair.h"

#include <gtest/gtest.h>

using opentelemetry::nostd::pair;

/* Tests constructing a pair object. */
TEST(PairTest, BasicConstruction)
{
  int val_1                = 1;
  int val_2                = 9;
  pair<int, int> temp_pair = pair<int, int>(val_1, val_2);

  EXPECT_EQ(temp_pair.first(), val_1);
  EXPECT_EQ(temp_pair.second(), val_2);
}
