// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <array>
#include <cstdint>
#include <limits>
#include <string>
#include <vector>

#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/exporters/otlp/otlp_populate_attribute_utils.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/utility.h"
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

// ---------------------------------------------------------------------------
// Utf8SafePrefixLength
// ---------------------------------------------------------------------------
//
// "h\xC3\xA9llo" is "héllo" in UTF-8 (6 bytes). 'é' occupies bytes 1-2 as a
// 2-byte sequence (0xC3 0xA9). These tests pin the behavior at the boundary.

TEST(Utf8SafePrefixLength, ReturnsFullSizeWhenValueShorterThanMax)
{
  EXPECT_EQ(OtlpPopulateAttributeUtils::Utf8SafePrefixLength("abc", 10), 3u);
}

TEST(Utf8SafePrefixLength, ReturnsZeroForZeroBudget)
{
  EXPECT_EQ(OtlpPopulateAttributeUtils::Utf8SafePrefixLength("anything", 0), 0u);
}

TEST(Utf8SafePrefixLength, AsciiBoundaryEqualsRawByteCount)
{
  EXPECT_EQ(OtlpPopulateAttributeUtils::Utf8SafePrefixLength("0123456789", 5), 5u);
}

TEST(Utf8SafePrefixLength, DropsPartialMultiByteSequenceAtBoundary)
{
  // limit=2: 'h' fits at byte 0; the next codepoint 'é' would span bytes 1-2,
  // ending past the limit, so the algorithm stops at byte 1.
  EXPECT_EQ(OtlpPopulateAttributeUtils::Utf8SafePrefixLength("h\xC3\xA9llo", 2), 1u);
}

TEST(Utf8SafePrefixLength, KeepsCompletedMultiByteSequenceWithinBudget)
{
  // limit=3: 'h' (1) + 'é' (2) = 3 bytes exactly, still valid UTF-8.
  EXPECT_EQ(OtlpPopulateAttributeUtils::Utf8SafePrefixLength("h\xC3\xA9llo", 3), 3u);
}

TEST(Utf8SafePrefixLength, MalformedContinuationFallsBackToByteCount)
{
  // 0xC3 lead byte announces a 2-byte sequence, but 0x28 ('(') is not a valid
  // continuation byte (not in 0x80-0xBF). The lead is treated as a one-byte
  // unit, then '(' is its own one-byte unit, yielding a 2-byte prefix.
  EXPECT_EQ(OtlpPopulateAttributeUtils::Utf8SafePrefixLength("\xC3(abc", 2), 2u);
}

TEST(Utf8SafePrefixLength, InvalidLeadByteCountedAsOneByte)
{
  // 0xFF is not a valid UTF-8 lead byte (>= 0xF8). The algorithm treats it as
  // a one-byte unit, identical to raw byte truncation in this case.
  std::string value("\xFF\xFF\xFF\xFF\xFF", 5);
  EXPECT_EQ(OtlpPopulateAttributeUtils::Utf8SafePrefixLength(value, 3), 3u);
}

TEST(Utf8SafePrefixLength, TruncatedTailLeadByteFallsBack)
{
  // String ends mid-sequence (lone 0xC3 at the tail); the algorithm should
  // treat it as one byte rather than reading past the buffer.
  EXPECT_EQ(OtlpPopulateAttributeUtils::Utf8SafePrefixLength("ab\xC3", 10), 3u);
}

// ---------------------------------------------------------------------------
// OtlpPopulateAttributeUtils::PopulateAnyValue with common::AttributeValue
// ---------------------------------------------------------------------------

namespace
{
class PopulateAnyValueTest : public ::testing::Test
{
protected:
  opentelemetry::proto::common::v1::AnyValue proto_anyvalue_;
};
}  // namespace

TEST_F(PopulateAnyValueTest, AnyValueBool)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(&proto_anyvalue_, common::AttributeValue{true});
  EXPECT_TRUE(proto_anyvalue_.bool_value());
}

TEST_F(PopulateAnyValueTest, AnyValueInt32)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(&proto_anyvalue_,
                                               common::AttributeValue{int32_t{42}});
  EXPECT_EQ(proto_anyvalue_.int_value(), 42);
}

TEST_F(PopulateAnyValueTest, AnyValueInt64)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(&proto_anyvalue_,
                                               common::AttributeValue{int64_t{-1}});
  EXPECT_EQ(proto_anyvalue_.int_value(), -1);
}

