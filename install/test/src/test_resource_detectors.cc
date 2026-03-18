// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>

#include <opentelemetry/resource_detectors/container_detector.h>
#include <opentelemetry/resource_detectors/env_entity_detector.h>
#include <opentelemetry/resource_detectors/process_detector.h>

TEST(ResourceDetectorsInstall, ContainerResourceDetector)
{
  ASSERT_NO_THROW(auto detector = opentelemetry::resource_detector::ContainerResourceDetector();
                  auto resource = detector.Detect());
}

TEST(ResourceDetectorsInstall, ProcessResourceDetector)
{
  ASSERT_NO_THROW(auto detector = opentelemetry::resource_detector::ProcessResourceDetector();
                  auto resource = detector.Detect());
}

TEST(ResourceDetectorsInstall, EnvEntityDetector)
{
  ASSERT_NO_THROW(auto detector = opentelemetry::resource_detector::EnvEntityDetector();
                  auto resource = detector.Detect());
}
