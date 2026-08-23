// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "opentelemetry/sdk/configuration/attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/attributes_configuration.h"
#include "opentelemetry/sdk/configuration/boolean_array_attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/boolean_attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/configuration.h"
#include "opentelemetry/sdk/configuration/double_array_attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/double_attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/extension_resource_detector_configuration.h"
#include "opentelemetry/sdk/configuration/include_exclude_configuration.h"
#include "opentelemetry/sdk/configuration/integer_array_attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/integer_attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/resource_configuration.h"
#include "opentelemetry/sdk/configuration/resource_detection_configuration.h"
#include "opentelemetry/sdk/configuration/resource_detector_configuration.h"
#include "opentelemetry/sdk/configuration/resource_detector_configuration_visitor.h"
#include "opentelemetry/sdk/configuration/string_array_attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/string_array_configuration.h"
#include "opentelemetry/sdk/configuration/string_attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/yaml_configuration_parser.h"

#if defined(_MSC_VER)
#  include "opentelemetry/sdk/common/env_variables.h"
using opentelemetry::sdk::common::setenv;
using opentelemetry::sdk::common::unsetenv;
#endif

namespace config_sdk = opentelemetry::sdk::configuration;

static std::unique_ptr<opentelemetry::sdk::configuration::Configuration> DoParse(
    const std::string &yaml)
{
  static const std::string source("test");
  return opentelemetry::sdk::configuration::YamlConfigurationParser::ParseString(source, yaml);
}

namespace
{
enum class DetectorType : std::uint8_t
{
  kNone,
  kContainer,
  kHost,
  kProcess,
  kService,
  kExtension,
};

class TestDetectorVisitor
    : public opentelemetry::sdk::configuration::ResourceDetectorConfigurationVisitor
{
public:
  DetectorType type_matched = DetectorType::kNone;
  std::string extension_name;

  void VisitContainer(
      const opentelemetry::sdk::configuration::ContainerResourceDetectorConfiguration *) override
  {
    type_matched = DetectorType::kContainer;
  }
  void VisitHost(
      const opentelemetry::sdk::configuration::HostResourceDetectorConfiguration *) override
  {
    type_matched = DetectorType::kHost;
  }
  void VisitProcess(
      const opentelemetry::sdk::configuration::ProcessResourceDetectorConfiguration *) override
  {
    type_matched = DetectorType::kProcess;
  }
  void VisitService(
      const opentelemetry::sdk::configuration::ServiceResourceDetectorConfiguration *) override
  {
    type_matched = DetectorType::kService;
  }
  void VisitExtension(
      const opentelemetry::sdk::configuration::ExtensionResourceDetectorConfiguration *model)
      override
  {
    type_matched   = DetectorType::kExtension;
    extension_name = model->name;
  }
};
}  // namespace

TEST(YamlResource, empty_resource)
{
  std::string yaml = R"(
file_format: "1.0-resource"
resource:
)";

  auto config = DoParse(yaml);
  ASSERT_NE(config, nullptr);
  ASSERT_NE(config->resource, nullptr);
  ASSERT_EQ(config->resource->attributes, nullptr);
  ASSERT_EQ(config->resource->attributes_list, "");
  ASSERT_EQ(config->resource->detection, nullptr);
}

TEST(YamlResource, empty_attributes)
{
  std::string yaml = R"(
file_format: "1.0-resource"
resource:
  attributes:
)";

  auto config = DoParse(yaml);
  ASSERT_NE(config, nullptr);
  ASSERT_NE(config->resource, nullptr);
  ASSERT_NE(config->resource->attributes, nullptr);
  ASSERT_EQ(config->resource->attributes->kv_map.size(), 0);
}

TEST(YamlResource, some_attributes_0_10)
{
  // This is the old 0.10 format, must fail
  std::string yaml = R"(
file_format: "1.0-resource"
resource:
  attributes:
    foo: "1234"
    bar: "5678"
)";

  auto config = DoParse(yaml);
  ASSERT_EQ(config, nullptr);
}

