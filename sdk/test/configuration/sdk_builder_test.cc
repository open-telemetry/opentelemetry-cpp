// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>

#include <cstdint>
#include <initializer_list>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/configuration/attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/attributes_configuration.h"
#include "opentelemetry/sdk/configuration/boolean_array_attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/boolean_attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/container_resource_detector_builder.h"
#include "opentelemetry/sdk/configuration/container_resource_detector_configuration.h"
#include "opentelemetry/sdk/configuration/double_array_attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/double_attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/extension_resource_detector_builder.h"
#include "opentelemetry/sdk/configuration/extension_resource_detector_configuration.h"
#include "opentelemetry/sdk/configuration/host_resource_detector_configuration.h"
#include "opentelemetry/sdk/configuration/include_exclude_configuration.h"
#include "opentelemetry/sdk/configuration/integer_array_attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/integer_attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/process_resource_detector_builder.h"
#include "opentelemetry/sdk/configuration/process_resource_detector_configuration.h"
#include "opentelemetry/sdk/configuration/registry.h"
#include "opentelemetry/sdk/configuration/resource_configuration.h"
#include "opentelemetry/sdk/configuration/resource_detection_configuration.h"
#include "opentelemetry/sdk/configuration/resource_detector_configuration.h"
#include "opentelemetry/sdk/configuration/sdk_builder.h"
#include "opentelemetry/sdk/configuration/service_resource_detector_builder.h"
#include "opentelemetry/sdk/configuration/service_resource_detector_configuration.h"
#include "opentelemetry/sdk/configuration/severity_number.h"
#include "opentelemetry/sdk/configuration/string_array_attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/string_array_configuration.h"
#include "opentelemetry/sdk/configuration/string_attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/unsupported_exception.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/sdk/resource/resource_detector.h"

namespace internal_log = opentelemetry::sdk::common::internal_log;
namespace config_sdk   = opentelemetry::sdk::configuration;
namespace nostd        = opentelemetry::nostd;

//------------------------------------------------------------------------------
// Tests for the SdkBuilder class methods that create SDK components from configuration models
// These tests focus on the API of the SdkBuilder for creating SDK components that can be
// independently verified. For full integration tests of the SdkBuilder with configuration models,
// see the programmatic_configuration_test.cc file.

TEST(SdkBuilder, SetLogLevel)
{
  config_sdk::SdkBuilder builder(std::make_shared<config_sdk::Registry>());

  internal_log::LogLevel level{};

  // trace/debug group -> Debug
  for (auto sev : {config_sdk::SeverityNumber::trace, config_sdk::SeverityNumber::trace2,
                   config_sdk::SeverityNumber::trace3, config_sdk::SeverityNumber::trace4,
                   config_sdk::SeverityNumber::debug, config_sdk::SeverityNumber::debug2,
                   config_sdk::SeverityNumber::debug3, config_sdk::SeverityNumber::debug4})
  {
    builder.SetLogLevel(level, sev);
    EXPECT_EQ(level, internal_log::LogLevel::Debug);
  }

  // info group -> Info
  for (auto sev : {config_sdk::SeverityNumber::info, config_sdk::SeverityNumber::info2,
                   config_sdk::SeverityNumber::info3, config_sdk::SeverityNumber::info4})
  {
    builder.SetLogLevel(level, sev);
    EXPECT_EQ(level, internal_log::LogLevel::Info);
  }

  // warn group -> Warning
  for (auto sev : {config_sdk::SeverityNumber::warn, config_sdk::SeverityNumber::warn2,
                   config_sdk::SeverityNumber::warn3, config_sdk::SeverityNumber::warn4})
  {
    builder.SetLogLevel(level, sev);
    EXPECT_EQ(level, internal_log::LogLevel::Warning);
  }

  // error/fatal group -> Error
  for (auto sev : {config_sdk::SeverityNumber::error, config_sdk::SeverityNumber::error2,
                   config_sdk::SeverityNumber::error3, config_sdk::SeverityNumber::error4,
                   config_sdk::SeverityNumber::fatal, config_sdk::SeverityNumber::fatal2,
                   config_sdk::SeverityNumber::fatal3, config_sdk::SeverityNumber::fatal4})
  {
    builder.SetLogLevel(level, sev);
    EXPECT_EQ(level, internal_log::LogLevel::Error);
  }
}

