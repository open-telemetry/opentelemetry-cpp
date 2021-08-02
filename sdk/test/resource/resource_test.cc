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
#  define putenv _putenv
#endif

using namespace opentelemetry::sdk::resource;

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
      {OTEL_CPP_GET_ATTR(AttrTelemetrySdkLanguage), "cpp"},
      {OTEL_CPP_GET_ATTR(AttrTelemetrySdkName), "opentelemetry"},
      {OTEL_CPP_GET_ATTR(AttrTelemetrySdkVersion), OPENTELEMETRY_SDK_VERSION},
      {OTEL_CPP_GET_ATTR(AttrServiceName), "unknown_service"}};

  ResourceAttributes attributes = {
      {"service", "backend"}, {"version", (uint32_t)1}, {"cost", 234.23}};
  auto resource            = Resource::Create(attributes);
  auto received_attributes = resource.GetAttributes();
  for (auto &e : received_attributes)
  {
    EXPECT_TRUE(expected_attributes.find(e.first) != expected_attributes.end());
    if (expected_attributes.find(e.first) != expected_attributes.end())
      if (e.first == "version")
        EXPECT_EQ(opentelemetry::nostd::get<uint32_t>(expected_attributes.find(e.first)->second),
                  opentelemetry::nostd::get<uint32_t>(e.second));
      else if (e.first == "cost")
        EXPECT_EQ(opentelemetry::nostd::get<double>(expected_attributes.find(e.first)->second),
                  opentelemetry::nostd::get<double>(e.second));
      else
        EXPECT_EQ(opentelemetry::nostd::get<std::string>(expected_attributes.find(e.first)->second),
                  opentelemetry::nostd::get<std::string>(e.second));
  }
  EXPECT_EQ(received_attributes.size(), expected_attributes.size());  // for missing service.name
}

TEST(ResourceTest, create_with_servicename)
{
  ResourceAttributes expected_attributes = {
      {"version", (uint32_t)1},
      {"cost", 234.23},
      {OTEL_CPP_GET_ATTR(AttrTelemetrySdkLanguage), "cpp"},
      {OTEL_CPP_GET_ATTR(AttrTelemetrySdkName), "opentelemetry"},
      {OTEL_CPP_GET_ATTR(AttrTelemetrySdkVersion), OPENTELEMETRY_SDK_VERSION},
      {OTEL_CPP_GET_ATTR(AttrServiceName), "backend"},
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
        EXPECT_EQ(opentelemetry::nostd::get<uint32_t>(expected_attributes.find(e.first)->second),
                  opentelemetry::nostd::get<uint32_t>(e.second));
      else if (e.first == "cost")
        EXPECT_EQ(opentelemetry::nostd::get<double>(expected_attributes.find(e.first)->second),
                  opentelemetry::nostd::get<double>(e.second));
      else
        EXPECT_EQ(opentelemetry::nostd::get<std::string>(expected_attributes.find(e.first)->second),
                  opentelemetry::nostd::get<std::string>(e.second));
    }
  }
  EXPECT_EQ(received_attributes.size(), expected_attributes.size());  // for missing service.name
}

TEST(ResourceTest, create_with_emptyatrributes)
{
  ResourceAttributes expected_attributes = {
      {OTEL_CPP_GET_ATTR(AttrTelemetrySdkLanguage), "cpp"},
      {OTEL_CPP_GET_ATTR(AttrTelemetrySdkName), "opentelemetry"},
      {OTEL_CPP_GET_ATTR(AttrTelemetrySdkVersion), OPENTELEMETRY_SDK_VERSION},
      {OTEL_CPP_GET_ATTR(AttrServiceName), "unknown_service"},
  };
  ResourceAttributes attributes = {};
  auto resource                 = Resource::Create(attributes);
  auto received_attributes      = resource.GetAttributes();
  for (auto &e : received_attributes)
  {
    EXPECT_TRUE(expected_attributes.find(e.first) != expected_attributes.end());
    if (expected_attributes.find(e.first) != expected_attributes.end())
      EXPECT_EQ(opentelemetry::nostd::get<std::string>(expected_attributes.find(e.first)->second),
                opentelemetry::nostd::get<std::string>(e.second));
  }
  EXPECT_EQ(received_attributes.size(), expected_attributes.size());  // for missing service.name
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
      EXPECT_EQ(expected_attributes.find(e.first)->second,
                opentelemetry::nostd::get<std::string>(e.second));
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
      EXPECT_EQ(expected_attributes.find(e.first)->second,
                opentelemetry::nostd::get<std::string>(e.second));
    }
  }
  EXPECT_EQ(received_attributes.size(), expected_attributes.size());
}

#ifndef NO_GETENV
TEST(ResourceTest, OtelResourceDetector)
{
  std::map<std::string, std::string> expected_attributes = {{"k", "v"}};

  char env[] = "OTEL_RESOURCE_ATTRIBUTES=k=v";
  putenv(env);

  OTELResourceDetector detector;
  auto resource            = detector.Detect();
  auto received_attributes = resource.GetAttributes();
  for (auto &e : received_attributes)
  {
    EXPECT_TRUE(expected_attributes.find(e.first) != expected_attributes.end());
    if (expected_attributes.find(e.first) != expected_attributes.end())
    {
      EXPECT_EQ(expected_attributes.find(e.first)->second,
                opentelemetry::nostd::get<std::string>(e.second));
    }
  }
  EXPECT_EQ(received_attributes.size(), expected_attributes.size());
#  if defined(_MSC_VER)
  putenv("OTEL_RESOURCE_ATTRIBUTES=");
#  else
  unsetenv("OTEL_RESOURCE_ATTRIBUTES");
#  endif
}

TEST(ResourceTest, OtelResourceDetectorEmptyEnv)
{
  std::map<std::string, std::string> expected_attributes = {};
#  if defined(_MSC_VER)
  putenv("OTEL_RESOURCE_ATTRIBUTES=");
#  else
  unsetenv("OTEL_RESOURCE_ATTRIBUTES");
#  endif
  OTELResourceDetector detector;
  auto resource            = detector.Detect();
  auto received_attributes = resource.GetAttributes();
  for (auto &e : received_attributes)
  {
    EXPECT_TRUE(expected_attributes.find(e.first) != expected_attributes.end());
    if (expected_attributes.find(e.first) != expected_attributes.end())
    {
      EXPECT_EQ(expected_attributes.find(e.first)->second,
                opentelemetry::nostd::get<std::string>(e.second));
    }
  }
  EXPECT_EQ(received_attributes.size(), expected_attributes.size());
}
#endif