TEST(YamlResource, some_attributes_0_30)
{
  // This is the new 0.30 format, must pass
  std::string yaml = R"(
file_format: "1.0-resource"
resource:
  attributes:
   - name: foo
     value: "1234"
   - name: bar
     value: "5678"
)";

  auto config = DoParse(yaml);
  ASSERT_NE(config, nullptr);
  ASSERT_NE(config->resource, nullptr);
  ASSERT_NE(config->resource->attributes, nullptr);
  ASSERT_EQ(config->resource->attributes->kv_map.size(), 2);

  {
    const auto &value = config->resource->attributes->kv_map["foo"];
    ASSERT_NE(value, nullptr);
    auto *value_ptr = value.get();
    auto *string_value =
        reinterpret_cast<opentelemetry::sdk::configuration::StringAttributeValueConfiguration *>(
            value_ptr);
    ASSERT_EQ(string_value->value, "1234");
  }

  {
    const auto &value = config->resource->attributes->kv_map["bar"];
    ASSERT_NE(value, nullptr);
    auto *value_ptr = value.get();
    auto *string_value =
        reinterpret_cast<opentelemetry::sdk::configuration::StringAttributeValueConfiguration *>(
            value_ptr);
    ASSERT_EQ(string_value->value, "5678");
  }
}

TEST(YamlResource, empty_attributes_list)
{
  std::string yaml = R"(
file_format: "1.0-resource"
resource:
  attributes_list:
)";

  auto config = DoParse(yaml);
  ASSERT_NE(config, nullptr);
  ASSERT_NE(config->resource, nullptr);
  ASSERT_EQ(config->resource->attributes_list, "");
}

TEST(YamlResource, some_attributes_list)
{
  std::string yaml = R"(
file_format: "1.0-resource"
resource:
  attributes_list: "foo=1234,bar=5678"
)";

  auto config = DoParse(yaml);
  ASSERT_NE(config, nullptr);
  ASSERT_NE(config->resource, nullptr);
  ASSERT_EQ(config->resource->attributes_list, "foo=1234,bar=5678");
}

TEST(YamlResource, attributes_list_substitution)
{
  setenv("TEST_ENV_NAME", "foo=1234,bar=5678", 1);

  std::string yaml = R"(
file_format: "1.0-resource"
resource:
  attributes_list: ${TEST_ENV_NAME}
)";

  auto config = DoParse(yaml);
  ASSERT_NE(config, nullptr);
  ASSERT_NE(config->resource, nullptr);
  ASSERT_EQ(config->resource->attributes_list, "foo=1234,bar=5678");

  unsetenv("TEST_ENV_NAME");
}

TEST(YamlResource, attributes_list_substitution_fallback)
{
  unsetenv("TEST_ENV_NAME");

  std::string yaml = R"(
file_format: "1.0-resource"
resource:
  attributes_list: ${TEST_ENV_NAME:-service.name=fallback}
)";

  auto config = DoParse(yaml);
  ASSERT_NE(config, nullptr);
  ASSERT_NE(config->resource, nullptr);
  ASSERT_EQ(config->resource->attributes_list, "service.name=fallback");
}

TEST(YamlResource, both_0_10)
{
  // This is the old 0.10 format, must fail
  std::string yaml = R"(
file_format: "1.0-resource"
resource:
  attributes:
    foo: "1234"
    bar: "5678"
  attributes_list: "foo=aaaa,bar=bbbb"
)";

  auto config = DoParse(yaml);
  ASSERT_EQ(config, nullptr);
}