TEST(SdkBuilder, SetResourceWithDefaults)
{
  config_sdk::SdkBuilder builder(std::make_shared<config_sdk::Registry>());
  opentelemetry::sdk::resource::Resource resource;

  builder.SetResource(resource, nullptr);

  const auto &attrs = resource.GetAttributes();
  EXPECT_NE(attrs.find("telemetry.sdk.name"), attrs.end());
  EXPECT_NE(attrs.find("telemetry.sdk.language"), attrs.end());
  EXPECT_NE(attrs.find("telemetry.sdk.version"), attrs.end());
}

TEST(SdkBuilder, SetResourceWithAttributes)
{
  config_sdk::SdkBuilder builder(std::make_shared<config_sdk::Registry>());
  opentelemetry::sdk::resource::Resource resource;

  auto model        = std::make_unique<config_sdk::ResourceConfiguration>();
  model->attributes = std::make_unique<config_sdk::AttributesConfiguration>();

  auto string_attr   = std::make_unique<config_sdk::StringAttributeValueConfiguration>();
  string_attr->value = "test_string_value";
  model->attributes->kv_map["string_key"] = std::move(string_attr);

  auto boolean_attr   = std::make_unique<config_sdk::BooleanAttributeValueConfiguration>();
  boolean_attr->value = true;
  model->attributes->kv_map["boolean_key"] = std::move(boolean_attr);

  auto integer_attr   = std::make_unique<config_sdk::IntegerAttributeValueConfiguration>();
  integer_attr->value = int64_t{42};
  model->attributes->kv_map["integer_key"] = std::move(integer_attr);

  auto double_attr   = std::make_unique<config_sdk::DoubleAttributeValueConfiguration>();
  double_attr->value = 3.14;
  model->attributes->kv_map["double_key"] = std::move(double_attr);

  auto string_array_attr   = std::make_unique<config_sdk::StringArrayAttributeValueConfiguration>();
  string_array_attr->value = {"alpha", "beta", "gamma"};
  model->attributes->kv_map["string_array_key"] = std::move(string_array_attr);

  auto boolean_array_attr = std::make_unique<config_sdk::BooleanArrayAttributeValueConfiguration>();
  boolean_array_attr->value                      = {true, false, true};
  model->attributes->kv_map["boolean_array_key"] = std::move(boolean_array_attr);

  auto integer_array_attr = std::make_unique<config_sdk::IntegerArrayAttributeValueConfiguration>();
  integer_array_attr->value                      = {10, 20, 30};
  model->attributes->kv_map["integer_array_key"] = std::move(integer_array_attr);

  auto double_array_attr   = std::make_unique<config_sdk::DoubleArrayAttributeValueConfiguration>();
  double_array_attr->value = {1.1, 2.2, 3.3};
  model->attributes->kv_map["double_array_key"] = std::move(double_array_attr);

  builder.SetResource(resource, model);
  const auto &attrs = resource.GetAttributes();

  ASSERT_NE(attrs.find("string_key"), attrs.end());
  EXPECT_EQ(nostd::get<std::string>(attrs.at("string_key")), "test_string_value");

  ASSERT_NE(attrs.find("boolean_key"), attrs.end());
  EXPECT_EQ(nostd::get<bool>(attrs.at("boolean_key")), true);

  ASSERT_NE(attrs.find("integer_key"), attrs.end());
  EXPECT_EQ(nostd::get<int64_t>(attrs.at("integer_key")), int64_t{42});

  ASSERT_NE(attrs.find("double_key"), attrs.end());
  EXPECT_DOUBLE_EQ(nostd::get<double>(attrs.at("double_key")), 3.14);

  ASSERT_NE(attrs.find("string_array_key"), attrs.end());
  EXPECT_EQ(nostd::get<std::vector<std::string>>(attrs.at("string_array_key")),
            (std::vector<std::string>{"alpha", "beta", "gamma"}));

  ASSERT_NE(attrs.find("boolean_array_key"), attrs.end());
  EXPECT_EQ(nostd::get<std::vector<bool>>(attrs.at("boolean_array_key")),
            (std::vector<bool>{true, false, true}));

  ASSERT_NE(attrs.find("integer_array_key"), attrs.end());
  EXPECT_EQ(nostd::get<std::vector<int64_t>>(attrs.at("integer_array_key")),
            (std::vector<int64_t>{10, 20, 30}));

  ASSERT_NE(attrs.find("double_array_key"), attrs.end());
  EXPECT_EQ(nostd::get<std::vector<double>>(attrs.at("double_array_key")),
            (std::vector<double>{1.1, 2.2, 3.3}));
}

