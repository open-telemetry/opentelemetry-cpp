// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <string>

#include "opentelemetry/exporters/otlp/otlp_populate_attribute_utils.h"
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

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
