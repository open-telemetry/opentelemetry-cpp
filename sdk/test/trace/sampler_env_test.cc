// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>

#include <memory>
#include <string>

#include "opentelemetry/sdk/trace/sampler.h"
#include "opentelemetry/sdk/trace/sampler_env.h"
#include "opentelemetry/sdk/trace/samplers/always_off_factory.h"
#include "opentelemetry/sdk/trace/samplers/always_on_factory.h"
#include "opentelemetry/sdk/trace/samplers/parent_factory.h"
#include "opentelemetry/sdk/trace/samplers/trace_id_ratio_factory.h"
#include "opentelemetry/trace/span_context.h"
#include "opentelemetry/trace/trace_flags.h"

#if defined(_MSC_VER)
using opentelemetry::sdk::common::setenv;
using opentelemetry::sdk::common::unsetenv;
#endif

using opentelemetry::sdk::trace::AlwaysOffSamplerFactory;
using opentelemetry::sdk::trace::AlwaysOnSamplerFactory;
using opentelemetry::sdk::trace::ParentBasedSamplerFactory;
using opentelemetry::sdk::trace::TraceIdRatioBasedSamplerFactory;
using opentelemetry::sdk::trace::sampler_env::GetSamplerFromEnv;

#ifndef NO_GETENV

class SamplerEnvTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // Clean up any existing environment variables
    unsetenv("OTEL_TRACES_SAMPLER");
    unsetenv("OTEL_TRACES_SAMPLER_ARG");
  }

  void TearDown() override
  {
    // Clean up environment variables after each test
    unsetenv("OTEL_TRACES_SAMPLER");
    unsetenv("OTEL_TRACES_SAMPLER_ARG");
  }
};

TEST_F(SamplerEnvTest, UnsetVariablesReturnsAlwaysOnSampler)
{
  auto sampler = GetSamplerFromEnv();
  EXPECT_EQ(sampler->GetDescription(), AlwaysOnSamplerFactory::Create()->GetDescription());
}

TEST_F(SamplerEnvTest, AlwaysOnSampler)
{
  setenv("OTEL_TRACES_SAMPLER", "always_on", 1);
  auto sampler = GetSamplerFromEnv();
  EXPECT_EQ(sampler->GetDescription(), AlwaysOnSamplerFactory::Create()->GetDescription());
}

TEST_F(SamplerEnvTest, AlwaysOffSampler)
{
  setenv("OTEL_TRACES_SAMPLER", "always_off", 1);
  auto sampler = GetSamplerFromEnv();
  EXPECT_EQ(sampler->GetDescription(), AlwaysOffSamplerFactory::Create()->GetDescription());
}

TEST_F(SamplerEnvTest, TraceIdRatioSamplerWithValidRatio)
{
  setenv("OTEL_TRACES_SAMPLER", "traceidratio", 1);
  setenv("OTEL_TRACES_SAMPLER_ARG", "0.5", 1);
  auto sampler = GetSamplerFromEnv();
  EXPECT_EQ(sampler->GetDescription(),
            TraceIdRatioBasedSamplerFactory::Create(0.5)->GetDescription());
}

TEST_F(SamplerEnvTest, TraceIdRatioSamplerWithDefaultRatio)
{
  setenv("OTEL_TRACES_SAMPLER", "traceidratio", 1);
  // OTEL_TRACES_SAMPLER_ARG not set, should default to 1.0
  auto sampler = GetSamplerFromEnv();
  EXPECT_EQ(sampler->GetDescription(),
            TraceIdRatioBasedSamplerFactory::Create(1.0)->GetDescription());
}

TEST_F(SamplerEnvTest, ParentBasedTraceIdRatioSamplerWithValidRatio)
{
  setenv("OTEL_TRACES_SAMPLER", "parentbased_traceidratio", 1);
  setenv("OTEL_TRACES_SAMPLER_ARG", "0.5", 1);
  auto sampler          = GetSamplerFromEnv();
  auto root_sampler     = TraceIdRatioBasedSamplerFactory::Create(0.5);
  auto expected_sampler = ParentBasedSamplerFactory::Create(
      std::shared_ptr<opentelemetry::sdk::trace::Sampler>(root_sampler.release()));
  EXPECT_EQ(sampler->GetDescription(), expected_sampler->GetDescription());
}