TEST(SdkBuilder, SetResourceWithAttributesList)
{
  config_sdk::SdkBuilder builder(std::make_shared<config_sdk::Registry>());
  opentelemetry::sdk::resource::Resource resource;

  auto model             = std::make_unique<config_sdk::ResourceConfiguration>();
  model->attributes_list = "service.name=my-service,service.version=1.2.3";

  builder.SetResource(resource, model);

  const auto &attrs = resource.GetAttributes();
  ASSERT_NE(attrs.find("service.name"), attrs.end());
  EXPECT_EQ(nostd::get<std::string>(attrs.at("service.name")), "my-service");
  ASSERT_NE(attrs.find("service.version"), attrs.end());
  EXPECT_EQ(nostd::get<std::string>(attrs.at("service.version")), "1.2.3");
}

TEST(SdkBuilder, SetResourceWithSchemaUrl)
{
  config_sdk::SdkBuilder builder(std::make_shared<config_sdk::Registry>());
  opentelemetry::sdk::resource::Resource resource;

  auto model        = std::make_unique<config_sdk::ResourceConfiguration>();
  model->schema_url = "https://opentelemetry.io/schemas/1.25.0";

  builder.SetResource(resource, model);

  EXPECT_EQ(resource.GetSchemaURL(), "https://opentelemetry.io/schemas/1.25.0");
}

TEST(SdkBuilder, SetResourceAttributesOverrideAttributesList)
{
  config_sdk::SdkBuilder builder(std::make_shared<config_sdk::Registry>());
  opentelemetry::sdk::resource::Resource resource;

  auto model             = std::make_unique<config_sdk::ResourceConfiguration>();
  model->attributes_list = "service.name=from-list";
  model->attributes      = std::make_unique<config_sdk::AttributesConfiguration>();
  auto override_attr     = std::make_unique<config_sdk::StringAttributeValueConfiguration>();
  override_attr->value   = "from-attributes";
  model->attributes->kv_map["service.name"] = std::move(override_attr);

  builder.SetResource(resource, model);

  const auto &attrs = resource.GetAttributes();
  ASSERT_NE(attrs.find("service.name"), attrs.end());
  EXPECT_EQ(nostd::get<std::string>(attrs.at("service.name")), "from-attributes");
}

//------------------------------------------------------------------------------
// Resource detection (SetResource)

namespace
{

class TestResourceDetector : public opentelemetry::sdk::resource::ResourceDetector
{
public:
  TestResourceDetector(opentelemetry::sdk::resource::ResourceAttributes attributes,
                       std::string schema_url)
      : attributes_(std::move(attributes)), schema_url_(std::move(schema_url))
  {}

  opentelemetry::sdk::resource::Resource Detect() override
  {
    return Create(attributes_, schema_url_);
  }

private:
  opentelemetry::sdk::resource::ResourceAttributes attributes_;
  std::string schema_url_;
};

class TestContainerResourceDetectorBuilder : public config_sdk::ContainerResourceDetectorBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::resource::ResourceDetector> Build(
      const config_sdk::ContainerResourceDetectorConfiguration * /* model */) const override
  {
    called = true;
    return std::make_unique<TestResourceDetector>(
        opentelemetry::sdk::resource::ResourceAttributes{{"container.id", "abc123"}},
        std::string{});
  }