TEST(YamlResource, both_0_30)
{
  // This is the new 0.30 format, must pass
  std::string yaml = R"(
file_format: "1.0-resource"
resource:
  attributes:
   - name: foo
     value: "1234"
   - name: bar
     value: "5678"
  attributes_list: "foo=aaaa,bar=bbbb"
)";

  auto config = DoParse(yaml);
  ASSERT_NE(config, nullptr);
  ASSERT_NE(config->resource, nullptr);
  ASSERT_NE(config->resource->attributes, nullptr);
  ASSERT_EQ(config->resource->attributes->kv_map.size(), 2);

  {
    const auto &value = config->resource->attributes->kv_map["foo"];
    ASSERT_NE(value, nullptr);
    auto *value_ptr = value.get();
    auto *string_value =
        reinterpret_cast<opentelemetry::sdk::configuration::StringAttributeValueConfiguration *>(
            value_ptr);
    ASSERT_EQ(string_value->value, "1234");
  }

  {
    const auto &value = config->resource->attributes->kv_map["bar"];
    ASSERT_NE(value, nullptr);
    auto *value_ptr = value.get();
    auto *string_value =
        reinterpret_cast<opentelemetry::sdk::configuration::StringAttributeValueConfiguration *>(
            value_ptr);
    ASSERT_EQ(string_value->value, "5678");
  }

  ASSERT_EQ(config->resource->attributes_list, "foo=aaaa,bar=bbbb");
}

TEST(YamlResource, empty_detection)
{
  std::string yaml = R"(
file_format: "1.0-resource"
resource:
  detection/development:
)";

  auto config = DoParse(yaml);
  ASSERT_NE(config, nullptr);
  ASSERT_NE(config->resource, nullptr);
  ASSERT_NE(config->resource->detection, nullptr);
  ASSERT_EQ(config->resource->detection->attributes, nullptr);
  ASSERT_EQ(config->resource->detection->detectors.size(), 0);
}

TEST(YamlResource, detection_with_attributes_include_exclude)
{
  std::string yaml = R"(
file_format: "1.0-resource"
resource:
  detection/development:
    attributes:
      included:
        - host.*
        - process.*
      excluded:
        - host.cpu.*
)";

  auto config = DoParse(yaml);
  ASSERT_NE(config, nullptr);
  ASSERT_NE(config->resource, nullptr);
  ASSERT_NE(config->resource->detection, nullptr);
  ASSERT_NE(config->resource->detection->attributes, nullptr);
  ASSERT_NE(config->resource->detection->attributes->included, nullptr);
  ASSERT_EQ(config->resource->detection->attributes->included->string_array.size(), 2);
  ASSERT_EQ(config->resource->detection->attributes->included->string_array[0], "host.*");
  ASSERT_EQ(config->resource->detection->attributes->included->string_array[1], "process.*");
  ASSERT_NE(config->resource->detection->attributes->excluded, nullptr);
  ASSERT_EQ(config->resource->detection->attributes->excluded->string_array.size(), 1);
  ASSERT_EQ(config->resource->detection->attributes->excluded->string_array[0], "host.cpu.*");
  ASSERT_EQ(config->resource->detection->detectors.size(), 0);
}

TEST(YamlResource, detection_with_container_detector)
{
  std::string yaml = R"(
file_format: "1.0-resource"
resource:
  detection/development:
    detectors:
      - container:
)";

  auto config = DoParse(yaml);
  ASSERT_NE(config, nullptr);
  ASSERT_NE(config->resource, nullptr);
  ASSERT_NE(config->resource->detection, nullptr);
  ASSERT_EQ(config->resource->detection->detectors.size(), 1);
  TestDetectorVisitor visitor;
  config->resource->detection->detectors[0]->Accept(&visitor);
  EXPECT_EQ(visitor.type_matched, DetectorType::kContainer);
}

TEST(YamlResource, detection_with_host_detector)
{
  std::string yaml = R"(
file_format: "1.0-resource"
resource:
  detection/development:
    detectors:
      - host:
)";

  auto config = DoParse(yaml);
  ASSERT_NE(config, nullptr);
  ASSERT_NE(config->resource, nullptr);
  ASSERT_NE(config->resource->detection, nullptr);
  ASSERT_EQ(config->resource->detection->detectors.size(), 1);
  TestDetectorVisitor visitor;
  config->resource->detection->detectors[0]->Accept(&visitor);
  EXPECT_EQ(visitor.type_matched, DetectorType::kHost);
}

