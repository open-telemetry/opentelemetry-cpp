// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <string>

#include "opentelemetry/resource_detectors/host_detector_builder.h"
#include "opentelemetry/sdk/configuration/host_resource_detector_configuration.h"
#include "opentelemetry/sdk/configuration/registry.h"
#include "opentelemetry/sdk/resource/resource_detector.h"

TEST(HostDetectorBuilderTest, Register)
{
  opentelemetry::sdk::configuration::Registry registry;
  ASSERT_EQ(registry.GetHostResourceDetectorBuilder(), nullptr);

  opentelemetry::resource_detector::HostDetectorBuilder::Register(&registry);
  ASSERT_NE(registry.GetHostResourceDetectorBuilder(), nullptr);
}

TEST(HostDetectorBuilderTest, Build)
{
  opentelemetry::resource_detector::HostDetectorBuilder builder;
  opentelemetry::sdk::configuration::HostResourceDetectorConfiguration model;

  auto detector = builder.Build(&model);
  ASSERT_NE(detector, nullptr);
}
