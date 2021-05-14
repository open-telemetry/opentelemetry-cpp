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

#include "opentelemetry/nostd/unique_ptr.h"

#include <gtest/gtest.h>

using opentelemetry::nostd::unique_ptr;

class A
{
public:
  explicit A(bool &destructed) noexcept : destructed_{destructed} { destructed_ = false; }

  ~A() { destructed_ = true; }

private:
  bool &destructed_;
};

class B
{
public:
  int f() const { return 123; }
};

TEST(UniquePtrTest, DefaultConstruction)
{
  unique_ptr<int> ptr1;
  EXPECT_EQ(ptr1.get(), nullptr);

  unique_ptr<int> ptr2{nullptr};
  EXPECT_EQ(ptr2.get(), nullptr);
}

TEST(UniquePtrTest, ExplicitConstruction)
{
  auto value = new int{123};
  unique_ptr<int> ptr1{value};
  EXPECT_EQ(ptr1.get(), value);

  auto array = new int[5];
  unique_ptr<int[]> ptr2{array};
  EXPECT_EQ(ptr2.get(), array);
}

TEST(UniquePtrTest, MoveConstruction)
{
  auto value = new int{123};
  unique_ptr<int> ptr1{value};
  unique_ptr<int> ptr2{std::move(ptr1)};
  EXPECT_EQ(ptr1.get(), nullptr);
  EXPECT_EQ(ptr2.get(), value);
}

TEST(UniquePtrTest, MoveConstructionFromDifferentType)
{
  auto value = new int{123};
  unique_ptr<int> ptr1{value};
  unique_ptr<const int> ptr2{std::move(ptr1)};
  EXPECT_EQ(ptr1.get(), nullptr);
  EXPECT_EQ(ptr2.get(), value);
}

TEST(UniquePtrTest, MoveConstructionFromStdUniquePtr)
{
  auto value = new int{123};
  std::unique_ptr<int> ptr1{value};
  unique_ptr<int> ptr2{std::move(ptr1)};
  EXPECT_EQ(ptr1.get(), nullptr);
  EXPECT_EQ(ptr2.get(), value);
}

TEST(UniquePtrTest, Destruction)
{
  bool was_destructed;
  unique_ptr<A>{new A{was_destructed}};
  EXPECT_TRUE(was_destructed);
}

TEST(UniquePtrTest, StdUniquePtrConversionOperator)
{
  auto value = new int{123};
  unique_ptr<int> ptr1{value};
  std::unique_ptr<int> ptr2{std::move(ptr1)};
  EXPECT_EQ(ptr1.get(), nullptr);
  EXPECT_EQ(ptr2.get(), value);

  value = new int{456};
  ptr1  = unique_ptr<int>{value};
  ptr2  = std::move(ptr1);
  EXPECT_EQ(ptr1.get(), nullptr);
  EXPECT_EQ(ptr2.get(), value);

  ptr2 = nullptr;
  EXPECT_EQ(ptr2.get(), nullptr);

  EXPECT_TRUE((std::is_assignable<std::unique_ptr<int>, unique_ptr<int> &&>::value));
  EXPECT_FALSE((std::is_assignable<std::unique_ptr<int>, unique_ptr<int> &>::value));
}

TEST(UniquePtrTest, BoolConversionOpertor)
{
  auto value = new int{123};
  unique_ptr<int> ptr1{value};

  EXPECT_TRUE(ptr1);
  EXPECT_FALSE(unique_ptr<int>{});
}

TEST(UniquePtrTest, PointerOperators)
{
  auto value = new int{123};
  unique_ptr<int> ptr1{value};

  EXPECT_EQ(&*ptr1, value);
  EXPECT_EQ(
      unique_ptr<B> { new B }->f(), 123);
}

TEST(UniquePtrTest, Reset)
{
  bool was_destructed1;
  unique_ptr<A> ptr{new A{was_destructed1}};
  bool was_destructed2;
  ptr.reset(new A{was_destructed2});
  EXPECT_TRUE(was_destructed1);
  EXPECT_FALSE(was_destructed2);
  ptr.reset();
  EXPECT_TRUE(was_destructed2);
}

TEST(UniquePtrTest, Release)
{
  auto value = new int{123};
  unique_ptr<int> ptr{value};
  EXPECT_EQ(ptr.release(), value);
  EXPECT_EQ(ptr.get(), nullptr);
  delete value;
}

TEST(UniquePtrTest, Swap)
{
  auto value1 = new int{123};
  unique_ptr<int> ptr1{value1};

  auto value2 = new int{456};
  unique_ptr<int> ptr2{value2};
  ptr1.swap(ptr2);

  EXPECT_EQ(ptr1.get(), value2);
  EXPECT_EQ(ptr2.get(), value1);
}

TEST(UniquePtrTest, Comparison)
{
  unique_ptr<int> ptr1{new int{123}};
  unique_ptr<int> ptr2{new int{456}};
  unique_ptr<int> ptr3{};

  EXPECT_EQ(ptr1, ptr1);
  EXPECT_NE(ptr1, ptr2);

  EXPECT_NE(ptr1, nullptr);
  EXPECT_NE(nullptr, ptr1);

  EXPECT_EQ(ptr3, nullptr);
  EXPECT_EQ(nullptr, ptr3);
}
