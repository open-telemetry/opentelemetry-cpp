// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <memory>
#include <string>

#include "opentelemetry/baggage/baggage_context.h"
#include "opentelemetry/context/propagation/text_map_propagator.h"
#include "opentelemetry/nostd/function_ref.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/trace/propagation/b3_propagator.h"
#include "opentelemetry/trace/propagation/http_trace_context.h"
#include "opentelemetry/trace/propagation/jaeger.h"

#include "opentelemetry/sdk/configuration/configuration.h"
#include "opentelemetry/sdk/configuration/configured_sdk.h"
#include "opentelemetry/sdk/configuration/propagator_configuration.h"
#include "opentelemetry/sdk/configuration/registry.h"
#include "opentelemetry/sdk/configuration/text_map_propagator_builder.h"

namespace config_sdk = opentelemetry::sdk::configuration;

namespace
{
// ---------------------------------------------------------------------------
// PropagatorBuilderTest: Test that builders are registerd and match the expected propagator type

class PropagatorBuilderTest : public ::testing::Test
{
protected:
  void SetUp() override { registry_ = std::make_shared<config_sdk::Registry>(); }

  static std::unique_ptr<config_sdk::Configuration> MakePropagatorConfig(const std::string &name)
  {
    auto model                        = std::make_unique<config_sdk::Configuration>();
    model->propagator                 = std::make_unique<config_sdk::PropagatorConfiguration>();
    model->propagator->composite_list = name;
    return model;
  }

  std::shared_ptr<config_sdk::Registry> registry_;

  static bool HasField(const opentelemetry::context::propagation::TextMapPropagator *propagator,
                       opentelemetry::nostd::string_view key)
  {
    bool found = false;
    propagator->Fields([&](opentelemetry::nostd::string_view f) {
      if (f == key)
      {
        found = true;
      }
      return !found;
    });
    return found;
  }
};

}  // namespace

TEST(PropagatorBuilders, DefaultRegistry)
{
  auto registry = std::make_shared<config_sdk::Registry>();
  EXPECT_NE(registry->GetTextMapPropagatorBuilder("tracecontext"), nullptr);
  EXPECT_NE(registry->GetTextMapPropagatorBuilder("baggage"), nullptr);
  EXPECT_NE(registry->GetTextMapPropagatorBuilder("b3"), nullptr);
  EXPECT_NE(registry->GetTextMapPropagatorBuilder("b3multi"), nullptr);
  EXPECT_NE(registry->GetTextMapPropagatorBuilder("jaeger"), nullptr);
}

TEST_F(PropagatorBuilderTest, TraceContextPropagatorBuilder)
{
  auto model = MakePropagatorConfig("tracecontext");
  auto sdk   = config_sdk::ConfiguredSdk::Create(registry_, model);
  ASSERT_NE(sdk, nullptr);
  ASSERT_NE(sdk->propagator, nullptr);
  EXPECT_TRUE(HasField(sdk->propagator.get(), opentelemetry::trace::propagation::kTraceParent));
}

TEST_F(PropagatorBuilderTest, BaggagePropagatorBuilder)
{
  auto model = MakePropagatorConfig("baggage");
  auto sdk   = config_sdk::ConfiguredSdk::Create(registry_, model);
  ASSERT_NE(sdk, nullptr);
  ASSERT_NE(sdk->propagator, nullptr);
  EXPECT_TRUE(HasField(sdk->propagator.get(), opentelemetry::baggage::kBaggageHeader));
}

TEST_F(PropagatorBuilderTest, B3PropagatorBuilder)
{
  auto model = MakePropagatorConfig("b3");
  auto sdk   = config_sdk::ConfiguredSdk::Create(registry_, model);
  ASSERT_NE(sdk, nullptr);
  ASSERT_NE(sdk->propagator, nullptr);
  EXPECT_TRUE(
      HasField(sdk->propagator.get(), opentelemetry::trace::propagation::kB3CombinedHeader));
}

TEST_F(PropagatorBuilderTest, B3MultiPropagatorBuilder)
{
  auto model = MakePropagatorConfig("b3multi");
  auto sdk   = config_sdk::ConfiguredSdk::Create(registry_, model);
  ASSERT_NE(sdk, nullptr);
  ASSERT_NE(sdk->propagator, nullptr);
  EXPECT_TRUE(HasField(sdk->propagator.get(), opentelemetry::trace::propagation::kB3TraceIdHeader));
}

TEST_F(PropagatorBuilderTest, JaegerPropagatorBuilder)
{
  auto model = MakePropagatorConfig("jaeger");
  auto sdk   = config_sdk::ConfiguredSdk::Create(registry_, model);
  ASSERT_NE(sdk, nullptr);
  ASSERT_NE(sdk->propagator, nullptr);
  EXPECT_TRUE(
      HasField(sdk->propagator.get(), opentelemetry::trace::propagation::kJaegerTraceHeader));
}

TEST_F(PropagatorBuilderTest, UnregisteredTraceContextPropagatorBuilder)
{
  registry_->SetTextMapPropagatorBuilder("tracecontext", nullptr);
  auto model = MakePropagatorConfig("tracecontext");
  EXPECT_EQ(config_sdk::ConfiguredSdk::Create(registry_, model), nullptr);
}

TEST_F(PropagatorBuilderTest, UnregisteredBaggagePropagatorBuilder)
{
  registry_->SetTextMapPropagatorBuilder("baggage", nullptr);
  auto model = MakePropagatorConfig("baggage");
  EXPECT_EQ(config_sdk::ConfiguredSdk::Create(registry_, model), nullptr);
}

TEST_F(PropagatorBuilderTest, UnregisteredB3PropagatorBuilder)
{
  registry_->SetTextMapPropagatorBuilder("b3", nullptr);
  auto model = MakePropagatorConfig("b3");
  EXPECT_EQ(config_sdk::ConfiguredSdk::Create(registry_, model), nullptr);
}

TEST_F(PropagatorBuilderTest, UnregisteredB3MultiPropagatorBuilder)
{
  registry_->SetTextMapPropagatorBuilder("b3multi", nullptr);
  auto model = MakePropagatorConfig("b3multi");
  EXPECT_EQ(config_sdk::ConfiguredSdk::Create(registry_, model), nullptr);
}

TEST_F(PropagatorBuilderTest, UnregisteredJaegerPropagatorBuilder)
{
  registry_->SetTextMapPropagatorBuilder("jaeger", nullptr);
  auto model = MakePropagatorConfig("jaeger");
  EXPECT_EQ(config_sdk::ConfiguredSdk::Create(registry_, model), nullptr);
}
