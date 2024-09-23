// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <map>
#include <string>
#include <utility>

#include "opentelemetry/ext/http/common/url_parser.h"

namespace http_common = opentelemetry::ext::http::common;

inline const char *BoolToString(bool b)
{
  return b ? "true" : "false";
}

TEST(UrlParserTests, BasicTests)
{
  std::map<std::string, std::map<std::string, std::string>> urls_map{
      {"www.abc.com",
       {{"host", "www.abc.com"},
        {"port", "80"},
        {"scheme", "http"},
        {"path", "/"},
        {"query", ""},
        {"success", "true"}}},
      {"http://www.abc.com",
       {{"host", "www.abc.com"},
        {"port", "80"},
        {"scheme", "http"},
        {"path", "/"},
        {"query", ""},
        {"success", "true"}}},
      {"https://www.abc.com",
       {{"host", "www.abc.com"},
        {"port", "443"},
        {"scheme", "https"},
        {"path", "/"},
        {"query", ""},
        {"success", "true"}}},
      {"https://www.abc.com:4431",
       {{"host", "www.abc.com"},
        {"port", "4431"},
        {"scheme", "https"},
        {"path", "/"},
        {"query", ""},
        {"success", "true"}}},
      {"https://www.abc.com:4431",
       {{"host", "www.abc.com"},
        {"port", "4431"},
        {"scheme", "https"},
        {"path", "/"},
        {"query", ""},
        {"success", "true"}}},
      {"https://www.abc.com:4431/path1",
       {{"host", "www.abc.com"},
        {"port", "4431"},
        {"scheme", "https"},
        {"path", "/path1"},
        {"query", ""},
        {"success", "true"}}},
      {"https://www.abc.com:4431/path1/path2",
       {{"host", "www.abc.com"},
        {"port", "4431"},
        {"scheme", "https"},
        {"path", "/path1/path2"},
        {"query", ""},
        {"success", "true"}}},
      {"https://www.abc.com/path1/path2",
       {{"host", "www.abc.com"},
        {"port", "443"},
        {"scheme", "https"},
        {"path", "/path1/path2"},
        {"query", ""},
        {"success", "true"}}},
      {"http://www.abc.com/path1/path2?q1=a1&q2=a2",
       {{"host", "www.abc.com"},
        {"port", "80"},
        {"scheme", "http"},
        {"path", "/path1/path2"},
        {"query", "q1=a1&q2=a2"},
        {"success", "true"}}},
      {"http://www.abc.com:8080/path1/path2?q1=a1&q2=a2",
       {{"host", "www.abc.com"},
        {"port", "8080"},
        {"scheme", "http"},
        {"path", "/path1/path2"},
        {"query", "q1=a1&q2=a2"},
        {"success", "true"}}},
      {"www.abc.com:8080/path1/path2?q1=a1&q2=a2",
       {{"host", "www.abc.com"},
        {"port", "8080"},
        {"scheme", "http"},
        {"path", "/path1/path2"},
        {"query", "q1=a1&q2=a2"},
        {"success", "true"}}},
      {"http://user:password@www.abc.com:8080/path1/path2?q1=a1&q2=a2",
       {{"host", "www.abc.com"},
        {"port", "8080"},
        {"scheme", "http"},
        {"path", "/path1/path2"},
        {"query", "q1=a1&q2=a2"},
        {"success", "true"}}},
      {"user:password@www.abc.com:8080/path1/path2?q1=a1&q2=a2",
       {{"host", "www.abc.com"},
        {"port", "8080"},
        {"scheme", "http"},
        {"path", "/path1/path2"},
        {"query", "q1=a1&q2=a2"},
        {"success", "true"}}},
      {"https://user@www.abc.com/path1/path2?q1=a1&q2=a2",
       {{"host", "www.abc.com"},
        {"port", "443"},
        {"scheme", "https"},
        {"path", "/path1/path2"},
        {"query", "q1=a1&q2=a2"},
        {"success", "true"}}},
      {"http://www.abc.com/path1@bbb/path2?q1=a1&q2=a2",
       {{"host", "www.abc.com"},
        {"port", "80"},
        {"scheme", "http"},
        {"path", "/path1@bbb/path2"},
        {"query", "q1=a1&q2=a2"},
        {"success", "true"}}},

  };
  for (auto &url_map : urls_map)
  {
    http_common::UrlParser url(url_map.first);
    auto url_properties = url_map.second;
    ASSERT_EQ(BoolToString(url.success_), url_properties["success"]);
    ASSERT_EQ(url.host_, url_properties["host"]);
    ASSERT_EQ(std::to_string(url.port_), url_properties["port"]);
    ASSERT_EQ(url.scheme_, url_properties["scheme"]);
    ASSERT_EQ(url.path_, url_properties["path"]);
    ASSERT_EQ(url.query_, url_properties["query"]);
  }
}

TEST(UrlDecoderTests, BasicTests)
{
  std::map<std::string, std::string> testdata{
      {"Authentication=Basic xxx", "Authentication=Basic xxx"},
      {"Authentication=Basic%20xxx", "Authentication=Basic xxx"},
      {"%C3%B6%C3%A0%C2%A7%C3%96abcd%C3%84",
       "\xc3\xb6\xc3\xa0\xc2\xa7\xc3\x96\x61\x62\x63\x64\xc3\x84"},
      {"%2x", "%2x"},
      {"%20", " "},
      {"text%2", "text%2"},
  };

  for (auto &testsample : testdata)
  {
    ASSERT_EQ(http_common::UrlDecoder::Decode(testsample.first), testsample.second);
    ASSERT_TRUE(http_common::UrlDecoder::Decode(testsample.first) == testsample.second);
  }
}
