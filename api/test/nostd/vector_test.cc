#include "opentelemetry/nostd/vector.h"
#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/nostd/pair.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using opentelemetry::common::AttributeValue;
using opentelemetry::nostd::pair;
using opentelemetry::nostd::vector;

// Tests that front() returns the first element and back()
// returns the last element.
TEST(VectorTest, FrontBackReturnsProperElements)
{
  vector<int> int_vector;

  int_vector.push_back(9);
  int_vector.push_back(8);
  int_vector.push_back(7);
  int_vector.push_back(6);

  EXPECT_EQ(int_vector.front(), 9);
  EXPECT_EQ(int_vector.back(), 6);
}

// Tests that the vector object can handle data types of a size
// other than an int.
TEST(VectorTest, VectorAcceptsPairDatatype)
{
  vector<pair<int, int>> pair_vector;

  pair_vector.push_back(pair<int, int>(1, 2));
  pair_vector.push_back(pair<int, int>(2, 3));
  pair_vector.push_back(pair<int, int>(4, 5));

  EXPECT_EQ(pair_vector.front().first(), 1);
  EXPECT_EQ(pair_vector.front().second(), 2);
  EXPECT_EQ(pair_vector.back().first(), 4);
  EXPECT_EQ(pair_vector.back().second(), 5);
}

// Tests that a vector object can be initialized with an initalizer vector.
TEST(VectorTest, VectorConstructorAcceptsInitializerVector)
{
  vector<int> int_vector = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  EXPECT_EQ(int_vector.front(), 1);
  EXPECT_EQ(int_vector.back(), 10);
}

// Tests that the vector object can handle an insert_iterator.
TEST(VectorTest, InsertIteratorAddsToVector)
{
  vector<int> int_vector    = {1, 2, 3, 4};
  vector<int> insert_vector = {5, 6, 7, 8};

  std::insert_iterator<vector<int>> back(int_vector, std::end(int_vector));

  for (auto iter = std::begin(insert_vector); iter != std::end(insert_vector); ++iter)
  {
    back = *iter;
  }

  int counter = 1;

  for (auto iter = std::begin(int_vector); iter != std::end(int_vector); ++iter)
  {
    EXPECT_EQ(*iter, counter);
    counter++;
  }
}

// Tests that the push_back method added elements at the back of the vector.
TEST(VectorTest, PushBackAddsDataAtBack)
{
  vector<vector<int>> vector_vector;
  vector<int> inner_vector = {0, 1, 2, 3, 4};
  vector<int> mid_vector   = {5, 6, 7, 8, 9};
  vector<int> end_vector   = {10, 11, 12, 13, 14};

  vector_vector.push_back(inner_vector);
  vector_vector.push_back(mid_vector);
  vector_vector.push_back(end_vector);

  EXPECT_EQ(vector_vector.front().front(), 0);
  EXPECT_EQ(vector_vector.front().back(), 4);
  EXPECT_EQ(vector_vector.back().front(), 10);
  EXPECT_EQ(vector_vector.back().back(), 14);
}

// Tests that you can modify elements via the subscript operator.
TEST(VectorTest, SubscriptOperatorAllowsForValueModification)
{
  vector<int> int_vector = {0, 1, 2, 3, 4};

  EXPECT_EQ(int_vector[2], 2);
  int_vector[2] = 9;
  EXPECT_EQ(int_vector[2], 9);
}

// Tests that the pop_back method returns and removes the back element
// from the vector.
TEST(VectorTest, PopBackReturnsAndRemovesFromBack)
{
  vector<int> int_vector = {0, 1, 2, 3, 4};
  int previous_size      = int_vector.size();

  EXPECT_EQ(int_vector.back(), 4);
  EXPECT_EQ(int_vector.pop_back(), 4);
  EXPECT_EQ(int_vector.size(), previous_size - 1);
  EXPECT_EQ(int_vector.back(), 3);
}

// Tests that after using the assignment operator, changing the new
// doesn't change the original
TEST(VectorTest, AssignmentOperatorDoesNotChangeOriginal)
{

  vector<int> first_vector      = {0, 1, 2, 3, 4};
  vector<int> first_vector_copy = {0, 1, 2, 3, 4};
  vector<int> second_vector     = first_vector;

  second_vector.pop_back();
  second_vector.pop_back();
  second_vector.pop_back();

  EXPECT_TRUE(first_vector == first_vector_copy);
  EXPECT_FALSE(first_vector == second_vector);
}

// Tests that two vectors with the same contents return true when compared
// using the comparison operator
TEST(VectorTest, ComparisonOperatureReturnsTrueWithSameContents)
{
  vector<int> first_vector  = {0, 1, 2, 3, 4};
  vector<int> second_vector = {0, 1, 2, 3, 4};
  vector<int> third_vector  = {0, 1, 2, 3, 5};

  EXPECT_TRUE(first_vector == second_vector);
  EXPECT_FALSE(first_vector == third_vector);
}

// Tests that constructing a vector with one element populates the vector object
// with that element
TEST(VectorTest, SingleElementConstruction)
{
  vector<int> single_vector = vector<int>(5);

  EXPECT_EQ(single_vector[0], 5);
}