TEST_F(PopulateAnyValueTest, AnyValueUint32)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(&proto_anyvalue_,
                                               common::AttributeValue{uint32_t{100}});
  EXPECT_EQ(proto_anyvalue_.int_value(), 100);
}

TEST_F(PopulateAnyValueTest, AnyValueDouble)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(&proto_anyvalue_, common::AttributeValue{3.14});
  EXPECT_DOUBLE_EQ(proto_anyvalue_.double_value(), 3.14);
}

TEST_F(PopulateAnyValueTest, AnyValueCString)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(&proto_anyvalue_, common::AttributeValue{"test123"});
  EXPECT_EQ(proto_anyvalue_.string_value(), "test123");
}

TEST_F(PopulateAnyValueTest, AnyValueStringView)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_anyvalue_, common::AttributeValue{nostd::string_view{"test123"}});
  EXPECT_EQ(proto_anyvalue_.string_value(), "test123");
}

#if defined(ENABLE_OTLP_UTF8_VALIDITY)
TEST_F(PopulateAnyValueTest, AnyValueCStringInvalidUtf8)
{
  // Bare continuation bytes are structurally invalid UTF-8.
  const std::array<char, 5> invalid_utf8 = {'\x80', '\x81', '\x82', '\x83', '\0'};
  OtlpPopulateAttributeUtils::PopulateAnyValue(&proto_anyvalue_,
                                               common::AttributeValue{invalid_utf8.data()});
  EXPECT_EQ(proto_anyvalue_.bytes_value(), std::string("\x80\x81\x82\x83", 4));
}

TEST_F(PopulateAnyValueTest, AnyValueStringViewInvalidUtf8)
{
  // Bare continuation bytes are structurally invalid UTF-8.
  const std::array<char, 4> invalid_utf8 = {'\x80', '\x81', '\x82', '\x83'};
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_anyvalue_,
      common::AttributeValue{nostd::string_view{invalid_utf8.data(), invalid_utf8.size()}});
  EXPECT_EQ(proto_anyvalue_.bytes_value(), std::string("\x80\x81\x82\x83", 4));
}
#endif  // defined(ENABLE_OTLP_UTF8_VALIDITY)

// uint64_t values within int64_t range map to int_value per the `Mapping Arbitrary Data to OTLP
// AnyValue` specification.
TEST_F(PopulateAnyValueTest, AnyValueUint64InRange)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_anyvalue_,
      common::AttributeValue{static_cast<uint64_t>(std::numeric_limits<int64_t>::max())});
  EXPECT_EQ(proto_anyvalue_.int_value(), std::numeric_limits<int64_t>::max());
}

// uint64_t values above INT64_MAX map to string_value per the `Mapping Arbitrary Data to OTLP
// AnyValue` specification.
TEST_F(PopulateAnyValueTest, AnyValueUint64OutOfRange)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_anyvalue_, common::AttributeValue{std::numeric_limits<uint64_t>::max()});
  EXPECT_EQ(proto_anyvalue_.string_value(), std::to_string(std::numeric_limits<uint64_t>::max()));
}

TEST_F(PopulateAnyValueTest, AnyValueSpanUint8)
{
  const std::array<uint8_t, 3> data = {0x01, 0x02, 0x03};
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_anyvalue_, common::AttributeValue{nostd::span<const uint8_t>{data}});
  EXPECT_EQ(proto_anyvalue_.bytes_value(), std::string("\x01\x02\x03", 3));
}

TEST_F(PopulateAnyValueTest, AnyValueSpanBool)
{
  const std::array<bool, 3> data = {true, false, true};
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_anyvalue_, common::AttributeValue{nostd::span<const bool>{data}});
  ASSERT_EQ(proto_anyvalue_.array_value().values_size(), 3);
  EXPECT_TRUE(proto_anyvalue_.array_value().values(0).bool_value());
  EXPECT_FALSE(proto_anyvalue_.array_value().values(1).bool_value());
  EXPECT_TRUE(proto_anyvalue_.array_value().values(2).bool_value());
}