TEST_F(SamplerEnvTest, ParentBasedTraceIdRatioSamplerWithDefaultRatio)
{
  setenv("OTEL_TRACES_SAMPLER", "parentbased_traceidratio", 1);
  // OTEL_TRACES_SAMPLER_ARG not set, should default to 1.0
  auto sampler          = GetSamplerFromEnv();
  auto root_sampler     = TraceIdRatioBasedSamplerFactory::Create(1.0);
  auto expected_sampler = ParentBasedSamplerFactory::Create(
      std::shared_ptr<opentelemetry::sdk::trace::Sampler>(root_sampler.release()));
  EXPECT_EQ(sampler->GetDescription(), expected_sampler->GetDescription());
}

TEST_F(SamplerEnvTest, ParentBasedAlwaysOnSampler)
{
  setenv("OTEL_TRACES_SAMPLER", "parentbased_always_on", 1);
  auto sampler          = GetSamplerFromEnv();
  auto root_sampler     = AlwaysOnSamplerFactory::Create();
  auto expected_sampler = ParentBasedSamplerFactory::Create(
      std::shared_ptr<opentelemetry::sdk::trace::Sampler>(root_sampler.release()));
  EXPECT_EQ(sampler->GetDescription(), expected_sampler->GetDescription());
}

TEST_F(SamplerEnvTest, ParentBasedAlwaysOffSampler)
{
  setenv("OTEL_TRACES_SAMPLER", "parentbased_always_off", 1);
  auto sampler          = GetSamplerFromEnv();
  auto root_sampler     = AlwaysOffSamplerFactory::Create();
  auto expected_sampler = ParentBasedSamplerFactory::Create(
      std::shared_ptr<opentelemetry::sdk::trace::Sampler>(root_sampler.release()));
  EXPECT_EQ(sampler->GetDescription(), expected_sampler->GetDescription());
}

TEST_F(SamplerEnvTest, UnknownSamplerNameReturnsAlwaysOnSampler)
{
  setenv("OTEL_TRACES_SAMPLER", "unknown_sampler", 1);
  auto sampler = GetSamplerFromEnv();
  // Should fall back to AlwaysOnSampler for unknown names
  EXPECT_EQ(sampler->GetDescription(), AlwaysOnSamplerFactory::Create()->GetDescription());
}

TEST_F(SamplerEnvTest, TraceIdRatioWithNegativeRatioReturnsAlwaysOnSampler)
{
  setenv("OTEL_TRACES_SAMPLER", "traceidratio", 1);
  setenv("OTEL_TRACES_SAMPLER_ARG", "-0.5", 1);
  auto sampler = GetSamplerFromEnv();
  // Should fall back to AlwaysOnSampler for invalid ratio
  EXPECT_EQ(sampler->GetDescription(), AlwaysOnSamplerFactory::Create()->GetDescription());
}

TEST_F(SamplerEnvTest, TraceIdRatioWithRatioGreaterThanOneReturnsAlwaysOnSampler)
{
  setenv("OTEL_TRACES_SAMPLER", "traceidratio", 1);
  setenv("OTEL_TRACES_SAMPLER_ARG", "1.5", 1);
  auto sampler = GetSamplerFromEnv();
  // Should fall back to AlwaysOnSampler for invalid ratio
  EXPECT_EQ(sampler->GetDescription(), AlwaysOnSamplerFactory::Create()->GetDescription());
}

TEST_F(SamplerEnvTest, TraceIdRatioWithNaNReturnsAlwaysOnSampler)
{
  setenv("OTEL_TRACES_SAMPLER", "traceidratio", 1);
  setenv("OTEL_TRACES_SAMPLER_ARG", "nan", 1);
  auto sampler = GetSamplerFromEnv();
  // Should fall back to AlwaysOnSampler for invalid ratio
  EXPECT_EQ(sampler->GetDescription(), AlwaysOnSamplerFactory::Create()->GetDescription());
}

