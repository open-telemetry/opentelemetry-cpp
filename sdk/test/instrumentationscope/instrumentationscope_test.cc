// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/instrumentationlibrary/instrumentation_library.h"
#include "opentelemetry/sdk/instrumentationscope/instrumentation_scope.h"

#include <gtest/gtest.h>
#include <string>
#include <unordered_map>
#include <vector>

using namespace opentelemetry;
using namespace opentelemetry::sdk::instrumentationscope;

TEST(InstrumentationScope, CreateInstrumentationScope)
{
  std::string library_name    = "opentelemetry-cpp";
  std::string library_version = "0.1.0";
  std::string schema_url      = "https://opentelemetry.io/schemas/1.2.0";
  uint32_t attrubite_value3[] = {7, 8, 9};
  auto instrumentation_scope  = InstrumentationScope::Create(
      library_name, library_version, schema_url,
      {{"attribute-key1", "attribute-value"},
       {"attribute-key2", static_cast<int32_t>(123)},
       {"attribute-key3", opentelemetry::nostd::span<uint32_t>(attrubite_value3)}});

  EXPECT_EQ(instrumentation_scope->GetName(), library_name);
  EXPECT_EQ(instrumentation_scope->GetVersion(), library_version);
  EXPECT_EQ(instrumentation_scope->GetSchemaURL(), schema_url);

  auto attribute1 = instrumentation_scope->GetAttributes().find("attribute-key1");
  auto attribute2 = instrumentation_scope->GetAttributes().find("attribute-key2");
  auto attribute3 = instrumentation_scope->GetAttributes().find("attribute-key3");

  ASSERT_FALSE(attribute1 == instrumentation_scope->GetAttributes().end());
  ASSERT_TRUE(opentelemetry::nostd::holds_alternative<std::string>(attribute1->second));
  EXPECT_EQ(opentelemetry::nostd::get<std::string>(attribute1->second), "attribute-value");

  ASSERT_FALSE(attribute2 == instrumentation_scope->GetAttributes().end());
  ASSERT_TRUE(opentelemetry::nostd::holds_alternative<int32_t>(attribute2->second));
  EXPECT_EQ(opentelemetry::nostd::get<int32_t>(attribute2->second), 123);

  ASSERT_FALSE(attribute3 == instrumentation_scope->GetAttributes().end());
  ASSERT_TRUE(opentelemetry::nostd::holds_alternative<std::vector<uint32_t>>(attribute3->second));

  const std::vector<uint32_t> &attribute3_values =
      opentelemetry::nostd::get<std::vector<uint32_t>>(attribute3->second);
  EXPECT_EQ(attribute3_values.size(), 3);
  for (std::vector<uint32_t>::size_type i = 0; i < 3; ++i)
  {
    EXPECT_EQ(attribute3_values[i], attrubite_value3[i]);
  }
}

TEST(InstrumentationScope, CreateInstrumentationScopeWithLoopForAttributes)
{
  std::string library_name    = "opentelemetry-cpp";
  std::string library_version = "0.1.0";
  std::string schema_url      = "https://opentelemetry.io/schemas/1.2.0";
  uint32_t attrubite_value3[] = {7, 8, 9};

  std::unordered_map<std::string, opentelemetry::common::AttributeValue> attributes = {
      {"attribute-key1", "attribute-value"},
      {"attribute-key2", static_cast<int32_t>(123)},
      {"attribute-key3", opentelemetry::nostd::span<uint32_t>(attrubite_value3)}};

  auto instrumentation_scope =
      InstrumentationScope::Create(library_name, library_version, schema_url, attributes);

  EXPECT_EQ(instrumentation_scope->GetName(), library_name);
  EXPECT_EQ(instrumentation_scope->GetVersion(), library_version);
  EXPECT_EQ(instrumentation_scope->GetSchemaURL(), schema_url);

  auto attribute1 = instrumentation_scope->GetAttributes().find("attribute-key1");
  auto attribute2 = instrumentation_scope->GetAttributes().find("attribute-key2");
  auto attribute3 = instrumentation_scope->GetAttributes().find("attribute-key3");

  ASSERT_FALSE(attribute1 == instrumentation_scope->GetAttributes().end());
  ASSERT_TRUE(opentelemetry::nostd::holds_alternative<std::string>(attribute1->second));
  EXPECT_EQ(opentelemetry::nostd::get<std::string>(attribute1->second), "attribute-value");

  ASSERT_FALSE(attribute2 == instrumentation_scope->GetAttributes().end());
  ASSERT_TRUE(opentelemetry::nostd::holds_alternative<int32_t>(attribute2->second));
  EXPECT_EQ(opentelemetry::nostd::get<int32_t>(attribute2->second), 123);

  ASSERT_FALSE(attribute3 == instrumentation_scope->GetAttributes().end());
  ASSERT_TRUE(opentelemetry::nostd::holds_alternative<std::vector<uint32_t>>(attribute3->second));
  const std::vector<uint32_t> &attribute3_values =
      opentelemetry::nostd::get<std::vector<uint32_t>>(attribute3->second);

  EXPECT_EQ(attribute3_values.size(), 3);
  for (std::vector<uint32_t>::size_type i = 0; i < 3; ++i)
  {
    EXPECT_EQ(attribute3_values[i], attrubite_value3[i]);
  }
}

