// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <opentelemetry/sdk/trace/tracer_config.h>

namespace trace_sdk = opentelemetry::sdk::trace;
namespace instrumentation_scope = opentelemetry::sdk::instrumentationscope;

/** Tests to verify the basic behavior of trace_sdk::TracerConfig */

TEST(TracerConfig, CheckDisabledWorksAsExpected)
{
  trace_sdk::TracerConfig disabled_config = trace_sdk::TracerConfig::Disabled();
  ASSERT_FALSE(disabled_config.IsEnabled());
}

TEST(TracerConfig, CheckEnabledWorksAsExpected)
{
  trace_sdk::TracerConfig enabled_config = trace_sdk::TracerConfig::Enabled();
  ASSERT_TRUE(enabled_config.IsEnabled());
}

TEST(TracerConfig, CheckDefaultConfigWorksAccToSpec)
{
  trace_sdk::TracerConfig default_config = trace_sdk::TracerConfig::Default();
  ASSERT_TRUE(default_config.IsEnabled());
}

/** Tests to verify the behavior of trace_sdk::TracerConfig::DefaultConfigurator */

std::pair<opentelemetry::nostd::string_view, opentelemetry::common::AttributeValue> attr1 = {
  "accept_single_attr", true};
std::pair<opentelemetry::nostd::string_view, opentelemetry::common::AttributeValue> attr2 = {
  "accept_second_attr", "some other attr"};
std::pair<opentelemetry::nostd::string_view, opentelemetry::common::AttributeValue> attr3 = {
  "accept_third_attr", 3};

const instrumentation_scope::InstrumentationScope instrumentation_scopes[] = {
  *instrumentation_scope::InstrumentationScope::Create("test_scope_1").get(),
  *instrumentation_scope::InstrumentationScope::Create("test_scope_2", "1.0").get(),
  *instrumentation_scope::InstrumentationScope::Create("test_scope_3", "0", "https://opentelemetry.io/schemas/v1.18.0").get(),
  *instrumentation_scope::InstrumentationScope::Create("test_scope_3", "0", "https://opentelemetry.io/schemas/v1.18.0", {attr1})
             .get(),
  *instrumentation_scope::InstrumentationScope::Create("test_scope_4", "0", "https://opentelemetry.io/schemas/v1.18.0", {attr1, attr2, attr3})
             .get(),
};

// Test fixture for VerifyDefaultConfiguratorBehavior
class DefaultTracerConfiguratorTestFixture : public ::testing::TestWithParam<instrumentation_scope::InstrumentationScope>
{};

// verifies that the default configurator always returns the default tracer config
TEST_P(DefaultTracerConfiguratorTestFixture, VerifyDefaultConfiguratorBehavior)
{
  const instrumentation_scope::InstrumentationScope& scope = GetParam();
  trace_sdk::TracerConfigurator default_configurator = trace_sdk::TracerConfig::DefaultConfigurator();

  ASSERT_EQ(default_configurator(scope), trace_sdk::TracerConfig::Default());
}

INSTANTIATE_TEST_SUITE_P(InstrumentationScopes, DefaultTracerConfiguratorTestFixture, ::testing::ValuesIn(instrumentation_scopes));