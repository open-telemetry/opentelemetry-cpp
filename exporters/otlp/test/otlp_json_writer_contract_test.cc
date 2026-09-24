// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "otlp_json_writer_contract_test.h"

#include <gtest/gtest.h>
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "opentelemetry/exporters/otlp/otlp_json_writer.h"
#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/test_common/sdk/common/scoped_test_log_handler.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{
namespace test
{
namespace
{

std::size_t CountErrors(const std::vector<test_common::ScopedTestLogHandler::Entry> &entries)
{
  return static_cast<std::size_t>(std::count_if(
      entries.begin(), entries.end(), [](const test_common::ScopedTestLogHandler::Entry &entry) {
        return entry.level == sdk::common::internal_log::LogLevel::Error;
      }));
}

// A finite double may be spelled in more than one way, so tests compare its parsed value.
double ParseDouble(const std::string &number)
{
  char *end           = nullptr;
  const double parsed = std::strtod(number.c_str(), &end);
  EXPECT_TRUE(!number.empty() && end == number.c_str() + number.size())
      << "not a number: " << number;
  return parsed;
}

}  // namespace

std::unique_ptr<JsonWriter> JsonWriterContract::MakeWriter() const
{
  return GetParam()->Create();
}

std::string JsonWriterContract::Finish(JsonWriter &writer)
{
  EXPECT_TRUE(writer.ok());
  std::string json = writer.ToString();
  EXPECT_TRUE(writer.ok());
  EXPECT_EQ(CountErrors(log_.Drain()), 0u);
  return json;
}

void JsonWriterContract::ExpectFailedOnce(JsonWriter &writer)
{
  EXPECT_FALSE(writer.ok());

  writer.BeginObject();
  writer.Key("a");
  writer.WriteInt32(1);
  writer.EndObject();
  writer.EndArray();
  writer.Key("b");
  EXPECT_FALSE(writer.ok());

  writer.ToString();
  EXPECT_FALSE(writer.ok());
  EXPECT_EQ(CountErrors(log_.Drain()), 1u);
}

TEST_P(JsonWriterContract, FactoryCreatesAWorkingWriter)
{
  auto writer = MakeWriter();
  ASSERT_NE(writer, nullptr);
  EXPECT_TRUE(writer->ok());
}

TEST_P(JsonWriterContract, FactoryCreatesWritersConcurrently)
{
  static constexpr int kThreads          = 8;
  static constexpr int kWritersPerThread = 200;
  std::vector<std::string> failures(kThreads);
  std::vector<std::thread> threads;
  threads.reserve(kThreads);
  for (int t = 0; t < kThreads; ++t)
  {
    threads.emplace_back([this, t, &failures] {
      for (int i = 0; i < kWritersPerThread && failures[t].empty(); ++i)
      {
        auto writer = MakeWriter();
        if (!writer)
        {
          failures[t] = "Create() returned nullptr";
          break;
        }
        writer->BeginArray();
        writer->WriteInt32(t);
        writer->WriteInt32(i);
        writer->EndArray();
        std::string expected = "[";
        expected += std::to_string(t);
        expected += ",";
        expected += std::to_string(i);
        expected += "]";
        const std::string json = writer->ToString();
        if (!writer->ok() || json != expected)
        {
          failures[t] = "expected " + expected;
          failures[t] += ", got ";
          failures[t] += json;
        }
      }
    });
  }
  for (auto &thread : threads)
  {
    thread.join();
  }
  for (const auto &failure : failures)
  {
    EXPECT_EQ(failure, "");
  }
}

TEST_P(JsonWriterContract, WritesNull)
{
  auto writer = MakeWriter();
  writer->WriteNull();
  EXPECT_EQ(Finish(*writer), "null");
}

TEST_P(JsonWriterContract, WritesTrue)
{
  auto writer = MakeWriter();
  writer->WriteBool(true);
  EXPECT_EQ(Finish(*writer), "true");
}

TEST_P(JsonWriterContract, WritesFalse)
{
  auto writer = MakeWriter();
  writer->WriteBool(false);
  EXPECT_EQ(Finish(*writer), "false");
}

TEST_P(JsonWriterContract, WritesInt32Min)
{
  auto writer = MakeWriter();
  writer->WriteInt32(std::numeric_limits<std::int32_t>::min());
  EXPECT_EQ(Finish(*writer), "-2147483648");
}

TEST_P(JsonWriterContract, WritesInt32Max)
{
  auto writer = MakeWriter();
  writer->WriteInt32(std::numeric_limits<std::int32_t>::max());
  EXPECT_EQ(Finish(*writer), "2147483647");
}

TEST_P(JsonWriterContract, WritesUInt32Max)
{
  auto writer = MakeWriter();
  writer->WriteUInt32(std::numeric_limits<std::uint32_t>::max());
  EXPECT_EQ(Finish(*writer), "4294967295");
}

// The extremes of both 64-bit types are outside the range a double represents exactly.
TEST_P(JsonWriterContract, WritesInt64Min)
{
  auto writer = MakeWriter();
  writer->WriteInt64(std::numeric_limits<std::int64_t>::min());
  EXPECT_EQ(Finish(*writer), "-9223372036854775808");
}

TEST_P(JsonWriterContract, WritesInt64Max)
{
  auto writer = MakeWriter();
  writer->WriteInt64(std::numeric_limits<std::int64_t>::max());
  EXPECT_EQ(Finish(*writer), "9223372036854775807");
}

TEST_P(JsonWriterContract, WritesUInt64Max)
{
  auto writer = MakeWriter();
  writer->WriteUInt64(std::numeric_limits<std::uint64_t>::max());
  EXPECT_EQ(Finish(*writer), "18446744073709551615");
}

TEST_P(JsonWriterContract, WritesDouble)
{
  auto writer = MakeWriter();
  writer->WriteDouble(0.1);
  EXPECT_EQ(ParseDouble(Finish(*writer)), 0.1);
}

TEST_P(JsonWriterContract, WritesLargeNegativeDouble)
{
  auto writer = MakeWriter();
  writer->WriteDouble(-1.5e300);
  EXPECT_EQ(ParseDouble(Finish(*writer)), -1.5e300);
}

TEST_P(JsonWriterContract, WritesDenormalDouble)
{
  auto writer = MakeWriter();
  writer->WriteDouble(std::numeric_limits<double>::denorm_min());
  EXPECT_EQ(ParseDouble(Finish(*writer)), std::numeric_limits<double>::denorm_min());
}

TEST_P(JsonWriterContract, WritesNaNAsNull)
{
  auto writer = MakeWriter();
  writer->WriteDouble(std::numeric_limits<double>::quiet_NaN());
  EXPECT_EQ(Finish(*writer), "null");
}

TEST_P(JsonWriterContract, WritesInfinityAsNull)
{
  auto writer = MakeWriter();
  writer->WriteDouble(std::numeric_limits<double>::infinity());
  EXPECT_EQ(Finish(*writer), "null");
}

TEST_P(JsonWriterContract, WritesNegativeInfinityAsNull)
{
  auto writer = MakeWriter();
  writer->WriteDouble(-std::numeric_limits<double>::infinity());
  EXPECT_EQ(Finish(*writer), "null");
}

TEST_P(JsonWriterContract, WritesString)
{
  auto writer = MakeWriter();
  writer->WriteString("hello");
  EXPECT_EQ(Finish(*writer), R"("hello")");
}

TEST_P(JsonWriterContract, WritesEmptyString)
{
  auto writer = MakeWriter();
  writer->WriteString("");
  EXPECT_EQ(Finish(*writer), R"("")");
}

TEST_P(JsonWriterContract, EscapesOnlyWhatJsonRequiresInString)
{
  auto writer = MakeWriter();
  writer->WriteString("\"\\\b\f\n\r\t\x01\x1F/\x7F");
  EXPECT_EQ(Finish(*writer), "\"\\\"\\\\\\b\\f\\n\\r\\t\\u0001\\u001f/\x7F\"");
}

TEST_P(JsonWriterContract, WritesStringWithNul)
{
  auto writer = MakeWriter();
  writer->WriteString(std::string("a\0b", 3));
  EXPECT_EQ(Finish(*writer), "\"a\\u0000b\"");
}

TEST_P(JsonWriterContract, WritesUtf8String)
{
  auto writer = MakeWriter();
  writer->WriteString("\xC3\xA9\xE2\x82\xAC\xF0\x9F\x98\x80");
  EXPECT_EQ(Finish(*writer), "\"\xC3\xA9\xE2\x82\xAC\xF0\x9F\x98\x80\"");
}

TEST_P(JsonWriterContract, ReplacesInvalidUtf8InString)
{
  auto writer = MakeWriter();
  writer->WriteString(
      "a\xFF"
      "b");
  EXPECT_EQ(Finish(*writer),
            "\"a\xEF\xBF\xBD"
            "b\"");
}

TEST_P(JsonWriterContract, CopiesString)
{
  std::string value = "value";
  auto writer       = MakeWriter();
  writer->WriteString(value);
  value.assign("xxxxx");
  EXPECT_EQ(Finish(*writer), R"("value")");
}

TEST_P(JsonWriterContract, WritesBytesAsBase64)
{
  const std::uint8_t bytes[] = {0x00, 0x01, 0x02, 0xFF};
  auto writer                = MakeWriter();
  writer->WriteBytes(bytes, sizeof(bytes));
  EXPECT_EQ(Finish(*writer), R"("AAEC/w==")");
}

TEST_P(JsonWriterContract, WritesEmptyBytesFromNull)
{
  auto writer = MakeWriter();
  writer->WriteBytes(nullptr, 0);
  EXPECT_EQ(Finish(*writer), R"("")");
}

TEST_P(JsonWriterContract, CopiesBytes)
{
  std::uint8_t bytes[] = {'a', 'b', 'c'};
  auto writer          = MakeWriter();
  writer->WriteBytes(bytes, sizeof(bytes));
  bytes[0] = bytes[1] = bytes[2] = 0;
  EXPECT_EQ(Finish(*writer), R"("YWJj")");
}

TEST_P(JsonWriterContract, WritesEmptyObject)
{
  auto writer = MakeWriter();
  writer->BeginObject();
  writer->EndObject();
  EXPECT_EQ(Finish(*writer), "{}");
}

TEST_P(JsonWriterContract, WritesEmptyArray)
{
  auto writer = MakeWriter();
  writer->BeginArray();
  writer->EndArray();
  EXPECT_EQ(Finish(*writer), "[]");
}

TEST_P(JsonWriterContract, WritesArray)
{
  auto writer = MakeWriter();
  writer->BeginArray();
  writer->WriteInt32(1);
  writer->WriteString("two");
  writer->WriteNull();
  writer->EndArray();
  EXPECT_EQ(Finish(*writer), R"([1,"two",null])");
}

TEST_P(JsonWriterContract, WritesObjectInAnyMemberOrder)
{
  auto writer = MakeWriter();
  writer->BeginObject();
  writer->Key("b");
  writer->WriteString("two");
  writer->Key("a");
  writer->WriteInt32(1);
  writer->EndObject();
  const std::string json = Finish(*writer);
  EXPECT_TRUE(json == R"({"b":"two","a":1})" || json == R"({"a":1,"b":"two"})") << json;
}

TEST_P(JsonWriterContract, WritesNestedContainers)
{
  auto writer = MakeWriter();
  writer->BeginObject();
  writer->Key("items");
  writer->BeginArray();
  writer->BeginObject();
  writer->Key("id");
  writer->WriteInt32(1);
  writer->EndObject();
  writer->BeginArray();
  writer->EndArray();
  writer->BeginObject();
  writer->Key("empty");
  writer->BeginObject();
  writer->EndObject();
  writer->EndObject();
  writer->EndArray();
  writer->EndObject();
  EXPECT_EQ(Finish(*writer), R"({"items":[{"id":1},[],{"empty":{}}]})");
}

TEST_P(JsonWriterContract, EscapesKey)
{
  auto writer = MakeWriter();
  writer->BeginObject();
  writer->Key(std::string("\"\\\n\x01\0", 5));
  writer->WriteInt32(1);
  writer->EndObject();
  EXPECT_EQ(Finish(*writer), "{\"\\\"\\\\\\n\\u0001\\u0000\":1}");
}

TEST_P(JsonWriterContract, ReplacesInvalidUtf8InKey)
{
  auto writer = MakeWriter();
  writer->BeginObject();
  writer->Key(
      "a\xFF"
      "b");
  writer->WriteInt32(1);
  writer->EndObject();
  EXPECT_EQ(Finish(*writer),
            "{\"a\xEF\xBF\xBD"
            "b\":1}");
}

TEST_P(JsonWriterContract, CopiesKey)
{
  std::string key = "key";
  auto writer     = MakeWriter();
  writer->BeginObject();
  writer->Key(key);
  key.assign("xxx");
  writer->WriteInt32(1);
  writer->EndObject();
  EXPECT_EQ(Finish(*writer), R"({"key":1})");
}

TEST_P(JsonWriterContract, KeyAtTopLevelFails)
{
  auto writer = MakeWriter();
  writer->Key("a");
  ExpectFailedOnce(*writer);
}

TEST_P(JsonWriterContract, KeyInArrayFails)
{
  auto writer = MakeWriter();
  writer->BeginArray();
  writer->Key("a");
  ExpectFailedOnce(*writer);
}

TEST_P(JsonWriterContract, KeyTwiceFails)
{
  auto writer = MakeWriter();
  writer->BeginObject();
  writer->Key("a");
  writer->Key("b");
  ExpectFailedOnce(*writer);
}

TEST_P(JsonWriterContract, ValueWithoutKeyFails)
{
  auto writer = MakeWriter();
  writer->BeginObject();
  writer->WriteInt32(1);
  ExpectFailedOnce(*writer);
}

TEST_P(JsonWriterContract, ContainerWithoutKeyFails)
{
  auto writer = MakeWriter();
  writer->BeginObject();
  writer->BeginArray();
  ExpectFailedOnce(*writer);
}

TEST_P(JsonWriterContract, ObjectClosedAfterKeyFails)
{
  auto writer = MakeWriter();
  writer->BeginObject();
  writer->Key("a");
  writer->EndObject();
  ExpectFailedOnce(*writer);
}

TEST_P(JsonWriterContract, MismatchedEndFails)
{
  auto writer = MakeWriter();
  writer->BeginObject();
  writer->EndArray();
  ExpectFailedOnce(*writer);
}

TEST_P(JsonWriterContract, EndWithoutBeginFails)
{
  auto writer = MakeWriter();
  writer->EndObject();
  ExpectFailedOnce(*writer);
}

TEST_P(JsonWriterContract, SecondTopLevelValueFails)
{
  auto writer = MakeWriter();
  writer->WriteInt32(1);
  writer->WriteInt32(2);
  ExpectFailedOnce(*writer);
}

}  // namespace test
}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
