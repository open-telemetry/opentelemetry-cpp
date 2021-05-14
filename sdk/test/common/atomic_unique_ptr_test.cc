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

#include "opentelemetry/sdk/common/atomic_unique_ptr.h"

#include <gtest/gtest.h>
using opentelemetry::sdk::common::AtomicUniquePtr;

TEST(AtomicUniquePtrTest, SwapIfNullWithNull)
{
  AtomicUniquePtr<int> ptr;
  EXPECT_TRUE(ptr.IsNull());

  std::unique_ptr<int> x{new int{33}};
  EXPECT_TRUE(ptr.SwapIfNull(x));
  EXPECT_EQ(x, nullptr);
}

TEST(AtomicUniquePtrTest, SwapIfNullWithNonNull)
{
  AtomicUniquePtr<int> ptr;
  ptr.Reset(new int{11});
  std::unique_ptr<int> x{new int{33}};
  EXPECT_TRUE(!ptr.SwapIfNull(x));
  EXPECT_NE(x, nullptr);
  EXPECT_EQ(*x, 33);
  EXPECT_EQ(*ptr, 11);
}

TEST(AtomicUniquePtrTest, Swap)
{
  AtomicUniquePtr<int> ptr;
  EXPECT_TRUE(ptr.IsNull());

  ptr.Reset(new int{11});
  std::unique_ptr<int> x{new int{33}};
  ptr.Swap(x);
  EXPECT_FALSE(ptr.IsNull());
  EXPECT_NE(x, nullptr);
  EXPECT_EQ(*x, 11);
  EXPECT_EQ(*ptr, 33);
}