TEST_F(PopulateAnyValueTest, AnyValueSpanInt32)
{
  const std::array<int32_t, 3> data = {10, -20, 30};
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_anyvalue_, common::AttributeValue{nostd::span<const int32_t>{data}});
  ASSERT_EQ(proto_anyvalue_.array_value().values_size(), 3);
  EXPECT_EQ(proto_anyvalue_.array_value().values(0).int_value(), 10);
  EXPECT_EQ(proto_anyvalue_.array_value().values(1).int_value(), -20);
  EXPECT_EQ(proto_anyvalue_.array_value().values(2).int_value(), 30);
}

TEST_F(PopulateAnyValueTest, AnyValueSpanInt64)
{
  const std::array<int64_t, 2> data = {-1000, 2000};
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_anyvalue_, common::AttributeValue{nostd::span<const int64_t>{data}});
  ASSERT_EQ(proto_anyvalue_.array_value().values_size(), 2);
  EXPECT_EQ(proto_anyvalue_.array_value().values(0).int_value(), -1000);
  EXPECT_EQ(proto_anyvalue_.array_value().values(1).int_value(), 2000);
}

TEST_F(PopulateAnyValueTest, AnyValueSpanUint32)
{
  const std::array<uint32_t, 2> data = {100, 200};
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_anyvalue_, common::AttributeValue{nostd::span<const uint32_t>{data}});
  ASSERT_EQ(proto_anyvalue_.array_value().values_size(), 2);
  EXPECT_EQ(proto_anyvalue_.array_value().values(0).int_value(), 100);
  EXPECT_EQ(proto_anyvalue_.array_value().values(1).int_value(), 200);
}

TEST_F(PopulateAnyValueTest, AnyValueSpanUint64)
{
  const std::array<uint64_t, 3> data = {100, 200, 300};
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_anyvalue_, common::AttributeValue{nostd::span<const uint64_t>{data}});
  ASSERT_EQ(proto_anyvalue_.array_value().values_size(), 3);
  EXPECT_EQ(proto_anyvalue_.array_value().values(0).int_value(), 100);
  EXPECT_EQ(proto_anyvalue_.array_value().values(1).int_value(), 200);
  EXPECT_EQ(proto_anyvalue_.array_value().values(2).int_value(), 300);
}

// uint64_t values above INT64_MAX in a span map to string_value per the `Mapping Arbitrary Data
// to OTLP AnyValue` specification.
TEST_F(PopulateAnyValueTest, AnyValueSpanUint64Overflow)
{
  const std::array<uint64_t, 2> data = {static_cast<uint64_t>(std::numeric_limits<int64_t>::max()),
                                        std::numeric_limits<uint64_t>::max()};
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_anyvalue_, common::AttributeValue{nostd::span<const uint64_t>{data}});
  ASSERT_EQ(proto_anyvalue_.array_value().values_size(), 2);
  EXPECT_EQ(proto_anyvalue_.array_value().values(0).int_value(),
            std::numeric_limits<int64_t>::max());
  EXPECT_EQ(proto_anyvalue_.array_value().values(1).string_value(),
            std::to_string(std::numeric_limits<uint64_t>::max()));
}

TEST_F(PopulateAnyValueTest, AnyValueSpanDouble)
{
  const std::array<double, 2> data = {1.5, 2.5};
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_anyvalue_, common::AttributeValue{nostd::span<const double>{data}});
  ASSERT_EQ(proto_anyvalue_.array_value().values_size(), 2);
  EXPECT_DOUBLE_EQ(proto_anyvalue_.array_value().values(0).double_value(), 1.5);
  EXPECT_DOUBLE_EQ(proto_anyvalue_.array_value().values(1).double_value(), 2.5);
}

TEST_F(PopulateAnyValueTest, AnyValueSpanStringView)
{
  const std::array<nostd::string_view, 2> data = {"foo", "bar"};
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_anyvalue_, common::AttributeValue{nostd::span<const nostd::string_view>{data}});
  ASSERT_EQ(proto_anyvalue_.array_value().values_size(), 2);
  EXPECT_EQ(proto_anyvalue_.array_value().values(0).string_value(), "foo");
  EXPECT_EQ(proto_anyvalue_.array_value().values(1).string_value(), "bar");
}

