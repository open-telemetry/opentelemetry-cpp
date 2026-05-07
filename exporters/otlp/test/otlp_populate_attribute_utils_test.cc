// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <cstdint>
#include <limits>
#include <string>
#include <vector>

#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/exporters/otlp/otlp_populate_attribute_utils.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/common/attribute_utils.h"
#include "opentelemetry/version.h"

// clang-format off
#include "opentelemetry/exporters/otlp/protobuf_include_prefix.h" // IWYU pragma: keep
#include "opentelemetry/proto/common/v1/common.pb.h"
#include "opentelemetry/exporters/otlp/protobuf_include_suffix.h" // IWYU pragma: keep
// clang-format on

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

class PopulateAnyValueTest : public ::testing::Test
{
protected:
  opentelemetry::proto::common::v1::AnyValue proto_;
};

class PopulateAttributeTest : public ::testing::Test
{
protected:
  opentelemetry::proto::common::v1::KeyValue kv_;
};

// ============================================================
// PopulateAnyValue with common::AttributeValue Tests
// ============================================================

TEST_F(PopulateAnyValueTest, AnyValueBool)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(&proto_, common::AttributeValue{true}, false);
  EXPECT_TRUE(proto_.bool_value());
}

TEST_F(PopulateAnyValueTest, AnyValueInt32)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(&proto_, common::AttributeValue{int32_t{42}}, false);
  EXPECT_EQ(proto_.int_value(), 42);
}

TEST_F(PopulateAnyValueTest, AnyValueInt64)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(&proto_, common::AttributeValue{int64_t{-1}}, false);
  EXPECT_EQ(proto_.int_value(), -1);
}

TEST_F(PopulateAnyValueTest, AnyValueUint32)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(&proto_, common::AttributeValue{uint32_t{100}},
                                               false);
  EXPECT_EQ(proto_.int_value(), 100);
}

TEST_F(PopulateAnyValueTest, AnyValueDouble)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(&proto_, common::AttributeValue{3.14}, false);
  EXPECT_DOUBLE_EQ(proto_.double_value(), 3.14);
}

TEST_F(PopulateAnyValueTest, AnyValueCString)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(&proto_, common::AttributeValue{"test123"}, false);
  EXPECT_EQ(proto_.string_value(), "test123");
}

TEST_F(PopulateAnyValueTest, AnyValueStringView)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_, common::AttributeValue{nostd::string_view{"test123"}}, false);
  EXPECT_EQ(proto_.string_value(), "test123");
}

#if defined(ENABLE_OTLP_UTF8_VALIDITY)
TEST_F(PopulateAnyValueTest, AnyValueCStringInvalidUtf8)
{
  // Bare continuation bytes are structurally invalid UTF-8.
  const char invalid_utf8[] = {'\x80', '\x81', '\x82', '\x83', '\0'};
  OtlpPopulateAttributeUtils::PopulateAnyValue(&proto_, common::AttributeValue{invalid_utf8},
                                               false);
  EXPECT_EQ(proto_.bytes_value(), std::string("\x80\x81\x82\x83", 4));
}

TEST_F(PopulateAnyValueTest, AnyValueStringViewInvalidUtf8)
{
  // Bare continuation bytes are structurally invalid UTF-8.
  const char invalid_utf8[] = {'\x80', '\x81', '\x82', '\x83'};
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_, common::AttributeValue{nostd::string_view{invalid_utf8, 4}}, false);
  EXPECT_EQ(proto_.bytes_value(), std::string("\x80\x81\x82\x83", 4));
}
#endif  // defined(ENABLE_OTLP_UTF8_VALIDITY)

// uint64_t values within int64_t range map to int_value per the `Mapping Arbitrary Data to OTLP
// AnyValue` specification.
TEST_F(PopulateAnyValueTest, AnyValueUint64InRange)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_, common::AttributeValue{static_cast<uint64_t>(std::numeric_limits<int64_t>::max())},
      false);
  EXPECT_EQ(proto_.int_value(), std::numeric_limits<int64_t>::max());
}

// uint64_t values above INT64_MAX map to string_value per the `Mapping Arbitrary Data to OTLP
// AnyValue` specification.
TEST_F(PopulateAnyValueTest, AnyValueUint64OutOfRange)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_, common::AttributeValue{std::numeric_limits<uint64_t>::max()}, false);
  EXPECT_EQ(proto_.string_value(), std::to_string(std::numeric_limits<uint64_t>::max()));
}

TEST_F(PopulateAnyValueTest, AnyValueSpanUint8AllowBytes)
{
  const uint8_t data[] = {0x01, 0x02, 0x03};
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_, common::AttributeValue{nostd::span<const uint8_t>{data}}, true);
  EXPECT_EQ(proto_.bytes_value(), std::string("\x01\x02\x03", 3));
}

