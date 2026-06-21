// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <cstdint>
#include <string>

#include "opentelemetry/exporters/otlp/otlp_populate_attribute_utils.h"
#include "opentelemetry/proto/common/v1/common.pb.h"
#include "opentelemetry/version.h"

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
// TruncateProtoAttributeValue
// ---------------------------------------------------------------------------

TEST(TruncateProtoAttributeValue, TruncatesStringValueAtUtf8Boundary)
{
  proto::common::v1::AnyValue value;
  value.set_string_value("h\xC3\xA9llo");
  OtlpPopulateAttributeUtils::TruncateProtoAttributeValue(&value, 2);
  EXPECT_EQ(value.string_value(), "h");
}

TEST(TruncateProtoAttributeValue, KeepsStringValueWhenWithinBudget)
{
  proto::common::v1::AnyValue value;
  value.set_string_value("short");
  OtlpPopulateAttributeUtils::TruncateProtoAttributeValue(&value, 100);
  EXPECT_EQ(value.string_value(), "short");
}

TEST(TruncateProtoAttributeValue, TruncatesBytesValueAtRawByteBoundary)
{
  proto::common::v1::AnyValue value;
  value.set_bytes_value(std::string("\x01\x02\x03\x04\x05", 5));
  OtlpPopulateAttributeUtils::TruncateProtoAttributeValue(&value, 3);
  const auto &b = value.bytes_value();
  ASSERT_EQ(b.size(), 3u);
  EXPECT_EQ(static_cast<uint8_t>(b[0]), 0x01);
  EXPECT_EQ(static_cast<uint8_t>(b[1]), 0x02);
  EXPECT_EQ(static_cast<uint8_t>(b[2]), 0x03);
}

TEST(TruncateProtoAttributeValue, TruncatesEachStringInArray)
{
  proto::common::v1::AnyValue value;
  auto *array = value.mutable_array_value();
  array->add_values()->set_string_value("aaaaaa");
  array->add_values()->set_string_value("bb");
  array->add_values()->set_string_value("cccc");

  OtlpPopulateAttributeUtils::TruncateProtoAttributeValue(&value, 3);

  ASSERT_EQ(value.array_value().values_size(), 3);
  EXPECT_EQ(value.array_value().values(0).string_value(), "aaa");
  EXPECT_EQ(value.array_value().values(1).string_value(), "bb");
  EXPECT_EQ(value.array_value().values(2).string_value(), "ccc");
}

TEST(TruncateProtoAttributeValue, LeavesNonStringTypesAlone)
{
  proto::common::v1::AnyValue int_value;
  int_value.set_int_value(1234567890);
  OtlpPopulateAttributeUtils::TruncateProtoAttributeValue(&int_value, 1);
  EXPECT_EQ(int_value.int_value(), 1234567890);

  proto::common::v1::AnyValue bool_value;
  bool_value.set_bool_value(true);
  OtlpPopulateAttributeUtils::TruncateProtoAttributeValue(&bool_value, 1);
  EXPECT_EQ(bool_value.bool_value(), true);
}

TEST(TruncateProtoAttributeValue, IgnoresNullValuePointer)
{
  OtlpPopulateAttributeUtils::TruncateProtoAttributeValue(nullptr, 5);
  // No crash, no observable side effect.
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
