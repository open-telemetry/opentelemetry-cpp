// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/common/key_value_iterable_view.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/common/attribute_utils.h"
#include "opentelemetry/sdk/resource/resource_detector.h"
#include "opentelemetry/sdk/resource/semantic_conventions.h"

#include <cstdlib>
#include <string>
#include <unordered_map>

#include <gtest/gtest.h>

#if defined(_MSC_VER)
#  include "opentelemetry/sdk/common/env_variables.h"
using opentelemetry::sdk::common::setenv;
using opentelemetry::sdk::common::unsetenv;
#endif

using namespace opentelemetry::sdk::resource;
namespace nostd = opentelemetry::nostd;

class TestResource : public Resource
{
public:
  TestResource(ResourceAttributes attributes = ResourceAttributes()) : Resource(attributes) {}
};

TEST(ResourceTest, create_without_servicename)
{
  ResourceAttributes expected_attributes = {
      {"service", "backend"},
      {"version", (uint32_t)1},
      {"cost", 234.23},
      {SemanticConventions::kTelemetrySdkLanguage, "cpp"},
      {SemanticConventions::kTelemetrySdkName, "opentelemetry"},
      {SemanticConventions::kTelemetrySdkVersion, OPENTELEMETRY_SDK_VERSION},
      {SemanticConventions::kServiceName, "unknown_service"}};

  ResourceAttributes attributes = {
      {"service", "backend"}, {"version", (uint32_t)1}, {"cost", 234.23}};
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
      {"version", (uint32_t)1},
      {"cost", 234.23},
      {SemanticConventions::kTelemetrySdkLanguage, "cpp"},
      {SemanticConventions::kTelemetrySdkName, "opentelemetry"},
      {SemanticConventions::kTelemetrySdkVersion, OPENTELEMETRY_SDK_VERSION},
      {SemanticConventions::kServiceName, "backend"},
  };
  ResourceAttributes attributes = {
      {"service.name", "backend"}, {"version", (uint32_t)1}, {"cost", 234.23}};
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
      {SemanticConventions::kTelemetrySdkLanguage, "cpp"},
      {SemanticConventions::kTelemetrySdkName, "opentelemetry"},
      {SemanticConventions::kTelemetrySdkVersion, OPENTELEMETRY_SDK_VERSION},
      {SemanticConventions::kServiceName, "unknown_service"},
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
  const std::string schema_url  = "https://opentelemetry.io/schemas/1.2.0";
  ResourceAttributes attributes = {};
  auto resource                 = Resource::Create(attributes, schema_url);
  auto received_schema_url      = resource.GetSchemaURL();

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

TEST(ResourceTest, Merge_with_schemaurl)
{
  const std::string schema_url       = "https://opentelemetry.io/schemas/1.2.0";
  const std::string schema_url1      = "https://opentelemetry.io/schemas/1.2.1";
  const std::string empty_schema_url = "";

  ResourceAttributes attributes = {};

  auto resource_00       = Resource::Create(attributes, empty_schema_url);
  auto resource_01       = Resource::Create(attributes, schema_url1);
  auto merged_resource_0 = resource_00.Merge(resource_01);
  auto new_schema_url_0  = merged_resource_0.GetSchemaURL();
  EXPECT_EQ(new_schema_url_0, schema_url1);

  auto resource_10       = Resource::Create(attributes, schema_url);
  auto resource_11       = Resource::Create(attributes, empty_schema_url);
  auto merged_resource_1 = resource_10.Merge(resource_11);
  auto new_schema_url_1  = merged_resource_1.GetSchemaURL();
  EXPECT_EQ(new_schema_url_1, schema_url);

  auto resource_20       = Resource::Create(attributes, empty_schema_url);
  auto resource_21       = Resource::Create(attributes, empty_schema_url);
  auto merged_resource_2 = resource_20.Merge(resource_21);
  auto new_schema_url_2  = merged_resource_2.GetSchemaURL();
  EXPECT_EQ(new_schema_url_2, empty_schema_url);

  auto resource_30       = Resource::Create(attributes, schema_url);
  auto resource_31       = Resource::Create(attributes, schema_url);
  auto merged_resource_3 = resource_30.Merge(resource_31);
  auto new_schema_url_3  = merged_resource_3.GetSchemaURL();
  EXPECT_EQ(new_schema_url_3, schema_url);

  auto resource_40       = Resource::Create(attributes, schema_url);
  auto resource_41       = Resource::Create(attributes, schema_url1);
  auto merged_resource_4 = resource_40.Merge(resource_41);
  auto new_schema_url_4  = merged_resource_4.GetSchemaURL();
  EXPECT_EQ(new_schema_url_4, schema_url1);
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
