// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/logs/logger_config.h"
#include <gtest/gtest.h>
#include <array>
#include <string>
#include <utility>
#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/logs/severity.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/instrumentationscope/instrumentation_scope.h"
#include "opentelemetry/sdk/instrumentationscope/scope_configurator.h"

namespace logs_sdk              = opentelemetry::sdk::logs;
namespace instrumentation_scope = opentelemetry::sdk::instrumentationscope;
namespace logs_api              = opentelemetry::logs;

/** Test to verify the basic behavior of logs_sdk::LoggerConfig */

TEST(LoggerConfig, CheckDisabledWorksAsExpected)
{
  logs_sdk::LoggerConfig disabled_config = logs_sdk::LoggerConfig::Disabled();
  ASSERT_FALSE(disabled_config.IsEnabled());
  ASSERT_EQ(disabled_config.GetMinimumSeverity(), logs_api::Severity::kInvalid);
  ASSERT_FALSE(disabled_config.IsTraceBased());
}

TEST(LoggerConfig, CheckEnabledWorksAsExpected)
{
  logs_sdk::LoggerConfig enabled_config = logs_sdk::LoggerConfig::Enabled();
  ASSERT_TRUE(enabled_config.IsEnabled());
  ASSERT_EQ(enabled_config.GetMinimumSeverity(), logs_api::Severity::kInvalid);
  ASSERT_FALSE(enabled_config.IsTraceBased());
}

TEST(LoggerConfig, CheckDefaultConfigWorksAccToSpec)
{
  logs_sdk::LoggerConfig default_config = logs_sdk::LoggerConfig::Default();
  ASSERT_TRUE(default_config.IsEnabled());
  ASSERT_EQ(default_config.GetMinimumSeverity(), logs_api::Severity::kInvalid);
  ASSERT_FALSE(default_config.IsTraceBased());
}

TEST(LoggerConfig, CheckCreateWorksAsExpected)
{
  logs_sdk::LoggerConfig custom_config =
      logs_sdk::LoggerConfig::Create(true, logs_api::Severity::kWarn, true);

  ASSERT_TRUE(custom_config.IsEnabled());
  ASSERT_EQ(custom_config.GetMinimumSeverity(), logs_api::Severity::kWarn);
  ASSERT_TRUE(custom_config.IsTraceBased());
}

/** Tests to verify the behavior of logs_sdk::LoggerConfig::Default */

static std::pair<opentelemetry::nostd::string_view, opentelemetry::common::AttributeValue> &
GetAttr1()
{
  static std::pair<opentelemetry::nostd::string_view, opentelemetry::common::AttributeValue> value{
      "accept_single_attr", true};
  return value;
}
static std::pair<opentelemetry::nostd::string_view, opentelemetry::common::AttributeValue> &
GetAttr2()
{
  static std::pair<opentelemetry::nostd::string_view, opentelemetry::common::AttributeValue> value{
      "accept_second_attr", "some other attr"};
  return value;
}
static std::pair<opentelemetry::nostd::string_view, opentelemetry::common::AttributeValue> &
GetAttr3()
{
  static std::pair<opentelemetry::nostd::string_view, opentelemetry::common::AttributeValue> value{
      "accept_third_attr", 3};
  return value;
}

static instrumentation_scope::InstrumentationScope &GetTestScope1()
{
  static auto value = *instrumentation_scope::InstrumentationScope::Create("test_scope_1");
  return value;
}
static instrumentation_scope::InstrumentationScope &GetTestScope2()
{
  static auto value = *instrumentation_scope::InstrumentationScope::Create("test_scope_2", "1.0");
  return value;
}
static instrumentation_scope::InstrumentationScope &GetTestScope3()
{
  static auto value = *instrumentation_scope::InstrumentationScope::Create(
      "test_scope_3", "0", "https://opentelemetry.io/schemas/v1.18.0");
  return value;
}
static instrumentation_scope::InstrumentationScope &GetTestScope4()
{
  static auto value = *instrumentation_scope::InstrumentationScope::Create(
      "test_scope_4", "0", "https://opentelemetry.io/schemas/v1.18.0", {GetAttr1()});
  return value;
}
static instrumentation_scope::InstrumentationScope &GetTestScope5()
{
  static auto value = *instrumentation_scope::InstrumentationScope::Create(
      "test_scope_5", "0", "https://opentelemetry.io/schemas/v1.18.0",
      {GetAttr1(), GetAttr2(), GetAttr3()});
  return value;
}

// This array could also directly contain the reference types, but that  leads to 'uninitialized
// value was created by heap allocation' errors in Valgrind memcheck. This is a bug in Googletest
// library, see https://github.com/google/googletest/issues/3805#issuecomment-1397301790 for more
// details. Using pointers is a workaround to prevent the Valgrind warnings.
static const std::array<instrumentation_scope::InstrumentationScope *, 5> &
GetInstrumentationScopes()
{
  static const std::array<instrumentation_scope::InstrumentationScope *, 5> value = {
      &GetTestScope1(), &GetTestScope2(), &GetTestScope3(), &GetTestScope4(), &GetTestScope5(),
  };
  return value;
}

namespace
{

// Test fixture for VerifyDefaultConfiguratorBehavior
class DefaultLoggerConfiguratorTestFixture
    : public ::testing::TestWithParam<instrumentation_scope::InstrumentationScope *>
{};

// Verifies that the default configurator always returns the default logger config.
TEST_P(DefaultLoggerConfiguratorTestFixture, VerifyDefaultConfiguratorBehavior)
{
  instrumentation_scope::InstrumentationScope *scope = GetParam();
  instrumentation_scope::ScopeConfigurator<logs_sdk::LoggerConfig> default_configurator =
      instrumentation_scope::ScopeConfigurator<logs_sdk::LoggerConfig>::Builder(
          logs_sdk::LoggerConfig::Default())
          .Build();

  ASSERT_EQ(default_configurator.ComputeConfig(*scope), logs_sdk::LoggerConfig::Default());
}

TEST(LoggerConfig, ScopeConfiguratorPreservesCustomConfig)
{
  logs_sdk::LoggerConfig default_config =
      logs_sdk::LoggerConfig::Create(false, logs_api::Severity::kError, true);
  logs_sdk::LoggerConfig matching_config =
      logs_sdk::LoggerConfig::Create(true, logs_api::Severity::kWarn, false);

  instrumentation_scope::ScopeConfigurator<logs_sdk::LoggerConfig> configurator =
      instrumentation_scope::ScopeConfigurator<logs_sdk::LoggerConfig>::Builder(default_config)
          .AddConditionNameEquals("test_scope_1", matching_config)
          .Build();

  ASSERT_EQ(configurator.ComputeConfig(GetTestScope1()), matching_config);
  ASSERT_EQ(configurator.ComputeConfig(GetTestScope2()), default_config);
}

INSTANTIATE_TEST_SUITE_P(InstrumentationScopes,
                         DefaultLoggerConfiguratorTestFixture,
                         ::testing::ValuesIn(GetInstrumentationScopes()));

}  // namespace