#if defined(ENABLE_OTLP_UTF8_VALIDITY)
TEST_F(PopulateAnyValueTest, AnyValueSpanStringViewInvalidUtf8)
{
  // Mix of valid and invalid UTF-8 entries.
  const std::array<char, 4> invalid_utf8       = {'\x80', '\x81', '\x82', '\x83'};
  const std::array<nostd::string_view, 2> data = {
      "valid", nostd::string_view{invalid_utf8.data(), invalid_utf8.size()}};
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_anyvalue_, common::AttributeValue{nostd::span<const nostd::string_view>{data}});
  ASSERT_EQ(proto_anyvalue_.array_value().values_size(), 2);
  EXPECT_EQ(proto_anyvalue_.array_value().values(0).string_value(), "valid");
  EXPECT_EQ(proto_anyvalue_.array_value().values(1).bytes_value(),
            std::string("\x80\x81\x82\x83", 4));
}

TEST_F(PopulateAnyValueTest, AnyValueSpanStringViewInvalidUtf8TruncatedAtMaxLength)
{
  // Invalid UTF-8 string that is also longer than max_length.
  // Must store the truncated string as a byte array.
  const std::array<char, 6> invalid_utf8       = {'\x80', '\x81', '\x82', '\x83', '\x84', '\x85'};
  const std::array<nostd::string_view, 1> data = {
      nostd::string_view{invalid_utf8.data(), invalid_utf8.size()}};
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_anyvalue_, common::AttributeValue{nostd::span<const nostd::string_view>{data}},
      AttributeConverterOptions{3});
  ASSERT_EQ(proto_anyvalue_.array_value().values_size(), 1);
  EXPECT_EQ(proto_anyvalue_.array_value().values(0).bytes_value(), std::string("\x80\x81\x82", 3));
}
#endif  // defined(ENABLE_OTLP_UTF8_VALIDITY)

// ---------------------------------------------------------------------------
// OtlpPopulateAttributeUtils::PopulateAnyValue with sdk::common::OwnedAttributeValue
// ---------------------------------------------------------------------------

TEST_F(PopulateAnyValueTest, OwnedAnyValueBool)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(&proto_anyvalue_,
                                               sdk::common::OwnedAttributeValue{false});
  EXPECT_FALSE(proto_anyvalue_.bool_value());
}

TEST_F(PopulateAnyValueTest, OwnedAnyValueInt32)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(&proto_anyvalue_,
                                               sdk::common::OwnedAttributeValue{int32_t{7}});
  EXPECT_EQ(proto_anyvalue_.int_value(), 7);
}

TEST_F(PopulateAnyValueTest, OwnedAnyValueUint32)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(&proto_anyvalue_,
                                               sdk::common::OwnedAttributeValue{uint32_t{200}});
  EXPECT_EQ(proto_anyvalue_.int_value(), 200);
}

TEST_F(PopulateAnyValueTest, OwnedAnyValueInt64)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(&proto_anyvalue_,
                                               sdk::common::OwnedAttributeValue{int64_t{-99}});
  EXPECT_EQ(proto_anyvalue_.int_value(), -99);
}

TEST_F(PopulateAnyValueTest, OwnedAnyValueDouble)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(&proto_anyvalue_,
                                               sdk::common::OwnedAttributeValue{2.71});
  EXPECT_DOUBLE_EQ(proto_anyvalue_.double_value(), 2.71);
}

TEST_F(PopulateAnyValueTest, OwnedAnyValueString)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_anyvalue_, sdk::common::OwnedAttributeValue{std::string{"owned"}});
  EXPECT_EQ(proto_anyvalue_.string_value(), "owned");
}

TEST_F(PopulateAnyValueTest, OwnedAnyValueVectorUint8)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_anyvalue_, sdk::common::OwnedAttributeValue{std::vector<uint8_t>{0xAA, 0xBB}});
  EXPECT_EQ(proto_anyvalue_.bytes_value(), std::string("\xAA\xBB", 2));
}

TEST_F(PopulateAnyValueTest, OwnedAnyValueVectorUint8TruncatedAtMaxLength)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_anyvalue_,
      sdk::common::OwnedAttributeValue{std::vector<uint8_t>{0x01, 0x02, 0x03, 0x04, 0x05}},
      AttributeConverterOptions{3});
  ASSERT_EQ(proto_anyvalue_.bytes_value().size(), 3u);
  EXPECT_EQ(static_cast<uint8_t>(proto_anyvalue_.bytes_value()[0]), 0x01u);
  EXPECT_EQ(static_cast<uint8_t>(proto_anyvalue_.bytes_value()[1]), 0x02u);
  EXPECT_EQ(static_cast<uint8_t>(proto_anyvalue_.bytes_value()[2]), 0x03u);
}