// TODO: This case is not mentioned by the spec. Consider whether to remove disallowing bytes for
// span<uint8_t>.
TEST_F(PopulateAnyValueTest, AnyValueSpanUint8DisallowBytes)
{
  const uint8_t data[] = {0x01, 0x02, 0x03};
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_, common::AttributeValue{nostd::span<const uint8_t>{data}}, false);
  ASSERT_EQ(proto_.array_value().values_size(), 3);
  EXPECT_EQ(proto_.array_value().values(0).int_value(), 1);
  EXPECT_EQ(proto_.array_value().values(1).int_value(), 2);
  EXPECT_EQ(proto_.array_value().values(2).int_value(), 3);
}

TEST_F(PopulateAnyValueTest, AnyValueSpanBool)
{
  const bool data[] = {true, false, true};
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_, common::AttributeValue{nostd::span<const bool>{data}}, false);
  ASSERT_EQ(proto_.array_value().values_size(), 3);
  EXPECT_TRUE(proto_.array_value().values(0).bool_value());
  EXPECT_FALSE(proto_.array_value().values(1).bool_value());
  EXPECT_TRUE(proto_.array_value().values(2).bool_value());
}

TEST_F(PopulateAnyValueTest, AnyValueSpanInt32)
{
  const int32_t data[] = {10, -20, 30};
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_, common::AttributeValue{nostd::span<const int32_t>{data}}, false);
  ASSERT_EQ(proto_.array_value().values_size(), 3);
  EXPECT_EQ(proto_.array_value().values(0).int_value(), 10);
  EXPECT_EQ(proto_.array_value().values(1).int_value(), -20);
  EXPECT_EQ(proto_.array_value().values(2).int_value(), 30);
}

TEST_F(PopulateAnyValueTest, AnyValueSpanInt64)
{
  const int64_t data[] = {-1000, 2000};
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_, common::AttributeValue{nostd::span<const int64_t>{data}}, false);
  ASSERT_EQ(proto_.array_value().values_size(), 2);
  EXPECT_EQ(proto_.array_value().values(0).int_value(), -1000);
  EXPECT_EQ(proto_.array_value().values(1).int_value(), 2000);
}

TEST_F(PopulateAnyValueTest, AnyValueSpanUint32)
{
  const uint32_t data[] = {100, 200};
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_, common::AttributeValue{nostd::span<const uint32_t>{data}}, false);
  ASSERT_EQ(proto_.array_value().values_size(), 2);
  EXPECT_EQ(proto_.array_value().values(0).int_value(), 100);
  EXPECT_EQ(proto_.array_value().values(1).int_value(), 200);
}

TEST_F(PopulateAnyValueTest, AnyValueSpanUint64)
{
  const uint64_t data[] = {100, 200, 300};
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_, common::AttributeValue{nostd::span<const uint64_t>{data}}, false);
  ASSERT_EQ(proto_.array_value().values_size(), 3);
  EXPECT_EQ(proto_.array_value().values(0).int_value(), 100);
  EXPECT_EQ(proto_.array_value().values(1).int_value(), 200);
  EXPECT_EQ(proto_.array_value().values(2).int_value(), 300);
}

// uint64_t values above INT64_MAX in a span map to string_value per the `Mapping Arbitrary Data
// to OTLP AnyValue` specification.
TEST_F(PopulateAnyValueTest, AnyValueSpanUint64Overflow)
{
  const uint64_t data[] = {static_cast<uint64_t>(std::numeric_limits<int64_t>::max()),
                           std::numeric_limits<uint64_t>::max()};
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_, common::AttributeValue{nostd::span<const uint64_t>{data}}, false);
  ASSERT_EQ(proto_.array_value().values_size(), 2);
  EXPECT_EQ(proto_.array_value().values(0).int_value(), std::numeric_limits<int64_t>::max());
  EXPECT_EQ(proto_.array_value().values(1).string_value(),
            std::to_string(std::numeric_limits<uint64_t>::max()));
}

TEST_F(PopulateAnyValueTest, AnyValueSpanDouble)
{
  const double data[] = {1.5, 2.5};
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_, common::AttributeValue{nostd::span<const double>{data}}, false);
  ASSERT_EQ(proto_.array_value().values_size(), 2);
  EXPECT_DOUBLE_EQ(proto_.array_value().values(0).double_value(), 1.5);
  EXPECT_DOUBLE_EQ(proto_.array_value().values(1).double_value(), 2.5);
}

TEST_F(PopulateAnyValueTest, AnyValueSpanStringView)
{
  const nostd::string_view data[] = {"foo", "bar"};
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_, common::AttributeValue{nostd::span<const nostd::string_view>{data}}, false);
  ASSERT_EQ(proto_.array_value().values_size(), 2);
  EXPECT_EQ(proto_.array_value().values(0).string_value(), "foo");
  EXPECT_EQ(proto_.array_value().values(1).string_value(), "bar");
}

