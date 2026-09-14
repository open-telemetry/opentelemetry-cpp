// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/otlp/otlp_json_converter.h"

#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>
#include <ostream>
#include <string>
#include <vector>

#include "opentelemetry/exporters/otlp/otlp_http.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/version.h"
#include "otlp_stub_json_writer.h"

// clang-format off
#include "opentelemetry/exporters/otlp/protobuf_include_prefix.h" // IWYU pragma: keep
#include "opentelemetry/proto/common/v1/common.pb.h"
#include "opentelemetry/proto/metrics/v1/metrics.pb.h"
#include "opentelemetry/proto/trace/v1/trace.pb.h"
#include "opentelemetry/exporters/otlp/protobuf_include_suffix.h" // IWYU pragma: keep
// clang-format on

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{
namespace
{

std::vector<std::string> ConvertStrings(const google::protobuf::Message &message,
                                        JsonBytesMappingKind bytes_mapping)
{
  std::vector<std::string> strings;
  test::StubJsonWriter writer;
  writer.on_write_string = [&strings](nostd::string_view value) {
    strings.emplace_back(value.data(), value.size());
  };
  ConvertGenericMessageToJson(writer, message, JsonConverterOptions{false, bytes_mapping});
  return strings;
}

class OtlpJsonConverterSigned64BitInteger : public ::testing::TestWithParam<std::int64_t>
{};

TEST_P(OtlpJsonConverterSigned64BitInteger, WritesAnExactDecimalString)
{
  proto::common::v1::AnyValue any_value;
  any_value.set_int_value(GetParam());
  EXPECT_EQ(ConvertStrings(any_value, JsonBytesMappingKind::kHexId),
            std::vector<std::string>{std::to_string(GetParam())});
}

INSTANTIATE_TEST_SUITE_P(Values,
                         OtlpJsonConverterSigned64BitInteger,
                         ::testing::Values(std::numeric_limits<std::int64_t>::min(),
                                           std::int64_t{-1},
                                           std::int64_t{0},
                                           std::int64_t{10},
                                           std::numeric_limits<std::int64_t>::max()));

class OtlpJsonConverterUnsigned64BitInteger : public ::testing::TestWithParam<std::uint64_t>
{};

// count is a singular field and bucket_counts a repeated one, so both code paths are checked.
TEST_P(OtlpJsonConverterUnsigned64BitInteger, WritesAnExactDecimalString)
{
  proto::metrics::v1::HistogramDataPoint point;
  point.set_count(GetParam());
  point.add_bucket_counts(GetParam());

  const std::string decimal = std::to_string(GetParam());
  std::vector<std::string> expected;
  // proto3 does not serialize a singular field left at its default, so a zero count is omitted.
  if (GetParam() != 0)
  {
    expected.push_back(decimal);
  }
  expected.push_back(decimal);
  EXPECT_EQ(ConvertStrings(point, JsonBytesMappingKind::kHexId), expected);
}

INSTANTIATE_TEST_SUITE_P(Values,
                         OtlpJsonConverterUnsigned64BitInteger,
                         ::testing::Values(std::uint64_t{0},
                                           std::uint64_t{9},
                                           std::numeric_limits<std::uint64_t>::max()));

struct HexIdCase
{
  const char *name;
  void (*set)(proto::trace::v1::Span &span, const std::string &id);
  std::string id;
  const char *expected;
};

void PrintTo(const HexIdCase &hex_id_case, std::ostream *os)
{
  *os << hex_id_case.name;
}

class OtlpJsonConverterHexId : public ::testing::TestWithParam<HexIdCase>
{};

TEST_P(OtlpJsonConverterHexId, WritesLowercaseHex)
{
  proto::trace::v1::Span span;
  GetParam().set(span, GetParam().id);
  EXPECT_EQ(ConvertStrings(span, JsonBytesMappingKind::kHexId),
            std::vector<std::string>{GetParam().expected});
}

INSTANTIATE_TEST_SUITE_P(
    Fields,
    OtlpJsonConverterHexId,
    ::testing::Values(
        HexIdCase{
            "TraceId",
            [](proto::trace::v1::Span &span, const std::string &id) { span.set_trace_id(id); },
            std::string("\x00\x01\x7f\x80\xab\xcd\xef\xff\x10\x20\x30\x40\x50\x60\x70\x90", 16),
            "00017f80abcdefff1020304050607090"},
        HexIdCase{"SpanId",
                  [](proto::trace::v1::Span &span, const std::string &id) { span.set_span_id(id); },
                  std::string("\xde\xad\xbe\xef\x00\x00\x00\x01", 8), "deadbeef00000001"},
        HexIdCase{"ParentSpanId",
                  [](proto::trace::v1::Span &span, const std::string &id) {
                    span.set_parent_span_id(id);
                  },
                  std::string("\x00\xff\x00\xff\x12\x34\x56\x78", 8), "00ff00ff12345678"}),
    [](const ::testing::TestParamInfo<HexIdCase> &info) { return info.param.name; });

class OtlpJsonConverterHexBytes : public ::testing::TestWithParam<std::size_t>
{};

TEST_P(OtlpJsonConverterHexBytes, HexEncodesEveryByte)
{
  proto::common::v1::AnyValue any_value;
  any_value.set_bytes_value(std::string(GetParam(), '\xa5'));

  std::string expected;
  for (std::size_t i = 0; i < GetParam(); ++i)
  {
    expected += "a5";
  }
  EXPECT_EQ(ConvertStrings(any_value, JsonBytesMappingKind::kHex),
            std::vector<std::string>{expected});
}

// 32 and 33 bytes sit on either side of the converter's stack buffer.
INSTANTIATE_TEST_SUITE_P(
    Sizes,
    OtlpJsonConverterHexBytes,
    ::testing::Values(std::size_t{0}, std::size_t{32}, std::size_t{33}, std::size_t{100}),
    [](const ::testing::TestParamInfo<std::size_t> &info) {
      return "Bytes" + std::to_string(info.param);
    });

class OtlpJsonConverterNesting : public ::testing::TestWithParam<int>
{};

TEST_P(OtlpJsonConverterNesting, ContinuesOuterFieldsAfterANestedMessage)
{
  proto::common::v1::AnyValue root;
  proto::common::v1::AnyValue *current = &root;
  std::vector<std::string> expected;
  for (int level = 0; level < GetParam(); ++level)
  {
    auto *key_value = current->mutable_kvlist_value()->add_values();
    key_value->set_key("k" + std::to_string(level));
    expected.emplace_back(key_value->key());
    current = key_value->mutable_value();
  }
  current->set_string_value("leaf");
  expected.emplace_back("leaf");

  auto *after = root.mutable_kvlist_value()->add_values();
  after->set_key("after");
  after->mutable_value()->set_string_value("after-value");
  expected.emplace_back("after");
  expected.emplace_back("after-value");

  EXPECT_EQ(ConvertStrings(root, JsonBytesMappingKind::kHexId), expected);
}

INSTANTIATE_TEST_SUITE_P(Levels,
                         OtlpJsonConverterNesting,
                         ::testing::Values(1, 4, 12),
                         [](const ::testing::TestParamInfo<int> &info) {
                           return "Levels" + std::to_string(info.param);
                         });

}  // namespace
}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
