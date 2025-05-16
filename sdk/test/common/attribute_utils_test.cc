// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <stddef.h>
#include <stdint.h>
#include <array>
#include <initializer_list>
#include <map>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/common/key_value_iterable_view.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/utility.h"
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

TEST(AttributeEqualToVisitorTest, AttributeValueEqualTo)
{
  namespace sdk   = opentelemetry::sdk::common;
  namespace api   = opentelemetry::common;
  namespace nostd = opentelemetry::nostd;

  using AV = api::AttributeValue;
  using OV = sdk::OwnedAttributeValue;

  sdk::AttributeEqualToVisitor equal_to_visitor;

  // arithmetic types
  EXPECT_TRUE(opentelemetry::nostd::visit(equal_to_visitor, OV{bool(true)}, AV{bool(true)}));
  EXPECT_TRUE(opentelemetry::nostd::visit(equal_to_visitor, OV{int32_t(22)}, AV{int32_t(22)}));
  EXPECT_TRUE(opentelemetry::nostd::visit(equal_to_visitor, OV{int64_t(22)}, AV{int64_t(22)}));
  EXPECT_TRUE(opentelemetry::nostd::visit(equal_to_visitor, OV{uint32_t(22)}, AV{uint32_t(22)}));
  EXPECT_TRUE(opentelemetry::nostd::visit(equal_to_visitor, OV{uint64_t(22)}, AV{uint64_t(22)}));
  EXPECT_TRUE(opentelemetry::nostd::visit(equal_to_visitor, OV{double(22.0)}, AV{double(22.0)}));

  // string types
  EXPECT_TRUE(opentelemetry::nostd::visit(
      equal_to_visitor, OV{std::string("string to const char*")}, AV{"string to const char*"}));
  EXPECT_TRUE(opentelemetry::nostd::visit(equal_to_visitor,
                                          OV{std::string("string to string_view")},
                                          AV{nostd::string_view("string to string_view")}));

  // container types
  EXPECT_TRUE(opentelemetry::nostd::visit(equal_to_visitor, OV{std::vector<bool>{true, false}},
                                          AV{std::array<const bool, 2>{true, false}}));
  EXPECT_TRUE(opentelemetry::nostd::visit(equal_to_visitor, OV{std::vector<uint8_t>{33, 44}},
                                          AV{std::array<const uint8_t, 2>{33, 44}}));
  EXPECT_TRUE(opentelemetry::nostd::visit(equal_to_visitor, OV{std::vector<int32_t>{33, 44}},
                                          AV{std::array<const int32_t, 2>{33, 44}}));
  EXPECT_TRUE(opentelemetry::nostd::visit(equal_to_visitor, OV{std::vector<int64_t>{33, 44}},
                                          AV{std::array<const int64_t, 2>{33, 44}}));
  EXPECT_TRUE(opentelemetry::nostd::visit(equal_to_visitor, OV{std::vector<uint32_t>{33, 44}},
                                          AV{std::array<const uint32_t, 2>{33, 44}}));
  EXPECT_TRUE(opentelemetry::nostd::visit(equal_to_visitor, OV{std::vector<uint64_t>{33, 44}},
                                          AV{std::array<const uint64_t, 2>{33, 44}}));
  EXPECT_TRUE(opentelemetry::nostd::visit(equal_to_visitor, OV{std::vector<double>{33.0, 44.0}},
                                          AV{std::array<const double, 2>{33.0, 44.0}}));
  EXPECT_TRUE(opentelemetry::nostd::visit(
      equal_to_visitor, OV{std::vector<std::string>{"a string", "another string"}},
      AV{std::array<const nostd::string_view, 2>{"a string", "another string"}}));
}

