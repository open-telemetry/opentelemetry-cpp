// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <string>

#include "opentelemetry/resource_detectors/service_detector_builder.h"
#include "opentelemetry/sdk/configuration/registry.h"
#include "opentelemetry/sdk/configuration/service_resource_detector_configuration.h"
#include "opentelemetry/sdk/resource/resource_detector.h"

TEST(ServiceDetectorBuilderTest, Register)
{
  opentelemetry::sdk::configuration::Registry registry;
  ASSERT_EQ(registry.GetServiceResourceDetectorBuilder(), nullptr);

  opentelemetry::resource_detector::ServiceDetectorBuilder::Register(&registry);
  ASSERT_NE(registry.GetServiceResourceDetectorBuilder(), nullptr);
}

TEST(ServiceDetectorBuilderTest, Build)
{
  opentelemetry::resource_detector::ServiceDetectorBuilder builder;
  opentelemetry::sdk::configuration::ServiceResourceDetectorConfiguration model;

  auto detector = builder.Build(&model);
  ASSERT_NE(detector, nullptr);
}
