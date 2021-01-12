#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/common/key_value_iterable_view.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/common/attribute_utils.h"
#include "opentelemetry/sdk/resource/resource_detector.h"

#include <cstdlib>
#include <string>
#include <unordered_map>

#include <gtest/gtest.h>

TEST(ResourceTest, create)
{

  std::map<std::string, std::string> expected_attributes = {
      {"service", "backend"},
      {"version", "1"},
      {"cost", "234.23"},
      {"telemetry.sdk.language", "cpp"},
      {"telemetry.sdk.name", "opentelemetry"},
      {"telemetry.sdk.version", OPENTELEMETRY_SDK_VERSION}};
  auto resource = opentelemetry::sdk::resource::Resource::Create(
      {{"service", "backend"}, {"version", "1"}, {"cost", "234.23"}});
  auto received_attributes = resource->GetAttributes();

  for (auto &e : received_attributes)
  {
    EXPECT_EQ(expected_attributes[e.first], opentelemetry::nostd::get<std::string>(e.second));
  }
  EXPECT_EQ(received_attributes.size(), expected_attributes.size());

  opentelemetry::sdk::resource::ResourceAttributes attributes = {
      {"service", "backend"}, {"version", "1"}, {"cost", "234.23"}};
  auto resource2            = opentelemetry::sdk::resource::Resource::Create(attributes);
  auto received_attributes2 = resource2->GetAttributes();
  for (auto &e : received_attributes2)
  {
    EXPECT_TRUE(expected_attributes.find(e.first) != expected_attributes.end());
    if (expected_attributes.find(e.first) != expected_attributes.end())
      EXPECT_EQ(expected_attributes.find(e.first)->second,
                opentelemetry::nostd::get<std::string>(e.second));
  }
  EXPECT_EQ(received_attributes2.size(), expected_attributes.size());
}

TEST(ResourceTest, Merge)
{
  std::map<std::string, std::string> expected_attributes = {{"service", "backend"},
                                                            {"host", "service-host"}};
  opentelemetry::sdk::resource::Resource resource1(
      opentelemetry::sdk::resource::ResourceAttributes({{"service", "backend"}}));
  opentelemetry::sdk::resource::Resource resource2(
      opentelemetry::sdk::resource::ResourceAttributes({{"host", "service-host"}}));

  auto merged_resource     = resource1.Merge(resource2);
  auto received_attributes = merged_resource->GetAttributes();
  for (auto &e : received_attributes)
  {
    EXPECT_TRUE(expected_attributes.find(e.first) != expected_attributes.end());
    if (expected_attributes.find(e.first) != expected_attributes.end())
      EXPECT_EQ(expected_attributes.find(e.first)->second,
                opentelemetry::nostd::get<std::string>(e.second));
  }
  EXPECT_EQ(received_attributes.size(), expected_attributes.size());
}

TEST(ResourceTest, MergeEmptyString)
{
  std::map<std::string, std::string> expected_attributes = {{"service", "backend"},
                                                            {"host", "service-host"}};
  opentelemetry::sdk::resource::Resource resource1({{"service", ""}, {"host", "service-host"}});
  opentelemetry::sdk::resource::Resource resource2(
      {{"service", "backend"}, {"host", "another-service-host"}});

  auto merged_resource     = resource1.Merge(resource2);
  auto received_attributes = merged_resource->GetAttributes();
}

// this test uses putenv to set the env variable - this is not available on windows
#ifdef __linux__
TEST(ResourceTest, OtelResourceDetector)
{
  std::map<std::string, std::string> expected_attributes = {{"k", "v"}};

  char env[] = "OTEL_RESOURCE_ATTRIBUTES=k=v";
  putenv(env);

  opentelemetry::sdk::resource::OTELResourceDetector detector;
  auto resource            = detector.Detect();
  auto received_attributes = resource->GetAttributes();
  for (auto &e : received_attributes)
  {
    EXPECT_TRUE(expected_attributes.find(e.first) != expected_attributes.end());
    if (expected_attributes.find(e.first) != expected_attributes.end())
      EXPECT_EQ(expected_attributes.find(e.first)->second,
                opentelemetry::nostd::get<std::string>(e.second));
  }
  EXPECT_EQ(received_attributes.size(), expected_attributes.size());
  char env2[] = "OTEL_RESOURCE_ATTRIBUTES=";
  putenv(env2);
}

TEST(ResourceTest, OtelResourceDetectorEmptyEnv)
{
  std::map<std::string, std::string> expected_attributes = {};
  char env[]                                             = "OTEL_RESOURCE_ATTRIBUTES=";
  putenv(env);
  opentelemetry::sdk::resource::OTELResourceDetector detector;
  auto resource            = detector.Detect();
  auto received_attributes = resource->GetAttributes();
  for (auto &e : received_attributes)
  {
    EXPECT_TRUE(expected_attributes.find(e.first) != expected_attributes.end());
    if (expected_attributes.find(e.first) != expected_attributes.end())
      EXPECT_EQ(expected_attributes.find(e.first)->second,
                opentelemetry::nostd::get<std::string>(e.second));
  }
  EXPECT_EQ(received_attributes.size(), expected_attributes.size());
}

#endif