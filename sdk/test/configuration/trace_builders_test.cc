// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <memory>
#include <string>

#include "opentelemetry/sdk/configuration/registry.h"
#include "opentelemetry/sdk/configuration/trace_builders.h"

namespace configuration = opentelemetry::sdk::configuration;

TEST(TraceBuilders, EmptyRegistryHasNoTraceBuilders)
{
  auto registry = std::make_shared<configuration::Registry>();

  EXPECT_EQ(registry->GetAlwaysOnSamplerBuilder(), nullptr);
  EXPECT_EQ(registry->GetAlwaysOffSamplerBuilder(), nullptr);
  EXPECT_EQ(registry->GetTraceIdRatioBasedSamplerBuilder(), nullptr);
  EXPECT_EQ(registry->GetProbabilitySamplerBuilder(), nullptr);
  EXPECT_EQ(registry->GetParentBasedSamplerBuilder(), nullptr);
  EXPECT_EQ(registry->GetJaegerRemoteSamplerBuilder(), nullptr);
  EXPECT_EQ(registry->GetBatchSpanProcessorBuilder(), nullptr);
  EXPECT_EQ(registry->GetSimpleSpanProcessorBuilder(), nullptr);
  EXPECT_EQ(registry->GetTracerConfiguratorBuilder(), nullptr);
  EXPECT_EQ(registry->GetComposableAlwaysOnSamplerBuilder(), nullptr);
  EXPECT_EQ(registry->GetComposableAlwaysOffSamplerBuilder(), nullptr);
  EXPECT_EQ(registry->GetComposableProbabilitySamplerBuilder(), nullptr);
  EXPECT_EQ(registry->GetComposableParentThresholdSamplerBuilder(), nullptr);
  EXPECT_EQ(registry->GetComposableRuleBasedSamplerBuilder(), nullptr);
  EXPECT_EQ(registry->GetCompositeSamplerBuilder(), nullptr);
}

TEST(TraceBuilders, RegisterDefaultTraceBuildersFillsAllTraceSlots)
{
  auto registry = std::make_shared<configuration::Registry>();

  configuration::RegisterDefaultTraceBuilders(registry.get());

  EXPECT_NE(registry->GetAlwaysOnSamplerBuilder(), nullptr);
  EXPECT_NE(registry->GetAlwaysOffSamplerBuilder(), nullptr);
  EXPECT_NE(registry->GetTraceIdRatioBasedSamplerBuilder(), nullptr);
  EXPECT_NE(registry->GetProbabilitySamplerBuilder(), nullptr);
  EXPECT_NE(registry->GetParentBasedSamplerBuilder(), nullptr);
  EXPECT_NE(registry->GetJaegerRemoteSamplerBuilder(), nullptr);
  EXPECT_NE(registry->GetBatchSpanProcessorBuilder(), nullptr);
  EXPECT_NE(registry->GetSimpleSpanProcessorBuilder(), nullptr);
  EXPECT_NE(registry->GetTracerConfiguratorBuilder(), nullptr);
  EXPECT_NE(registry->GetComposableAlwaysOnSamplerBuilder(), nullptr);
  EXPECT_NE(registry->GetComposableAlwaysOffSamplerBuilder(), nullptr);
  EXPECT_NE(registry->GetComposableProbabilitySamplerBuilder(), nullptr);
  EXPECT_NE(registry->GetComposableParentThresholdSamplerBuilder(), nullptr);
  EXPECT_NE(registry->GetComposableRuleBasedSamplerBuilder(), nullptr);
  EXPECT_NE(registry->GetCompositeSamplerBuilder(), nullptr);
}
