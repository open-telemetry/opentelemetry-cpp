// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <vector>

#include "opentelemetry/logs/severity.h"
#include "opentelemetry/sdk/configuration/logger_config_configuration.h"
#include "opentelemetry/sdk/configuration/logger_configurator_configuration.h"
#include "opentelemetry/sdk/configuration/logger_matcher_and_config_configuration.h"
#include "opentelemetry/sdk/configuration/registry.h"
#include "opentelemetry/sdk/configuration/sdk_builder.h"
#include "opentelemetry/sdk/configuration/severity_number.h"
#include "opentelemetry/sdk/instrumentationscope/instrumentation_scope.h"
#include "opentelemetry/sdk/instrumentationscope/scope_configurator.h"
#include "opentelemetry/sdk/logs/logger_config.h"

namespace config_sdk = opentelemetry::sdk::configuration;
namespace scope_sdk  = opentelemetry::sdk::instrumentationscope;
namespace logs_sdk   = opentelemetry::sdk::logs;

TEST(SdkBuilder, CreateLoggerConfigurator)
{
  config_sdk::LoggerConfigConfiguration default_config;
  default_config.enabled          = true;
  default_config.minimum_severity = config_sdk::SeverityNumber::warn;
  default_config.trace_based      = false;

  config_sdk::LoggerMatcherAndConfigConfiguration matcher1;
  matcher1.name                    = "enabled_minsev_error_not_trace_based";
  matcher1.config.enabled          = true;
  matcher1.config.minimum_severity = config_sdk::SeverityNumber::error3;
  matcher1.config.trace_based      = false;

  config_sdk::LoggerMatcherAndConfigConfiguration matcher2;
  matcher2.name                    = "disabled_minsev_info_trace_based";
  matcher2.config.enabled          = false;
  matcher2.config.minimum_severity = config_sdk::SeverityNumber::debug;
  matcher2.config.trace_based      = true;

  auto model            = std::make_unique<config_sdk::LoggerConfiguratorConfiguration>();
  model->default_config = default_config;
  model->loggers.push_back(matcher1);
  model->loggers.push_back(matcher2);

  config_sdk::SdkBuilder builder(std::make_shared<config_sdk::Registry>());

  auto logger_configurator = builder.CreateLoggerConfigurator(model);
  ASSERT_NE(logger_configurator, nullptr);

  auto default_scope = scope_sdk::InstrumentationScope::Create("default_scope");
  logs_sdk::LoggerConfig sdk_logger_config_default =
      logger_configurator->ComputeConfig(*default_scope);

  auto scope_1 = scope_sdk::InstrumentationScope::Create(matcher1.name);
  logs_sdk::LoggerConfig sdk_logger_config_1 = logger_configurator->ComputeConfig(*scope_1);

  auto scope_2 = scope_sdk::InstrumentationScope::Create(matcher2.name);
  logs_sdk::LoggerConfig sdk_logger_config_2 = logger_configurator->ComputeConfig(*scope_2);

  EXPECT_TRUE(sdk_logger_config_default.IsEnabled());
  EXPECT_EQ(sdk_logger_config_default.GetMinimumSeverity(), opentelemetry::logs::Severity::kWarn);
  EXPECT_FALSE(sdk_logger_config_default.IsTraceBased());

  EXPECT_TRUE(sdk_logger_config_1.IsEnabled());
  EXPECT_EQ(sdk_logger_config_1.GetMinimumSeverity(), opentelemetry::logs::Severity::kError3);
  EXPECT_FALSE(sdk_logger_config_1.IsTraceBased());

  EXPECT_FALSE(sdk_logger_config_2.IsEnabled());
  EXPECT_EQ(sdk_logger_config_2.GetMinimumSeverity(), opentelemetry::logs::Severity::kDebug);
  EXPECT_TRUE(sdk_logger_config_2.IsTraceBased());
}
