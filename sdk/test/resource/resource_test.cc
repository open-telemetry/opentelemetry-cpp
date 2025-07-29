// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <stdint.h>
#include <cstdlib>
#include <map>
#include <string>
#include <unordered_map>
#include <utility>
#include<fstream>

#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/sdk/common/attribute_utils.h"
#include "opentelemetry/sdk/common/container.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/sdk/resource/resource_detector.h"
#include "opentelemetry/sdk/version/version.h"
#include "opentelemetry/semconv/service_attributes.h"
#include "opentelemetry/semconv/telemetry_attributes.h"

#if defined(_MSC_VER)
#  include "opentelemetry/sdk/common/env_variables.h"
using opentelemetry::sdk::common::setenv;
using opentelemetry::sdk::common::unsetenv;
#endif

using namespace opentelemetry::sdk::resource;
namespace nostd   = opentelemetry::nostd;
namespace semconv = opentelemetry::semconv;

class TestResource : public Resource
{
public:
  TestResource(const ResourceAttributes &attributes = ResourceAttributes(),
               const std::string &schema_url        = {})
      : Resource(attributes, schema_url)
  {}
};

class TestResourceDetector : public ResourceDetector
{
public:
  TestResourceDetector() = default;
  Resource Detect() noexcept override { return Create(attributes, schema_url); }
  ResourceAttributes attributes;
  std::string schema_url;
};

TEST(ResourceTest, create_without_servicename)
{
  ResourceAttributes expected_attributes = {
      {"service", "backend"},
      {"version", static_cast<uint32_t>(1)},
      {"cost", 234.23},
      {semconv::telemetry::kTelemetrySdkLanguage, "cpp"},
      {semconv::telemetry::kTelemetrySdkName, "opentelemetry"},
      {semconv::telemetry::kTelemetrySdkVersion, OPENTELEMETRY_SDK_VERSION},
      {semconv::service::kServiceName, "unknown_service"}};

  ResourceAttributes attributes = {
      {"service", "backend"}, {"version", static_cast<uint32_t>(1)}, {"cost", 234.23}};
  auto resource            = Resource::Create(attributes);
  auto received_attributes = resource.GetAttributes();
  for (auto &e : received_attributes)
  {
    EXPECT_TRUE(expected_attributes.find(e.first) != expected_attributes.end());
    if (expected_attributes.find(e.first) != expected_attributes.end())
    {
      if (e.first == "version")
        EXPECT_EQ(nostd::get<uint32_t>(expected_attributes.find(e.first)->second),
                  nostd::get<uint32_t>(e.second));
      else if (e.first == "cost")
        EXPECT_EQ(nostd::get<double>(expected_attributes.find(e.first)->second),
                  nostd::get<double>(e.second));
      else
        EXPECT_EQ(opentelemetry::nostd::get<std::string>(expected_attributes.find(e.first)->second),
                  opentelemetry::nostd::get<std::string>(e.second));
    }
  }
  EXPECT_EQ(received_attributes.size(), expected_attributes.size());  // for missing service.name
}

TEST(ResourceTest, create_with_servicename)
{
  ResourceAttributes expected_attributes = {
      {"version", static_cast<uint32_t>(1)},
      {"cost", 234.23},
      {semconv::telemetry::kTelemetrySdkLanguage, "cpp"},
      {semconv::telemetry::kTelemetrySdkName, "opentelemetry"},
      {semconv::telemetry::kTelemetrySdkVersion, OPENTELEMETRY_SDK_VERSION},
      {semconv::service::kServiceName, "backend"},
  };
  ResourceAttributes attributes = {
      {"service.name", "backend"}, {"version", static_cast<uint32_t>(1)}, {"cost", 234.23}};
  auto resource            = Resource::Create(attributes);
  auto received_attributes = resource.GetAttributes();
  for (auto &e : received_attributes)
  {
    EXPECT_TRUE(expected_attributes.find(e.first) != expected_attributes.end());
    if (expected_attributes.find(e.first) != expected_attributes.end())
    {
      if (e.first == "version")
        EXPECT_EQ(nostd::get<uint32_t>(expected_attributes.find(e.first)->second),
                  nostd::get<uint32_t>(e.second));
      else if (e.first == "cost")
        EXPECT_EQ(nostd::get<double>(expected_attributes.find(e.first)->second),
                  nostd::get<double>(e.second));
      else
        EXPECT_EQ(nostd::get<std::string>(expected_attributes.find(e.first)->second),
                  nostd::get<std::string>(e.second));
    }
  }
  EXPECT_EQ(received_attributes.size(), expected_attributes.size());  // for missing service.name
}

