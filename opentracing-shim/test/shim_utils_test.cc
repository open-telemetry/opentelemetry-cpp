/*
 * Copyright The OpenTelemetry Authors
 * SPDX-License-Identifier: Apache-2.0
 */

#include "shim_utils.h"
#include "shim_mocks.h"

#include <gtest/gtest.h>

namespace baggage = opentelemetry::baggage;
namespace common  = opentelemetry::common;
namespace nostd   = opentelemetry::nostd;
namespace shim    = opentelemetry::opentracingshim;

class ShimUtilsTest : public testing::Test
{
protected:
  virtual void SetUp()
  {
  }

  virtual void TearDown()
  {
  }
};

TEST_F(ShimUtilsTest, IsBaggageEmpty)
{
  auto none = nostd::shared_ptr<baggage::Baggage>(nullptr);
  ASSERT_TRUE(shim::utils::isBaggageEmpty(none));

  auto empty = nostd::shared_ptr<baggage::Baggage>(new baggage::Baggage({}));
  ASSERT_TRUE(shim::utils::isBaggageEmpty(empty));

  std::map<std::string, std::string> list{{ "foo", "bar" }};
  auto non_empty = nostd::shared_ptr<baggage::Baggage>(new baggage::Baggage(list));
  ASSERT_FALSE(shim::utils::isBaggageEmpty(non_empty));
}

TEST_F(ShimUtilsTest, StringFromValue)
{
  ASSERT_EQ(shim::utils::stringFromValue(true), "true");
  ASSERT_EQ(shim::utils::stringFromValue(false), "false");
  ASSERT_EQ(shim::utils::stringFromValue(1234.567890), "1234.567890");
  ASSERT_EQ(shim::utils::stringFromValue(42l), "42");
  ASSERT_EQ(shim::utils::stringFromValue(55ul), "55");
  ASSERT_EQ(shim::utils::stringFromValue(std::string{"a string"}), "a string");
  ASSERT_EQ(shim::utils::stringFromValue(opentracing::string_view{"a string view"}), "a string view");
  ASSERT_EQ(shim::utils::stringFromValue(nullptr), "");
  ASSERT_EQ(shim::utils::stringFromValue("a char ptr"), "a char ptr");

  opentracing::util::recursive_wrapper<opentracing::Values> values{};
  ASSERT_EQ(shim::utils::stringFromValue(values.get()), "");

  opentracing::util::recursive_wrapper<opentracing::Dictionary> dict{};
  ASSERT_EQ(shim::utils::stringFromValue(dict.get()), "");
}

TEST_F(ShimUtilsTest, AttributeFromValue)
{
  auto value = shim::utils::attributeFromValue(true);
  ASSERT_EQ(value.index(), common::AttributeType::kTypeBool);
  ASSERT_TRUE(nostd::get<bool>(value));

  value = shim::utils::attributeFromValue(false);
  ASSERT_EQ(value.index(), common::AttributeType::kTypeBool);
  ASSERT_FALSE(nostd::get<bool>(value));

  value = shim::utils::attributeFromValue(1234.567890);
  ASSERT_EQ(value.index(), common::AttributeType::kTypeDouble);
  ASSERT_EQ(nostd::get<double>(value), 1234.567890);

  value = shim::utils::attributeFromValue(42l);
  ASSERT_EQ(value.index(), common::AttributeType::kTypeInt64);
  ASSERT_EQ(nostd::get<int64_t>(value), 42l);

  value = shim::utils::attributeFromValue(55ul);
  ASSERT_EQ(value.index(), common::AttributeType::kTypeUInt64);
  ASSERT_EQ(nostd::get<uint64_t>(value), 55ul);

  value = shim::utils::attributeFromValue(std::string{"a string"});
  ASSERT_EQ(value.index(), common::AttributeType::kTypeCString);
  ASSERT_STREQ(nostd::get<const char *>(value), "a string");

  value = shim::utils::attributeFromValue(opentracing::string_view{"a string view"});
  ASSERT_EQ(value.index(), common::AttributeType::kTypeString);
  ASSERT_EQ(nostd::get<nostd::string_view>(value), nostd::string_view{"a string view"});

  value = shim::utils::attributeFromValue(nullptr);
  ASSERT_EQ(value.index(), common::AttributeType::kTypeString);
  ASSERT_EQ(nostd::get<nostd::string_view>(value), nostd::string_view{});

  value = shim::utils::attributeFromValue("a char ptr");
  ASSERT_EQ(value.index(), common::AttributeType::kTypeCString);
  ASSERT_STREQ(nostd::get<const char *>(value), "a char ptr");

  opentracing::util::recursive_wrapper<opentracing::Values> values{};
  value = shim::utils::attributeFromValue(values.get());
  ASSERT_EQ(value.index(), common::AttributeType::kTypeString);
  ASSERT_EQ(nostd::get<nostd::string_view>(value), nostd::string_view{});

  opentracing::util::recursive_wrapper<opentracing::Dictionary> dict{};
  value = shim::utils::attributeFromValue(dict.get());
  ASSERT_EQ(value.index(), common::AttributeType::kTypeString);
  ASSERT_EQ(nostd::get<nostd::string_view>(value), nostd::string_view{});
}

