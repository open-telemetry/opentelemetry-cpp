// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>

#include <opentelemetry/resource_detectors/container_detector.h>
#include <opentelemetry/resource_detectors/env_entity_detector.h>
#include <opentelemetry/resource_detectors/process_detector.h>
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
