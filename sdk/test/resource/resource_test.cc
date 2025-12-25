// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <stdint.h>
#include <cstdlib>
#include <map>
#include <string>
#include <unordered_map>
#include <utility>

#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/sdk/common/attribute_utils.h"
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

#ifndef NO_GETENV
TEST(ResourceTest, EnvEntityDetectorBasic)
{
  setenv("OTEL_ENTITIES", "service{service.name=my-app,service.instance.id=instance-1}", 1);

  EnvEntityDetector detector;
  auto resource                   = detector.Detect();
  const auto &received_attributes = resource.GetAttributes();

  EXPECT_TRUE(received_attributes.find("service.name") != received_attributes.end());
  EXPECT_EQ(nostd::get<std::string>(received_attributes.at("service.name")), "my-app");
  EXPECT_TRUE(received_attributes.find("service.instance.id") != received_attributes.end());
  EXPECT_EQ(nostd::get<std::string>(received_attributes.at("service.instance.id")), "instance-1");

  unsetenv("OTEL_ENTITIES");
}

TEST(ResourceTest, EnvEntityDetectorWithDescriptiveAttributes)
{
  setenv("OTEL_ENTITIES",
         "service{service.name=my-app,service.instance.id=instance-1}[service.version=1.0.0]", 1);

  EnvEntityDetector detector;
  auto resource                   = detector.Detect();
  const auto &received_attributes = resource.GetAttributes();

  EXPECT_EQ(nostd::get<std::string>(received_attributes.at("service.name")), "my-app");
  EXPECT_EQ(nostd::get<std::string>(received_attributes.at("service.instance.id")), "instance-1");
  EXPECT_EQ(nostd::get<std::string>(received_attributes.at("service.version")), "1.0.0");

  unsetenv("OTEL_ENTITIES");
}

TEST(ResourceTest, EnvEntityDetectorMultipleEntities)
{
  setenv("OTEL_ENTITIES",
         "service{service.name=my-app,service.instance.id=instance-1}[service.version=1.0.0];"
         "host{host.id=host-123}[host.name=web-server-01]",
         1);

  EnvEntityDetector detector;
  auto resource                   = detector.Detect();
  const auto &received_attributes = resource.GetAttributes();

  EXPECT_EQ(nostd::get<std::string>(received_attributes.at("service.name")), "my-app");
  EXPECT_EQ(nostd::get<std::string>(received_attributes.at("service.instance.id")), "instance-1");
  EXPECT_EQ(nostd::get<std::string>(received_attributes.at("service.version")), "1.0.0");
  EXPECT_EQ(nostd::get<std::string>(received_attributes.at("host.id")), "host-123");
  EXPECT_EQ(nostd::get<std::string>(received_attributes.at("host.name")), "web-server-01");

  unsetenv("OTEL_ENTITIES");
}

TEST(ResourceTest, EnvEntityDetectorPercentEncoding)
{
  setenv("OTEL_ENTITIES", "service{service.name=my%2Capp,service.instance.id=inst-1}", 1);

  EnvEntityDetector detector;
  auto resource                   = detector.Detect();
  const auto &received_attributes = resource.GetAttributes();

  EXPECT_EQ(nostd::get<std::string>(received_attributes.at("service.name")), "my,app");
  EXPECT_EQ(nostd::get<std::string>(received_attributes.at("service.instance.id")), "inst-1");

  unsetenv("OTEL_ENTITIES");
}

TEST(ResourceTest, EnvEntityDetectorDuplicateEntities)
{
  setenv("OTEL_ENTITIES",
         "service{service.name=app1}[version=1.0];service{service.name=app1}[version=2.0]", 1);

  EnvEntityDetector detector;
  auto resource                   = detector.Detect();
  const auto &received_attributes = resource.GetAttributes();

  // Last occurrence should win
  EXPECT_EQ(nostd::get<std::string>(received_attributes.at("service.name")), "app1");
  EXPECT_EQ(nostd::get<std::string>(received_attributes.at("version")), "2.0");

  unsetenv("OTEL_ENTITIES");
}

TEST(ResourceTest, EnvEntityDetectorEmptyEnv)
{
  unsetenv("OTEL_ENTITIES");
  EnvEntityDetector detector;
  auto resource                   = detector.Detect();
  const auto &received_attributes = resource.GetAttributes();

  EXPECT_TRUE(received_attributes.empty());
}

