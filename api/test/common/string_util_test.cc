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

#include <gtest/gtest.h>
#include <opentelemetry/common/string_util.h>

#include <string>
#include <utility>
#include <vector>

// ------------------------- StringUtil class tests ---------------------------------

using opentelemetry::common::StringUtil;

TEST(StringUtilTest, TrimStringWithIndex)
{
  struct
  {
    const char *input;
    const char *expected;
  } testcases[] = {{"k1=v1", "k1=v1"},     {"k1=v1,k2=v2, k3=v3", "k1=v1,k2=v2, k3=v3"},
                   {"   k1=v1", "k1=v1"},  {"k1=v1   ", "k1=v1"},
                   {"   k1=v1 ", "k1=v1"}, {"  ", ""}};
  for (auto &testcase : testcases)
  {
    EXPECT_EQ(StringUtil::Trim(testcase.input, 0, strlen(testcase.input) - 1), testcase.expected);
  }
}

TEST(StringUtilTest, TrimString)
{
  struct
  {
    const char *input;
    const char *expected;
  } testcases[] = {{"k1=v1", "k1=v1"},
                   {"k1=v1,k2=v2, k3=v3", "k1=v1,k2=v2, k3=v3"},
                   {"   k1=v1", "k1=v1"},
                   {"k1=v1   ", "k1=v1"},
                   {"   k1=v1 ", "k1=v1"},
                   {" ", ""},
                   {"", ""}};
  for (auto &testcase : testcases)
  {
    EXPECT_EQ(StringUtil::Trim(testcase.input), testcase.expected);
  }
}
