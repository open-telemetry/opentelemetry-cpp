#include "opentelemetry/nostd/list.h"
#include "opentelemetry/nostd/pair.h"

#include <gtest/gtest.h>

using opentelemetry::nostd::list;
using opentelemetry::nostd::pair;

/* Tests that front() returns the first element and back() 
 * returns the last element. 
 */
TEST(ListTest, FrontBack)
{
  list<int> list_1;

  list_1.push_back(9);
  list_1.push_back(8);
  list_1.push_back(7);
  list_1.push_back(6);

  EXPECT_EQ(list_1.front(), 9);
  EXPECT_EQ(list_1.back(), 6);

}

/* Tests that the list object can handle data types of a size 
 * other than an int. */
TEST(ListTest, PairList)
{
  list<pair<int,int>> pair_list;
  
  pair_list.push_back(pair<int,int>(1,2));
  pair_list.push_back(pair<int,int>(2,3));
  pair_list.push_back(pair<int,int>(4,5));

  pair<int,int> temp_pair;
  temp_pair = pair_list.front();

  EXPECT_EQ(pair_list.front().first(), 1);
  EXPECT_EQ(pair_list.front().second(), 2);

  EXPECT_EQ(pair_list.back().first(), 4);
  EXPECT_EQ(pair_list.back().second(), 5);
}

/* Tests taht a list object can be initialized with an initalizer list. */
TEST(ListTest, InitializerList)
{

  list<int> pair_list = {1,2,3,4,5,6,7,8,9,10};

  EXPECT_EQ(pair_list.front(), 1);
  EXPECT_EQ(pair_list.back(), 10);
}

/* Tests that the list object can handle an insert_iterator. */
TEST(ListTest, IteratorInsertion)
{

  list<int> int_list = {1,2,3,4};
  list<int> insert_list = {5,6,7,8};

  std::insert_iterator<list<int>> back(int_list, std::end(int_list));
  
  for (auto iter = std::begin(insert_list); iter != std::end(insert_list); ++iter)
  {
    back = *iter;
  }
  
  int counter = 1;  
  for(auto iter = std::begin(int_list); iter != std::end(int_list); ++iter){
    EXPECT_EQ(*iter,counter);
    counter++;
  }
}

/* Tests that the push_front method added elements to the front of the list. */
TEST(ListTest, PushFront){
  list<list<int>> list_list;
  list<int> inner_list = {0,1,2,3,4};
  list<int> mid_list = {9,9,9,9,9};
  list_list.push_front(inner_list);
  list_list.push_back(mid_list);
  list_list.push_front(inner_list);

  EXPECT_EQ(list_list.front().front(),0);
  EXPECT_EQ(list_list.front().back(),4);

  EXPECT_EQ(list_list.back().front(), 9);
  EXPECT_EQ(list_list.back().back(), 9);
}

/* Tests that you can modify elements via the subscript operator. */
TEST(ListTest, SubscriptOperator){
  list<int> int_list = {0,1,2,3,4};
  
  EXPECT_EQ(int_list[2],2);
  int_list[2] = 9;
  EXPECT_EQ(int_list[2],9);
}

/* Tests that the pop_back method, returns and removes the back element
 * from the list. 
 */
TEST(ListTest, PopBack){
  list<int> int_list = {0,1,2,3,4};
  
  int previous_size = int_list.size(); 
  EXPECT_EQ(int_list.back(),4);
  EXPECT_EQ(int_list.pop_back(), 4);
  EXPECT_EQ(int_list.size(), previous_size - 1);
  
  EXPECT_EQ(int_list.back(), 3);
}

/* Tests that the pop_front method, returns and removes the front element
 * from the list. 
 */
TEST(ListTest, PopFront){
  list<int> int_list = {0,1,2,3,4};
  
  int previous_size = int_list.size(); 
  EXPECT_EQ(int_list.front(),0);
  EXPECT_EQ(int_list.pop_front(), 0);
  EXPECT_EQ(int_list.size(), previous_size - 1);
  
  EXPECT_EQ(int_list.front(), 1);
}