TEST(ResourceTest, EnvEntityDetectorEmptyString)
{
  setenv("OTEL_ENTITIES", "", 1);

  EnvEntityDetector detector;
  auto resource                   = detector.Detect();
  const auto &received_attributes = resource.GetAttributes();

  EXPECT_TRUE(received_attributes.empty());

  unsetenv("OTEL_ENTITIES");
}

TEST(ResourceTest, EnvEntityDetectorMalformedEntity)
{
  setenv("OTEL_ENTITIES", "service{service.name=app1};invalid{syntax;service{service.name=app2}",
         1);

  EnvEntityDetector detector;
  auto resource                   = detector.Detect();
  const auto &received_attributes = resource.GetAttributes();

  // Should process valid entities and skip malformed ones
  EXPECT_TRUE(received_attributes.find("service.name") != received_attributes.end());
  EXPECT_EQ(nostd::get<std::string>(received_attributes.at("service.name")), "app2");

  unsetenv("OTEL_ENTITIES");
}

TEST(ResourceTest, EnvEntityDetectorWhitespaceHandling)
{
  setenv("OTEL_ENTITIES", " ; service { service.name = app1 } ; ", 1);

  EnvEntityDetector detector;
  auto resource                   = detector.Detect();
  const auto &received_attributes = resource.GetAttributes();

  EXPECT_EQ(nostd::get<std::string>(received_attributes.at("service.name")), "app1");

  unsetenv("OTEL_ENTITIES");
}

TEST(ResourceTest, EnvEntityDetectorEmptySemicolons)
{
  // Test: Empty strings are allowed (leading, trailing, and consecutive semicolons are ignored)
  setenv("OTEL_ENTITIES", ";service{service.name=app1};;host{host.id=host-123};", 1);

  EnvEntityDetector detector;
  auto resource                   = detector.Detect();
  const auto &received_attributes = resource.GetAttributes();

  EXPECT_EQ(nostd::get<std::string>(received_attributes.at("service.name")), "app1");
  EXPECT_EQ(nostd::get<std::string>(received_attributes.at("host.id")), "host-123");

  unsetenv("OTEL_ENTITIES");
}

TEST(ResourceTest, EnvEntityDetectorMissingRequiredFields)
{
  // Test: Missing required fields (type or identifying attributes) - should skip entity
  setenv("OTEL_ENTITIES", "service{};host{host.id=123}", 1);

  EnvEntityDetector detector;
  auto resource                   = detector.Detect();
  const auto &received_attributes = resource.GetAttributes();

  EXPECT_TRUE(received_attributes.find("service.name") == received_attributes.end());
  EXPECT_EQ(nostd::get<std::string>(received_attributes.at("host.id")), "123");

  unsetenv("OTEL_ENTITIES");
}

TEST(ResourceTest, EnvEntityDetectorConflictingIdentifyingAttributes)
{
  // Test: Conflicting identifying attributes - last entity wins
  setenv("OTEL_ENTITIES", "service{service.name=app1};service{service.name=app2}", 1);

  EnvEntityDetector detector;
  auto resource                   = detector.Detect();
  const auto &received_attributes = resource.GetAttributes();

  // Last entity should win
  EXPECT_EQ(nostd::get<std::string>(received_attributes.at("service.name")), "app2");

  unsetenv("OTEL_ENTITIES");
}

TEST(ResourceTest, EnvEntityDetectorConflictingDescriptiveAttributes)
{
  // Test: Conflicting descriptive attributes - last entity's value is used
  setenv("OTEL_ENTITIES",
         "service{service.name=app1}[version=1.0];service{service.name=app2}[version=2.0]", 1);

  EnvEntityDetector detector;
  auto resource                   = detector.Detect();
  const auto &received_attributes = resource.GetAttributes();

  EXPECT_EQ(nostd::get<std::string>(received_attributes.at("service.name")), "app2");
  EXPECT_EQ(nostd::get<std::string>(received_attributes.at("version")), "2.0");

  unsetenv("OTEL_ENTITIES");
}

