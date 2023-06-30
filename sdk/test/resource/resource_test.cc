// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/sdk/common/attribute_utils.h"
#include "opentelemetry/sdk/resource/resource_detector.h"
#include "opentelemetry/sdk/resource/semantic_conventions.h"
#include "opentelemetry/sdk/version/version.h"

#include <cstdlib>
#include <map>
#include <string>

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
  TestResource(ResourceAttributes attributes = ResourceAttributes(), std::string schema_url = {})
      : Resource(attributes, schema_url)
  {}
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
