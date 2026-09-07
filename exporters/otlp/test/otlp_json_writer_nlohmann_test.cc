// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/otlp/otlp_json_writer.h"

#include <gtest/gtest.h>
#include <cstdint>
#include <functional>
#include <memory>
#include <ostream>
#include <string>

#include "opentelemetry/exporters/otlp/detail/default_json_writer_factory.h"
#include "opentelemetry/exporters/otlp/otlp_json_writer_factory.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{
namespace
{

std::unique_ptr<JsonWriter> MakeWriter()
{
  return detail::GetDefaultJsonWriterFactory()->Create();
}

}  // namespace

TEST(NlohmannJsonWriter, FactoryReturnsAWorkingWriter)
{
  auto writer = MakeWriter();
  ASSERT_NE(writer, nullptr);
  EXPECT_TRUE(writer->ok());
}

struct ScalarCase
{
  const char *name;
  std::function<void(JsonWriter &)> write;
  const char *expected;
};

void PrintTo(const ScalarCase &scalar_case, std::ostream *os)
{
  *os << scalar_case.name;
}

class NlohmannJsonWriterScalar : public ::testing::TestWithParam<ScalarCase>
{};

TEST_P(NlohmannJsonWriterScalar, WritesTopLevelScalar)
{
  auto writer = MakeWriter();
  GetParam().write(*writer);
  EXPECT_TRUE(writer->ok());
  EXPECT_EQ(writer->ToString(), GetParam().expected);
}

INSTANTIATE_TEST_SUITE_P(
    Scalars,
    NlohmannJsonWriterScalar,
    ::testing::Values(
        ScalarCase{"Null", [](JsonWriter &w) { w.WriteNull(); }, "null"},
        ScalarCase{"String", [](JsonWriter &w) { w.WriteString("hello"); }, "\"hello\""},
        ScalarCase{"Int32", [](JsonWriter &w) { w.WriteInt32(-7); }, "-7"},
        ScalarCase{"Int64", [](JsonWriter &w) { w.WriteInt64(-7000000000LL); }, "-7000000000"},
        ScalarCase{"UInt32", [](JsonWriter &w) { w.WriteUInt32(42u); }, "42"},
        ScalarCase{"UInt64", [](JsonWriter &w) { w.WriteUInt64(42000000000ULL); }, "42000000000"},
        ScalarCase{"Double", [](JsonWriter &w) { w.WriteDouble(3.5); }, "3.5"},
        ScalarCase{"Bool", [](JsonWriter &w) { w.WriteBool(true); }, "true"},
        // Base64 of 00 01 02 FF
        ScalarCase{"Bytes",
                   [](JsonWriter &w) {
                     const std::uint8_t bytes[] = {0, 1, 2, 255};
                     w.WriteBytes(bytes, sizeof(bytes));
                   },
                   "\"AAEC/w==\""}),
    [](const ::testing::TestParamInfo<ScalarCase> &info) { return info.param.name; });

// The extremes of both 64-bit types are outside the range a double represents
// exactly.
TEST(NlohmannJsonWriter, PreservesFull64BitRangeExactly)
{
  {
    auto writer = MakeWriter();
    writer->WriteInt64(-9223372036854775807LL - 1);
    EXPECT_EQ(writer->ToString(), "-9223372036854775808");
  }
  {
    auto writer = MakeWriter();
    writer->WriteInt64(9223372036854775807LL);
    EXPECT_EQ(writer->ToString(), "9223372036854775807");
  }
  {
    auto writer = MakeWriter();
    writer->WriteUInt64(18446744073709551615ULL);
    EXPECT_EQ(writer->ToString(), "18446744073709551615");
  }
}

TEST(NlohmannJsonWriter, WritesAnEmptyObject)
{
  auto writer = MakeWriter();
  writer->BeginObject();
  writer->EndObject();
  EXPECT_TRUE(writer->ok());
  EXPECT_EQ(writer->ToString(), "{}");
}

TEST(NlohmannJsonWriter, WritesAnEmptyArray)
{
  auto writer = MakeWriter();
  writer->BeginArray();
  writer->EndArray();
  EXPECT_TRUE(writer->ok());
  EXPECT_EQ(writer->ToString(), "[]");
}

TEST(NlohmannJsonWriter, WritesAFlatObject)
{
  auto writer = MakeWriter();
  writer->BeginObject();
  writer->Key("a");
  writer->WriteInt32(1);
  writer->Key("b");
  writer->WriteString("two");
  writer->EndObject();
  EXPECT_TRUE(writer->ok());
  EXPECT_EQ(writer->ToString(), R"({"a":1,"b":"two"})");
}

TEST(NlohmannJsonWriter, WritesAFlatArray)
{
  auto writer = MakeWriter();
  writer->BeginArray();
  writer->WriteInt32(1);
  writer->WriteInt32(2);
  writer->WriteInt32(3);
  writer->EndArray();
  EXPECT_TRUE(writer->ok());
  EXPECT_EQ(writer->ToString(), "[1,2,3]");
}

TEST(NlohmannJsonWriter, WritesNestedObjectsAndArrays)
{
  auto writer = MakeWriter();
  writer->BeginObject();
  writer->Key("items");
  writer->BeginArray();
  writer->BeginObject();
  writer->Key("id");
  writer->WriteInt32(1);
  writer->EndObject();
  writer->BeginObject();
  writer->Key("id");
  writer->WriteInt32(2);
  writer->EndObject();
  writer->EndArray();
  writer->EndObject();
  EXPECT_TRUE(writer->ok());
  EXPECT_EQ(writer->ToString(), R"({"items":[{"id":1},{"id":2}]})");
}

TEST(NlohmannJsonWriter, KeyOutsideObjectFailsStickily)
{
  auto writer = MakeWriter();
  writer->BeginArray();
  writer->Key("bad");
  EXPECT_FALSE(writer->ok());

  // Once failed, ok() stays false even if later calls look well-formed.
  writer->WriteInt32(1);
  writer->EndArray();
  EXPECT_FALSE(writer->ok());
}

TEST(NlohmannJsonWriter, ValueWithoutPrecedingKeyFails)
{
  auto writer = MakeWriter();
  writer->BeginObject();
  writer->WriteInt32(1);
  EXPECT_FALSE(writer->ok());
}

TEST(NlohmannJsonWriter, MismatchedEndFails)
{
  auto writer = MakeWriter();
  writer->BeginObject();
  writer->EndArray();
  EXPECT_FALSE(writer->ok());
}

TEST(NlohmannJsonWriter, EndWithoutBeginFails)
{
  auto writer = MakeWriter();
  writer->EndObject();
  EXPECT_FALSE(writer->ok());
}

TEST(NlohmannJsonWriter, SecondTopLevelValueFails)
{
  auto writer = MakeWriter();
  writer->WriteInt32(1);
  EXPECT_TRUE(writer->ok());
  writer->WriteInt32(2);
  EXPECT_FALSE(writer->ok());
}

TEST(NlohmannJsonWriter, MisuseDoesNotCrashToString)
{
  auto writer = MakeWriter();
  writer->BeginObject();
  writer->EndArray();
  EXPECT_FALSE(writer->ok());
  // Must not throw or crash even though the writer failed.
  writer->ToString();
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
