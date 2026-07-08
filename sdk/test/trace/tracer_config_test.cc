// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/trace/tracer_config.h"
#include <gtest/gtest.h>
#include <array>
#include <string>
#include <utility>
#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/instrumentationscope/instrumentation_scope.h"
#include "opentelemetry/sdk/instrumentationscope/scope_configurator.h"

namespace trace_sdk             = opentelemetry::sdk::trace;
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
class DefaultTracerConfiguratorTestFixture
    : public ::testing::TestWithParam<instrumentation_scope::InstrumentationScope *>
{};

// verifies that the default configurator always returns the default tracer config
TEST_P(DefaultTracerConfiguratorTestFixture, VerifyDefaultConfiguratorBehavior)
{
  instrumentation_scope::InstrumentationScope *scope = GetParam();
  instrumentation_scope::ScopeConfigurator<trace_sdk::TracerConfig> default_configurator =
      instrumentation_scope::ScopeConfigurator<trace_sdk::TracerConfig>::Builder(
          trace_sdk::TracerConfig::Default())
          .Build();

  ASSERT_EQ(default_configurator.ComputeConfig(*scope), trace_sdk::TracerConfig::Default());
}

INSTANTIATE_TEST_SUITE_P(InstrumentationScopes,
                         DefaultTracerConfiguratorTestFixture,
                         ::testing::ValuesIn(GetInstrumentationScopes()));

}  // namespace