TEST(ResourceTest, EnvEntityDetectorKubernetesPod)
{
  // Test: Kubernetes pod entity example from spec
  setenv("OTEL_ENTITIES",
         "k8s.pod{k8s.pod.uid=pod-abc123}[k8s.pod.name=my-pod,k8s.pod.label.app=my-app]", 1);

  EnvEntityDetector detector;
  auto resource                   = detector.Detect();
  const auto &received_attributes = resource.GetAttributes();

  EXPECT_EQ(nostd::get<std::string>(received_attributes.at("k8s.pod.uid")), "pod-abc123");
  EXPECT_EQ(nostd::get<std::string>(received_attributes.at("k8s.pod.name")), "my-pod");
  EXPECT_EQ(nostd::get<std::string>(received_attributes.at("k8s.pod.label.app")), "my-app");

  unsetenv("OTEL_ENTITIES");
}

TEST(ResourceTest, EnvEntityDetectorContainerWithHost)
{
  // Test: Container with host (minimal descriptive attributes)
  setenv("OTEL_ENTITIES",
         "container{container.id=cont-456};host{host.id=host-789}[host.name=docker-host]", 1);

  EnvEntityDetector detector;
  auto resource                   = detector.Detect();
  const auto &received_attributes = resource.GetAttributes();

  EXPECT_EQ(nostd::get<std::string>(received_attributes.at("container.id")), "cont-456");
  EXPECT_EQ(nostd::get<std::string>(received_attributes.at("host.id")), "host-789");
  EXPECT_EQ(nostd::get<std::string>(received_attributes.at("host.name")), "docker-host");

  unsetenv("OTEL_ENTITIES");
}

TEST(ResourceTest, EnvEntityDetectorMinimalEntity)
{
  // Test: Minimal entity (only required fields)
  setenv("OTEL_ENTITIES", "service{service.name=minimal-app}", 1);

  EnvEntityDetector detector;
  auto resource                   = detector.Detect();
  const auto &received_attributes = resource.GetAttributes();

  EXPECT_EQ(nostd::get<std::string>(received_attributes.at("service.name")), "minimal-app");
  EXPECT_EQ(received_attributes.size(), 1);

  unsetenv("OTEL_ENTITIES");
}

TEST(ResourceTest, EnvEntityDetectorPercentEncodingMultiple)
{
  // Test: Multiple percent-encoded characters in one value
  setenv("OTEL_ENTITIES",
         "service{service.name=my%2Capp,service.instance.id=inst-1}[config=key%3Dvalue%5Bprod%5D]",
         1);

  EnvEntityDetector detector;
  auto resource                   = detector.Detect();
  const auto &received_attributes = resource.GetAttributes();

  EXPECT_EQ(nostd::get<std::string>(received_attributes.at("service.name")), "my,app");
  EXPECT_EQ(nostd::get<std::string>(received_attributes.at("config")), "key=value[prod]");

  unsetenv("OTEL_ENTITIES");
}

TEST(ResourceTest, EnvEntityDetectorInvalidSchemaUrl)
{
  // Test: Invalid schema URL - should log warning and ignore URL
  setenv("OTEL_ENTITIES", "service{service.name=app1}@invalid-url", 1);

  EnvEntityDetector detector;
  auto resource                   = detector.Detect();
  const auto &received_attributes = resource.GetAttributes();

  // Entity should be processed but schema URL ignored
  EXPECT_EQ(nostd::get<std::string>(received_attributes.at("service.name")), "app1");

  unsetenv("OTEL_ENTITIES");
}

TEST(ResourceTest, EnvEntityDetectorAllMalformedEntities)
{
  // Test: All entities are malformed - ParseEntities returns empty, should return empty resource
  setenv("OTEL_ENTITIES", "invalid{syntax};{missing-type};123{invalid-type}", 1);

  EnvEntityDetector detector;
  auto resource                   = detector.Detect();
  const auto &received_attributes = resource.GetAttributes();

  // All entities are invalid, so parsed_entities.empty() is true, should return empty resource
  EXPECT_TRUE(received_attributes.empty());

  unsetenv("OTEL_ENTITIES");
}

TEST(ResourceTest, EnvEntityDetectorEmptySchemaUrl)
{
  // Test: Empty schema URL - should log warning and ignore URL
  setenv("OTEL_ENTITIES", "service{service.name=app1}@", 1);

  EnvEntityDetector detector;
  auto resource                   = detector.Detect();
  const auto &received_attributes = resource.GetAttributes();

  // Entity should be processed but empty schema URL ignored
  EXPECT_EQ(nostd::get<std::string>(received_attributes.at("service.name")), "app1");

  unsetenv("OTEL_ENTITIES");
}

