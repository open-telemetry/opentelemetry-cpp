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

#include "opentelemetry/nostd/function_ref.h"

#include <gtest/gtest.h>
using namespace opentelemetry::nostd;

int Call(function_ref<int()> f)
{
  return f();
}

int Return3()
{
  return 3;
}

TEST(FunctionRefTest, Call)
{
  int x = 9;

  auto f = [&] { return x; };
  EXPECT_EQ(Call(f), 9);

  EXPECT_EQ(Call(Return3), 3);
}

TEST(FunctionRefTest, BoolConversion)
{
  auto f = [] { return 0; };
  function_ref<int()> fref1{nullptr};
  function_ref<int()> fref2{f};
  EXPECT_TRUE(!static_cast<bool>(fref1));
  EXPECT_TRUE(static_cast<bool>(fref2));
}
