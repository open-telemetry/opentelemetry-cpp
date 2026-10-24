// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <cstddef>
#include <string>
#include <vector>

#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/metrics/instrument_metadata_validator.h"

static std::string CreateVeryLargeString(size_t multiple)
{
  std::string result   = "START";
  std::string repeater = "0123456789";
  for (size_t i = 0; i < multiple; i++)
  {
    result += repeater;
  }
  return result;
}

TEST(InstrumentMetadataValidator, TestName)
{
  opentelemetry::sdk::metrics::InstrumentMetaDataValidator validator;
  std::vector<std::string> invalid_names = {
      "",      // empty string
      "1sdf",  // string starting with number
      "\x31\x32\x33\xe2\x82\xac\x41\x41\x41\xe2\x82\xac\x42\x42\x42",  // unicode characters
      "/\\sdsd",                        // string starting with special character
      "***sSSs",                        // string starting with special character
      "a\\broken\\path",                // contains backward slash
      CreateVeryLargeString(25) + "X",  // total 256 characters
      CreateVeryLargeString(26),        // string much bigger than 255 character
      std::string("abc\0def", 7),       // contains embedded NUL
      "ab\xc3\xa9",                     // non-ASCII letter
      "\xc3\xa9t",                      // starting with non-ASCII letter
      "a b",                            // contains space
  };
  for (auto const &str : invalid_names)
  {
    EXPECT_FALSE(validator.ValidateName(str));
  }

  std::vector<std::string> valid_names = {
      "T",                                      // single char string
      "s123",                                   // starting with char, followed by numbers
      "dsdsdsd_-.",                             // string , and valid nonalphanumeric
      "d1234_-sDSDs.sdsd344",                   // combination of all valid characters
      "a/path/to/some/metric",                  // contains forward slash
      CreateVeryLargeString(5) + "ABCERTYG",    // total 63 characters
      CreateVeryLargeString(5) + "ABCERTYGJ",   // total 64 characters
      CreateVeryLargeString(24) + "ABCDEFGHI",  // total 254 characters
      CreateVeryLargeString(25),                // total 255 characters
  };
  for (auto const &str : valid_names)
  {
    EXPECT_TRUE(validator.ValidateName(str));
  }
}

TEST(InstrumentMetadataValidator, TestUnit)
{
  opentelemetry::sdk::metrics::InstrumentMetaDataValidator validator;
  std::vector<std::string> invalid_units = {
      CreateVeryLargeString(5) + "ABCERTYGJ",  // total 64 charactes
      CreateVeryLargeString(7),                // string bigger than 63 chars
      "\x31\x32\x33\xe2\x82\xac\x41\x41\x41\xe2\x82\xac\x42\x42\x42",  // unicode string
      std::string("m\0s", 3),                                          // contains embedded NUL
  };
  for (auto const &str : invalid_units)
  {
    EXPECT_FALSE(validator.ValidateUnit(str));
  }

  std::vector<std::string> valid_units = {
      "T",                                    // single char
      "s123",                                 // starting with char, followed by numbers
      "dsdsdsd_-.",                           // string , and valid nonalphanumeric
      "d1234_-sdsds.sdsd344",                 // combination of all valid characters
      CreateVeryLargeString(5) + "ABCERTYG",  // total 63 charactes
      "ASDDSDF",                              // uppercase
      "",                                     // empty
      "{request}/s",                          // annotation and slash
      "\x01\x7F",                             // lowest and highest ASCII characters
  };
  for (auto const &str : valid_units)
  {
    EXPECT_TRUE(validator.ValidateUnit(str));
  }
}

// The views below are not NUL-terminated, so the characters after their end
// must not change the result.

TEST(InstrumentMetadataValidator, NameViewEndsBeforeInvalidCharacter)
{
  opentelemetry::sdk::metrics::InstrumentMetaDataValidator validator;
  const std::string buffer = "abc def";
  EXPECT_TRUE(validator.ValidateName(opentelemetry::nostd::string_view(buffer.data(), 3)));
}

TEST(InstrumentMetadataValidator, EmptyNameViewOverValidName)
{
  opentelemetry::sdk::metrics::InstrumentMetaDataValidator validator;
  const std::string buffer = "abcdef";
  EXPECT_FALSE(validator.ValidateName(opentelemetry::nostd::string_view(buffer.data(), 0)));
}

TEST(InstrumentMetadataValidator, MaxLengthNameViewOverLongerName)
{
  opentelemetry::sdk::metrics::InstrumentMetaDataValidator validator;
  const std::string buffer = CreateVeryLargeString(26);  // 265 characters
  EXPECT_TRUE(validator.ValidateName(opentelemetry::nostd::string_view(buffer.data(), 255)));
}

TEST(InstrumentMetadataValidator, UnitViewEndsBeforeNonAsciiCharacter)
{
  opentelemetry::sdk::metrics::InstrumentMetaDataValidator validator;
  const std::string buffer = "ms\xe2\x82\xac";
  EXPECT_TRUE(validator.ValidateUnit(opentelemetry::nostd::string_view(buffer.data(), 2)));
}

TEST(InstrumentMetadataValidator, MaxLengthUnitViewOverLongerUnit)
{
  opentelemetry::sdk::metrics::InstrumentMetaDataValidator validator;
  const std::string buffer = CreateVeryLargeString(7);  // 75 characters
  EXPECT_TRUE(validator.ValidateUnit(opentelemetry::nostd::string_view(buffer.data(), 63)));
}