TEST_F(PopulateAnyValueTest, OwnedAnyValueVectorBool)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_anyvalue_, sdk::common::OwnedAttributeValue{std::vector<bool>{true, false, true}});
  ASSERT_EQ(proto_anyvalue_.array_value().values_size(), 3);
  EXPECT_TRUE(proto_anyvalue_.array_value().values(0).bool_value());
  EXPECT_FALSE(proto_anyvalue_.array_value().values(1).bool_value());
  EXPECT_TRUE(proto_anyvalue_.array_value().values(2).bool_value());
}

TEST_F(PopulateAnyValueTest, OwnedAnyValueVectorInt32)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_anyvalue_, sdk::common::OwnedAttributeValue{std::vector<int32_t>{10, -20, 30}});
  ASSERT_EQ(proto_anyvalue_.array_value().values_size(), 3);
  EXPECT_EQ(proto_anyvalue_.array_value().values(0).int_value(), 10);
  EXPECT_EQ(proto_anyvalue_.array_value().values(1).int_value(), -20);
  EXPECT_EQ(proto_anyvalue_.array_value().values(2).int_value(), 30);
}

TEST_F(PopulateAnyValueTest, OwnedAnyValueVectorUint32)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_anyvalue_, sdk::common::OwnedAttributeValue{std::vector<uint32_t>{100, 200}});
  ASSERT_EQ(proto_anyvalue_.array_value().values_size(), 2);
  EXPECT_EQ(proto_anyvalue_.array_value().values(0).int_value(), 100);
  EXPECT_EQ(proto_anyvalue_.array_value().values(1).int_value(), 200);
}

TEST_F(PopulateAnyValueTest, OwnedAnyValueVectorInt64)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_anyvalue_, sdk::common::OwnedAttributeValue{std::vector<int64_t>{-1000, 2000}});
  ASSERT_EQ(proto_anyvalue_.array_value().values_size(), 2);
  EXPECT_EQ(proto_anyvalue_.array_value().values(0).int_value(), -1000);
  EXPECT_EQ(proto_anyvalue_.array_value().values(1).int_value(), 2000);
}

TEST_F(PopulateAnyValueTest, OwnedAnyValueVectorDouble)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_anyvalue_, sdk::common::OwnedAttributeValue{std::vector<double>{1.1, 2.2, 3.3}});
  ASSERT_EQ(proto_anyvalue_.array_value().values_size(), 3);
  EXPECT_DOUBLE_EQ(proto_anyvalue_.array_value().values(0).double_value(), 1.1);
  EXPECT_DOUBLE_EQ(proto_anyvalue_.array_value().values(1).double_value(), 2.2);
  EXPECT_DOUBLE_EQ(proto_anyvalue_.array_value().values(2).double_value(), 3.3);
}

TEST_F(PopulateAnyValueTest, OwnedAnyValueVectorString)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_anyvalue_,
      sdk::common::OwnedAttributeValue{std::vector<std::string>{"one", "two", "three"}});
  ASSERT_EQ(proto_anyvalue_.array_value().values_size(), 3);
  EXPECT_EQ(proto_anyvalue_.array_value().values(0).string_value(), "one");
  EXPECT_EQ(proto_anyvalue_.array_value().values(1).string_value(), "two");
  EXPECT_EQ(proto_anyvalue_.array_value().values(2).string_value(), "three");
}

#if defined(ENABLE_OTLP_UTF8_VALIDITY)
TEST_F(PopulateAnyValueTest, OwnedAnyValueStringInvalidUtf8)
{
  // Bare continuation bytes are structurally invalid UTF-8.
  const std::string invalid_utf8("\x80\x81\x82\x83", 4);
  OtlpPopulateAttributeUtils::PopulateAnyValue(&proto_anyvalue_,
                                               sdk::common::OwnedAttributeValue{invalid_utf8});
  EXPECT_EQ(proto_anyvalue_.bytes_value(), std::string("\x80\x81\x82\x83", 4));
}

