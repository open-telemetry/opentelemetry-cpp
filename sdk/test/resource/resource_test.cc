// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <cstdint>
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

#include "src/resource/detail/percent_decode.h"

#if defined(_MSC_VER)
#  include "opentelemetry/sdk/common/env_variables.h"
using opentelemetry::sdk::common::setenv;
using opentelemetry::sdk::common::unsetenv;
#endif

using namespace opentelemetry::sdk::resource;
namespace nostd   = opentelemetry::nostd;
namespace semconv = opentelemetry::semconv;

namespace
{

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

TEST(ResourceTest, PercentDecode)
{
  using opentelemetry::sdk::resource::detail::PercentDecode;

  EXPECT_EQ(PercentDecode("hello%20world"), "hello world");
  EXPECT_EQ(PercentDecode("a%2Cb"), "a,b");
  EXPECT_EQ(PercentDecode("100%25"), "100%");
  EXPECT_EQ(PercentDecode("%41%42%43"), "ABC");

  EXPECT_EQ(PercentDecode("100%"), "100%");
  EXPECT_EQ(PercentDecode("50%z"), "50%z");
  EXPECT_EQ(PercentDecode("%GG"), "%GG");
  EXPECT_EQ(PercentDecode("plain"), "plain");
}

TEST(ResourceTest, OtelResourceDetectorPercentDecodesValues)
{
  std::map<std::string, std::string> expected_attributes = {
      {"key1", "hello world"}, {"key2", "a,b"}, {"key3", "100%"}};

  setenv("OTEL_RESOURCE_ATTRIBUTES", "key1=hello%20world,key2=a%2Cb,key3=100%25", 1);

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

TEST(ResourceTest, OtelResourceDetectorMalformedEscapeLeftAsIs)
{
  std::map<std::string, std::string> expected_attributes = {{"key", "100%"}, {"bad", "50%z"}};

  setenv("OTEL_RESOURCE_ATTRIBUTES", "key=100%,bad=50%z", 1);

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

TEST(ResourceTest, EmptyHasNoEntities)
{
  Resource empty;
  EXPECT_TRUE(empty.GetEntities().empty());
  EXPECT_TRUE(empty.GetUnassociatedAttributes().empty());
  EXPECT_EQ(empty.GetUnassociatedAttributes(), empty.GetAttributes());

  Resource &get_empty = Resource::GetEmpty();
  EXPECT_TRUE(get_empty.GetEntities().empty());
  EXPECT_TRUE(get_empty.GetUnassociatedAttributes().empty());
  EXPECT_EQ(get_empty.GetUnassociatedAttributes(), get_empty.GetAttributes());
}

TEST(ResourceTest, ConstructFromAttributes)
{
  ResourceAttributes attributes = {{"service", "backend"}, {"host", "service-host"}};
  Resource resource(attributes);

  EXPECT_TRUE(resource.GetEntities().empty());
  EXPECT_EQ(resource.GetUnassociatedAttributes(), attributes);
  EXPECT_EQ(resource.GetAttributes(), resource.GetUnassociatedAttributes());
  EXPECT_TRUE(resource.GetSchemaURL().empty());
}

TEST(ResourceTest, ConstructFromAttributesAndSchemaUrl)
{
  ResourceAttributes attributes = {{"service", "backend"}};
  const std::string schema_url  = "https://opentelemetry.io/schemas/1.2.0";
  Resource resource(attributes, schema_url);

  EXPECT_TRUE(resource.GetEntities().empty());
  EXPECT_EQ(resource.GetUnassociatedAttributes(), attributes);
  EXPECT_EQ(resource.GetAttributes(), resource.GetUnassociatedAttributes());
  EXPECT_EQ(resource.GetSchemaURL(), schema_url);
}

TEST(ResourceTest, GetDefaultHasNoEntities)
{
  Resource &resource = Resource::GetDefault();
  EXPECT_TRUE(resource.GetEntities().empty());
  EXPECT_EQ(resource.GetUnassociatedAttributes(), resource.GetAttributes());
  EXPECT_FALSE(resource.GetAttributes().empty());
}

TEST(ResourceTest, CreateUnassociatedMatchesFlattened)
{
  ResourceAttributes attributes = {{"service", "backend"}};
  auto without_name             = Resource::Create(attributes);
  EXPECT_TRUE(without_name.GetEntities().empty());
  EXPECT_EQ(without_name.GetUnassociatedAttributes(), without_name.GetAttributes());
  EXPECT_EQ(
      nostd::get<std::string>(without_name.GetAttributes().at(semconv::service::kServiceName)),
      "unknown_service");

  ResourceAttributes with_name = {{"service.name", "backend"}};
  auto named                   = Resource::Create(with_name);
  EXPECT_TRUE(named.GetEntities().empty());
  EXPECT_EQ(named.GetUnassociatedAttributes(), named.GetAttributes());
  EXPECT_EQ(nostd::get<std::string>(named.GetAttributes().at(semconv::service::kServiceName)),
            "backend");
}

TEST(ResourceTest, MergeUnassociatedMatchesFlattened)
{
  TestResource resource1(ResourceAttributes({{"service", "backend"}}));
  TestResource resource2(ResourceAttributes({{"host", "service-host"}}));
  auto merged = resource1.Merge(resource2);

  EXPECT_TRUE(merged.GetEntities().empty());
  EXPECT_EQ(merged.GetUnassociatedAttributes(), merged.GetAttributes());
  EXPECT_EQ(merged.GetAttributes().size(), 2);
  EXPECT_EQ(nostd::get<std::string>(merged.GetAttributes().at("service")), "backend");
  EXPECT_EQ(nostd::get<std::string>(merged.GetAttributes().at("host")), "service-host");
}

TEST(ResourceTest, CopyAndAssignmentPreservesNewMembers)
{
  ResourceAttributes attributes = {{"service", "backend"}};
  const std::string schema_url  = "https://opentelemetry.io/schemas/1.2.0";
  Resource original(attributes, schema_url);

  Resource copied(original);
  EXPECT_TRUE(copied.GetEntities().empty());
  EXPECT_EQ(copied.GetUnassociatedAttributes(), original.GetUnassociatedAttributes());
  EXPECT_EQ(copied.GetAttributes(), original.GetAttributes());
  EXPECT_EQ(copied.GetSchemaURL(), original.GetSchemaURL());

  Resource assigned;
  assigned = original;
  EXPECT_TRUE(assigned.GetEntities().empty());
  EXPECT_EQ(assigned.GetUnassociatedAttributes(), original.GetUnassociatedAttributes());
  EXPECT_EQ(assigned.GetAttributes(), original.GetAttributes());
  EXPECT_EQ(assigned.GetSchemaURL(), original.GetSchemaURL());
}

TEST(ResourceTest, ConstructEntitiesOnly)
{
  Entity host("host", ResourceAttributes{{"host.id", "H1"}});
  const std::string constructor_schema = "https://opentelemetry.io/schemas/1.2.0";
  Resource resource(ResourceAttributes{}, constructor_schema, {host});

  ASSERT_EQ(resource.GetEntities().size(), 1);
  EXPECT_EQ(resource.GetEntities()[0], host);
  EXPECT_TRUE(resource.GetUnassociatedAttributes().empty());
  ASSERT_EQ(resource.GetAttributes().size(), 1);
  EXPECT_EQ(nostd::get<std::string>(resource.GetAttributes().at("host.id")), "H1");
  EXPECT_TRUE(resource.GetSchemaURL().empty());
}

TEST(ResourceTest, ConstructAttributesAndEntity)
{
  ResourceAttributes attributes = {{"env", "prod"}};
  Entity host("host", ResourceAttributes{{"host.id", "H1"}},
              ResourceAttributes{{"host.name", "entity-host"}});
  Resource resource(attributes, std::string{}, {host});

  ASSERT_EQ(resource.GetEntities().size(), 1);
  EXPECT_EQ(resource.GetUnassociatedAttributes().size(), 1);
  EXPECT_EQ(nostd::get<std::string>(resource.GetUnassociatedAttributes().at("env")), "prod");
  EXPECT_TRUE(resource.GetUnassociatedAttributes().find("host.id") ==
              resource.GetUnassociatedAttributes().end());
  EXPECT_TRUE(resource.GetUnassociatedAttributes().find("host.name") ==
              resource.GetUnassociatedAttributes().end());
  EXPECT_EQ(nostd::get<std::string>(resource.GetAttributes().at("host.id")), "H1");
  EXPECT_EQ(nostd::get<std::string>(resource.GetAttributes().at("host.name")), "entity-host");
  EXPECT_EQ(nostd::get<std::string>(resource.GetAttributes().at("env")), "prod");
}

TEST(ResourceTest, ConstructLooseAttributeYieldsToEntity)
{
  ResourceAttributes attributes = {{"host.name", "loose"}};
  Entity host("host", ResourceAttributes{{"host.id", "H1"}},
              ResourceAttributes{{"host.name", "entity"}});
  Resource resource(attributes, std::string{}, {host});

  EXPECT_TRUE(resource.GetUnassociatedAttributes().empty());
  EXPECT_EQ(nostd::get<std::string>(resource.GetAttributes().at("host.name")), "entity");
  EXPECT_EQ(nostd::get<std::string>(resource.GetAttributes().at("host.id")), "H1");
}

TEST(ResourceTest, ConstructDropsInvalidEntities)
{
  Entity empty_type("", ResourceAttributes{{"service.name", "app"}});
  Entity empty_identity("host", ResourceAttributes{});
  Entity valid("service", ResourceAttributes{{"service.name", "app"}});

  Resource empty_type_only(ResourceAttributes{}, std::string{}, {empty_type});
  EXPECT_TRUE(empty_type_only.GetEntities().empty());

  Resource empty_identity_only(ResourceAttributes{}, std::string{}, {empty_identity});
  EXPECT_TRUE(empty_identity_only.GetEntities().empty());

  Resource mixed(ResourceAttributes{{"env", "prod"}}, std::string{},
                 {empty_type, valid, empty_identity});
  ASSERT_EQ(mixed.GetEntities().size(), 1);
  EXPECT_EQ(mixed.GetEntities()[0], valid);
  EXPECT_EQ(nostd::get<std::string>(mixed.GetUnassociatedAttributes().at("env")), "prod");
}

TEST(ResourceTest, ConstructDuplicateTypeFirstWins)
{
  Entity first("host", ResourceAttributes{{"host.id", "H1"}});
  Entity second("host", ResourceAttributes{{"host.id", "H2"}});
  Resource resource(ResourceAttributes{}, std::string{}, {first, second});

  ASSERT_EQ(resource.GetEntities().size(), 1);
  EXPECT_EQ(resource.GetEntities()[0], first);
  EXPECT_EQ(nostd::get<std::string>(resource.GetAttributes().at("host.id")), "H1");
}

TEST(ResourceTest, ConstructTwoEntityTypesNoKeyConflict)
{
  Entity host("host", ResourceAttributes{{"host.id", "H1"}});
  Entity service("service", ResourceAttributes{{"service.name", "app"}});
  Resource resource(ResourceAttributes{{"env", "prod"}}, std::string{}, {host, service});

  ASSERT_EQ(resource.GetEntities().size(), 2);
  EXPECT_EQ(resource.GetEntities()[0], host);
  EXPECT_EQ(resource.GetEntities()[1], service);
  EXPECT_EQ(nostd::get<std::string>(resource.GetUnassociatedAttributes().at("env")), "prod");
  EXPECT_EQ(resource.GetAttributes().size(), 3);
}

TEST(ResourceTest, ConstructSharedEntityKeyDropsLowerPriority)
{
  Entity host("host", ResourceAttributes{{"host.id", "H1"}}, ResourceAttributes{{"env", "prod"}});
  Entity service("service", ResourceAttributes{{"service.name", "app"}},
                 ResourceAttributes{{"env", "dev"}});
  Resource description_conflict(ResourceAttributes{}, std::string{}, {host, service});

  ASSERT_EQ(description_conflict.GetEntities().size(), 1);
  EXPECT_EQ(description_conflict.GetEntities()[0], host);
  EXPECT_EQ(nostd::get<std::string>(description_conflict.GetAttributes().at("env")), "prod");
  EXPECT_TRUE(description_conflict.GetAttributes().find("service.name") ==
              description_conflict.GetAttributes().end());

  Entity host_identity("host", ResourceAttributes{{"shared.id", "from-host"}});
  Entity service_identity("service", ResourceAttributes{{"shared.id", "from-service"}});
  Resource identity_conflict(ResourceAttributes{}, std::string{},
                             {host_identity, service_identity});
  ASSERT_EQ(identity_conflict.GetEntities().size(), 1);
  EXPECT_EQ(identity_conflict.GetEntities()[0], host_identity);
}

TEST(ResourceTest, ConstructMixedEntitySchemaUrls)
{
  Entity host("host", ResourceAttributes{{"host.id", "H1"}}, ResourceAttributes{},
              "https://opentelemetry.io/schemas/1.21.0");
  Entity service("service", ResourceAttributes{{"service.name", "app"}}, ResourceAttributes{},
                 "https://opentelemetry.io/schemas/1.22.0");
  Resource resource(ResourceAttributes{}, "https://opentelemetry.io/schemas/1.2.0",
                    {host, service});

  EXPECT_TRUE(resource.GetSchemaURL().empty());
  ASSERT_EQ(resource.GetEntities().size(), 2);
}

TEST(ResourceTest, ConstructEqualEntitySchemaUrls)
{
  const std::string entity_schema      = "https://opentelemetry.io/schemas/1.21.0";
  const std::string constructor_schema = "https://opentelemetry.io/schemas/1.2.0";
  Entity host("host", ResourceAttributes{{"host.id", "H1"}}, ResourceAttributes{}, entity_schema);
  Entity service("service", ResourceAttributes{{"service.name", "app"}}, ResourceAttributes{},
                 entity_schema);
  Resource resource(ResourceAttributes{}, constructor_schema, {host, service});

  EXPECT_EQ(resource.GetSchemaURL(), entity_schema);
  ASSERT_EQ(resource.GetEntities().size(), 2);
}

TEST(ResourceTest, ConstructEmptyEntityVector)
{
  ResourceAttributes attributes = {{"service", "backend"}};
  const std::string schema_url  = "https://opentelemetry.io/schemas/1.2.0";
  Resource with_empty_vector(attributes, schema_url, {});
  Resource two_arg(attributes, schema_url);

  EXPECT_TRUE(with_empty_vector.GetEntities().empty());
  EXPECT_EQ(with_empty_vector.GetSchemaURL(), schema_url);
  EXPECT_EQ(with_empty_vector.GetUnassociatedAttributes(), attributes);
  EXPECT_EQ(with_empty_vector.GetUnassociatedAttributes(), with_empty_vector.GetAttributes());
  EXPECT_EQ(with_empty_vector.GetAttributes(), two_arg.GetAttributes());
  EXPECT_EQ(with_empty_vector.GetSchemaURL(), two_arg.GetSchemaURL());
}

TEST(ResourceTest, ConstructAllInvalidEntities)
{
  ResourceAttributes attributes = {{"service", "backend"}};
  const std::string schema_url  = "https://opentelemetry.io/schemas/1.2.0";
  Entity empty_type("", ResourceAttributes{{"host.id", "H1"}});
  Entity empty_identity("host", ResourceAttributes{});
  Resource resource(attributes, schema_url, {empty_type, empty_identity});

  EXPECT_TRUE(resource.GetEntities().empty());
  EXPECT_EQ(resource.GetUnassociatedAttributes(), attributes);
  EXPECT_EQ(resource.GetAttributes(), resource.GetUnassociatedAttributes());
  EXPECT_EQ(resource.GetSchemaURL(), schema_url);
}

TEST(ResourceTest, CopyAndAssignmentPreservesEntities)
{
  ResourceAttributes attributes = {{"env", "prod"}};
  const std::string schema_url  = "https://opentelemetry.io/schemas/1.21.0";
  Entity host("host", ResourceAttributes{{"host.id", "H1"}}, ResourceAttributes{}, schema_url);
  Resource original(attributes, "https://opentelemetry.io/schemas/1.2.0", {host});

  Resource copied(original);
  ASSERT_EQ(copied.GetEntities().size(), 1);
  EXPECT_EQ(copied.GetEntities()[0], original.GetEntities()[0]);
  EXPECT_EQ(copied.GetUnassociatedAttributes(), original.GetUnassociatedAttributes());
  EXPECT_EQ(copied.GetAttributes(), original.GetAttributes());
  EXPECT_EQ(copied.GetSchemaURL(), original.GetSchemaURL());
  EXPECT_EQ(copied.GetSchemaURL(), schema_url);

  Resource assigned;
  assigned = original;
  ASSERT_EQ(assigned.GetEntities().size(), 1);
  EXPECT_EQ(assigned.GetEntities()[0], original.GetEntities()[0]);
  EXPECT_EQ(assigned.GetUnassociatedAttributes(), original.GetUnassociatedAttributes());
  EXPECT_EQ(assigned.GetAttributes(), original.GetAttributes());
  EXPECT_EQ(assigned.GetSchemaURL(), original.GetSchemaURL());
}

TEST(ResourceTest, MergeExample1EntityReplacesLooseAttribute)
{
  Resource old_resource(ResourceAttributes{{"host.name", "old-name"}, {"env", "prod"}},
                        std::string{});
  Entity host("host", ResourceAttributes{{"host.id", "H1"}},
              ResourceAttributes{{"host.name", "new-name"}});
  Resource updating(ResourceAttributes{}, std::string{}, {host});
  auto merged = old_resource.Merge(updating);

  ASSERT_EQ(merged.GetEntities().size(), 1);
  EXPECT_EQ(merged.GetEntities()[0].GetType(), "host");
  EXPECT_EQ(merged.GetUnassociatedAttributes().size(), 1);
  EXPECT_EQ(nostd::get<std::string>(merged.GetUnassociatedAttributes().at("env")), "prod");
  EXPECT_TRUE(merged.GetUnassociatedAttributes().find("host.name") ==
              merged.GetUnassociatedAttributes().end());
  EXPECT_EQ(nostd::get<std::string>(merged.GetAttributes().at("host.id")), "H1");
  EXPECT_EQ(nostd::get<std::string>(merged.GetAttributes().at("host.name")), "new-name");
  EXPECT_EQ(nostd::get<std::string>(merged.GetAttributes().at("env")), "prod");
}

TEST(ResourceTest, MergeExample2LooseAttributeEvictsEntity)
{
  Entity host("host", ResourceAttributes{{"host.id", "H1"}},
              ResourceAttributes{{"host.name", "detected-name"}});
  Entity process("process", ResourceAttributes{{"process.pid", "12345"}});
  Resource old_resource(ResourceAttributes{}, std::string{}, {host, process});
  Resource updating(ResourceAttributes{{"host.id", "H2"}, {"env", "prod"}}, std::string{});
  auto merged = old_resource.Merge(updating);

  ASSERT_EQ(merged.GetEntities().size(), 1);
  EXPECT_EQ(merged.GetEntities()[0].GetType(), "process");
  EXPECT_EQ(nostd::get<std::string>(merged.GetUnassociatedAttributes().at("host.id")), "H2");
  EXPECT_EQ(nostd::get<std::string>(merged.GetUnassociatedAttributes().at("env")), "prod");
  EXPECT_TRUE(merged.GetUnassociatedAttributes().find("host.name") ==
              merged.GetUnassociatedAttributes().end());
  EXPECT_EQ(nostd::get<std::string>(merged.GetAttributes().at("process.pid")), "12345");
  EXPECT_EQ(nostd::get<std::string>(merged.GetAttributes().at("host.id")), "H2");
  EXPECT_TRUE(merged.GetAttributes().find("host.name") == merged.GetAttributes().end());
}

TEST(ResourceTest, MergeExample3IdentityConflictKeepsUpdatingHostRank)
{
  Entity old_host("host", ResourceAttributes{{"host.id", "H1"}},
                  ResourceAttributes{{"env", "prod"}});
  Resource old_resource(ResourceAttributes{}, std::string{}, {old_host});
  Entity updating_host("host", ResourceAttributes{{"host.id", "H2"}});
  Entity service("service", ResourceAttributes{{"service.name", "S1"}},
                 ResourceAttributes{{"env", "dev"}});
  Resource updating(ResourceAttributes{}, std::string{}, {updating_host, service});
  auto merged = old_resource.Merge(updating);

  ASSERT_EQ(merged.GetEntities().size(), 1);
  EXPECT_EQ(merged.GetEntities()[0].GetType(), "host");
  EXPECT_EQ(nostd::get<std::string>(merged.GetEntities()[0].GetIdentity().at("host.id")), "H1");
  EXPECT_EQ(nostd::get<std::string>(merged.GetAttributes().at("env")), "prod");
  EXPECT_TRUE(merged.GetAttributes().find("service.name") == merged.GetAttributes().end());
}

TEST(ResourceTest, MergeSameTypeSameIdentityOverlaysDescription)
{
  const std::string schema_url = "https://opentelemetry.io/schemas/1.21.0";
  Entity old_host("host", ResourceAttributes{{"host.id", "H1"}},
                  ResourceAttributes{{"env", "prod"}, {"host.type", "machine"}}, schema_url);
  Entity updating_host("host", ResourceAttributes{{"host.id", "H1"}},
                       ResourceAttributes{{"env", "dev"}}, schema_url);
  Resource old_resource(ResourceAttributes{}, std::string{}, {old_host});
  Resource updating(ResourceAttributes{}, std::string{}, {updating_host});
  auto merged = old_resource.Merge(updating);

  ASSERT_EQ(merged.GetEntities().size(), 1);
  EXPECT_EQ(nostd::get<std::string>(merged.GetEntities()[0].GetDescription().at("env")), "dev");
  EXPECT_EQ(nostd::get<std::string>(merged.GetEntities()[0].GetDescription().at("host.type")),
            "machine");
  EXPECT_EQ(merged.GetEntities()[0].GetSchemaURL(), schema_url);
}

TEST(ResourceTest, MergeSameTypeDifferentIdentityKeepsOld)
{
  Entity old_host("host", ResourceAttributes{{"host.id", "H1"}});
  Entity updating_host("host", ResourceAttributes{{"host.id", "H2"}});
  auto merged = Resource(ResourceAttributes{}, std::string{}, {old_host})
                    .Merge(Resource(ResourceAttributes{}, std::string{}, {updating_host}));

  ASSERT_EQ(merged.GetEntities().size(), 1);
  EXPECT_EQ(nostd::get<std::string>(merged.GetEntities()[0].GetIdentity().at("host.id")), "H1");
}

TEST(ResourceTest, MergeSameTypeDifferentSchemaKeepsOld)
{
  Entity old_host("host", ResourceAttributes{{"host.id", "H1"}}, ResourceAttributes{},
                  "https://opentelemetry.io/schemas/1.21.0");
  Entity updating_host("host", ResourceAttributes{{"host.id", "H1"}}, ResourceAttributes{},
                       "https://opentelemetry.io/schemas/1.22.0");
  auto merged = Resource(ResourceAttributes{}, std::string{}, {old_host})
                    .Merge(Resource(ResourceAttributes{}, std::string{}, {updating_host}));

  ASSERT_EQ(merged.GetEntities().size(), 1);
  EXPECT_EQ(merged.GetEntities()[0].GetSchemaURL(), "https://opentelemetry.io/schemas/1.21.0");
}

TEST(ResourceTest, MergeDifferentTypesAppend)
{
  Entity host("host", ResourceAttributes{{"host.id", "H1"}});
  Entity service("service", ResourceAttributes{{"service.name", "app"}});
  auto merged = Resource(ResourceAttributes{}, std::string{}, {host})
                    .Merge(Resource(ResourceAttributes{}, std::string{}, {service}));

  ASSERT_EQ(merged.GetEntities().size(), 2);
  EXPECT_EQ(merged.GetEntities()[0].GetType(), "service");
  EXPECT_EQ(merged.GetEntities()[1].GetType(), "host");
  EXPECT_EQ(nostd::get<std::string>(merged.GetAttributes().at("host.id")), "H1");
  EXPECT_EQ(nostd::get<std::string>(merged.GetAttributes().at("service.name")), "app");
}

TEST(ResourceTest, MergeUpdatingTypeRankBeatsOldOnlyType)
{
  Entity service("service", ResourceAttributes{{"service.name", "app"}},
                 ResourceAttributes{{"env", "prod"}});
  Entity host("host", ResourceAttributes{{"host.id", "H1"}}, ResourceAttributes{{"env", "dev"}});
  auto merged = Resource(ResourceAttributes{}, std::string{}, {service})
                    .Merge(Resource(ResourceAttributes{}, std::string{}, {host}));

  ASSERT_EQ(merged.GetEntities().size(), 1);
  EXPECT_EQ(merged.GetEntities()[0].GetType(), "host");
  EXPECT_EQ(nostd::get<std::string>(merged.GetAttributes().at("env")), "dev");
}

TEST(ResourceTest, MergeCascadingKeyConflictKeepsHighestAndUnrelated)
{
  Entity mid("mid", ResourceAttributes{{"x", "from-mid"}, {"y", "from-mid"}});
  Entity a("a", ResourceAttributes{{"x", "from-a"}});
  Entity c("c", ResourceAttributes{{"y", "from-c"}});
  auto merged = Resource(ResourceAttributes{}, std::string{}, {mid})
                    .Merge(Resource(ResourceAttributes{}, std::string{}, {a, c}));

  ASSERT_EQ(merged.GetEntities().size(), 2);
  EXPECT_EQ(merged.GetEntities()[0].GetType(), "a");
  EXPECT_EQ(merged.GetEntities()[1].GetType(), "c");
  EXPECT_EQ(nostd::get<std::string>(merged.GetAttributes().at("x")), "from-a");
  EXPECT_EQ(nostd::get<std::string>(merged.GetAttributes().at("y")), "from-c");
}

TEST(ResourceTest, MergeAllEntitiesDroppedUsesClassicSchema)
{
  const std::string old_schema      = "https://opentelemetry.io/schemas/1.21.0";
  const std::string updating_schema = "https://opentelemetry.io/schemas/1.22.0";
  Entity host("host", ResourceAttributes{{"host.id", "H1"}}, ResourceAttributes{}, old_schema);
  Resource old_resource(ResourceAttributes{}, old_schema, {host});
  Resource updating(ResourceAttributes{{"host.id", "H2"}}, updating_schema);
  auto merged = old_resource.Merge(updating);

  EXPECT_TRUE(merged.GetEntities().empty());
  EXPECT_EQ(merged.GetSchemaURL(), updating_schema);
  EXPECT_EQ(nostd::get<std::string>(merged.GetUnassociatedAttributes().at("host.id")), "H2");
}

TEST(ResourceTest, MergeSurvivingEntitiesSetResourceSchema)
{
  const std::string entity_schema = "https://opentelemetry.io/schemas/1.21.0";
  Entity host("host", ResourceAttributes{{"host.id", "H1"}}, ResourceAttributes{}, entity_schema);
  Resource old_resource(ResourceAttributes{}, "https://opentelemetry.io/schemas/1.2.0");
  Resource updating(ResourceAttributes{{"env", "prod"}}, "https://opentelemetry.io/schemas/9.9.9",
                    {host});
  auto merged = old_resource.Merge(updating);

  EXPECT_EQ(merged.GetSchemaURL(), entity_schema);
  EXPECT_EQ(nostd::get<std::string>(merged.GetUnassociatedAttributes().at("env")), "prod");
}

TEST(ResourceTest, MergeMixedEntitySchemaUrls)
{
  Entity host("host", ResourceAttributes{{"host.id", "H1"}}, ResourceAttributes{},
              "https://opentelemetry.io/schemas/1.21.0");
  Entity service("service", ResourceAttributes{{"service.name", "app"}}, ResourceAttributes{},
                 "https://opentelemetry.io/schemas/1.22.0");
  auto merged = Resource(ResourceAttributes{}, std::string{}, {host})
                    .Merge(Resource(ResourceAttributes{}, std::string{}, {service}));

  EXPECT_TRUE(merged.GetSchemaURL().empty());
  ASSERT_EQ(merged.GetEntities().size(), 2);
}

TEST(ResourceTest, MergeUpdatingLooseBeatsOldLoose)
{
  Resource old_resource(ResourceAttributes{{"foo", "old"}, {"keep", "old"}}, std::string{},
                        {Entity("host", ResourceAttributes{{"host.id", "H1"}})});
  Resource updating(ResourceAttributes{{"foo", "new"}}, std::string{},
                    {Entity("service", ResourceAttributes{{"service.name", "app"}})});
  auto merged = old_resource.Merge(updating);

  ASSERT_EQ(merged.GetEntities().size(), 2);
  EXPECT_EQ(nostd::get<std::string>(merged.GetUnassociatedAttributes().at("foo")), "new");
  EXPECT_EQ(nostd::get<std::string>(merged.GetUnassociatedAttributes().at("keep")), "old");
  EXPECT_TRUE(merged.GetUnassociatedAttributes().find("host.id") ==
              merged.GetUnassociatedAttributes().end());
  EXPECT_TRUE(merged.GetUnassociatedAttributes().find("service.name") ==
              merged.GetUnassociatedAttributes().end());
}

TEST(ResourceTest, MergeCopyAssignmentPreservesMergedEntities)
{
  Entity host("host", ResourceAttributes{{"host.id", "H1"}});
  auto merged = Resource(ResourceAttributes{{"env", "prod"}}, std::string{})
                    .Merge(Resource(ResourceAttributes{}, std::string{}, {host}));

  Resource copied(merged);
  EXPECT_EQ(copied.GetEntities(), merged.GetEntities());
  EXPECT_EQ(copied.GetUnassociatedAttributes(), merged.GetUnassociatedAttributes());
  EXPECT_EQ(copied.GetAttributes(), merged.GetAttributes());
  EXPECT_EQ(copied.GetSchemaURL(), merged.GetSchemaURL());

  Resource assigned;
  assigned = merged;
  EXPECT_EQ(assigned.GetEntities(), merged.GetEntities());
  EXPECT_EQ(assigned.GetAttributes(), merged.GetAttributes());
}

TEST(ResourceTest, CreateEmptyEntitiesMatchesTwoArg)
{
  ResourceAttributes attributes = {{"service", "backend"}};
  auto two_arg                  = Resource::Create(attributes);
  auto three_arg                = Resource::Create(attributes, std::string{}, {});

  EXPECT_TRUE(two_arg.GetEntities().empty());
  EXPECT_TRUE(three_arg.GetEntities().empty());
  EXPECT_EQ(two_arg.GetAttributes(), three_arg.GetAttributes());
  EXPECT_EQ(two_arg.GetSchemaURL(), three_arg.GetSchemaURL());
}

TEST(ResourceTest, CreateWithEntityPreservesEntityAndSdkDefaults)
{
  Entity host("host", ResourceAttributes{{"host.id", "H1"}});
  auto resource = Resource::Create(ResourceAttributes{{"env", "prod"}}, std::string{}, {host});

  ASSERT_EQ(resource.GetEntities().size(), 1);
  EXPECT_EQ(resource.GetEntities()[0].GetType(), "host");
  EXPECT_EQ(nostd::get<std::string>(resource.GetAttributes().at("host.id")), "H1");
  EXPECT_EQ(nostd::get<std::string>(resource.GetUnassociatedAttributes().at("env")), "prod");
  EXPECT_EQ(nostd::get<std::string>(
                resource.GetAttributes().at(semconv::telemetry::kTelemetrySdkLanguage)),
            "cpp");
  EXPECT_EQ(nostd::get<std::string>(resource.GetAttributes().at(semconv::service::kServiceName)),
            "unknown_service");
}

TEST(ResourceTest, CreateEntityOwnedServiceNameSkipsFallback)
{
  Entity service("service", ResourceAttributes{{"service.name", "from-entity"}});
  auto resource = Resource::Create(ResourceAttributes{}, std::string{}, {service});

  ASSERT_EQ(resource.GetEntities().size(), 1);
  EXPECT_EQ(nostd::get<std::string>(resource.GetAttributes().at(semconv::service::kServiceName)),
            "from-entity");
  EXPECT_TRUE(resource.GetUnassociatedAttributes().find(semconv::service::kServiceName) ==
              resource.GetUnassociatedAttributes().end());
}

TEST(ResourceTest, CreateWithEntitySchemaUrl)
{
  const std::string entity_schema = "https://opentelemetry.io/schemas/1.21.0";
  Entity host("host", ResourceAttributes{{"host.id", "H1"}}, ResourceAttributes{}, entity_schema);
  auto resource =
      Resource::Create(ResourceAttributes{}, "https://opentelemetry.io/schemas/1.2.0", {host});

  EXPECT_EQ(resource.GetSchemaURL(), entity_schema);
}

}  // namespace
