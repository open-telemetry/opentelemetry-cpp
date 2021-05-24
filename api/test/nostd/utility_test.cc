// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/nostd/utility.h"

#include <tuple>
#include <type_traits>
#include <vector>

#include <gtest/gtest.h>

template <class T>
auto IsDataCallable(const T &t) -> decltype(opentelemetry::nostd::data(t), std::true_type{});

std::false_type IsDataCallable(...);

template <class T>
auto IsSizeCallable(const T &t) -> decltype(opentelemetry::nostd::size(t), std::true_type{});

std::false_type IsSizeCallable(...);

TEST(UtilityTest, Data)
{
  std::vector<int> v = {1, 2, 3};
  int array[3]       = {1, 2, 3};
  std::initializer_list<int> list{1, 2, 3};
  int x       = 0;
  std::ignore = x;

  EXPECT_EQ(opentelemetry::nostd::data(v), v.data());
  EXPECT_EQ(opentelemetry::nostd::data(array), array);
  EXPECT_EQ(opentelemetry::nostd::data(list), list.begin());
  EXPECT_FALSE(decltype(IsDataCallable(x)){});
}

TEST(UtilityTest, Size)
{
  std::vector<int> v = {1, 2, 3};
  int array[3]       = {1, 2, 3};
  int x              = 0;
  std::ignore        = x;

  EXPECT_EQ(opentelemetry::nostd::size(v), v.size());
  EXPECT_EQ(opentelemetry::nostd::size(array), 3);

  EXPECT_FALSE(decltype(IsSizeCallable(x)){});
}

TEST(UtilityTest, MakeIndexSequence)
{
  EXPECT_TRUE((std::is_same<opentelemetry::nostd::make_index_sequence<0>,
                            opentelemetry::nostd::index_sequence<>>::value));
  EXPECT_TRUE((std::is_same<opentelemetry::nostd::make_index_sequence<1>,
                            opentelemetry::nostd::index_sequence<0>>::value));
  EXPECT_TRUE((std::is_same<opentelemetry::nostd::make_index_sequence<2>,
                            opentelemetry::nostd::index_sequence<0, 1>>::value));
}
