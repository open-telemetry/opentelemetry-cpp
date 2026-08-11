// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <memory>
#include <string>

#include "opentelemetry/sdk/configuration/logs_builders.h"
#include "opentelemetry/sdk/configuration/registry.h"

namespace configuration = opentelemetry::sdk::configuration;

TEST(LogsBuilders, EmptyRegistryHasNoLogsBuilders)
{
  auto registry = std::make_shared<configuration::Registry>();

  EXPECT_EQ(registry->GetBatchLogRecordProcessorBuilder(), nullptr);
  EXPECT_EQ(registry->GetSimpleLogRecordProcessorBuilder(), nullptr);
  EXPECT_EQ(registry->GetLoggerConfiguratorBuilder(), nullptr);
}

TEST(LogsBuilders, RegisterDefaultLogsBuildersFillesAllLogsSlots)
{
  auto registry = std::make_shared<configuration::Registry>();

  configuration::RegisterDefaultLogsBuilders(registry.get());

  EXPECT_NE(registry->GetBatchLogRecordProcessorBuilder(), nullptr);
  EXPECT_NE(registry->GetSimpleLogRecordProcessorBuilder(), nullptr);
  EXPECT_NE(registry->GetLoggerConfiguratorBuilder(), nullptr);
}
