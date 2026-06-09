// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>

#include "opentelemetry/sdk/configuration/configuration.h"
#include "opentelemetry/sdk/configuration/yaml_configuration_parser.h"

TEST(ConfigurationYamlInstallTest, ParseYamlString)
{
  static const std::string source("test");

  std::string yaml = R"(
file_format: "1.0"
logger_provider:
  processors:
    - simple:
        exporter:
          console:
)";

  auto model =
      opentelemetry::sdk::configuration::YamlConfigurationParser::ParseString(source, yaml);
  EXPECT_NE(model, nullptr);
}