TEST_F(PopulateAnyValueTest, OwnedAnyValueStringInvalidUtf8TruncatedAtMaxLength)
{
  // Invalid UTF-8 string that is also longer than max_length
  const std::string invalid_utf8("\x80\x81\x82\x83\x84\x85", 6);
  OtlpPopulateAttributeUtils::PopulateAnyValue(&proto_anyvalue_,
                                               sdk::common::OwnedAttributeValue{invalid_utf8},
                                               AttributeConverterOptions{3});
  EXPECT_EQ(proto_anyvalue_.bytes_value(), std::string("\x80\x81\x82", 3));
}

TEST_F(PopulateAnyValueTest, OwnedAnyValueVectorStringInvalidUtf8)
{
  // Mix of valid and invalid UTF-8 entries.
  const std::string invalid_utf8("\x80\x81\x82\x83", 4);
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_anyvalue_,
      sdk::common::OwnedAttributeValue{std::vector<std::string>{"valid", invalid_utf8}});
  ASSERT_EQ(proto_anyvalue_.array_value().values_size(), 2);
  EXPECT_EQ(proto_anyvalue_.array_value().values(0).string_value(), "valid");
  EXPECT_EQ(proto_anyvalue_.array_value().values(1).bytes_value(),
            std::string("\x80\x81\x82\x83", 4));
}
#endif  // defined(ENABLE_OTLP_UTF8_VALIDITY)

// uint64_t values within int64_t range map to int_value per the `Mapping Arbitrary Data to OTLP
// AnyValue` specification.
TEST_F(PopulateAnyValueTest, OwnedAnyValueUint64InRange)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_anyvalue_,
      sdk::common::OwnedAttributeValue{static_cast<uint64_t>(std::numeric_limits<int64_t>::max())});
  EXPECT_EQ(proto_anyvalue_.int_value(), std::numeric_limits<int64_t>::max());
}

// uint64_t values above INT64_MAX map to string_value per the `Mapping Arbitrary Data to OTLP
// AnyValue` specification.
TEST_F(PopulateAnyValueTest, OwnedAnyValueUint64OutOfRange)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_anyvalue_, sdk::common::OwnedAttributeValue{std::numeric_limits<uint64_t>::max()});
  EXPECT_EQ(proto_anyvalue_.string_value(), std::to_string(std::numeric_limits<uint64_t>::max()));
}

TEST_F(PopulateAnyValueTest, OwnedAnyValueVectorUint64)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_anyvalue_, sdk::common::OwnedAttributeValue{std::vector<uint64_t>{100, 200, 300}});
  ASSERT_EQ(proto_anyvalue_.array_value().values_size(), 3);
  EXPECT_EQ(proto_anyvalue_.array_value().values(0).int_value(), 100);
  EXPECT_EQ(proto_anyvalue_.array_value().values(1).int_value(), 200);
  EXPECT_EQ(proto_anyvalue_.array_value().values(2).int_value(), 300);
}

// uint64_t values above INT64_MAX in a vector map to string_value per the `Mapping Arbitrary Data
// to OTLP AnyValue` specification.
TEST_F(PopulateAnyValueTest, OwnedAnyValueVectorUint64Overflow)
{
  const std::vector<uint64_t> data = {static_cast<uint64_t>(std::numeric_limits<int64_t>::max()),
                                      std::numeric_limits<uint64_t>::max()};
  OtlpPopulateAttributeUtils::PopulateAnyValue(&proto_anyvalue_,
                                               sdk::common::OwnedAttributeValue{data});
  ASSERT_EQ(proto_anyvalue_.array_value().values_size(), 2);
  EXPECT_EQ(proto_anyvalue_.array_value().values(0).int_value(),
            std::numeric_limits<int64_t>::max());
  EXPECT_EQ(proto_anyvalue_.array_value().values(1).string_value(),
            std::to_string(std::numeric_limits<uint64_t>::max()));
}

// ---------------------------------------------------------------------------
// OtlpPopulateAttributeUtils::PopulateAnyValue AttributeConverterOptions truncation
// ---------------------------------------------------------------------------

TEST_F(PopulateAnyValueTest, CStringTruncatedAtMaxLength)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_anyvalue_, common::AttributeValue{"hello world"}, AttributeConverterOptions{5});
  EXPECT_EQ(proto_anyvalue_.string_value(), "hello");
}