TEST(AttributeEqualToVisitorTest, AttributeValueNotEqualTo)
{
  namespace sdk   = opentelemetry::sdk::common;
  namespace api   = opentelemetry::common;
  namespace nostd = opentelemetry::nostd;

  using AV = api::AttributeValue;
  using OV = sdk::OwnedAttributeValue;

  sdk::AttributeEqualToVisitor equal_to_visitor;

  // check different values of the same type
  EXPECT_FALSE(opentelemetry::nostd::visit(equal_to_visitor, OV{bool(true)}, AV{bool(false)}));
  EXPECT_FALSE(opentelemetry::nostd::visit(equal_to_visitor, OV{int32_t(22)}, AV{int32_t(33)}));
  EXPECT_FALSE(opentelemetry::nostd::visit(equal_to_visitor, OV{int64_t(22)}, AV{int64_t(33)}));
  EXPECT_FALSE(opentelemetry::nostd::visit(equal_to_visitor, OV{uint32_t(22)}, AV{uint32_t(33)}));
  EXPECT_FALSE(opentelemetry::nostd::visit(equal_to_visitor, OV{double(22.2)}, AV{double(33.3)}));
  EXPECT_FALSE(opentelemetry::nostd::visit(equal_to_visitor, OV{std::string("string one")},
                                           AV{"another string"}));
  EXPECT_FALSE(opentelemetry::nostd::visit(equal_to_visitor, OV{std::string("string one")},
                                           AV{nostd::string_view("another string")}));

  // check different value types
  EXPECT_FALSE(opentelemetry::nostd::visit(equal_to_visitor, OV{bool(true)}, AV{uint32_t(1)}));
  EXPECT_FALSE(opentelemetry::nostd::visit(equal_to_visitor, OV{int32_t(22)}, AV{uint32_t(22)}));

  // check containers of different element values
  EXPECT_FALSE(opentelemetry::nostd::visit(equal_to_visitor, OV{std::vector<bool>{true, false}},
                                           AV{std::array<const bool, 2>{false, true}}));
  EXPECT_FALSE(opentelemetry::nostd::visit(equal_to_visitor, OV{std::vector<int32_t>{22, 33}},
                                           AV{std::array<const int32_t, 2>{33, 44}}));
  EXPECT_FALSE(opentelemetry::nostd::visit(
      equal_to_visitor, OV{std::vector<std::string>{"a string", "another string"}},
      AV{std::array<const nostd::string_view, 2>{"a string", "a really different string"}}));

  // check containers of different element types
  EXPECT_FALSE(opentelemetry::nostd::visit(equal_to_visitor, OV{std::vector<int32_t>{22, 33}},
                                           AV{std::array<const uint32_t, 2>{22, 33}}));
}

TEST(AttributeMapTest, EqualTo)
{
  using Attributes = std::initializer_list<
      std::pair<opentelemetry::nostd::string_view, opentelemetry::common::AttributeValue>>;

  // check for case where both are empty
  Attributes attributes_empty = {};
  auto kv_iterable_empty =
      opentelemetry::common::MakeKeyValueIterableView<Attributes>(attributes_empty);
  opentelemetry::sdk::common::AttributeMap attribute_map_empty(kv_iterable_empty);
  EXPECT_TRUE(attribute_map_empty.EqualTo(kv_iterable_empty));

  // check for equality with a range of attributes and types
  Attributes attributes  = {{"key0", "some value"}, {"key1", 1}, {"key2", 2.0}, {"key3", true}};
  auto kv_iterable_match = opentelemetry::common::MakeKeyValueIterableView<Attributes>(attributes);
  opentelemetry::sdk::common::AttributeMap attribute_map(attributes);
  EXPECT_TRUE(attribute_map.EqualTo(kv_iterable_match));

  // check for several cases where the attributes are different
  Attributes attributes_different_value = {
      {"key0", "some value"}, {"key1", 1}, {"key2", 2.0}, {"key3", false}};
  Attributes attributes_different_type = {
      {"key0", "some value"}, {"key1", 1.0}, {"key2", 2.0}, {"key3", true}};
  Attributes attributes_different_size = {{"key0", "some value"}};

  // check for the case where the number of attributes is the same but all keys are different
  Attributes attributes_different_all = {{"a", "b"}, {"c", "d"}, {"e", 4.0}, {"f", uint8_t(5)}};

  auto kv_iterable_different_value =
      opentelemetry::common::MakeKeyValueIterableView<Attributes>(attributes_different_value);
  auto kv_iterable_different_type =
      opentelemetry::common::MakeKeyValueIterableView<Attributes>(attributes_different_type);
  auto kv_iterable_different_size =
      opentelemetry::common::MakeKeyValueIterableView<Attributes>(attributes_different_size);
  auto kv_iterable_different_all =
      opentelemetry::common::MakeKeyValueIterableView<Attributes>(attributes_different_all);

  EXPECT_FALSE(attribute_map.EqualTo(kv_iterable_different_value));
  EXPECT_FALSE(attribute_map.EqualTo(kv_iterable_different_type));
  EXPECT_FALSE(attribute_map.EqualTo(kv_iterable_different_size));
  EXPECT_FALSE(attribute_map.EqualTo(kv_iterable_different_all));
}
