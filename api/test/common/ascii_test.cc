// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <cctype>
#include <cstdio>
#include <string>

#include "opentelemetry/common/detail/ascii.h"

namespace ascii = opentelemetry::common::detail;

namespace
{

// The helpers must agree with <cctype> in the "C" locale, which every program
// starts in, for every byte value.
template <typename Helper, typename Reference>
std::string Mismatches(Helper helper, Reference reference)
{
  std::string mismatches;
  for (int byte = 0; byte < 256; ++byte)
  {
    const char c = static_cast<char>(byte);
    if (helper(c) != reference(byte))
    {
      mismatches += std::to_string(byte) + " ";
    }
  }
  return mismatches;
}

int ReferenceHexDigitValue(int byte)
{
  if (std::isxdigit(byte) == 0)
  {
    return -1;
  }
  return std::isdigit(byte) != 0 ? byte - '0' : std::tolower(byte) - 'a' + 10;
}

// The digit for the low four bits of every byte value, as printf formats it.
template <typename Encoder>
std::string EncodingMismatches(Encoder encoder, const char *format)
{
  std::string mismatches;
  for (int value = 0; value < 256; ++value)
  {
    char expected[2];
    std::snprintf(expected, sizeof(expected), format, value & 0xF);
    if (encoder(value) != expected[0])
    {
      mismatches += std::to_string(value) + " ";
    }
  }
  return mismatches;
}

}  // namespace

static_assert(ascii::IsAsciiAlnum('z'), "helpers are usable in constant expressions");
static_assert(ascii::HexDigitValue('F') == 15, "helpers are usable in constant expressions");
static_assert(ascii::LowerHexDigit(0xab) == 'b', "helpers are usable in constant expressions");

TEST(AsciiTest, IsAsciiDigit)
{
  EXPECT_EQ(Mismatches(ascii::IsAsciiDigit, [](int b) { return std::isdigit(b) != 0; }), "");
}

TEST(AsciiTest, IsAsciiLower)
{
  EXPECT_EQ(Mismatches(ascii::IsAsciiLower, [](int b) { return std::islower(b) != 0; }), "");
}

TEST(AsciiTest, IsAsciiUpper)
{
  EXPECT_EQ(Mismatches(ascii::IsAsciiUpper, [](int b) { return std::isupper(b) != 0; }), "");
}

TEST(AsciiTest, IsAsciiAlpha)
{
  EXPECT_EQ(Mismatches(ascii::IsAsciiAlpha, [](int b) { return std::isalpha(b) != 0; }), "");
}

TEST(AsciiTest, IsAsciiAlnum)
{
  EXPECT_EQ(Mismatches(ascii::IsAsciiAlnum, [](int b) { return std::isalnum(b) != 0; }), "");
}

TEST(AsciiTest, IsAsciiHexDigit)
{
  EXPECT_EQ(Mismatches(ascii::IsAsciiHexDigit, [](int b) { return std::isxdigit(b) != 0; }), "");
}

TEST(AsciiTest, IsAsciiControl)
{
  EXPECT_EQ(Mismatches(ascii::IsAsciiControl, [](int b) { return std::iscntrl(b) != 0; }), "");
}

TEST(AsciiTest, IsAsciiSpace)
{
  EXPECT_EQ(Mismatches(ascii::IsAsciiSpace, [](int b) { return std::isspace(b) != 0; }), "");
}

TEST(AsciiTest, ToAsciiLower)
{
  EXPECT_EQ(
      Mismatches(ascii::ToAsciiLower, [](int b) { return static_cast<char>(std::tolower(b)); }),
      "");
}

TEST(AsciiTest, ToAsciiUpper)
{
  EXPECT_EQ(
      Mismatches(ascii::ToAsciiUpper, [](int b) { return static_cast<char>(std::toupper(b)); }),
      "");
}

TEST(AsciiTest, HexDigitValue)
{
  EXPECT_EQ(Mismatches(ascii::HexDigitValue, ReferenceHexDigitValue), "");
}

TEST(AsciiTest, LowerHexDigit)
{
  EXPECT_EQ(EncodingMismatches(ascii::LowerHexDigit, "%x"), "");
}

TEST(AsciiTest, UpperHexDigit)
{
  EXPECT_EQ(EncodingMismatches(ascii::UpperHexDigit, "%X"), "");
}