#if defined(ENABLE_OTLP_UTF8_VALIDITY)
TEST_F(PopulateAnyValueTest, AnyValueSpanStringViewInvalidUtf8)
{
  // Mix of valid and invalid UTF-8 entries.
  const char invalid_utf8[]       = {'\x80', '\x81', '\x82', '\x83'};
  const nostd::string_view data[] = {"valid", nostd::string_view{invalid_utf8, 4}};
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_, common::AttributeValue{nostd::span<const nostd::string_view>{data}}, false);
  ASSERT_EQ(proto_.array_value().values_size(), 2);
  EXPECT_EQ(proto_.array_value().values(0).string_value(), "valid");
  EXPECT_EQ(proto_.array_value().values(1).bytes_value(), std::string("\x80\x81\x82\x83", 4));
}
#endif  // defined(ENABLE_OTLP_UTF8_VALIDITY)

// ============================================================
// PopulateAnyValue with sdk::common::OwnedAttributeValue Tests
// ============================================================

TEST_F(PopulateAnyValueTest, OwnedAnyValueBool)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(&proto_, sdk::common::OwnedAttributeValue{false},
                                               false);
  EXPECT_FALSE(proto_.bool_value());
}

TEST_F(PopulateAnyValueTest, OwnedAnyValueInt32)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(&proto_,
                                               sdk::common::OwnedAttributeValue{int32_t{7}}, false);
  EXPECT_EQ(proto_.int_value(), 7);
}

TEST_F(PopulateAnyValueTest, OwnedAnyValueUint32)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_, sdk::common::OwnedAttributeValue{uint32_t{200}}, false);
  EXPECT_EQ(proto_.int_value(), 200);
}

TEST_F(PopulateAnyValueTest, OwnedAnyValueInt64)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_, sdk::common::OwnedAttributeValue{int64_t{-99}}, false);
  EXPECT_EQ(proto_.int_value(), -99);
}

TEST_F(PopulateAnyValueTest, OwnedAnyValueDouble)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(&proto_, sdk::common::OwnedAttributeValue{2.71},
                                               false);
  EXPECT_DOUBLE_EQ(proto_.double_value(), 2.71);
}

TEST_F(PopulateAnyValueTest, OwnedAnyValueString)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_, sdk::common::OwnedAttributeValue{std::string{"owned"}}, false);
  EXPECT_EQ(proto_.string_value(), "owned");
}

TEST_F(PopulateAnyValueTest, OwnedAnyValueVectorUint8AllowBytes)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_, sdk::common::OwnedAttributeValue{std::vector<uint8_t>{0xAA, 0xBB}}, true);
  EXPECT_EQ(proto_.bytes_value(), std::string("\xAA\xBB", 2));
}

// TODO: This case is not mentioned by the spec. Consider whether to remove disallowing bytes for
// vector<uint8_t>.
TEST_F(PopulateAnyValueTest, OwnedAnyValueVectorUint8DisallowBytes)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_, sdk::common::OwnedAttributeValue{std::vector<uint8_t>{0xAA, 0xBB}}, false);
  ASSERT_EQ(proto_.array_value().values_size(), 2);
  EXPECT_EQ(proto_.array_value().values(0).int_value(), 0xAA);
  EXPECT_EQ(proto_.array_value().values(1).int_value(), 0xBB);
}

TEST_F(PopulateAnyValueTest, OwnedAnyValueVectorBool)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_, sdk::common::OwnedAttributeValue{std::vector<bool>{true, false, true}}, false);
  ASSERT_EQ(proto_.array_value().values_size(), 3);
  EXPECT_TRUE(proto_.array_value().values(0).bool_value());
  EXPECT_FALSE(proto_.array_value().values(1).bool_value());
  EXPECT_TRUE(proto_.array_value().values(2).bool_value());
}

TEST_F(PopulateAnyValueTest, OwnedAnyValueVectorInt32)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_, sdk::common::OwnedAttributeValue{std::vector<int32_t>{10, -20, 30}}, false);
  ASSERT_EQ(proto_.array_value().values_size(), 3);
  EXPECT_EQ(proto_.array_value().values(0).int_value(), 10);
  EXPECT_EQ(proto_.array_value().values(1).int_value(), -20);
  EXPECT_EQ(proto_.array_value().values(2).int_value(), 30);
}

TEST_F(PopulateAnyValueTest, OwnedAnyValueVectorUint32)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_, sdk::common::OwnedAttributeValue{std::vector<uint32_t>{100, 200}}, false);
  ASSERT_EQ(proto_.array_value().values_size(), 2);
  EXPECT_EQ(proto_.array_value().values(0).int_value(), 100);
  EXPECT_EQ(proto_.array_value().values(1).int_value(), 200);
}

