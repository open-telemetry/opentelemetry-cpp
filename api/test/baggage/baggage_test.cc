// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <cassert>
#include <cstddef>
#include <initializer_list>
#include <string>
#include <utility>
#include <vector>

#include "opentelemetry/baggage/baggage.h"
#include "opentelemetry/common/kv_properties.h"
#include "opentelemetry/nostd/function_ref.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/string_view.h"

using namespace opentelemetry::baggage;

TEST(BaggageTest, RejectInvalidMetadataOnExtractAndInject)
{
  const std::vector<std::string> invalid = {
      ";a\r\nX-Injected: 1",
      ";a\r\n\r\nGET /smuggled HTTP/1.1\r\nHost: localhost\r\nX-Smuggled: 1",
      ";a\rb",
      ";a\nb",
      std::string(";a\0b", 4),
      ";a=\x7f",
      ";a=\xc3\xa9",
      ";",
      ";a;;b",
      ";=value",
      ";bad key=value",
      ";a=bad value",
      ";a=bad\tvalue",
      ";a=\"quoted\"",
      ";a=back\\slash"};
  for (const auto &metadata : invalid)
  {
    SCOPED_TRACE(metadata);
    auto baggage = Baggage::FromHeader("before=ok,bad=value" + metadata + ",after=ok");
    std::string value;
    EXPECT_FALSE(baggage->GetValue("bad", value));
    EXPECT_TRUE(baggage->GetValue("before", value));
    EXPECT_TRUE(baggage->GetValue("after", value));
    EXPECT_EQ(baggage->ToHeader(), "before=ok,after=ok");

    // Direct construction does not validate input. Output must still be safe.
    const std::vector<std::pair<std::string, std::string>> entries = {
        {"before", "ok"}, {"bad", "value" + metadata}, {"after", "ok"}};
    // KeyValueProperties exposes NUL-terminated strings, so direct construction
    // already truncates at an embedded NUL. Extraction must reject it above.
    if (metadata.find('\0') == std::string::npos)
    {
      EXPECT_EQ(Baggage(entries).ToHeader(), "before=ok,after=ok");
    }
    EXPECT_EQ(Baggage().Set("bad", "value" + metadata)->ToHeader(), "");
  }
}

TEST(BaggageTest, RejectMemberInjectionFromSet)
{
  auto baggage = Baggage::FromHeader("good=ok")->Set("k", "v;a,evil=1");
  EXPECT_EQ(baggage->ToHeader(), "good=ok");
}

TEST(BaggageTest, SetWithInvalidMetadataPreservesExistingEntry)
{
  auto original = Baggage::FromHeader("k=good;flag,other=kept");
  for (const auto *invalid : {"v;a,evil=1", "v;", "v;;x", "v;p:x", "v;p=bad value"})
  {
    SCOPED_TRACE(invalid);
    auto updated = original->Set("k", invalid);
    std::string value;
    ASSERT_TRUE(updated->GetValue("k", value));
    EXPECT_EQ(value, "good;flag");
    EXPECT_EQ(updated->ToHeader(), "k=good;flag,other=kept");
    EXPECT_EQ(original->ToHeader(), "k=good;flag,other=kept");

    auto inserted = original->Set("new", invalid);
    EXPECT_FALSE(inserted->GetValue("new", value));
    EXPECT_EQ(inserted->ToHeader(), "k=good;flag,other=kept");
  }

  // Valid metadata must still replace the existing value.
  auto updated = original->Set("k", "replacement;p=100%");
  std::string value;
  ASSERT_TRUE(updated->GetValue("k", value));
  EXPECT_EQ(value, "replacement;p=100%");
  EXPECT_EQ(updated->ToHeader(), "k=replacement;p=100%,other=kept");
}

TEST(BaggageTest, RejectMemberInjectionFromDecodedDelimiters)
{
  // Decoding the value introduces a semicolon that output treats as metadata.
  auto baggage = Baggage::FromHeader("k=a%3B%2Cx%3Dy,good=ok");
  EXPECT_EQ(baggage->ToHeader(), "good=ok");
}

