// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <memory>
#include <string>

#include "opentelemetry/sdk/configuration/configuration.h"
#include "opentelemetry/sdk/configuration/yaml_configuration_factory.h"

std::unique_ptr<opentelemetry::sdk::configuration::Configuration> DoParse(const std::string &yaml)
{
  static const std::string source("test");
  return opentelemetry::sdk::configuration::YamlConfigurationFactory::ParseString(source, yaml);
}

TEST(YamlTrace, no_processors)
{
  std::string yaml = R"(
file_format: xx.yy
tracer_provider:
)";

  auto config = DoParse(yaml);
  ASSERT_EQ(config, nullptr);
}

TEST(YamlTrace, empty_processors)
{
  std::string yaml = R"(
file_format: xx.yy
tracer_provider:
  processors:
)";

  auto config = DoParse(yaml);
  ASSERT_EQ(config, nullptr);
}
