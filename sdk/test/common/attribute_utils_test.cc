// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <map>
#include <string>
#include <unordered_map>

#include "opentelemetry/common/key_value_iterable_view.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/sdk/common/attribute_utils.h"

namespace
{

template <class DataType>
static void CheckExceptOneValue(const opentelemetry::sdk::common::MixedAttributeMap &data,
                                opentelemetry::nostd::string_view key,
                                DataType value)
{
  auto iter1 = data.GetAttributes().find(std::string(key));
  EXPECT_TRUE(iter1 != data.GetAttributes().end());

  auto iter2 = data.GetOwnedAttributes().find(std::string(key));
  EXPECT_TRUE(iter2 != data.GetOwnedAttributes().end());

  EXPECT_EQ(opentelemetry::nostd::get<DataType>(iter1->second), value);
  EXPECT_EQ(opentelemetry::nostd::get<DataType>(iter2->second), value);
}

template <class DataType>
static void CheckExceptOneStringView(const opentelemetry::sdk::common::MixedAttributeMap &data,
                                     opentelemetry::nostd::string_view key,
                                     opentelemetry::nostd::string_view value)
{
  auto iter1 = data.GetAttributes().find(std::string(key));
  EXPECT_TRUE(iter1 != data.GetAttributes().end());

  auto iter2 = data.GetOwnedAttributes().find(std::string(key));
  EXPECT_TRUE(iter2 != data.GetOwnedAttributes().end());

  EXPECT_EQ(opentelemetry::nostd::get<DataType>(iter1->second), value);
  EXPECT_EQ(opentelemetry::nostd::get<std::string>(iter2->second), value);
}

template <class DataType, size_t DataSize>
static void CheckExceptArrayValue(const opentelemetry::sdk::common::MixedAttributeMap &data,
                                  opentelemetry::nostd::string_view key,
                                  DataType (&value)[DataSize])
{
  auto iter1 = data.GetAttributes().find(std::string(key));
  EXPECT_TRUE(iter1 != data.GetAttributes().end());

  auto iter2 = data.GetOwnedAttributes().find(std::string(key));
  EXPECT_TRUE(iter2 != data.GetOwnedAttributes().end());

  for (size_t i = 0; i < DataSize; ++i)
  {
    EXPECT_EQ(
        opentelemetry::nostd::get<opentelemetry::nostd::span<const DataType>>(iter1->second)[i],
        value[i]);
    EXPECT_EQ(opentelemetry::nostd::get<std::vector<DataType>>(iter2->second)[i], value[i]);
  }
}

template <size_t DataSize>
static void CheckExceptArrayString(const opentelemetry::sdk::common::MixedAttributeMap &data,
                                   opentelemetry::nostd::string_view key,
                                   opentelemetry::nostd::string_view (&value)[DataSize])
{
  auto iter1 = data.GetAttributes().find(std::string(key));
  EXPECT_TRUE(iter1 != data.GetAttributes().end());

  auto iter2 = data.GetOwnedAttributes().find(std::string(key));
  EXPECT_TRUE(iter2 != data.GetOwnedAttributes().end());

  for (size_t i = 0; i < DataSize; ++i)
  {
    EXPECT_EQ(
        opentelemetry::nostd::get<
            opentelemetry::nostd::span<const opentelemetry::nostd::string_view>>(iter1->second)[i],
        value[i]);
    EXPECT_EQ(opentelemetry::nostd::get<std::vector<std::string>>(iter2->second)[i], value[i]);
  }
}

}  // namespace

TEST(AttributeMapTest, DefaultConstruction)
{
  opentelemetry::sdk::common::AttributeMap attribute_map;
  EXPECT_EQ(attribute_map.GetAttributes().size(), 0);
}

TEST(OrderedAttributeMapTest, DefaultConstruction)
{
  opentelemetry::sdk::common::OrderedAttributeMap attribute_map;
  EXPECT_EQ(attribute_map.GetAttributes().size(), 0);
}

TEST(AttributeMapTest, AttributesConstruction)
{
  const int kNumAttributes              = 3;
  std::string keys[kNumAttributes]      = {"attr1", "attr2", "attr3"};
  int values[kNumAttributes]            = {15, 24, 37};
  std::map<std::string, int> attributes = {
      {keys[0], values[0]}, {keys[1], values[1]}, {keys[2], values[2]}};

  opentelemetry::common::KeyValueIterableView<std::map<std::string, int>> iterable(attributes);
  opentelemetry::sdk::common::AttributeMap attribute_map(iterable);

  for (int i = 0; i < kNumAttributes; i++)
  {
    EXPECT_EQ(opentelemetry::nostd::get<int>(attribute_map.GetAttributes().at(keys[i])), values[i]);
  }
}

