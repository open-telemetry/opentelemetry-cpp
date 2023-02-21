// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/common/env_variables.h"

#include <gtest/gtest.h>

#if defined(_MSC_VER)
using opentelemetry::sdk::common::setenv;
using opentelemetry::sdk::common::unsetenv;
#endif

using opentelemetry::sdk::common::GetBoolEnvironmentVariable;
using opentelemetry::sdk::common::GetDurationEnvironmentVariable;
using opentelemetry::sdk::common::GetStringEnvironmentVariable;

#ifndef NO_GETENV
TEST(EnvVarTest, BoolEnvVar)
{
  unsetenv("BOOL_ENV_VAR_NONE");
  setenv("BOOL_ENV_VAR_EMPTY", "", 1);
  setenv("BOOL_ENV_VAR_T1", "true", 1);
  setenv("BOOL_ENV_VAR_T2", "TRUE", 1);
  setenv("BOOL_ENV_VAR_T3", "TrUe", 1);
  setenv("BOOL_ENV_VAR_F1", "false", 1);
  setenv("BOOL_ENV_VAR_F2", "FALSE", 1);
  setenv("BOOL_ENV_VAR_F3", "FaLsE", 1);
  setenv("BOOL_ENV_VAR_BROKEN", "Maybe ?", 1);

  bool exists;
  bool value = true;

  exists = GetBoolEnvironmentVariable("BOOL_ENV_VAR_NONE", value);
  EXPECT_FALSE(exists);

  value  = true;
  exists = GetBoolEnvironmentVariable("BOOL_ENV_VAR_EMPTY", value);
  EXPECT_FALSE(exists);
  EXPECT_FALSE(value);

  value  = false;
  exists = GetBoolEnvironmentVariable("BOOL_ENV_VAR_T1", value);
  EXPECT_TRUE(exists);
  EXPECT_TRUE(value);

  value  = false;
  exists = GetBoolEnvironmentVariable("BOOL_ENV_VAR_T2", value);
  EXPECT_TRUE(exists);
  EXPECT_TRUE(value);

  value  = false;
  exists = GetBoolEnvironmentVariable("BOOL_ENV_VAR_T3", value);
  EXPECT_TRUE(exists);
  EXPECT_TRUE(value);

  value  = true;
  exists = GetBoolEnvironmentVariable("BOOL_ENV_VAR_F1", value);
  EXPECT_TRUE(exists);
  EXPECT_FALSE(value);

  value  = true;
  exists = GetBoolEnvironmentVariable("BOOL_ENV_VAR_F2", value);
  EXPECT_TRUE(exists);
  EXPECT_FALSE(value);

  value  = true;
  exists = GetBoolEnvironmentVariable("BOOL_ENV_VAR_F3", value);
  EXPECT_TRUE(exists);
  EXPECT_FALSE(value);

  // This raises a warning, not verifying the warning text.
  value  = true;
  exists = GetBoolEnvironmentVariable("BOOL_ENV_VAR_BROKEN", value);
  EXPECT_TRUE(exists);
  EXPECT_FALSE(value);

  unsetenv("BOOL_ENV_VAR_EMPTY");
  unsetenv("BOOL_ENV_VAR_T1");
  unsetenv("BOOL_ENV_VAR_T2");
  unsetenv("BOOL_ENV_VAR_T3");
  unsetenv("BOOL_ENV_VAR_F1");
  unsetenv("BOOL_ENV_VAR_F2");
  unsetenv("BOOL_ENV_VAR_F3");
  unsetenv("BOOL_ENV_VAR_BROKEN");
}

TEST(EnvVarTest, StringEnvVar)
{
  unsetenv("STRING_ENV_VAR_NONE");
  setenv("STRING_ENV_VAR_EMPTY", "", 1);
  setenv("STRING_ENV_VAR", "my string", 1);

  bool exists;
  std::string value;

  exists = GetStringEnvironmentVariable("STRING_ENV_VAR_NONE", value);
  EXPECT_FALSE(exists);

  exists = GetStringEnvironmentVariable("STRING_ENV_VAR_EMPTY", value);
  EXPECT_FALSE(exists);
  EXPECT_EQ(value, "");

  value  = "poison";
  exists = GetStringEnvironmentVariable("STRING_ENV_VAR", value);
  EXPECT_TRUE(exists);
  EXPECT_EQ(value, "my string");

  unsetenv("STRING_ENV_VAR_EMPTY");
  unsetenv("STRING_ENV_VAR");
}