  mutable bool called{false};
};

class TestServiceResourceDetectorBuilder : public config_sdk::ServiceResourceDetectorBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::resource::ResourceDetector> Build(
      const config_sdk::ServiceResourceDetectorConfiguration * /* model */) const override
  {
    called = true;
    return std::make_unique<TestResourceDetector>(
        opentelemetry::sdk::resource::ResourceAttributes{{"service.name", "my-service"}},
        std::string{});
  }

  mutable bool called{false};
};

class TestProcessResourceDetectorBuilder : public config_sdk::ProcessResourceDetectorBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::resource::ResourceDetector> Build(
      const config_sdk::ProcessResourceDetectorConfiguration * /* model */) const override
  {
    called = true;
    return std::make_unique<TestResourceDetector>(
        opentelemetry::sdk::resource::ResourceAttributes{{"process.pid", "12345"},
                                                         {"process.command_args", "detected-args"}},
        "https://opentelemetry.io/schemas/1.0.0");
  }

  mutable bool called{false};
};

class TestExtensionResourceDetectorBuilder : public config_sdk::ExtensionResourceDetectorBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::resource::ResourceDetector> Build(
      const config_sdk::ExtensionResourceDetectorConfiguration *model) const override
  {
    called = true;
    name   = model->name;
    return std::make_unique<TestResourceDetector>(
        opentelemetry::sdk::resource::ResourceAttributes{{"custom.key", "custom-value"}},
        std::string{});
  }

  mutable bool called{false};
  mutable std::string name;
};

std::string GetStringAttribute(const opentelemetry::sdk::resource::Resource &resource,
                               const std::string &key)
{
  const auto &attributes = resource.GetAttributes();
  auto it                = attributes.find(key);
  if (it == attributes.end())
  {
    return std::string{};
  }
  return opentelemetry::nostd::get<std::string>(it->second);
}

}  // namespace

TEST(SdkBuilder, SetResourceWithoutModel)
{
  auto registry = std::make_shared<config_sdk::Registry>();
  config_sdk::SdkBuilder builder(registry);

  auto resource = opentelemetry::sdk::resource::Resource::GetEmpty();
  std::unique_ptr<config_sdk::ResourceConfiguration> model;
  builder.SetResource(resource, model);

  EXPECT_EQ(GetStringAttribute(resource, "telemetry.sdk.name"), "opentelemetry");
  EXPECT_EQ(GetStringAttribute(resource, "telemetry.sdk.language"), "cpp");
}

TEST(SdkBuilder, SetResourceDetectorDispatch)
{
  auto registry           = std::make_shared<config_sdk::Registry>();
  auto process_builder    = std::make_unique<TestProcessResourceDetectorBuilder>();
  auto *process_builder_p = process_builder.get();
  registry->SetProcessResourceDetectorBuilder(std::move(process_builder));
  config_sdk::SdkBuilder builder(std::move(registry));

  auto model       = std::make_unique<config_sdk::ResourceConfiguration>();
  model->detection = std::make_unique<config_sdk::ResourceDetectionConfiguration>();
  model->detection->detectors.push_back(
      std::make_unique<config_sdk::ProcessResourceDetectorConfiguration>());

  auto resource = opentelemetry::sdk::resource::Resource::GetEmpty();
  builder.SetResource(resource, model);

  EXPECT_TRUE(process_builder_p->called);
  EXPECT_EQ(GetStringAttribute(resource, "process.pid"), "12345");
  // Detected attributes win over the default resource.
  EXPECT_EQ(GetStringAttribute(resource, "telemetry.sdk.name"), "opentelemetry");
  // The detected schema url is preserved.
  EXPECT_EQ(resource.GetSchemaURL(), "https://opentelemetry.io/schemas/1.0.0");
}

