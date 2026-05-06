// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>

#include <opentelemetry/sdk/version/version.h>
#include "opentelemetry/exporters/ostream/console_log_record_builder.h"
#include "opentelemetry/exporters/ostream/console_push_metric_builder.h"
#include "opentelemetry/exporters/ostream/console_span_builder.h"
#include "opentelemetry/sdk/configuration/configuration.h"
#include "opentelemetry/sdk/configuration/configured_sdk.h"
#include "opentelemetry/sdk/configuration/registry.h"
#include "opentelemetry/sdk/configuration/yaml_configuration_parser.h"

TEST(ConfigurationInstallTest, ConfigurationCheck)
{
  std::shared_ptr<opentelemetry::sdk::configuration::Registry> registry(
      new opentelemetry::sdk::configuration::Registry);
  EXPECT_NE(registry, nullptr);

  opentelemetry::exporter::trace::ConsoleSpanBuilder::Register(registry.get());
  opentelemetry::exporter::metrics::ConsolePushMetricBuilder::Register(registry.get());
  opentelemetry::exporter::logs::ConsoleLogRecordBuilder::Register(registry.get());

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

  auto test_sdk = opentelemetry::sdk::configuration::ConfiguredSdk::Create(registry, model);
  EXPECT_NE(test_sdk, nullptr);

  test_sdk->Install();
  test_sdk->UnInstall();
  test_sdk.reset(nullptr);
}