TEST(BaggageTest, ReplaceInvalidUtf8InPercentDecodedValue)
{
  auto baggage = Baggage::FromHeader("k=aa%ffcc%fedd%fa,valid=Am%C3%A9lie");
  std::string value;
  ASSERT_TRUE(baggage->GetValue("k", value));
  EXPECT_EQ(value, std::string("aa\xef\xbf\xbd" "cc\xef\xbf\xbd\xef\xbf\xbd"));
  ASSERT_TRUE(baggage->GetValue("valid", value));
  EXPECT_EQ(value, "Am\xc3\xa9lie");
  EXPECT_EQ(baggage->ToHeader(), "k=aa%EF%BF%BDcc%EF%BF%BD%EF%BF%BD,valid=Am%C3%A9lie");
}

TEST(BaggageTest, RejectDecodedAsciiControlInValue)
{
  auto baggage = Baggage::FromHeader("bad=before%0Aafter,good=ok");
  EXPECT_EQ(baggage->ToHeader(), "good=ok");
}

TEST(BaggageTest, PreserveValidMetadataWireRepresentation)
{
  for (const std::string metadata :
       {";flag", ";empty=", ";a=b=c", ";p=100%", ";a=%20%0D%0A%25", "; flag \t; key \t=\t value",
        ";a=!#$&'()*+-./:<=>?@[]^_`{|}~"})
  {
    SCOPED_TRACE(metadata);
    const auto header = "key=value" + metadata;
    EXPECT_EQ(Baggage::FromHeader(header)->ToHeader(), header);
    const std::vector<std::pair<std::string, std::string>> entries = {{"key", "value" + metadata}};
    EXPECT_EQ(Baggage(entries).ToHeader(), header);
  }
}

TEST(BaggageTest, RejectUnsafeBytesAfterTokenization)
{
  for (int byte = 0; byte <= 255; ++byte)
  {
    if ((byte >= 0x20 && byte <= 0x7e) || byte == '\t')
    {
      continue;
    }
    SCOPED_TRACE(byte);
    const std::string control(1, static_cast<char>(byte));
    // Interior bytes must be checked in both the key and the complete value.
    for (const auto &member :
         {"bad" + control + "key=value;flag", "bad=value;flag=" + control + "value"})
    {
      EXPECT_EQ(Baggage::FromHeader(member + ",good=ok")->ToHeader(), "good=ok");
    }
    // The shared tokenizer has always normalized these whitespace bytes at edges.
    const bool trimmed = byte >= 0x0a && byte <= 0x0d;
    for (const auto &member : {control + "bad=value;flag", "bad=value;flag" + control})
    {
      EXPECT_EQ(Baggage::FromHeader(member + ",good=ok")->ToHeader(),
                trimmed ? "bad=value;flag,good=ok" : "good=ok");
    }
  }
}

TEST(BaggageTest, PreserveTokenizerEdgeWhitespaceNormalization)
{
  EXPECT_EQ(Baggage::FromHeader(" \t\r\n\v\fk=v;flag\r\n\v\f \t,other=ok")->ToHeader(),
            "k=v;flag,other=ok");
  // Directly constructed metadata is not tokenizer-normalized and must remain rejected.
  const std::vector<std::pair<std::string, std::string>> entries = {{"k", "v;flag\r\n"}};
  EXPECT_EQ(Baggage(entries).ToHeader(), "");
}

static std::string header_with_custom_entries(size_t num_entries)
{
  std::string header;
  for (size_t i = 0; i < num_entries; i++)
  {
    std::string key   = "key" + std::to_string(i);
    std::string value = "value" + std::to_string(i);
    header.append(key).append("=").append(value);
    if (i != num_entries - 1)
    {
      header += ",";
    }
  }
  return header;
}

static std::string header_with_custom_size(size_t key_value_size, size_t num_entries)
{
  std::string header = "";
  for (size_t i = 0; i < num_entries; i++)
  {
    std::string str = std::to_string(i + 1);
    str += "=";
    assert(key_value_size > str.size());
    for (size_t j = str.size(); j < key_value_size; j++)
    {
      str += "a";
    }

    header += str + ',';
  }

  header.pop_back();
  return header;
}