TEST(SdkBuilder, SetResourceExtensionDetector)
{
  auto registry             = std::make_shared<config_sdk::Registry>();
  auto extension_builder    = std::make_unique<TestExtensionResourceDetectorBuilder>();
  auto *extension_builder_p = extension_builder.get();
  registry->SetExtensionResourceDetectorBuilder("my_custom_detector", std::move(extension_builder));
  config_sdk::SdkBuilder builder(std::move(registry));

  auto model       = std::make_unique<config_sdk::ResourceConfiguration>();
  model->detection = std::make_unique<config_sdk::ResourceDetectionConfiguration>();
  auto extension   = std::make_unique<config_sdk::ExtensionResourceDetectorConfiguration>();
  extension->name  = "my_custom_detector";
  model->detection->detectors.push_back(std::move(extension));

  auto resource = opentelemetry::sdk::resource::Resource::GetEmpty();
  builder.SetResource(resource, model);

  EXPECT_TRUE(extension_builder_p->called);
  EXPECT_EQ(extension_builder_p->name, "my_custom_detector");
  EXPECT_EQ(GetStringAttribute(resource, "custom.key"), "custom-value");
}

TEST(SdkBuilder, SetResourceUnregisteredDetectorFails)
{
  auto registry = std::make_shared<config_sdk::Registry>();
  config_sdk::SdkBuilder builder(std::move(registry));

  auto model       = std::make_unique<config_sdk::ResourceConfiguration>();
  model->detection = std::make_unique<config_sdk::ResourceDetectionConfiguration>();
  model->detection->detectors.push_back(
      std::make_unique<config_sdk::HostResourceDetectorConfiguration>());

  auto resource = opentelemetry::sdk::resource::Resource::GetEmpty();
  EXPECT_THROW(builder.SetResource(resource, model),
               opentelemetry::sdk::configuration::UnsupportedException);
}

TEST(SdkBuilder, SetResourceUnregisteredContainerDetectorFails)
{
  auto registry = std::make_shared<config_sdk::Registry>();
  config_sdk::SdkBuilder builder(std::move(registry));

  auto model       = std::make_unique<config_sdk::ResourceConfiguration>();
  model->detection = std::make_unique<config_sdk::ResourceDetectionConfiguration>();
  model->detection->detectors.push_back(
      std::make_unique<config_sdk::ContainerResourceDetectorConfiguration>());

  auto resource = opentelemetry::sdk::resource::Resource::GetEmpty();
  EXPECT_THROW(builder.SetResource(resource, model),
               opentelemetry::sdk::configuration::UnsupportedException);
}

TEST(SdkBuilder, SetResourceUnregisteredProcessDetectorFails)
{
  auto registry = std::make_shared<config_sdk::Registry>();
  config_sdk::SdkBuilder builder(std::move(registry));

  auto model       = std::make_unique<config_sdk::ResourceConfiguration>();
  model->detection = std::make_unique<config_sdk::ResourceDetectionConfiguration>();
  model->detection->detectors.push_back(
      std::make_unique<config_sdk::ProcessResourceDetectorConfiguration>());

  auto resource = opentelemetry::sdk::resource::Resource::GetEmpty();
  EXPECT_THROW(builder.SetResource(resource, model),
               opentelemetry::sdk::configuration::UnsupportedException);
}

TEST(SdkBuilder, SetResourceUnregisteredServiceDetectorFails)
{
  auto registry = std::make_shared<config_sdk::Registry>();
  config_sdk::SdkBuilder builder(std::move(registry));

  auto model       = std::make_unique<config_sdk::ResourceConfiguration>();
  model->detection = std::make_unique<config_sdk::ResourceDetectionConfiguration>();
  model->detection->detectors.push_back(
      std::make_unique<config_sdk::ServiceResourceDetectorConfiguration>());

  auto resource = opentelemetry::sdk::resource::Resource::GetEmpty();
  EXPECT_THROW(builder.SetResource(resource, model),
               opentelemetry::sdk::configuration::UnsupportedException);
}

