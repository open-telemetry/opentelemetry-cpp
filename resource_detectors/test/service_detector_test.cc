// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <string>
#include <utility>

#ifdef _MSC_VER
#  include "opentelemetry/sdk/common/env_variables.h"
using opentelemetry::sdk::common::setenv;
using opentelemetry::sdk::common::unsetenv;
#else
#  include <cstdlib>
#endif

#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/resource_detectors/detail/service_detector_utils.h"
#include "opentelemetry/resource_detectors/service_detector.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/semconv/schema_url.h"
#include "opentelemetry/semconv/service_attributes.h"

namespace semconv = opentelemetry::semconv;

namespace
{
constexpr const char *kOtelServiceName = "OTEL_SERVICE_NAME";
}  // namespace

TEST(ServiceDetectorTest, DetectSetsServiceAttributes)
{
  opentelemetry::resource_detector::ServiceResourceDetector detector;
  auto resource     = detector.Detect();
  const auto &attrs = resource.GetAttributes();

  auto name_it = attrs.find(semconv::service::kServiceName);
  ASSERT_NE(name_it, attrs.end());
  EXPECT_FALSE(opentelemetry::nostd::get<std::string>(name_it->second).empty());

  auto instance_id_it = attrs.find(semconv::service::kServiceInstanceId);
  ASSERT_NE(instance_id_it, attrs.end());
  EXPECT_EQ(opentelemetry::nostd::get<std::string>(instance_id_it->second),
            opentelemetry::resource_detector::detail::GenerateServiceInstanceId());

  EXPECT_EQ(resource.GetSchemaURL(), std::string{semconv::kSchemaUrl});
}

TEST(ServiceDetectorTest, DetectUsesServiceNameFromEnvironment)
{
  setenv(kOtelServiceName, "stage2-detector-service", 1);

  opentelemetry::resource_detector::ServiceResourceDetector detector;
  auto resource     = detector.Detect();
  const auto &attrs = resource.GetAttributes();

  auto name_it = attrs.find(semconv::service::kServiceName);
  ASSERT_NE(name_it, attrs.end());
  EXPECT_EQ(opentelemetry::nostd::get<std::string>(name_it->second),
            std::string{"stage2-detector-service"});

  unsetenv(kOtelServiceName);
}

TEST(ServiceDetectorTest, DetectUsesServiceNameFallbackWhenEnvUnset)
{
  unsetenv(kOtelServiceName);

  opentelemetry::resource_detector::ServiceResourceDetector detector;
  auto resource     = detector.Detect();
  const auto &attrs = resource.GetAttributes();

  auto name_it = attrs.find(semconv::service::kServiceName);
  ASSERT_NE(name_it, attrs.end());
  EXPECT_EQ(opentelemetry::nostd::get<std::string>(name_it->second),
            opentelemetry::resource_detector::detail::GetServiceName());
}
