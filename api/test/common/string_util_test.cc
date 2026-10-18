// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <cstring>
#include <ostream>
#include <string>

#include <opentelemetry/common/macros.h>
#include <opentelemetry/common/string_util.h>

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
    EXPECT_EQ(StringUtil::Trim(testcase.input, 0, std::strlen(testcase.input) - 1),
              testcase.expected);
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
                   {"k1=v1\t", "k1=v1"},
                   {"\t k1=v1 \t", "k1=v1"},
                   {"\t\t k1=v1\t  ", "k1=v1"},
                   {"\t\t k1=v1\t  ,k2=v2", "k1=v1\t  ,k2=v2"},
                   {"   k1=v1 ", "k1=v1"},
                   {" ", ""},
                   {"", ""},
                   {"\n_some string_\t", "_some string_"}};

  for (auto &testcase : testcases)
  {
    EXPECT_EQ(StringUtil::Trim(testcase.input), testcase.expected);
  }
}

TEST(StringUtilTest, TrimStringOutOfRange)
{
  EXPECT_EQ(StringUtil::Trim("x", 2, 1), "");
}

struct SubstrTestData
{
  const char *input;
  size_t pos;
  size_t count;

  friend void PrintTo(const SubstrTestData &data, std::ostream *os)
  {
    *os << "(\"" << data.input << "\"," << data.pos << "," << data.count << ")";
  }
};

const SubstrTestData substr_in_range_test_cases[] = {
    {"k1=v1", 0, std::string::npos},
    {"k1=v1", 3, std::string::npos},
    {"k1=v1", 0, 2},
    {"k1=v1", 3, 100},
    {"k1=v1", 2, 0},
    {"k1=v1", 5, std::string::npos},
    {"x", 0, std::string::npos},
    {"x", 1, std::string::npos},
    {"", 0, std::string::npos},
    {"", 0, 0},
};

using SubstrTestFixture = ::testing::TestWithParam<SubstrTestData>;

TEST_P(SubstrTestFixture, MatchesStdSubstr)
{
  const SubstrTestData test_param = GetParam();
  const std::string input(test_param.input);

  EXPECT_EQ(StringUtil::Substr(input, test_param.pos, test_param.count),
            input.substr(test_param.pos, test_param.count));
}

INSTANTIATE_TEST_SUITE_P(SubstrTestCases,
                         SubstrTestFixture,
                         ::testing::ValuesIn(substr_in_range_test_cases));

const SubstrTestData substr_out_of_range_test_cases[] = {
    {"k1=v1", 6, std::string::npos},
    {"k1=v1", 100, 2},
    {"k1=v1", std::string::npos, std::string::npos},
    {"x", 2, std::string::npos},
    {"", 1, std::string::npos},
};

using SubstrOutOfRangeTestFixture = ::testing::TestWithParam<SubstrTestData>;

TEST_P(SubstrOutOfRangeTestFixture, HandlesPosPastTheEnd)
{
  const SubstrTestData test_param = GetParam();

#if OPENTELEMETRY_HAVE_EXCEPTIONS
  EXPECT_EQ(StringUtil::Substr(test_param.input, test_param.pos, test_param.count), "");
#else
  // Without exceptions the underlying calls std::terminate(), so helper cannot recover.
  EXPECT_DEATH({ StringUtil::Substr(test_param.input, test_param.pos, test_param.count); }, "");
#endif
}

INSTANTIATE_TEST_SUITE_P(SubstrOutOfRangeTestCases,
                         SubstrOutOfRangeTestFixture,
                         ::testing::ValuesIn(substr_out_of_range_test_cases));
