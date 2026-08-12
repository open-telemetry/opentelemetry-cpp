// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <memory>

#include <opentelemetry/resource_detectors/container_detector_builder.h>
#include <opentelemetry/resource_detectors/process_detector_builder.h>

TEST(ResourceDetectorsBuilderInstall, ContainerDetectorBuilder)
{
  auto builder = std::make_unique<opentelemetry::resource_detector::ContainerDetectorBuilder>();
  ASSERT_TRUE(builder != nullptr);

  opentelemetry::sdk::configuration::ContainerResourceDetectorConfiguration model;
  auto detector = builder->Build(&model);
  ASSERT_TRUE(detector != nullptr);
}

TEST(ResourceDetectorsBuilderInstall, ProcessDetectorBuilder)
{
  auto builder = std::make_unique<opentelemetry::resource_detector::ProcessDetectorBuilder>();
  ASSERT_TRUE(builder != nullptr);

  opentelemetry::sdk::configuration::ProcessResourceDetectorConfiguration model;
  auto detector = builder->Build(&model);
  ASSERT_TRUE(detector != nullptr);
}