TEST(ResourceTest, create_with_emptyatrributes)
{
  ResourceAttributes expected_attributes = {
      {semconv::telemetry::kTelemetrySdkLanguage, "cpp"},
      {semconv::telemetry::kTelemetrySdkName, "opentelemetry"},
      {semconv::telemetry::kTelemetrySdkVersion, OPENTELEMETRY_SDK_VERSION},
      {semconv::service::kServiceName, "unknown_service"},
  };
  ResourceAttributes attributes = {};
  auto resource                 = Resource::Create(attributes);
  auto received_attributes      = resource.GetAttributes();
  for (auto &e : received_attributes)
  {
    EXPECT_TRUE(expected_attributes.find(e.first) != expected_attributes.end());
    if (expected_attributes.find(e.first) != expected_attributes.end())
    {
      EXPECT_EQ(opentelemetry::nostd::get<std::string>(expected_attributes.find(e.first)->second),
                opentelemetry::nostd::get<std::string>(e.second));
    }
  }
  EXPECT_EQ(received_attributes.size(), expected_attributes.size());  // for missing service.name
}

TEST(ResourceTest, create_with_schemaurl)
{
  const std::string schema_url    = "https://opentelemetry.io/schemas/1.2.0";
  ResourceAttributes attributes   = {};
  auto resource                   = Resource::Create(attributes, schema_url);
  const auto &received_schema_url = resource.GetSchemaURL();

  EXPECT_EQ(received_schema_url, schema_url);
}

TEST(ResourceTest, Merge)
{
  TestResource resource1(ResourceAttributes({{"service", "backend"}}));
  TestResource resource2(ResourceAttributes({{"host", "service-host"}}));
  std::map<std::string, std::string> expected_attributes = {{"service", "backend"},
                                                            {"host", "service-host"}};

  auto merged_resource     = resource1.Merge(resource2);
  auto received_attributes = merged_resource.GetAttributes();
  for (auto &e : received_attributes)
  {
    EXPECT_TRUE(expected_attributes.find(e.first) != expected_attributes.end());
    if (expected_attributes.find(e.first) != expected_attributes.end())
    {
      EXPECT_EQ(expected_attributes.find(e.first)->second, nostd::get<std::string>(e.second));
    }
  }
  EXPECT_EQ(received_attributes.size(), expected_attributes.size());
}

TEST(ResourceTest, MergeEmptyString)
{
  TestResource resource1({{"service", "backend"}, {"host", "service-host"}});
  TestResource resource2({{"service", ""}, {"host", "another-service-host"}});
  std::map<std::string, std::string> expected_attributes = {{"service", ""},
                                                            {"host", "another-service-host"}};

  auto merged_resource     = resource1.Merge(resource2);
  auto received_attributes = merged_resource.GetAttributes();

  for (auto &e : received_attributes)
  {
    EXPECT_TRUE(expected_attributes.find(e.first) != expected_attributes.end());
    if (expected_attributes.find(e.first) != expected_attributes.end())
    {
      EXPECT_EQ(expected_attributes.find(e.first)->second, nostd::get<std::string>(e.second));
    }
  }
  EXPECT_EQ(received_attributes.size(), expected_attributes.size());
}

TEST(ResourceTest, MergeSchemaUrl)
{
  const std::string url = "https://opentelemetry.io/schemas/v3.1.4";

  TestResource resource_empty_url({}, "");
  TestResource resource_some_url({}, url);
  TestResource resource_different_url({}, "different");

  // Specified behavior:
  auto merged_both_empty = resource_empty_url.Merge(resource_empty_url);
  EXPECT_TRUE(merged_both_empty.GetSchemaURL().empty());

  auto merged_old_empty = resource_empty_url.Merge(resource_some_url);
  EXPECT_EQ(merged_old_empty.GetSchemaURL(), url);

  auto merged_updating_empty = resource_some_url.Merge(resource_empty_url);
  EXPECT_EQ(merged_updating_empty.GetSchemaURL(), url);

  auto merged_same_url = resource_some_url.Merge(resource_some_url);
  EXPECT_EQ(merged_same_url.GetSchemaURL(), url);

  // Implementation-defined behavior:
  auto merged_different_url = resource_different_url.Merge(resource_some_url);
  EXPECT_EQ(merged_different_url.GetSchemaURL(), url);
}

#ifndef NO_GETENV
TEST(ResourceTest, OtelResourceDetector)
{
  std::map<std::string, std::string> expected_attributes = {{"k", "v"}};

  setenv("OTEL_RESOURCE_ATTRIBUTES", "k=v", 1);

  OTELResourceDetector detector;
  auto resource            = detector.Detect();
  auto received_attributes = resource.GetAttributes();
  for (auto &e : received_attributes)
  {
    EXPECT_TRUE(expected_attributes.find(e.first) != expected_attributes.end());
    if (expected_attributes.find(e.first) != expected_attributes.end())
    {
      EXPECT_EQ(expected_attributes.find(e.first)->second, nostd::get<std::string>(e.second));
    }
  }
  EXPECT_EQ(received_attributes.size(), expected_attributes.size());

  unsetenv("OTEL_RESOURCE_ATTRIBUTES");
}