TEST(OrderedAttributeMapTest, AttributesConstruction)
{
  const int kNumAttributes              = 3;
  std::string keys[kNumAttributes]      = {"attr1", "attr2", "attr3"};
  int values[kNumAttributes]            = {15, 24, 37};
  std::map<std::string, int> attributes = {
      {keys[0], values[0]}, {keys[1], values[1]}, {keys[2], values[2]}};

  opentelemetry::common::KeyValueIterableView<std::map<std::string, int>> iterable(attributes);
  opentelemetry::sdk::common::OrderedAttributeMap attribute_map(iterable);

  for (int i = 0; i < kNumAttributes; i++)
  {
    EXPECT_EQ(opentelemetry::nostd::get<int>(attribute_map.GetAttributes().at(keys[i])), values[i]);
  }
}

TEST(MixedAttributeMapTest, SetterAndGetter)
{
  opentelemetry::sdk::common::MixedAttributeMap attribute_map;
  attribute_map.Reserve(16);
  EXPECT_TRUE(attribute_map.Empty());

  attribute_map.SetAttribute("bool", true);
  attribute_map.SetAttribute("int32_t", static_cast<int32_t>(42));
  attribute_map.SetAttribute("int64_t", static_cast<int64_t>(43));
  attribute_map.SetAttribute("uint32_t", static_cast<uint32_t>(44));
  attribute_map.SetAttribute("double", static_cast<double>(45.0));
  attribute_map.SetAttribute("uint64_t", static_cast<uint64_t>(46));
  attribute_map.SetAttribute("const char *", "const char *");
  attribute_map.SetAttribute("string_view", opentelemetry::nostd::string_view("string_view"));

  bool array_bool[]       = {true, false, true};
  uint8_t array_uint8[]   = {47, 48};
  int32_t array_int32[]   = {48, 49};
  int64_t array_int64[]   = {49, 50};
  uint32_t array_uint32[] = {50, 51};
  double array_double[]   = {51.0, 52.0};
  uint64_t array_uint64[] = {52, 53};

  attribute_map.SetAttribute("sbool", opentelemetry::nostd::span<const bool>(array_bool));
  attribute_map.SetAttribute("suint8_t", opentelemetry::nostd::span<const uint8_t>(array_uint8));
  attribute_map.SetAttribute("sint32_t", opentelemetry::nostd::span<const int32_t>(array_int32));
  attribute_map.SetAttribute("sint64_t", opentelemetry::nostd::span<const int64_t>(array_int64));
  attribute_map.SetAttribute("suint32_t", opentelemetry::nostd::span<const uint32_t>(array_uint32));
  attribute_map.SetAttribute("sdouble", opentelemetry::nostd::span<const double>(array_double));
  attribute_map.SetAttribute("suint64_t", opentelemetry::nostd::span<const uint64_t>(array_uint64));

  opentelemetry::nostd::string_view array_string_view[] = {"string_view1", "string_view2"};
  attribute_map.SetAttribute(
      "sstring_view",
      opentelemetry::nostd::span<const opentelemetry::nostd::string_view>(array_string_view));

  EXPECT_FALSE(attribute_map.Empty());
  EXPECT_EQ(attribute_map.Size(), 16);

  CheckExceptOneValue<bool>(attribute_map, "bool", true);
  CheckExceptOneValue<int32_t>(attribute_map, "int32_t", static_cast<int32_t>(42));
  CheckExceptOneValue<int64_t>(attribute_map, "int64_t", static_cast<int64_t>(43));
  CheckExceptOneValue<uint32_t>(attribute_map, "uint32_t", static_cast<uint32_t>(44));
  CheckExceptOneValue<double>(attribute_map, "double", static_cast<double>(45.0));
  CheckExceptOneValue<uint64_t>(attribute_map, "uint64_t", static_cast<uint64_t>(46));

  CheckExceptOneStringView<const char *>(attribute_map, "const char *", "const char *");
  CheckExceptOneStringView<opentelemetry::nostd::string_view>(attribute_map, "string_view",
                                                              "string_view");

  CheckExceptArrayValue(attribute_map, "sbool", array_bool);
  CheckExceptArrayValue(attribute_map, "suint8_t", array_uint8);
  CheckExceptArrayValue(attribute_map, "sint32_t", array_int32);
  CheckExceptArrayValue(attribute_map, "sint64_t", array_int64);
  CheckExceptArrayValue(attribute_map, "suint32_t", array_uint32);
  CheckExceptArrayValue(attribute_map, "sdouble", array_double);
  CheckExceptArrayValue(attribute_map, "suint64_t", array_uint64);
  CheckExceptArrayString(attribute_map, "sstring_view", array_string_view);
}