TEST_F(PopulateAnyValueTest, StringViewTruncatedAtMaxLength)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_anyvalue_, common::AttributeValue{nostd::string_view{"hello world"}},
      AttributeConverterOptions{5});
  EXPECT_EQ(proto_anyvalue_.string_value(), "hello");
}

TEST_F(PopulateAnyValueTest, StringViewNotTruncatedWhenWithinLimit)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_anyvalue_, common::AttributeValue{nostd::string_view{"hello world"}},
      AttributeConverterOptions{15});
  EXPECT_EQ(proto_anyvalue_.string_value(), "hello world");
}

TEST_F(PopulateAnyValueTest, OwnedStringTruncatedAtMaxLength)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_anyvalue_, sdk::common::OwnedAttributeValue{std::string{"hello world"}},
      AttributeConverterOptions{5});
  EXPECT_EQ(proto_anyvalue_.string_value(), "hello");
}

TEST_F(PopulateAnyValueTest, ByteSpanTruncatedAtMaxLength)
{
  const std::array<uint8_t, 5> data = {0x01, 0x02, 0x03, 0x04, 0x05};
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_anyvalue_, common::AttributeValue{nostd::span<const uint8_t>{data}},
      AttributeConverterOptions{3});
  EXPECT_EQ(proto_anyvalue_.bytes_value().size(), 3u);
  EXPECT_EQ(static_cast<uint8_t>(proto_anyvalue_.bytes_value()[0]), 0x01);
  EXPECT_EQ(static_cast<uint8_t>(proto_anyvalue_.bytes_value()[1]), 0x02);
  EXPECT_EQ(static_cast<uint8_t>(proto_anyvalue_.bytes_value()[2]), 0x03);
}

TEST_F(PopulateAnyValueTest, NonStringNotAffectedByMaxLength)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_anyvalue_, common::AttributeValue{int64_t{42}}, AttributeConverterOptions{1});
  EXPECT_EQ(proto_anyvalue_.int_value(), 42);
}

TEST_F(PopulateAnyValueTest, StringViewSpanElementsTruncatedAtMaxLength)
{
  const std::array<nostd::string_view, 2> data = {"hello", "world"};
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_anyvalue_, common::AttributeValue{nostd::span<const nostd::string_view>{data}},
      AttributeConverterOptions{3});
  ASSERT_EQ(proto_anyvalue_.array_value().values_size(), 2);
  EXPECT_EQ(proto_anyvalue_.array_value().values(0).string_value(), "hel");
  EXPECT_EQ(proto_anyvalue_.array_value().values(1).string_value(), "wor");
}

TEST_F(PopulateAnyValueTest, OwnedVectorOfStringsTruncatedAtMaxLength)
{
  OtlpPopulateAttributeUtils::PopulateAnyValue(
      &proto_anyvalue_,
      sdk::common::OwnedAttributeValue{std::vector<std::string>{"hello", "world"}},
      AttributeConverterOptions{3});
  ASSERT_EQ(proto_anyvalue_.array_value().values_size(), 2);
  EXPECT_EQ(proto_anyvalue_.array_value().values(0).string_value(), "hel");
  EXPECT_EQ(proto_anyvalue_.array_value().values(1).string_value(), "wor");
}

// -----------------------------------------------------------
// OtlpPopulateAttributeUtils::PopulateAttribute tests
// -----------------------------------------------------------

namespace
{
class PopulateAttributeTest : public ::testing::Test
{
protected:
  opentelemetry::proto::common::v1::KeyValue proto_keyvalue_;
};
}  // namespace

TEST_F(PopulateAttributeTest, PopulateAttributeKeyValue)
{
  OtlpPopulateAttributeUtils::PopulateAttribute(&proto_keyvalue_, nostd::string_view{"my-key"},
                                                common::AttributeValue{int64_t{123}});
  EXPECT_EQ(proto_keyvalue_.key(), "my-key");
  EXPECT_EQ(proto_keyvalue_.value().int_value(), 123);
}

TEST_F(PopulateAttributeTest, PopulateAttributeKeyValueOwned)
{
  OtlpPopulateAttributeUtils::PopulateAttribute(
      &proto_keyvalue_, nostd::string_view{"my-key"},
      sdk::common::OwnedAttributeValue{std::string{"my-value"}});
  EXPECT_EQ(proto_keyvalue_.key(), "my-key");
  EXPECT_EQ(proto_keyvalue_.value().string_value(), "my-value");
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