TEST_F(PopulateAnyValueTest, OwnedAnyValueVectorInt64)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_, sdk::common::OwnedAttributeValue{std::vector<int64_t>{-1000, 2000}}, false);
  ASSERT_EQ(proto_.array_value().values_size(), 2);
  EXPECT_EQ(proto_.array_value().values(0).int_value(), -1000);
  EXPECT_EQ(proto_.array_value().values(1).int_value(), 2000);
}

TEST_F(PopulateAnyValueTest, OwnedAnyValueVectorDouble)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_, sdk::common::OwnedAttributeValue{std::vector<double>{1.1, 2.2, 3.3}}, false);
  ASSERT_EQ(proto_.array_value().values_size(), 3);
  EXPECT_DOUBLE_EQ(proto_.array_value().values(0).double_value(), 1.1);
  EXPECT_DOUBLE_EQ(proto_.array_value().values(1).double_value(), 2.2);
  EXPECT_DOUBLE_EQ(proto_.array_value().values(2).double_value(), 3.3);
}

TEST_F(PopulateAnyValueTest, OwnedAnyValueVectorString)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_, sdk::common::OwnedAttributeValue{std::vector<std::string>{"one", "two", "three"}},
      false);
  ASSERT_EQ(proto_.array_value().values_size(), 3);
  EXPECT_EQ(proto_.array_value().values(0).string_value(), "one");
  EXPECT_EQ(proto_.array_value().values(1).string_value(), "two");
  EXPECT_EQ(proto_.array_value().values(2).string_value(), "three");
}

#if defined(ENABLE_OTLP_UTF8_VALIDITY)
TEST_F(PopulateAnyValueTest, OwnedAnyValueVectorStringInvalidUtf8)
{
  // Mix of valid and invalid UTF-8 entries.
  const std::string invalid_utf8("\x80\x81\x82\x83", 4);
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_, sdk::common::OwnedAttributeValue{std::vector<std::string>{"valid", invalid_utf8}},
      false);
  ASSERT_EQ(proto_.array_value().values_size(), 2);
  EXPECT_EQ(proto_.array_value().values(0).string_value(), "valid");
  EXPECT_EQ(proto_.array_value().values(1).bytes_value(), std::string("\x80\x81\x82\x83", 4));
}
#endif  // defined(ENABLE_OTLP_UTF8_VALIDITY)

// uint64_t values within int64_t range map to int_value per the `Mapping Arbitrary Data to OTLP
// AnyValue` specification.
TEST_F(PopulateAnyValueTest, OwnedAnyValueUint64InRange)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_,
      sdk::common::OwnedAttributeValue{static_cast<uint64_t>(std::numeric_limits<int64_t>::max())},
      false);
  EXPECT_EQ(proto_.int_value(), std::numeric_limits<int64_t>::max());
}

// uint64_t values above INT64_MAX map to string_value per the `Mapping Arbitrary Data to OTLP
// AnyValue` specification.
TEST_F(PopulateAnyValueTest, OwnedAnyValueUint64OutOfRange)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_, sdk::common::OwnedAttributeValue{std::numeric_limits<uint64_t>::max()}, false);
  EXPECT_EQ(proto_.string_value(), std::to_string(std::numeric_limits<uint64_t>::max()));
}

TEST_F(PopulateAnyValueTest, OwnedAnyValueVectorUint64)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_, sdk::common::OwnedAttributeValue{std::vector<uint64_t>{100, 200, 300}}, false);
  ASSERT_EQ(proto_.array_value().values_size(), 3);
  EXPECT_EQ(proto_.array_value().values(0).int_value(), 100);
  EXPECT_EQ(proto_.array_value().values(1).int_value(), 200);
  EXPECT_EQ(proto_.array_value().values(2).int_value(), 300);
}

// ============================================================
// PopulateAttribute Tests
// ============================================================

TEST_F(PopulateAttributeTest, PopulateAttributeKeyValue)
{
  OtlpPopulateAttributeUtils::PopulateAttribute(&kv_, nostd::string_view{"my-key"},
                                                common::AttributeValue{int64_t{123}}, false);
  EXPECT_EQ(kv_.key(), "my-key");
  EXPECT_EQ(kv_.value().int_value(), 123);
}

TEST_F(PopulateAttributeTest, PopulateAttributeKeyValueOwned)
{
  OtlpPopulateAttributeUtils::PopulateAttribute(
      &kv_, nostd::string_view{"my-key"}, sdk::common::OwnedAttributeValue{std::string{"my-value"}},
      false);
  EXPECT_EQ(kv_.key(), "my-key");
  EXPECT_EQ(kv_.value().string_value(), "my-value");
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
