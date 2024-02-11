// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>

#include "opentelemetry/sdk/configuration/configuration_factory.h"

std::unique_ptr<opentelemetry::sdk::configuration::Configuration> DoParse(std::string yaml)
{
  return opentelemetry::sdk::configuration::ConfigurationFactory::ParseString(yaml);
}

TEST(Yaml, empty)
{
  std::string yaml = "";

  auto config = DoParse(yaml);
  ASSERT_EQ(config, nullptr);
}

TEST(Yaml, no_format)
{
  std::string yaml = R"(
file_format:
)";

  auto config = DoParse(yaml);
  ASSERT_EQ(config, nullptr);
}

TEST(Yaml, just_format)
{
  std::string yaml = R"(
file_format: xx.yy
)";

  auto config = DoParse(yaml);
  ASSERT_NE(config, nullptr);
  ASSERT_EQ(config->file_format, "xx.yy");
}

TEST(Yaml, disabled)
{
  std::string yaml = R"(
file_format: xx.yy
disabled: true
)";

  auto config = DoParse(yaml);
  ASSERT_NE(config, nullptr);
  ASSERT_EQ(config->file_format, "xx.yy");
  ASSERT_EQ(config->disabled, true);
}

TEST(Yaml, enabled)
{
  std::string yaml = R"(
file_format: xx.yy
disabled: false
)";

  auto config = DoParse(yaml);
  ASSERT_NE(config, nullptr);
  ASSERT_EQ(config->file_format, "xx.yy");
  ASSERT_EQ(config->disabled, false);
}

TEST(Yaml, enabled_by_default)
{
  std::string yaml = R"(
file_format: xx.yy
)";

  auto config = DoParse(yaml);
  ASSERT_NE(config, nullptr);
  ASSERT_EQ(config->file_format, "xx.yy");
  ASSERT_EQ(config->disabled, false);
}

TEST(Yaml, no_attribute_limits)
{
  std::string yaml = R"(
file_format: xx.yy
)";

  auto config = DoParse(yaml);
  ASSERT_NE(config, nullptr);
  ASSERT_EQ(config->attribute_limits, nullptr);
}

TEST(Yaml, empty_attribute_limits)
{
  std::string yaml = R"(
file_format: xx.yy
attribute_limits:
)";

  auto config = DoParse(yaml);
  ASSERT_NE(config, nullptr);
  ASSERT_EQ(config->file_format, "xx.yy");
  ASSERT_NE(config->attribute_limits, nullptr);
  ASSERT_EQ(config->attribute_limits->attribute_value_length_limit, 4096);
  ASSERT_EQ(config->attribute_limits->attribute_count_limit, 128);
}

TEST(Yaml, attribute_limits)
{
  std::string yaml = R"(
file_format: xx.yy
attribute_limits:
  attribute_value_length_limit: 1234
  attribute_count_limit: 5678
)";

  auto config = DoParse(yaml);
  ASSERT_NE(config, nullptr);
  ASSERT_EQ(config->file_format, "xx.yy");
  ASSERT_NE(config->attribute_limits, nullptr);
  ASSERT_EQ(config->attribute_limits->attribute_value_length_limit, 1234);
  ASSERT_EQ(config->attribute_limits->attribute_count_limit, 5678);
}

TEST(Yaml, no_processors)
{
  std::string yaml = R"(
file_format: xx.yy
tracer_provider:
)";

  auto config = DoParse(yaml);
  ASSERT_EQ(config, nullptr);
}

TEST(Yaml, empty_processors)
{
  std::string yaml = R"(
file_format: xx.yy
tracer_provider:
  processors:
)";

  auto config = DoParse(yaml);
  ASSERT_EQ(config, nullptr);
}
