// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <string>

#include "opentelemetry/resource_detectors/process_detector_builder.h"
#include "opentelemetry/sdk/configuration/process_resource_detector_configuration.h"
#include "opentelemetry/sdk/configuration/registry.h"
#include "opentelemetry/sdk/resource/resource_detector.h"

TEST(ProcessDetectorBuilderTest, Register)
{
  opentelemetry::sdk::configuration::Registry registry;
  ASSERT_EQ(registry.GetProcessResourceDetectorBuilder(), nullptr);

  opentelemetry::resource_detector::ProcessDetectorBuilder::Register(&registry);
  ASSERT_NE(registry.GetProcessResourceDetectorBuilder(), nullptr);
}

TEST(ProcessDetectorBuilderTest, Build)
{
  opentelemetry::resource_detector::ProcessDetectorBuilder builder;
  opentelemetry::sdk::configuration::ProcessResourceDetectorConfiguration model;

  auto detector = builder.Build(&model);
  ASSERT_NE(detector, nullptr);
}
