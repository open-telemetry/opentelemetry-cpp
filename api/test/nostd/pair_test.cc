#include "opentelemetry/nostd/pair.h"
#include "opentelemetry/nostd/list.h"

#include <string>

#include <gtest/gtest.h>

using opentelemetry::nostd::list;
using opentelemetry::nostd::pair;

// Tests constructing a pair object.
TEST(PairTest, BasicConstruction)
{
  int val_1                = 1;
  int val_2                = 9;
  pair<int, int> temp_pair = pair<int, int>(val_1, val_2);

  EXPECT_EQ(temp_pair.first(), val_1);
  EXPECT_EQ(temp_pair.second(), val_2);
}

// Tests that the comparison operator returns true when pair values
//  are the same and returns false when different.
TEST(PairTest, ComparisonSameValuesReturnTrue)
{

  int val_1             = 1;
  int val_2             = 2;
  pair<int, int> pair_1 = pair<int, int>(val_1, val_2);
  pair<int, int> pair_2 = pair<int, int>(val_1, val_2);

  EXPECT_EQ(pair_1 == pair_2, true);

  pair_1.first() = 8;

  EXPECT_EQ(pair_1 == pair_2, false);
}

// Tests that the assignment operator sets the values as one would expect.
TEST(PairTest, AssignmentBasic)
{

  int val_1             = 1;
  int val_2             = 2;
  pair<int, int> pair_1 = pair<int, int>(val_1, val_2);

  int val_3             = 3;
  int val_4             = 4;
  pair<int, int> pair_2 = pair<int, int>(val_3, val_4);

  // EXPECT_EQ(pair_1 == pair_2, false);
  EXPECT_EQ(pair_1, pair_2);

  pair_1 = pair_2;

  EXPECT_EQ(pair_1 == pair_2, true);
  EXPECT_EQ(pair_1.first(), val_3);
  EXPECT_EQ(pair_1.second(), val_4);
}

// Tests that after using the assignment operator, changing values of one
// object does not change both.
TEST(PairTest, AssignmentOtherObjectImmutable)
{

  int val_1             = 1;
  int val_2             = 2;
  pair<int, int> pair_1 = pair<int, int>(val_1, val_2);

  int val_3             = 3;
  int val_4             = 4;
  pair<int, int> pair_2 = pair<int, int>(val_3, val_4);

  pair_1 = pair_2;

  pair_1.first()  = 8;
  pair_1.second() = 7;

  EXPECT_EQ(pair_1.first(), 8);
  EXPECT_EQ(pair_1.second(), 7);
  EXPECT_EQ(pair_2.first(), val_3);
  EXPECT_EQ(pair_2.second(), val_4);
}

// Tests that the pair class can accept containers
TEST(PairTest, PairAcceptContainers)
{
  list<int> int_list      = {1, 2, 3, 4};
  std::string test_string = "test";

  pair<list<int>, std::string> container_pair = pair<list<int>, std::string>(int_list, test_string);

  EXPECT_EQ(container_pair.second(), test_string);
  EXPECT_EQ(container_pair.first().front(), int_list.front());
  EXPECT_EQ(container_pair.first().back(), int_list.back());
}