TEST(BaggageTest, ValidateExtractHeader)
{
  auto invalid_key_value_size_header = header_with_custom_size(Baggage::kMaxKeyValueSize + 5, 1);

  struct
  {
    const char *input;
    std::vector<const char *> keys;
    std::vector<const char *> values;
  } testcases[] = {
      {"k1=v1", {"k1"}, {"v1"}},
      {"k1=V1,K2=v2;metadata,k3=v3",
       {"k1", "K2", "k3"},
       {"V1", "v2;metadata", "v3"}},  // metadata is part of value
      {",k1 =v1,k2=v2 ; metadata,",
       {"k1", "k2"},
       {"v1", "v2; metadata"}},  // key and value are trimmed
      {"1a-2f%40foo=bar%251,a%2A%2Ffoo-_%2Fbar=bar+4",
       {"1a-2f@foo", "a*/foo-_/bar"},
       {"bar%1", "bar 4"}},                                       // decoding is done properly
      {"field=foo:bar", {"field"}, {"foo:bar"}},                  // colon in value
      {"mixed=a/b:c?d=e", {"mixed"}, {"a/b:c?d=e"}},              // mixed special characters
      {"k1=v1,invalidmember,k2=v2", {"k1", "k2"}, {"v1", "v2"}},  // invalid member is skipped
      {",", {}, {}},
      {",=,", {}, {}},
      {"", {}, {}},
      {"k1=%5zv", {}, {}},  // invalid hex : invalid second digit
      {"k1=%5", {}, {}},    // invalid hex : missing two digits
      {"k%z2=v1", {}, {}},  // invalid hex : invalid first digit
      {"k%00=v1", {}, {}},  // key not valid
      {"k=v%7f", {}, {}},   // value not valid
      {invalid_key_value_size_header.data(), {}, {}}};
  for (auto &testcase : testcases)
  {
    auto baggage = Baggage::FromHeader(testcase.input);
    size_t index = 0;
    baggage->GetAllEntries([&testcase, &index](opentelemetry::nostd::string_view key,
                                               opentelemetry::nostd::string_view value) {
      EXPECT_EQ(key, testcase.keys[index]);
      EXPECT_EQ(value, testcase.values[index]);
      index++;
      return true;
    });
  }

  // For header with maximum threshold pairs, no pair is dropped
  auto max_pairs_header = header_with_custom_entries(Baggage::kMaxKeyValuePairs);
  EXPECT_EQ(Baggage::FromHeader(max_pairs_header.data())->ToHeader(), max_pairs_header.data());

  // Entries beyond threshold are dropped
  auto baggage = Baggage::FromHeader(header_with_custom_entries(Baggage::kMaxKeyValuePairs + 1));
  auto header  = baggage->ToHeader();
  opentelemetry::common::KeyValueStringTokenizer kv_str_tokenizer(header);
  int expected_tokens = Baggage::kMaxKeyValuePairs;
  EXPECT_EQ(kv_str_tokenizer.NumTokens(), expected_tokens);

  // For header with total size more than threshold, baggage is empty
  int num_pairs_with_max_size = Baggage::kMaxSize / Baggage::kMaxKeyValueSize;
  auto invalid_total_size_header =
      header_with_custom_size(Baggage::kMaxKeyValueSize, num_pairs_with_max_size + 1);
  EXPECT_EQ(Baggage::FromHeader(invalid_total_size_header.data())->ToHeader(), "");
}

