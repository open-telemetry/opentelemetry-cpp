#include "opentelemetry/nostd/list.h"
#include "opentelemetry/nostd/pair.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using opentelemetry::nostd::list;
using opentelemetry::nostd::pair;

// Tests that front() returns the first element and back()
// returns the last element.
TEST(ListTest, FrontBackReturnsProperElements)
{
  list<int> int_list;

  int_list.push_back(9);
  int_list.push_back(8);
  int_list.push_back(7);
  int_list.push_back(6);

  EXPECT_EQ(int_list.front(), 9);
  EXPECT_EQ(int_list.back(), 6);
}

// Tests that the list object can handle data types of a size
// other than an int.
TEST(ListTest, ListAcceptsPairDatatype)
{
  list<pair<int, int>> pair_list;

  pair_list.push_back(pair<int, int>(1, 2));
  pair_list.push_back(pair<int, int>(2, 3));
  pair_list.push_back(pair<int, int>(4, 5));

  EXPECT_EQ(pair_list.front().first(), 1);
  EXPECT_EQ(pair_list.front().second(), 2);
  EXPECT_EQ(pair_list.back().first(), 4);
  EXPECT_EQ(pair_list.back().second(), 5);
}

// Tests that a list object can be initialized with an initalizer list.
TEST(ListTest, ListConstructorAcceptsInitializerList)
{
  list<int> int_list = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  EXPECT_EQ(int_list.front(), 1);
  EXPECT_EQ(int_list.back(), 10);
}

// Tests that the list object can handle an insert_iterator.
TEST(ListTest, InsertIteratorAddsToList)
{
  list<int> int_list    = {1, 2, 3, 4};
  list<int> insert_list = {5, 6, 7, 8};

  std::insert_iterator<list<int>> back(int_list, std::end(int_list));

  for (auto iter = std::begin(insert_list); iter != std::end(insert_list); ++iter)
  {
    back = *iter;
  }

  int counter = 1;

  for (auto iter = std::begin(int_list); iter != std::end(int_list); ++iter)
  {
    EXPECT_EQ(*iter, counter);
    counter++;
  }
}

// Tests that the push_front method added elements to the front of the list.
TEST(ListTest, PushFrontAddsDataAtFront)
{
  list<list<int>> list_list;
  list<int> inner_list = {0, 1, 2, 3, 4};
  list<int> mid_list   = {5, 6, 7, 8, 9};
  list<int> end_list   = {10, 11, 12, 13, 14};

  list_list.push_front(end_list);
  list_list.push_front(mid_list);
  list_list.push_front(inner_list);

  EXPECT_EQ(list_list.front().front(), 0);
  EXPECT_EQ(list_list.front().back(), 4);
  EXPECT_EQ(list_list.back().front(), 10);
  EXPECT_EQ(list_list.back().back(), 14);
}

// Tests that the push_back method added elements at the back of the list.
TEST(ListTest, PushBackAddsDataAtBack)
{
  list<list<int>> list_list;
  list<int> inner_list = {0, 1, 2, 3, 4};
  list<int> mid_list   = {5, 6, 7, 8, 9};
  list<int> end_list   = {10, 11, 12, 13, 14};

  list_list.push_back(inner_list);
  list_list.push_back(mid_list);
  list_list.push_back(end_list);

  EXPECT_EQ(list_list.front().front(), 0);
  EXPECT_EQ(list_list.front().back(), 4);
  EXPECT_EQ(list_list.back().front(), 10);
  EXPECT_EQ(list_list.back().back(), 14);
}

// Tests that you can modify elements via the subscript operator.
TEST(ListTest, SubscriptOperatorAllowsForValueModification)
{
  list<int> int_list = {0, 1, 2, 3, 4};

  EXPECT_EQ(int_list[2], 2);
  int_list[2] = 9;
  EXPECT_EQ(int_list[2], 9);
}

// Tests that the pop_back method returns and removes the back element
// from the list.
TEST(ListTest, PopBackReturnsAndRemovesFromBack)
{
  list<int> int_list = {0, 1, 2, 3, 4};
  int previous_size  = int_list.size();

  EXPECT_EQ(int_list.back(), 4);
  EXPECT_EQ(int_list.pop_back(), 4);
  EXPECT_EQ(int_list.size(), previous_size - 1);
  EXPECT_EQ(int_list.back(), 3);
}

// Tests that the pop_front method returns and removes the front element
// from the list.
TEST(ListTest, PopFrontReturnsAndRemovesFromFront)
{
  list<int> int_list = {0, 1, 2, 3, 4};
  int previous_size  = int_list.size();

  EXPECT_EQ(int_list.front(), 0);
  EXPECT_EQ(int_list.pop_front(), 0);
  EXPECT_EQ(int_list.size(), previous_size - 1);
  EXPECT_EQ(int_list.front(), 1);
}

// Tests that after using the assignment operator, changing the new
// doesn't change the original
TEST(ListTest, AssignmentOperatorDoesNotChangeOriginal)
{

  list<int> first_list      = {0, 1, 2, 3, 4};
  list<int> first_list_copy = {0, 1, 2, 3, 4};
  list<int> second_list     = first_list;

  second_list.pop_back();
  second_list.pop_back();
  second_list.pop_back();

  EXPECT_TRUE(first_list == first_list_copy);
  EXPECT_FALSE(first_list == second_list);
}

// Tests that two lists with the same contents return true when compared
// using the comparison operator
TEST(ListTest, ComparisonOperatureReturnsTrueWithSameContents)
{
  list<int> first_list  = {0, 1, 2, 3, 4};
  list<int> second_list = {0, 1, 2, 3, 4};
  list<int> third_list  = {0, 1, 2, 3, 5};

  EXPECT_TRUE(first_list == second_list);
  EXPECT_FALSE(first_list == third_list);
}

// Tests that constructing a list with one element populates the list object
// with that element
TEST(ListTest, SingleElementConstruction)
{
  list<int> single_list = list<int>(5);

  EXPECT_EQ(single_list[0], 5);
}