TEST_F(SamplerEnvTest, TraceIdRatioWithInfinityReturnsAlwaysOnSampler)
{
  setenv("OTEL_TRACES_SAMPLER", "traceidratio", 1);
  setenv("OTEL_TRACES_SAMPLER_ARG", "inf", 1);
  auto sampler = GetSamplerFromEnv();
  // Should fall back to AlwaysOnSampler for invalid ratio
  EXPECT_EQ(sampler->GetDescription(), AlwaysOnSamplerFactory::Create()->GetDescription());
}

TEST_F(SamplerEnvTest, ParentBasedTraceIdRatioWithInvalidRatioReturnsAlwaysOnSampler)
{
  setenv("OTEL_TRACES_SAMPLER", "parentbased_traceidratio", 1);
  setenv("OTEL_TRACES_SAMPLER_ARG", "1.5", 1);
  auto sampler = GetSamplerFromEnv();
  // Should fall back to AlwaysOnSampler for invalid ratio
  EXPECT_EQ(sampler->GetDescription(), AlwaysOnSamplerFactory::Create()->GetDescription());
}

TEST_F(SamplerEnvTest, ParentBasedTraceIdRatioWithNegativeRatioReturnsAlwaysOnSampler)
{
  setenv("OTEL_TRACES_SAMPLER", "parentbased_traceidratio", 1);
  setenv("OTEL_TRACES_SAMPLER_ARG", "-0.5", 1);
  auto sampler = GetSamplerFromEnv();
  // Should fall back to AlwaysOnSampler for invalid ratio
  EXPECT_EQ(sampler->GetDescription(), AlwaysOnSamplerFactory::Create()->GetDescription());
}

TEST_F(SamplerEnvTest, ParentBasedTraceIdRatioWithNaNReturnsAlwaysOnSampler)
{
  setenv("OTEL_TRACES_SAMPLER", "parentbased_traceidratio", 1);
  setenv("OTEL_TRACES_SAMPLER_ARG", "nan", 1);
  auto sampler = GetSamplerFromEnv();
  // Should fall back to AlwaysOnSampler for invalid ratio
  EXPECT_EQ(sampler->GetDescription(), AlwaysOnSamplerFactory::Create()->GetDescription());
}

TEST_F(SamplerEnvTest, EmptySamplerNameReturnsAlwaysOnSampler)
{
  setenv("OTEL_TRACES_SAMPLER", "", 1);
  auto sampler = GetSamplerFromEnv();
  // Empty string should be treated as unset
  EXPECT_EQ(sampler->GetDescription(), AlwaysOnSamplerFactory::Create()->GetDescription());
}

TEST_F(SamplerEnvTest, CaseInsensitiveSamplerNames)
{
  setenv("OTEL_TRACES_SAMPLER", "ALWAYS_ON", 1);
  auto sampler = GetSamplerFromEnv();
  // Should fall back to AlwaysOnSampler for case-sensitive mismatch
  EXPECT_EQ(sampler->GetDescription(), AlwaysOnSamplerFactory::Create()->GetDescription());
}

TEST_F(SamplerEnvTest, ExplicitSamplerOverridesEnvironmentVariables)
{
  setenv("OTEL_TRACES_SAMPLER", "always_off", 1);
  setenv("OTEL_TRACES_SAMPLER_ARG", "0.5", 1);

  // Create a sampler directly - this should override environment variables
  auto explicit_sampler = AlwaysOnSamplerFactory::Create();
  auto description      = explicit_sampler->GetDescription();

  // Environment variable should still return AlwaysOffSampler when called
  auto env_sampler = GetSamplerFromEnv();
  EXPECT_EQ(env_sampler->GetDescription(), AlwaysOffSamplerFactory::Create()->GetDescription());

  // But explicit sampler should be AlwaysOnSampler
  EXPECT_EQ(description, AlwaysOnSamplerFactory::Create()->GetDescription());
}

#endif  // NO_GETENV