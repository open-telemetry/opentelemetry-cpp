// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>

#include <opentelemetry/resource_detectors/container_detector.h>
#include <opentelemetry/resource_detectors/container_detector_builder.h>
#include <opentelemetry/resource_detectors/env_entity_detector.h>
#include <opentelemetry/resource_detectors/process_detector.h>
#include <opentelemetry/resource_detectors/process_detector_builder.h>
#include <memory>

TEST(ResourceDetectorsInstall, ContainerResourceDetector)
{
  std::unique_ptr<opentelemetry::sdk::resource::ResourceDetector> detector =
      std::make_unique<opentelemetry::resource_detector::ContainerResourceDetector>();
  ASSERT_TRUE(detector != nullptr);
  ASSERT_NO_THROW(auto resource = detector->Detect());
}

TEST(ResourceDetectorsInstall, EnvEntityDetector)
{
  std::unique_ptr<opentelemetry::sdk::resource::ResourceDetector> detector =
      std::make_unique<opentelemetry::resource_detector::EnvEntityDetector>();
  ASSERT_TRUE(detector != nullptr);
  ASSERT_NO_THROW(auto resource = detector->Detect());
}

TEST(ResourceDetectorsInstall, ProcessResourceDetector)
{
  std::unique_ptr<opentelemetry::sdk::resource::ResourceDetector> detector =
      std::make_unique<opentelemetry::resource_detector::ProcessResourceDetector>();
  ASSERT_TRUE(detector != nullptr);
  ASSERT_NO_THROW(auto resource = detector->Detect());
}

TEST(ResourceDetectorsInstall, ContainerDetectorBuilder)
{
  auto builder = std::make_unique<opentelemetry::resource_detector::ContainerDetectorBuilder>();
  ASSERT_TRUE(builder != nullptr);

  opentelemetry::sdk::configuration::ContainerResourceDetectorConfiguration model;
  auto detector = builder->Build(&model);
  ASSERT_TRUE(detector != nullptr);
}

TEST(ResourceDetectorsInstall, ProcessDetectorBuilder)
{
  auto builder = std::make_unique<opentelemetry::resource_detector::ProcessDetectorBuilder>();
  ASSERT_TRUE(builder != nullptr);

  opentelemetry::sdk::configuration::ProcessResourceDetectorConfiguration model;
  auto detector = builder->Build(&model);
  ASSERT_TRUE(detector != nullptr);
}