TEST(SdkBuilder, SetResourceContainerDetector)
{
  auto registry         = std::make_shared<config_sdk::Registry>();
  auto container_b      = std::make_unique<TestContainerResourceDetectorBuilder>();
  auto *container_b_ptr = container_b.get();
  registry->SetContainerResourceDetectorBuilder(std::move(container_b));
  config_sdk::SdkBuilder builder(std::move(registry));

  auto model       = std::make_unique<config_sdk::ResourceConfiguration>();
  model->detection = std::make_unique<config_sdk::ResourceDetectionConfiguration>();
  model->detection->detectors.push_back(
      std::make_unique<config_sdk::ContainerResourceDetectorConfiguration>());

  auto resource = opentelemetry::sdk::resource::Resource::GetEmpty();
  builder.SetResource(resource, model);

  EXPECT_TRUE(container_b_ptr->called);
  EXPECT_EQ(GetStringAttribute(resource, "container.id"), "abc123");
}

TEST(SdkBuilder, SetResourceServiceDetector)
{
  auto registry       = std::make_shared<config_sdk::Registry>();
  auto service_b      = std::make_unique<TestServiceResourceDetectorBuilder>();
  auto *service_b_ptr = service_b.get();
  registry->SetServiceResourceDetectorBuilder(std::move(service_b));
  config_sdk::SdkBuilder builder(std::move(registry));

  auto model       = std::make_unique<config_sdk::ResourceConfiguration>();
  model->detection = std::make_unique<config_sdk::ResourceDetectionConfiguration>();
  model->detection->detectors.push_back(
      std::make_unique<config_sdk::ServiceResourceDetectorConfiguration>());

  auto resource = opentelemetry::sdk::resource::Resource::GetEmpty();
  builder.SetResource(resource, model);

  EXPECT_TRUE(service_b_ptr->called);
  EXPECT_EQ(GetStringAttribute(resource, "service.name"), "my-service");
}

TEST(SdkBuilder, SetResourceUnregisteredExtensionDetectorFails)
{
  auto registry = std::make_shared<config_sdk::Registry>();
  config_sdk::SdkBuilder builder(std::move(registry));

  auto model       = std::make_unique<config_sdk::ResourceConfiguration>();
  model->detection = std::make_unique<config_sdk::ResourceDetectionConfiguration>();
  auto ext         = std::make_unique<config_sdk::ExtensionResourceDetectorConfiguration>();
  ext->name        = "unregistered_detector";
  model->detection->detectors.push_back(std::move(ext));

  auto resource = opentelemetry::sdk::resource::Resource::GetEmpty();
  EXPECT_THROW(builder.SetResource(resource, model),
               opentelemetry::sdk::configuration::UnsupportedException);
}

TEST(SdkBuilder, SetResourceDetectionAttributeFilter)
{
  auto registry = std::make_shared<config_sdk::Registry>();
  registry->SetProcessResourceDetectorBuilder(
      std::make_unique<TestProcessResourceDetectorBuilder>());
  config_sdk::SdkBuilder builder(std::move(registry));

  auto model       = std::make_unique<config_sdk::ResourceConfiguration>();
  model->detection = std::make_unique<config_sdk::ResourceDetectionConfiguration>();
  model->detection->detectors.push_back(
      std::make_unique<config_sdk::ProcessResourceDetectorConfiguration>());

  auto filter                    = std::make_unique<config_sdk::IncludeExcludeConfiguration>();
  filter->included               = std::make_unique<config_sdk::StringArrayConfiguration>();
  filter->included->string_array = {"process.*"};
  filter->excluded               = std::make_unique<config_sdk::StringArrayConfiguration>();
  filter->excluded->string_array = {"process.command_args"};
  model->detection->attributes   = std::move(filter);

  auto resource = opentelemetry::sdk::resource::Resource::GetEmpty();
  builder.SetResource(resource, model);

  // Matched by included.
  EXPECT_EQ(GetStringAttribute(resource, "process.pid"), "12345");
  // Matched by included AND excluded: excluded wins.
  EXPECT_EQ(GetStringAttribute(resource, "process.command_args"), "");
  // The filter applies to detected attributes only, not to the default resource.
  EXPECT_EQ(GetStringAttribute(resource, "telemetry.sdk.name"), "opentelemetry");
}