TEST(ResourceTest, OtelResourceDetectorServiceNameOverride)
{
  std::map<std::string, std::string> expected_attributes = {{"service.name", "new_name"}};

  setenv("OTEL_RESOURCE_ATTRIBUTES", "service.name=old_name", 1);
  setenv("OTEL_SERVICE_NAME", "new_name", 1);

  OTELResourceDetector detector;
  auto resource            = detector.Detect();
  auto received_attributes = resource.GetAttributes();
  for (auto &e : received_attributes)
  {
    EXPECT_TRUE(expected_attributes.find(e.first) != expected_attributes.end());
    if (expected_attributes.find(e.first) != expected_attributes.end())
    {
      EXPECT_EQ(expected_attributes.find(e.first)->second, nostd::get<std::string>(e.second));
    }
  }
  EXPECT_EQ(received_attributes.size(), expected_attributes.size());

  unsetenv("OTEL_SERVICE_NAME");
  unsetenv("OTEL_RESOURCE_ATTRIBUTES");
}

TEST(ResourceTest, OtelResourceDetectorEmptyEnv)
{
  std::map<std::string, std::string> expected_attributes = {};
  unsetenv("OTEL_RESOURCE_ATTRIBUTES");
  OTELResourceDetector detector;
  auto resource            = detector.Detect();
  auto received_attributes = resource.GetAttributes();
  for (auto &e : received_attributes)
  {
    EXPECT_TRUE(expected_attributes.find(e.first) != expected_attributes.end());
    if (expected_attributes.find(e.first) != expected_attributes.end())
    {
      EXPECT_EQ(expected_attributes.find(e.first)->second, nostd::get<std::string>(e.second));
    }
  }
  EXPECT_EQ(received_attributes.size(), expected_attributes.size());
}

#endif

TEST(ResourceTest, DerivedResourceDetector)
{
  TestResourceDetector detector;

  detector.attributes             = {{"key", "value"}};
  detector.schema_url             = "https://opentelemetry.io/schemas/v3.1.4";
  const auto resource             = detector.Detect();
  const auto &received_attributes = resource.GetAttributes();

  EXPECT_EQ(received_attributes.size(), 1);
  EXPECT_EQ(resource.GetSchemaURL(), detector.schema_url);
  EXPECT_TRUE(received_attributes.find("key") != received_attributes.end());
}

TEST(ResourceTest, ExctractValidContainerId)
{
  std::string line = "13:name=systemd:/podruntime/docker/kubepods/ac679f8a8319c8cf7d38e1adf263bc08d23.aaaa";
  std::string extracted_id = opentelemetry::sdk::common::ExtractContainerIDFromLine(line);
  EXPECT_EQ(std::string{"ac679f8a8319c8cf7d38e1adf263bc08d23"}, extracted_id);
}

TEST(ResourceTest, ExtractIdFromMockUpCGroupFile)
{
  const char *filename = "test_cgroup.txt";

  {
    std::ofstream outfile(filename);
    outfile << "13:name=systemd:/kuberuntime/containerd/kubepods-pod872d2066_00ef_48ea_a7d8_51b18b72d739:cri-containerd:e857a4bf05a69080a759574949d7a0e69572e27647800fa7faff6a05a8332aa1\n";
    outfile << "9:cpu:/not-a-container\n";
  }

  std::string container_id = opentelemetry::sdk::common::GetContainerIDFromCgroup(filename);
  EXPECT_EQ(container_id, std::string{"e857a4bf05a69080a759574949d7a0e69572e27647800fa7faff6a05a8332aa1"});

  std::remove(filename);
}

TEST(ResourceTest, DoesNotExtractInvalidLine)
{
  std::string line = "this line does not contain a container id";
  std::string id = opentelemetry::sdk::common::ExtractContainerIDFromLine(line);
  EXPECT_EQ(id, std::string{""});
}

TEST(ContainerIdDetectorTest, ReturnsEmptyOnNoMatch)
{
  const char *filename = "test_empty_cgroup.txt";

  {
    std::ofstream outfile(filename);
    outfile << "no container id here\n";
  }

  std::string id = opentelemetry::sdk::common::GetContainerIDFromCgroup(filename);
  EXPECT_EQ(id, std::string{""});

  std::remove(filename);  // cleanup
}