TEST(YamlResource, detection_with_process_detector)
{
  std::string yaml = R"(
file_format: "1.0-resource"
resource:
  detection/development:
    detectors:
      - process:
)";

  auto config = DoParse(yaml);
  ASSERT_NE(config, nullptr);
  ASSERT_NE(config->resource, nullptr);
  ASSERT_NE(config->resource->detection, nullptr);
  ASSERT_EQ(config->resource->detection->detectors.size(), 1);
  TestDetectorVisitor visitor;
  config->resource->detection->detectors[0]->Accept(&visitor);
  EXPECT_EQ(visitor.type_matched, DetectorType::kProcess);
}

TEST(YamlResource, detection_with_service_detector)
{
  std::string yaml = R"(
file_format: "1.0-resource"
resource:
  detection/development:
    detectors:
      - service:
)";

  auto config = DoParse(yaml);
  ASSERT_NE(config, nullptr);
  ASSERT_NE(config->resource, nullptr);
  ASSERT_NE(config->resource->detection, nullptr);
  ASSERT_EQ(config->resource->detection->detectors.size(), 1);
  TestDetectorVisitor visitor;
  config->resource->detection->detectors[0]->Accept(&visitor);
  EXPECT_EQ(visitor.type_matched, DetectorType::kService);
}

TEST(YamlResource, detection_with_multiple_detectors)
{
  std::string yaml = R"(
file_format: "1.0-resource"
resource:
  detection/development:
    detectors:
      - container:
      - host:
      - process:
      - service:
)";

  auto config = DoParse(yaml);
  ASSERT_NE(config, nullptr);
  ASSERT_NE(config->resource, nullptr);
  ASSERT_NE(config->resource->detection, nullptr);
  ASSERT_EQ(config->resource->detection->detectors.size(), 4);
  const DetectorType expected[] = {DetectorType::kContainer, DetectorType::kHost,
                                   DetectorType::kProcess, DetectorType::kService};
  for (std::size_t i = 0; i < config->resource->detection->detectors.size(); ++i)
  {
    TestDetectorVisitor visitor;
    config->resource->detection->detectors[i]->Accept(&visitor);
    EXPECT_EQ(visitor.type_matched, expected[i]);
  }
}

TEST(YamlResource, detection_with_extension_detector)
{
  std::string yaml = R"(
file_format: "1.0-resource"
resource:
  detection/development:
    detectors:
      - my_custom_detector:
)";

  auto config = DoParse(yaml);
  ASSERT_NE(config, nullptr);
  ASSERT_NE(config->resource, nullptr);
  ASSERT_NE(config->resource->detection, nullptr);
  ASSERT_EQ(config->resource->detection->detectors.size(), 1);
  TestDetectorVisitor visitor;
  config->resource->detection->detectors[0]->Accept(&visitor);
  EXPECT_EQ(visitor.type_matched, DetectorType::kExtension);
  EXPECT_EQ(visitor.extension_name, "my_custom_detector");
}

TEST(YamlResource, detection_with_attributes_and_detectors)
{
  std::string yaml = R"(
file_format: "1.0-resource"
resource:
  detection/development:
    attributes:
      included:
        - container.*
    detectors:
      - container:
      - host:
)";

  auto config = DoParse(yaml);
  ASSERT_NE(config, nullptr);
  ASSERT_NE(config->resource, nullptr);
  ASSERT_NE(config->resource->detection, nullptr);
  ASSERT_NE(config->resource->detection->attributes, nullptr);
  ASSERT_NE(config->resource->detection->attributes->included, nullptr);
  ASSERT_EQ(config->resource->detection->attributes->included->string_array.size(), 1);
  ASSERT_EQ(config->resource->detection->attributes->included->string_array[0], "container.*");
  ASSERT_EQ(config->resource->detection->detectors.size(), 2);
}