TEST(SdkBuilder, SetResourceMergePriority)
{
  auto registry = std::make_shared<config_sdk::Registry>();
  registry->SetProcessResourceDetectorBuilder(
      std::make_unique<TestProcessResourceDetectorBuilder>());
  config_sdk::SdkBuilder builder(std::move(registry));

  auto model       = std::make_unique<config_sdk::ResourceConfiguration>();
  model->detection = std::make_unique<config_sdk::ResourceDetectionConfiguration>();
  model->detection->detectors.push_back(
      std::make_unique<config_sdk::ProcessResourceDetectorConfiguration>());

  // process.pid: detected, then overridden by attributes_list, then by attributes.
  model->attributes_list = "process.pid=from-list,list.only=list-value";

  auto typed_value   = std::make_unique<config_sdk::StringAttributeValueConfiguration>();
  typed_value->value = "from-attributes";
  model->attributes  = std::make_unique<config_sdk::AttributesConfiguration>();
  model->attributes->kv_map.emplace("process.pid", std::move(typed_value));

  model->schema_url = "https://opentelemetry.io/schemas/1.2.0";

  auto resource = opentelemetry::sdk::resource::Resource::GetEmpty();
  builder.SetResource(resource, model);

  EXPECT_EQ(GetStringAttribute(resource, "process.pid"), "from-attributes");
  EXPECT_EQ(GetStringAttribute(resource, "list.only"), "list-value");
  EXPECT_EQ(GetStringAttribute(resource, "process.command_args"), "detected-args");
  EXPECT_EQ(GetStringAttribute(resource, "telemetry.sdk.name"), "opentelemetry");
  EXPECT_EQ(resource.GetSchemaURL(), "https://opentelemetry.io/schemas/1.2.0");
}

TEST(SdkBuilder, SetResourceFilterAppliesToDetectedAttributesOnly)
{
  auto registry = std::make_shared<config_sdk::Registry>();
  registry->SetProcessResourceDetectorBuilder(
      std::make_unique<TestProcessResourceDetectorBuilder>());
  config_sdk::SdkBuilder builder(std::move(registry));

  auto model       = std::make_unique<config_sdk::ResourceConfiguration>();
  model->detection = std::make_unique<config_sdk::ResourceDetectionConfiguration>();
  model->detection->detectors.push_back(
      std::make_unique<config_sdk::ProcessResourceDetectorConfiguration>());

  // Exclude everything the detector produced.
  auto filter                    = std::make_unique<config_sdk::IncludeExcludeConfiguration>();
  filter->excluded               = std::make_unique<config_sdk::StringArrayConfiguration>();
  filter->excluded->string_array = {"process.*"};
  model->detection->attributes   = std::move(filter);

  // Attributes matching the excluded pattern, from attributes_list and attributes.
  model->attributes_list = "process.pid=from-list";

  auto typed_value   = std::make_unique<config_sdk::StringAttributeValueConfiguration>();
  typed_value->value = "from-attributes";
  model->attributes  = std::make_unique<config_sdk::AttributesConfiguration>();
  model->attributes->kv_map.emplace("process.command_args", std::move(typed_value));

  auto resource = opentelemetry::sdk::resource::Resource::GetEmpty();
  builder.SetResource(resource, model);

  // The filter removed every detected attribute, but does not apply to the
  // attributes and attributes_list fields, even when their keys match.
  EXPECT_EQ(GetStringAttribute(resource, "process.pid"), "from-list");
  EXPECT_EQ(GetStringAttribute(resource, "process.command_args"), "from-attributes");
}
