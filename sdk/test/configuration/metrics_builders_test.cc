// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <memory>
#include <string>

#include "opentelemetry/sdk/configuration/metrics_builders.h"
#include "opentelemetry/sdk/configuration/registry.h"

namespace configuration = opentelemetry::sdk::configuration;

TEST(MetricsBuilders, EmptyRegistryHasNoMetricsBuilders)
{
  auto registry = std::make_shared<configuration::Registry>();

  EXPECT_EQ(registry->GetPeriodicMetricReaderBuilder(), nullptr);
  EXPECT_EQ(registry->GetMeterConfiguratorBuilder(), nullptr);
}

TEST(MetricsBuilders, RegisterDefaultMetricsBuildersFillesAllMetricsSlots)
{
  auto registry = std::make_shared<configuration::Registry>();

  configuration::RegisterDefaultMetricsBuilders(registry.get());

  EXPECT_NE(registry->GetPeriodicMetricReaderBuilder(), nullptr);
  EXPECT_NE(registry->GetMeterConfiguratorBuilder(), nullptr);
}