TEST(InstrumentationScope, CreateInstrumentationScopeWithKeyValueIterableAttributes)
{
  std::string library_name    = "opentelemetry-cpp";
  std::string library_version = "0.1.0";
  std::string schema_url      = "https://opentelemetry.io/schemas/1.2.0";
  uint32_t attrubite_value3[] = {7, 8, 9};

  std::unordered_map<std::string, opentelemetry::common::AttributeValue> attributes = {
      {"attribute-key1", "attribute-value"},
      {"attribute-key2", static_cast<int32_t>(123)},
      {"attribute-key3", opentelemetry::nostd::span<uint32_t>(attrubite_value3)}};

  opentelemetry::common::KeyValueIterableView<
      std::unordered_map<std::string, opentelemetry::common::AttributeValue>>
      attributes_view{attributes};

  auto instrumentation_scope =
      InstrumentationScope::Create(library_name, library_version, schema_url, attributes_view);

  EXPECT_EQ(instrumentation_scope->GetName(), library_name);
  EXPECT_EQ(instrumentation_scope->GetVersion(), library_version);
  EXPECT_EQ(instrumentation_scope->GetSchemaURL(), schema_url);

  auto attribute1 = instrumentation_scope->GetAttributes().find("attribute-key1");
  auto attribute2 = instrumentation_scope->GetAttributes().find("attribute-key2");
  auto attribute3 = instrumentation_scope->GetAttributes().find("attribute-key3");

  ASSERT_FALSE(attribute1 == instrumentation_scope->GetAttributes().end());
  ASSERT_TRUE(opentelemetry::nostd::holds_alternative<std::string>(attribute1->second));
  EXPECT_EQ(opentelemetry::nostd::get<std::string>(attribute1->second), "attribute-value");

  ASSERT_FALSE(attribute2 == instrumentation_scope->GetAttributes().end());
  ASSERT_TRUE(opentelemetry::nostd::holds_alternative<int32_t>(attribute2->second));
  EXPECT_EQ(opentelemetry::nostd::get<int32_t>(attribute2->second), 123);

  ASSERT_FALSE(attribute3 == instrumentation_scope->GetAttributes().end());
  ASSERT_TRUE(opentelemetry::nostd::holds_alternative<std::vector<uint32_t>>(attribute3->second));
  const std::vector<uint32_t> &attribute3_values =
      opentelemetry::nostd::get<std::vector<uint32_t>>(attribute3->second);

  EXPECT_EQ(attribute3_values.size(), 3);
  for (std::vector<uint32_t>::size_type i = 0; i < 3; ++i)
  {
    EXPECT_EQ(attribute3_values[i], attrubite_value3[i]);
  }
}

TEST(InstrumentationScope, SetAttribute)
{
  std::string library_name    = "opentelemetry-cpp";
  std::string library_version = "0.1.0";
  std::string schema_url      = "https://opentelemetry.io/schemas/1.2.0";
  uint32_t attrubite_value3[] = {7, 8, 9};
  auto instrumentation_scope =
      InstrumentationScope::Create(library_name, library_version, schema_url);

  EXPECT_EQ(instrumentation_scope->GetName(), library_name);
  EXPECT_EQ(instrumentation_scope->GetVersion(), library_version);
  EXPECT_EQ(instrumentation_scope->GetSchemaURL(), schema_url);
  EXPECT_EQ(instrumentation_scope->GetAttributes().size(), 0);

  instrumentation_scope->SetAttribute("attribute-key1", "attribute-value");
  instrumentation_scope->SetAttribute("attribute-key2", static_cast<int32_t>(123));
  instrumentation_scope->SetAttribute("attribute-key3",
                                      opentelemetry::nostd::span<uint32_t>(attrubite_value3));
  EXPECT_EQ(instrumentation_scope->GetAttributes().size(), 3);

  auto attribute1 = instrumentation_scope->GetAttributes().find("attribute-key1");
  auto attribute2 = instrumentation_scope->GetAttributes().find("attribute-key2");
  auto attribute3 = instrumentation_scope->GetAttributes().find("attribute-key3");

  ASSERT_FALSE(attribute1 == instrumentation_scope->GetAttributes().end());
  ASSERT_TRUE(opentelemetry::nostd::holds_alternative<std::string>(attribute1->second));
  EXPECT_EQ(opentelemetry::nostd::get<std::string>(attribute1->second), "attribute-value");

  ASSERT_FALSE(attribute2 == instrumentation_scope->GetAttributes().end());
  ASSERT_TRUE(opentelemetry::nostd::holds_alternative<int32_t>(attribute2->second));
  EXPECT_EQ(opentelemetry::nostd::get<int32_t>(attribute2->second), 123);

  ASSERT_FALSE(attribute3 == instrumentation_scope->GetAttributes().end());
  ASSERT_TRUE(opentelemetry::nostd::holds_alternative<std::vector<uint32_t>>(attribute3->second));

  const std::vector<uint32_t> &attribute3_values =
      opentelemetry::nostd::get<std::vector<uint32_t>>(attribute3->second);
  EXPECT_EQ(attribute3_values.size(), 3);
  for (std::vector<uint32_t>::size_type i = 0; i < 3; ++i)
  {
    EXPECT_EQ(attribute3_values[i], attrubite_value3[i]);
  }
}

TEST(InstrumentationScope, LegacyInstrumentationLibrary)
{

  std::string library_name    = "opentelemetry-cpp";
  std::string library_version = "0.1.0";
  std::string schema_url      = "https://opentelemetry.io/schemas/1.2.0";
  auto instrumentation_library =
      opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary::Create(
          library_name, library_version, schema_url);

  EXPECT_EQ(instrumentation_library->GetName(), library_name);
  EXPECT_EQ(instrumentation_library->GetVersion(), library_version);
  EXPECT_EQ(instrumentation_library->GetSchemaURL(), schema_url);
}