TEST(BaggageTest, ValidateInjectHeader)
{
  struct
  {
    std::vector<const char *> keys;
    std::vector<const char *> values;
    const char *header;
  } testcases[] = {{{"k1"}, {"v1"}, "k1=v1"},
                   {{"k3", "k2", "k1"}, {"", "v2", "v1"}, "k1=v1,k2=v2,k3="},  // empty value
                   {{"1a-2f@foo", "a*/foo-_/bar"},
                    {"bar%1", "bar 4"},
                    "a%2A%2Ffoo-_%2Fbar=bar+4,1a-2f%40foo=bar%251"},  // encoding is done properly
                   {{"foo 1"},
                    {"bar 1;  metadata ; flag=encoded%20value"},
                    "foo+1=bar+1;  metadata ; flag=encoded%20value"}};

  for (auto &testcase : testcases)
  {
    opentelemetry::nostd::shared_ptr<Baggage> baggage(new Baggage{});
    for (size_t i = 0; i < testcase.keys.size(); i++)
    {
      baggage = baggage->Set(testcase.keys[i], testcase.values[i]);
    }
    EXPECT_EQ(baggage->ToHeader(), testcase.header);
  }
}

TEST(BaggageTest, BaggageGet)
{
  auto header  = header_with_custom_entries(Baggage::kMaxKeyValuePairs);
  auto baggage = Baggage::FromHeader(header);

  std::string value;
  EXPECT_TRUE(baggage->GetValue("key0", value));
  EXPECT_EQ(value, "value0");
  EXPECT_TRUE(baggage->GetValue("key16", value));
  EXPECT_EQ(value, "value16");

  EXPECT_TRUE(baggage->GetValue("key31", value));
  EXPECT_EQ(value, "value31");

  EXPECT_FALSE(baggage->GetValue("key181", value));
}

TEST(BaggageTest, BaggageSet)
{
  std::string header = "k1=v1,k2=v2";
  auto baggage       = Baggage::FromHeader(header);

  std::string value;
  baggage = baggage->Set("k3", "v3");
  EXPECT_TRUE(baggage->GetValue("k3", value));
  EXPECT_EQ(value, "v3");

  baggage = baggage->Set("k3", "v3_1");  // key should be updated with the latest value
  EXPECT_TRUE(baggage->GetValue("k3", value));
  EXPECT_EQ(value, "v3_1");

  header  = header_with_custom_entries(Baggage::kMaxKeyValuePairs);
  baggage = Baggage::FromHeader(header);
  baggage = baggage->Set("key0", "0");  // updating on max list should work
  EXPECT_TRUE(baggage->GetValue("key0", value));
  EXPECT_EQ(value, "0");

  header  = "k1=v1,k2=v2";
  baggage = Baggage::FromHeader(header);
  baggage = baggage->Set("", "n_v1");  // adding invalid key, should return copy of same baggage
  EXPECT_EQ(baggage->ToHeader(), header);

  header  = "k1=v1,k2=v2";
  baggage = Baggage::FromHeader(header);
  baggage = baggage->Set("k1", "\x1A");  // adding invalid value, should return copy of same baggage
  EXPECT_EQ(baggage->ToHeader(), header);
}

TEST(BaggageTest, BaggageRemove)
{
  auto header  = header_with_custom_entries(Baggage::kMaxKeyValuePairs);
  auto baggage = Baggage::FromHeader(header);
  std::string value;

  // existing key is removed
  EXPECT_TRUE(baggage->GetValue("key0", value));
  auto new_baggage = baggage->Delete("key0");
  EXPECT_FALSE(new_baggage->GetValue("key0", value));

  // trying Delete on non existent key
  EXPECT_FALSE(baggage->GetValue("key181", value));
  auto new_baggage_2 = baggage->Delete("key181");
  EXPECT_FALSE(new_baggage_2->GetValue("key181", value));
}

TEST(BaggageTest, BaggageGetAll)
{
  std::string baggage_header                          = "k1=v1,k2=v2,k3=v3";
  auto baggage                                        = Baggage::FromHeader(baggage_header);
  const int kNumPairs                                 = 3;
  opentelemetry::nostd::string_view keys[kNumPairs]   = {"k1", "k2", "k3"};
  opentelemetry::nostd::string_view values[kNumPairs] = {"v1", "v2", "v3"};
  size_t index                                        = 0;
  baggage->GetAllEntries([&keys, &values, &index](opentelemetry::nostd::string_view key,
                                                  opentelemetry::nostd::string_view value) {
    EXPECT_EQ(key, keys[index]);
    EXPECT_EQ(value, values[index]);
    index++;
    return true;
  });
}
