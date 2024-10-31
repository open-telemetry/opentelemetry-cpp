// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <opentelemetry/trace/propagation/b3_propagator.h>
#include <string>

#include "opentelemetry/nostd/string_view.h"

struct SplitStringTestData
{
  opentelemetry::nostd::string_view input;
  char separator;
  size_t max_count;
  std::vector<opentelemetry::nostd::string_view> splits;
  size_t expected_number_strings;
};

const SplitStringTestData split_string_test_cases[] = {
    {"foo,bar,baz", ',', 4, std::vector<opentelemetry::nostd::string_view>{"foo", "bar", "baz"}, 3},
    {"foo,bar,baz,foobar", ',', 4,
     std::vector<opentelemetry::nostd::string_view>{"foo", "bar", "baz", "foobar"}, 4},
    {"foo,bar,baz,foobar", '.', 4,
     std::vector<opentelemetry::nostd::string_view>{"foo,bar,baz,foobar"}, 1},
    {"foo,bar,baz,", ',', 4,
     std::vector<opentelemetry::nostd::string_view>{"foo", "bar", "baz", ""}, 4},
    {"foo,bar,baz,", ',', 2, std::vector<opentelemetry::nostd::string_view>{"foo", "bar"}, 2},
    {"foo ,bar, baz ", ',', 4,
     std::vector<opentelemetry::nostd::string_view>{"foo ", "bar", " baz "}, 3},
    {"foo ,bar, baz ", ',', 4,
     std::vector<opentelemetry::nostd::string_view>{"foo ", "bar", " baz "}, 3},
    {"00-0af7651916cd43dd8448eb211c80319c-00f067aa0ba902b7-01", '-', 4,
     std::vector<opentelemetry::nostd::string_view>{"00", "0af7651916cd43dd8448eb211c80319c",
                                                    "00f067aa0ba902b7", "01"},
     4},
};

// Test fixture
class SplitStringTestFixture : public ::testing::TestWithParam<SplitStringTestData>
{};

TEST_P(SplitStringTestFixture, SplitsAsExpected)
{
  SplitStringTestData test_param = GetParam();
  std::vector<opentelemetry::nostd::string_view> fields{};
  fields.reserve(test_param.expected_number_strings);
  size_t got_splits_num = opentelemetry::trace::propagation::detail::SplitString(
      test_param.input, test_param.separator, fields.data(), test_param.max_count);

  // Assert on the output
  EXPECT_EQ(got_splits_num, test_param.expected_number_strings);
  for (size_t i = 0; i < got_splits_num; i++)
  {
    // Checks for resulting strings in-order
    EXPECT_EQ(fields[i], test_param.splits[i]);
  }
}

INSTANTIATE_TEST_SUITE_P(SplitStringTestCases,
                         SplitStringTestFixture,
                         ::testing::ValuesIn(split_string_test_cases));