TEST(EnvVarTest, DurationEnvVar)
{
  unsetenv("DURATION_ENV_VAR_NONE");
  setenv("DURATION_ENV_VAR_EMPTY", "", 1);
  setenv("DURATION_ENV_VAR_1", "10ns", 1);
  setenv("DURATION_ENV_VAR_2", "20us", 1);
  setenv("DURATION_ENV_VAR_3", "30ms", 1);
  setenv("DURATION_ENV_VAR_4", "40s", 1);
  setenv("DURATION_ENV_VAR_5", "50m", 1);
  setenv("DURATION_ENV_VAR_6", "60h", 1);
  setenv("DURATION_ENV_VAR_7", "123", 1);
  setenv("DURATION_ENV_VAR_BROKEN_1", "123 ms", 1);
  setenv("DURATION_ENV_VAR_BROKEN_2", "1mississippi", 1);

  bool exists;
  std::chrono::system_clock::duration poison;
  std::chrono::system_clock::duration value;
  std::chrono::system_clock::duration expected;

  poison =
      std::chrono::duration_cast<std::chrono::system_clock::duration>(std::chrono::seconds{666});

  exists = GetDurationEnvironmentVariable("DURATION_ENV_VAR_NONE", value);
  EXPECT_FALSE(exists);

  exists = GetDurationEnvironmentVariable("DURATION_ENV_VAR_EMPTY", value);
  EXPECT_FALSE(exists);

  value = poison;
  expected =
      std::chrono::duration_cast<std::chrono::system_clock::duration>(std::chrono::nanoseconds{10});
  exists = GetDurationEnvironmentVariable("DURATION_ENV_VAR_1", value);
  EXPECT_TRUE(exists);
  EXPECT_EQ(value, expected);

  value    = poison;
  expected = std::chrono::duration_cast<std::chrono::system_clock::duration>(
      std::chrono::microseconds{20});
  exists = GetDurationEnvironmentVariable("DURATION_ENV_VAR_2", value);
  EXPECT_TRUE(exists);
  EXPECT_EQ(value, expected);

  value    = poison;
  expected = std::chrono::duration_cast<std::chrono::system_clock::duration>(
      std::chrono::milliseconds{30});
  exists = GetDurationEnvironmentVariable("DURATION_ENV_VAR_3", value);
  EXPECT_TRUE(exists);
  EXPECT_EQ(value, expected);

  value = poison;
  expected =
      std::chrono::duration_cast<std::chrono::system_clock::duration>(std::chrono::seconds{40});
  exists = GetDurationEnvironmentVariable("DURATION_ENV_VAR_4", value);
  EXPECT_TRUE(exists);
  EXPECT_EQ(value, expected);

  value = poison;
  expected =
      std::chrono::duration_cast<std::chrono::system_clock::duration>(std::chrono::minutes{50});
  exists = GetDurationEnvironmentVariable("DURATION_ENV_VAR_5", value);
  EXPECT_TRUE(exists);
  EXPECT_EQ(value, expected);

  value = poison;
  expected =
      std::chrono::duration_cast<std::chrono::system_clock::duration>(std::chrono::hours{60});
  exists = GetDurationEnvironmentVariable("DURATION_ENV_VAR_6", value);
  EXPECT_TRUE(exists);
  EXPECT_EQ(value, expected);

  value = poison;
  // Deviation from the spec: 123 seconds instead of 123 milliseconds
  expected =
      std::chrono::duration_cast<std::chrono::system_clock::duration>(std::chrono::seconds{123});
  exists = GetDurationEnvironmentVariable("DURATION_ENV_VAR_7", value);
  EXPECT_TRUE(exists);
  EXPECT_EQ(value, expected);

  // This raises a warning, not verifying the warning text.
  exists = GetDurationEnvironmentVariable("DURATION_ENV_VAR_BROKEN_1", value);
  EXPECT_FALSE(exists);

  // This raises a warning, not verifying the warning text.
  exists = GetDurationEnvironmentVariable("DURATION_ENV_VAR_BROKEN_2", value);
  EXPECT_FALSE(exists);

  unsetenv("STRING_ENV_VAR_EMPTY");
  unsetenv("STRING_ENV_VAR_1");
  unsetenv("STRING_ENV_VAR_2");
  unsetenv("STRING_ENV_VAR_3");
  unsetenv("STRING_ENV_VAR_4");
  unsetenv("STRING_ENV_VAR_5");
  unsetenv("STRING_ENV_VAR_6");
  unsetenv("STRING_ENV_VAR_7");
  unsetenv("STRING_ENV_VAR_BROKEN_1");
  unsetenv("STRING_ENV_VAR_BROKEN_2");
}

#endif
