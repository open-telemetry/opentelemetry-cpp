// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <regex>
#include <string>

#ifdef _MSC_VER
#  include "opentelemetry/sdk/common/env_variables.h"
using opentelemetry::sdk::common::setenv;
using opentelemetry::sdk::common::unsetenv;
#else
#  include <cstdlib>
#endif

#include "opentelemetry/resource_detectors/detail/service_detector_utils.h"

namespace detail = opentelemetry::resource_detector::detail;

namespace
{
constexpr const char *kOtelServiceName = "OTEL_SERVICE_NAME";

bool IsUuidV4(const std::string &value)
{
  static const std::regex kUuidV4Pattern(
      "^[0-9a-f]{8}-[0-9a-f]{4}-4[0-9a-f]{3}-[89ab][0-9a-f]{3}-[0-9a-f]{12}$");
  return std::regex_match(value, kUuidV4Pattern);
}
}  // namespace

TEST(ServiceDetectorUtilsTest, GetServiceNameFromEnvironment)
{
  setenv(kOtelServiceName, "stage1-test-service", 1);

  EXPECT_EQ(detail::GetServiceName(), std::string{"stage1-test-service"});

  unsetenv(kOtelServiceName);
}

TEST(ServiceDetectorUtilsTest, GetServiceNameFallbackUsesUnknownServicePrefix)
{
  unsetenv(kOtelServiceName);

  const std::string service_name = detail::GetServiceName();
  if (service_name.rfind("unknown_service:", 0) == 0)
  {
    EXPECT_GT(service_name.size(), std::string{"unknown_service:"}.size());
  }
  else
  {
    EXPECT_EQ(service_name, std::string{"unknown_service"});
  }
}

TEST(ServiceDetectorUtilsTest, GenerateServiceInstanceIdIsUuidV4)
{
  const std::string instance_id = detail::GenerateServiceInstanceId();
  EXPECT_FALSE(instance_id.empty());
  EXPECT_TRUE(IsUuidV4(instance_id));
}

TEST(ServiceDetectorUtilsTest, GenerateServiceInstanceIdIsStableWithinProcess)
{
  const std::string first_id  = detail::GenerateServiceInstanceId();
  const std::string second_id = detail::GenerateServiceInstanceId();
  EXPECT_EQ(first_id, second_id);
}
