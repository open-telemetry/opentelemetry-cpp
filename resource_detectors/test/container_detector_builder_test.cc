// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <string>

#include "opentelemetry/resource_detectors/container_detector_builder.h"
#include "opentelemetry/sdk/configuration/container_resource_detector_configuration.h"
#include "opentelemetry/sdk/configuration/registry.h"
#include "opentelemetry/sdk/resource/resource_detector.h"

TEST(ContainerDetectorBuilderTest, Register)
{
  opentelemetry::sdk::configuration::Registry registry;
  ASSERT_EQ(registry.GetContainerResourceDetectorBuilder(), nullptr);

  opentelemetry::resource_detector::ContainerDetectorBuilder::Register(&registry);
  ASSERT_NE(registry.GetContainerResourceDetectorBuilder(), nullptr);
}

TEST(ContainerDetectorBuilderTest, Build)
{
  opentelemetry::resource_detector::ContainerDetectorBuilder builder;
  opentelemetry::sdk::configuration::ContainerResourceDetectorConfiguration model;

  auto detector = builder.Build(&model);
  ASSERT_NE(detector, nullptr);
}