TEST(YamlResource, string_attribute_substitution)
{
  setenv("TEST_ENV_NAME", "my-service", 1);

  std::string yaml = R"(
file_format: "1.0-resource"
resource:
  attributes:
    - name: service.name
      value: ${TEST_ENV_NAME}
)";

  auto config = DoParse(yaml);
  ASSERT_NE(config, nullptr);
  ASSERT_NE(config->resource, nullptr);
  ASSERT_NE(config->resource->attributes, nullptr);
  auto &kv = config->resource->attributes->kv_map;
  ASSERT_EQ(kv.count("service.name"), 1u);
  auto *val =
      static_cast<config_sdk::StringAttributeValueConfiguration *>(kv.at("service.name").get());
  ASSERT_EQ(val->value, "my-service");

  unsetenv("TEST_ENV_NAME");
}

TEST(YamlResource, string_attribute_substitution_fallback_empty)
{
  unsetenv("TEST_ENV_NAME");

  std::string yaml = R"(
file_format: "1.0-resource"
resource:
  attributes:
    - name: service.name
      value: ${TEST_ENV_NAME:-fallback-name}
)";

  auto config = DoParse(yaml);
  ASSERT_NE(config, nullptr);
  ASSERT_NE(config->resource, nullptr);
  ASSERT_NE(config->resource->attributes, nullptr);
  auto &kv = config->resource->attributes->kv_map;
  ASSERT_EQ(kv.count("service.name"), 1u);
  auto *val =
      static_cast<config_sdk::StringAttributeValueConfiguration *>(kv.at("service.name").get());
  ASSERT_EQ(val->value, "fallback-name");
}

TEST(YamlResource, boolean_attribute_substitution)
{
  setenv("TEST_ENV_NAME", "true", 1);

  std::string yaml = R"(
file_format: "1.0-resource"
resource:
  attributes:
    - name: flag
      value: ${TEST_ENV_NAME}
      type: bool
)";

  auto config = DoParse(yaml);
  ASSERT_NE(config, nullptr);
  ASSERT_NE(config->resource, nullptr);
  ASSERT_NE(config->resource->attributes, nullptr);
  auto &kv = config->resource->attributes->kv_map;
  ASSERT_EQ(kv.count("flag"), 1u);
  auto *val = static_cast<config_sdk::BooleanAttributeValueConfiguration *>(kv.at("flag").get());
  ASSERT_EQ(val->value, true);

  unsetenv("TEST_ENV_NAME");
}

TEST(YamlResource, boolean_attribute_substitution_fallback)
{
  unsetenv("TEST_ENV_NAME");

  std::string yaml = R"(
file_format: "1.0-resource"
resource:
  attributes:
    - name: flag
      value: ${TEST_ENV_NAME:-false}
      type: bool
)";

  auto config = DoParse(yaml);
  ASSERT_NE(config, nullptr);
  ASSERT_NE(config->resource, nullptr);
  ASSERT_NE(config->resource->attributes, nullptr);
  auto &kv = config->resource->attributes->kv_map;
  ASSERT_EQ(kv.count("flag"), 1u);
  auto *val = static_cast<config_sdk::BooleanAttributeValueConfiguration *>(kv.at("flag").get());
  ASSERT_EQ(val->value, false);
}

TEST(YamlResource, integer_attribute_substitution)
{
  setenv("TEST_ENV_NAME", "42", 1);

  std::string yaml = R"(
file_format: "1.0-resource"
resource:
  attributes:
    - name: count
      value: ${TEST_ENV_NAME}
      type: int
)";

  auto config = DoParse(yaml);
  ASSERT_NE(config, nullptr);
  ASSERT_NE(config->resource, nullptr);
  ASSERT_NE(config->resource->attributes, nullptr);
  auto &kv = config->resource->attributes->kv_map;
  ASSERT_EQ(kv.count("count"), 1u);
  auto *val = static_cast<config_sdk::IntegerAttributeValueConfiguration *>(kv.at("count").get());
  ASSERT_EQ(val->value, 42);

  unsetenv("TEST_ENV_NAME");
}