TEST(ResourceTest, EnvEntityDetectorValidSchemaUrl)
{
  // Test: Valid schema URL with "://" - should be accepted
  setenv("OTEL_ENTITIES", "service{service.name=app1}@https://opentelemetry.io/schemas/1.0.0", 1);

  EnvEntityDetector detector;
  auto resource                   = detector.Detect();
  const auto &received_attributes = resource.GetAttributes();

  // Entity should be processed and schema URL should be valid (not cleared)
  EXPECT_EQ(nostd::get<std::string>(received_attributes.at("service.name")), "app1");
  EXPECT_EQ(resource.GetSchemaURL(), "https://opentelemetry.io/schemas/1.0.0");
  unsetenv("OTEL_ENTITIES");
}

TEST(ResourceTest, EnvEntityDetectorSchemaUrlWithWhitespace)
{
  // Test: Schema URL with only whitespace - should be treated as empty after trim
  setenv("OTEL_ENTITIES", "service{service.name=app1}@   ", 1);

  EnvEntityDetector detector;
  auto resource                   = detector.Detect();
  const auto &received_attributes = resource.GetAttributes();

  // Entity should be processed but whitespace-only schema URL should be ignored
  EXPECT_EQ(nostd::get<std::string>(received_attributes.at("service.name")), "app1");

  unsetenv("OTEL_ENTITIES");
}

TEST(ResourceTest, EnvEntityDetectorMissingClosingBracket)
{
  // Test: Missing closing bracket ']' for desc_attrs - should be rejected
  setenv("OTEL_ENTITIES", "service{service.name=app1}[version=1.0", 1);

  EnvEntityDetector detector;
  auto resource                   = detector.Detect();
  const auto &received_attributes = resource.GetAttributes();

  // Entity with missing closing bracket should be rejected
  EXPECT_TRUE(received_attributes.empty());

  unsetenv("OTEL_ENTITIES");
}

TEST(ResourceTest, EnvEntityDetectorMissingClosingBrace)
{
  // Test: Missing closing brace '}' for id_attrs - should be rejected
  setenv("OTEL_ENTITIES", "service{service.name=app1", 1);

  EnvEntityDetector detector;
  auto resource                   = detector.Detect();
  const auto &received_attributes = resource.GetAttributes();

  // Entity with missing closing brace should be rejected
  EXPECT_TRUE(received_attributes.empty());

  unsetenv("OTEL_ENTITIES");
}

TEST(ResourceTest, EnvEntityDetectorInvalidTypeCharacters)
{
  // Test: Type with invalid characters (not alphanumeric, '.', '_', or '-')
  setenv("OTEL_ENTITIES", "service@name{service.name=app1}", 1);

  EnvEntityDetector detector;
  auto resource                   = detector.Detect();
  const auto &received_attributes = resource.GetAttributes();

  // Entity with invalid type characters should be rejected
  EXPECT_TRUE(received_attributes.empty());

  unsetenv("OTEL_ENTITIES");
}

TEST(ResourceTest, EnvEntityDetectorEmptyKeyInAttributes)
{
  // Test: Key-value pair with empty key after trimming - should be skipped
  setenv("OTEL_ENTITIES", "service{=value,service.name=app1, =another}", 1);

  EnvEntityDetector detector;
  auto resource                   = detector.Detect();
  const auto &received_attributes = resource.GetAttributes();

  EXPECT_EQ(nostd::get<std::string>(received_attributes.at("service.name")), "app1");
  // Empty key entries should not appear in attributes
  EXPECT_EQ(received_attributes.size(), 1);

  unsetenv("OTEL_ENTITIES");
}

TEST(ResourceTest, EnvEntityDetectorEmptyEntityString)
{
  // Test: Only empty entity strings - should all be skipped and return empty resource
  setenv("OTEL_ENTITIES", ";;;", 1);

  EnvEntityDetector detector;
  auto resource                   = detector.Detect();
  const auto &received_attributes = resource.GetAttributes();

  // All empty strings should be skipped, resulting in empty resource
  EXPECT_TRUE(received_attributes.empty());

  unsetenv("OTEL_ENTITIES");
}
#endif