TEST_F(ShimUtilsTest, TextMapReader_Get_LookupKey_Unsupported)
{
  std::unordered_map<std::string, std::string> text_map;
  TextMapCarrier testee{text_map};
  ASSERT_FALSE(testee.supports_lookup);
  ASSERT_EQ(testee.foreach_key_call_count, 0);

  shim::utils::CarrierReaderShim<TextMapCarrier> tester{testee};
  auto lookup_unsupported = testee.LookupKey("foo");
  ASSERT_TRUE(text_map.empty());
  ASSERT_TRUE(opentracing::are_errors_equal(lookup_unsupported.error(), opentracing::lookup_key_not_supported_error));
  ASSERT_EQ(tester.Get("foo"), nostd::string_view{});
  ASSERT_EQ(testee.foreach_key_call_count, 1);

  text_map["foo"] = "bar";
  auto lookup_found = testee.LookupKey("foo");
  ASSERT_FALSE(text_map.empty());
  ASSERT_TRUE(opentracing::are_errors_equal(lookup_unsupported.error(), opentracing::lookup_key_not_supported_error));
  ASSERT_EQ(tester.Get("foo"), nostd::string_view{"bar"});
  ASSERT_EQ(testee.foreach_key_call_count, 2);
}

TEST_F(ShimUtilsTest, TextMapReader_Get_LookupKey_Supported)
{
  std::unordered_map<std::string, std::string> text_map;
  TextMapCarrier testee{text_map};
  testee.supports_lookup = true;
  ASSERT_TRUE(testee.supports_lookup);
  ASSERT_EQ(testee.foreach_key_call_count, 0);

  shim::utils::CarrierReaderShim<TextMapCarrier> tester{testee};
  auto lookup_not_found = testee.LookupKey("foo");
  ASSERT_TRUE(text_map.empty());
  ASSERT_TRUE(opentracing::are_errors_equal(lookup_not_found.error(), opentracing::key_not_found_error));
  ASSERT_EQ(tester.Get("foo"), nostd::string_view{});
  ASSERT_EQ(testee.foreach_key_call_count, 1);

  text_map["foo"] = "bar";
  auto lookup_found = testee.LookupKey("foo");
  ASSERT_FALSE(text_map.empty());
  ASSERT_EQ(lookup_found.value(), opentracing::string_view{"bar"});
  ASSERT_EQ(tester.Get("foo"), nostd::string_view{"bar"});
  ASSERT_EQ(testee.foreach_key_call_count, 1);
}

TEST_F(ShimUtilsTest, TextMapReader_Keys)
{
  std::unordered_map<std::string, std::string> text_map;
  TextMapCarrier testee{text_map};
  ASSERT_EQ(testee.foreach_key_call_count, 0);

  shim::utils::CarrierReaderShim<TextMapCarrier> tester{testee};
  std::vector<nostd::string_view> kvs;
  auto callback = [&text_map,&kvs](nostd::string_view k){
    kvs.emplace_back(k);
    return !text_map.empty();
  };

  ASSERT_TRUE(tester.Keys(callback));
  ASSERT_TRUE(text_map.empty());
  ASSERT_TRUE(kvs.empty());
  ASSERT_EQ(testee.foreach_key_call_count, 1);

  text_map["foo"] = "bar";
  text_map["bar"] = "baz";
  text_map["baz"] = "foo";
  ASSERT_TRUE(tester.Keys(callback));
  ASSERT_FALSE(text_map.empty());
  ASSERT_EQ(text_map.size(), kvs.size());
  ASSERT_EQ(testee.foreach_key_call_count, 2);
}

TEST_F(ShimUtilsTest, TextMapWriter_Set)
{
  std::unordered_map<std::string, std::string> text_map;
  TextMapCarrier testee{text_map};
  shim::utils::CarrierWriterShim<TextMapCarrier> tester{testee};
  ASSERT_TRUE(text_map.empty());

  tester.Set("foo", "bar");
  tester.Set("bar", "baz");
  tester.Set("baz", "foo");
  ASSERT_FALSE(text_map.empty());
  ASSERT_EQ(text_map.size(), 3);
  ASSERT_EQ(text_map["foo"], "bar");
  ASSERT_EQ(text_map["bar"], "baz");
  ASSERT_EQ(text_map["baz"], "foo");
}