TEST(YamlResource, integer_attribute_substitution_fallback)
{
  unsetenv("TEST_ENV_NAME");

  std::string yaml = R"(
file_format: "1.0-resource"
resource:
  attributes:
    - name: count
      value: ${TEST_ENV_NAME:-99}
      type: int
)";

  auto config = DoParse(yaml);
  ASSERT_NE(config, nullptr);
  ASSERT_NE(config->resource, nullptr);
  ASSERT_NE(config->resource->attributes, nullptr);
  auto &kv = config->resource->attributes->kv_map;
  ASSERT_EQ(kv.count("count"), 1u);
  auto *val = static_cast<config_sdk::IntegerAttributeValueConfiguration *>(kv.at("count").get());
  ASSERT_EQ(val->value, 99);
}

TEST(YamlResource, double_attribute_substitution)
{
  setenv("TEST_ENV_NAME", "3.14", 1);

  std::string yaml = R"(
file_format: "1.0-resource"
resource:
  attributes:
    - name: ratio
      value: ${TEST_ENV_NAME}
      type: double
)";

  auto config = DoParse(yaml);
  ASSERT_NE(config, nullptr);
  ASSERT_NE(config->resource, nullptr);
  ASSERT_NE(config->resource->attributes, nullptr);
  auto &kv = config->resource->attributes->kv_map;
  ASSERT_EQ(kv.count("ratio"), 1u);
  auto *val = static_cast<config_sdk::DoubleAttributeValueConfiguration *>(kv.at("ratio").get());
  ASSERT_DOUBLE_EQ(val->value, 3.14);

  unsetenv("TEST_ENV_NAME");
}

TEST(YamlResource, double_attribute_substitution_fallback)
{
  unsetenv("TEST_ENV_NAME");

  std::string yaml = R"(
file_format: "1.0-resource"
resource:
  attributes:
    - name: ratio
      value: ${TEST_ENV_NAME:-2.71}
      type: double
)";

  auto config = DoParse(yaml);
  ASSERT_NE(config, nullptr);
  ASSERT_NE(config->resource, nullptr);
  ASSERT_NE(config->resource->attributes, nullptr);
  auto &kv = config->resource->attributes->kv_map;
  ASSERT_EQ(kv.count("ratio"), 1u);
  auto *val = static_cast<config_sdk::DoubleAttributeValueConfiguration *>(kv.at("ratio").get());
  ASSERT_DOUBLE_EQ(val->value, 2.71);
}

TEST(YamlResource, string_array_attribute_substitution)
{
  setenv("TEST_ENV_NAME", "hello", 1);

  std::string yaml = R"(
file_format: "1.0-resource"
resource:
  attributes:
    - name: tags
      value:
        - ${TEST_ENV_NAME}
        - world
      type: string_array
)";

  auto config = DoParse(yaml);
  ASSERT_NE(config, nullptr);
  ASSERT_NE(config->resource, nullptr);
  ASSERT_NE(config->resource->attributes, nullptr);
  auto &kv = config->resource->attributes->kv_map;
  ASSERT_EQ(kv.count("tags"), 1u);
  auto *val =
      static_cast<config_sdk::StringArrayAttributeValueConfiguration *>(kv.at("tags").get());
  ASSERT_EQ(val->value.size(), 2u);
  ASSERT_EQ(val->value[0], "hello");
  ASSERT_EQ(val->value[1], "world");

  unsetenv("TEST_ENV_NAME");
}

