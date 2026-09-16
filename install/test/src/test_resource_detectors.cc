// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>

#include <opentelemetry/nostd/variant.h>
#include <opentelemetry/resource_detectors/container_detector.h>
#include <opentelemetry/resource_detectors/container_detector_builder.h>
#include <opentelemetry/resource_detectors/env_entity_detector.h>
#include <opentelemetry/resource_detectors/host_detector.h>
#include <opentelemetry/resource_detectors/host_detector_builder.h>
#include <opentelemetry/resource_detectors/process_detector.h>
#include <opentelemetry/resource_detectors/process_detector_builder.h>
#include <opentelemetry/resource_detectors/service_detector.h>
#include <opentelemetry/resource_detectors/service_detector_builder.h>
#include <opentelemetry/semconv/service_attributes.h>
#include <memory>
#include <string>

namespace semconv = opentelemetry::semconv;

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

TEST(ResourceDetectorsInstall, HostResourceDetector)
{
  std::unique_ptr<opentelemetry::sdk::resource::ResourceDetector> detector =
      std::make_unique<opentelemetry::resource_detector::HostResourceDetector>();
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

TEST(ResourceDetectorsInstall, ServiceResourceDetector)
{
  std::unique_ptr<opentelemetry::sdk::resource::ResourceDetector> detector =
      std::make_unique<opentelemetry::resource_detector::ServiceResourceDetector>();
  ASSERT_TRUE(detector != nullptr);

  auto resource     = detector->Detect();
  const auto &attrs = resource.GetAttributes();

  auto name_it = attrs.find(semconv::service::kServiceName);
  ASSERT_NE(name_it, attrs.end());
  EXPECT_FALSE(opentelemetry::nostd::get<std::string>(name_it->second).empty());

  auto instance_id_it = attrs.find(semconv::service::kServiceInstanceId);
  ASSERT_NE(instance_id_it, attrs.end());
  EXPECT_FALSE(opentelemetry::nostd::get<std::string>(instance_id_it->second).empty());
}

TEST(ResourceDetectorsInstall, ContainerDetectorBuilder)
{
  auto builder = std::make_unique<opentelemetry::resource_detector::ContainerDetectorBuilder>();
  ASSERT_TRUE(builder != nullptr);

  opentelemetry::sdk::configuration::ContainerResourceDetectorConfiguration model;
  auto detector = builder->Build(&model);
  ASSERT_TRUE(detector != nullptr);
}

TEST(ResourceDetectorsInstall, HostDetectorBuilder)
{
  auto builder = std::make_unique<opentelemetry::resource_detector::HostDetectorBuilder>();
  ASSERT_TRUE(builder != nullptr);

  opentelemetry::sdk::configuration::HostResourceDetectorConfiguration model;
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

TEST(ResourceDetectorsInstall, ServiceDetectorBuilder)
{
  auto builder = std::make_unique<opentelemetry::resource_detector::ServiceDetectorBuilder>();
  ASSERT_TRUE(builder != nullptr);

  opentelemetry::sdk::configuration::ServiceResourceDetectorConfiguration model;
  auto detector = builder->Build(&model);
  ASSERT_TRUE(detector != nullptr);
  ASSERT_NO_THROW(auto resource = detector->Detect());
}