TEST(YamlResource, integer_array_attribute_substitution)
{
  setenv("TEST_ENV_NAME", "7", 1);

  std::string yaml = R"(
file_format: "1.0-resource"
resource:
  attributes:
    - name: ids
      value:
        - ${TEST_ENV_NAME}
        - 8
      type: int_array
)";

  auto config = DoParse(yaml);
  ASSERT_NE(config, nullptr);
  ASSERT_NE(config->resource, nullptr);
  ASSERT_NE(config->resource->attributes, nullptr);
  auto &kv = config->resource->attributes->kv_map;
  ASSERT_EQ(kv.count("ids"), 1u);
  auto *val =
      static_cast<config_sdk::IntegerArrayAttributeValueConfiguration *>(kv.at("ids").get());
  ASSERT_EQ(val->value.size(), 2u);
  ASSERT_EQ(val->value[0], 7u);
  ASSERT_EQ(val->value[1], 8u);

  unsetenv("TEST_ENV_NAME");
}

TEST(YamlResource, double_array_attribute_substitution)
{
  setenv("TEST_ENV_NAME", "1.5", 1);

  std::string yaml = R"(
file_format: "1.0-resource"
resource:
  attributes:
    - name: scores
      value:
        - ${TEST_ENV_NAME}
        - 2.5
      type: double_array
)";

  auto config = DoParse(yaml);
  ASSERT_NE(config, nullptr);
  ASSERT_NE(config->resource, nullptr);
  ASSERT_NE(config->resource->attributes, nullptr);
  auto &kv = config->resource->attributes->kv_map;
  ASSERT_EQ(kv.count("scores"), 1u);
  auto *val =
      static_cast<config_sdk::DoubleArrayAttributeValueConfiguration *>(kv.at("scores").get());
  ASSERT_EQ(val->value.size(), 2u);
  ASSERT_DOUBLE_EQ(val->value[0], 1.5);
  ASSERT_DOUBLE_EQ(val->value[1], 2.5);

  unsetenv("TEST_ENV_NAME");
}

TEST(YamlResource, boolean_array_attribute_substitution)
{
  setenv("TEST_ENV_NAME", "true", 1);

  std::string yaml = R"(
file_format: "1.0-resource"
resource:
  attributes:
    - name: flags
      value:
        - ${TEST_ENV_NAME}
        - false
      type: bool_array
)";

  auto config = DoParse(yaml);
  ASSERT_NE(config, nullptr);
  ASSERT_NE(config->resource, nullptr);
  ASSERT_NE(config->resource->attributes, nullptr);
  auto &kv = config->resource->attributes->kv_map;
  ASSERT_EQ(kv.count("flags"), 1u);
  auto *val =
      static_cast<config_sdk::BooleanArrayAttributeValueConfiguration *>(kv.at("flags").get());
  ASSERT_EQ(val->value.size(), 2u);
  ASSERT_EQ(val->value[0], true);
  ASSERT_EQ(val->value[1], false);

  unsetenv("TEST_ENV_NAME");
}

TEST(YamlResource, null_attribute_value_skipped)
{
  // YAML null value (value:) must be skipped per schema nullBehavior.
  std::string yaml = R"(
file_format: "1.0-resource"
resource:
  attributes:
    - name: service.name
      value:
    - name: host.name
      value: "my-host"
)";

  auto config = DoParse(yaml);
  ASSERT_NE(config, nullptr);
  ASSERT_NE(config->resource, nullptr);
  ASSERT_NE(config->resource->attributes, nullptr);
  const auto &kv = config->resource->attributes->kv_map;
  // null value entry must be absent
  ASSERT_EQ(kv.count("service.name"), 0u);
  // non-null entry must be present
  ASSERT_EQ(kv.count("host.name"), 1u);
}

TEST(YamlResource, empty_name_attribute_parse_error)
{
  // An empty attribute name (e.g. from an unset env var) is an invalid config.
  unsetenv("TEST_ENV_NAME");

  std::string yaml = R"(
file_format: "1.0-resource"
resource:
  attributes:
    - name: ${TEST_ENV_NAME}
      value: "some-value"
    - name: host.name
      value: "my-host"
)";

  auto config = DoParse(yaml);
  ASSERT_EQ(config, nullptr);
}
