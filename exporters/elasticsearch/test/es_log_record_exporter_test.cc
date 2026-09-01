// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/elasticsearch/es_log_record_exporter.h"
#include "opentelemetry/common/timestamp.h"
#include "opentelemetry/exporters/elasticsearch/es_log_recordable.h"
#include "opentelemetry/ext/http/client/http_client.h"
#include "opentelemetry/logs/severity.h"
#include "opentelemetry/nostd/function_ref.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/utility.h"
#include "opentelemetry/sdk/common/exporter_utils.h"
#include "opentelemetry/sdk/instrumentationscope/instrumentation_scope.h"
#include "opentelemetry/sdk/logs/exporter.h"
#include "opentelemetry/sdk/logs/recordable.h"
#include "opentelemetry/sdk/resource/resource.h"

#include <gtest/gtest.h>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include "nlohmann/json.hpp"

namespace
{
namespace http_client = opentelemetry::ext::http::client;

// A response shaped like a successful Elasticsearch bulk reply: the exporter looks for
// `"failed" : 0` in the body (see ElasticsearchLogRecordExporter::Export) in addition to the
// status code before reporting success.
class FakeResponse final : public http_client::Response
{
public:
  FakeResponse()
  {
    static const std::string kSuccessBody = R"({"errors": false, "failed" : 0})";
    body_.assign(kSuccessBody.begin(), kSuccessBody.end());
  }

  const http_client::Body &GetBody() const noexcept override { return body_; }

  bool ForEachHeader(opentelemetry::nostd::function_ref<bool(opentelemetry::nostd::string_view,
                                                             opentelemetry::nostd::string_view)>)
      const noexcept override
  {
    return true;
  }

  bool ForEachHeader(const opentelemetry::nostd::string_view &,
                     opentelemetry::nostd::function_ref<bool(opentelemetry::nostd::string_view,
                                                             opentelemetry::nostd::string_view)>)
      const noexcept override
  {
    return true;
  }

  http_client::StatusCode GetStatusCode() const noexcept override { return 200; }

private:
  http_client::Body body_;
};

// A request that accepts and discards everything set on it: the synchronous export path only
// needs a Request to exist, not to inspect what was written to it.
class FakeRequest final : public http_client::Request
{
public:
  void SetMethod(http_client::Method) noexcept override {}
  void SetUri(opentelemetry::nostd::string_view) noexcept override {}
  void SetSslOptions(const http_client::HttpSslOptions &) noexcept override {}
  void SetBody(http_client::Body &) noexcept override {}
  void AddHeader(opentelemetry::nostd::string_view,
                 opentelemetry::nostd::string_view) noexcept override
  {}
  void ReplaceHeader(opentelemetry::nostd::string_view,
                     opentelemetry::nostd::string_view) noexcept override
  {}
  void SetTimeoutMs(std::chrono::milliseconds) noexcept override {}
  void SetCompression(const http_client::Compression &) noexcept override {}
  void EnableLogging(bool) noexcept override {}
  void SetRetryPolicy(const http_client::RetryPolicy &) noexcept override {}
};

// A session whose SendRequest() answers synchronously with a successful FakeResponse, so the
// exporter's own wait for a response returns immediately without needing a real connection.
class FakeSession final : public http_client::Session
{
public:
  std::shared_ptr<http_client::Request> CreateRequest() noexcept override
  {
    return std::make_shared<FakeRequest>();
  }

  void SendRequest(std::shared_ptr<http_client::EventHandler> handler) noexcept override
  {
    FakeResponse response;
    handler->OnResponse(response);
  }

  bool IsSessionActive() noexcept override { return true; }
  bool CancelSession() noexcept override { return true; }
  bool FinishSession() noexcept override { return true; }
};

class FakeHttpClient final : public http_client::HttpClient
{
public:
  std::shared_ptr<http_client::Session> CreateSession(
      opentelemetry::nostd::string_view) noexcept override
  {
    return std::make_shared<FakeSession>();
  }

  bool CancelAllSessions() noexcept override { return true; }
  bool FinishAllSessions() noexcept override { return true; }
  void SetMaxSessionsPerConnection(std::size_t) noexcept override {}
};

}  // namespace

namespace sdklogs       = opentelemetry::sdk::logs;
namespace logs_api      = opentelemetry::logs;
namespace nostd         = opentelemetry::nostd;
namespace logs_exporter = opentelemetry::exporter::logs;

// Regression test: a log record whose body carries bytes that are not valid UTF-8 used to
// abort the process. ElasticSearchRecordable::WriteValue stores the value as given, and
// Export() previously called nlohmann::json::dump() with its default strict error handler,
// which throws on invalid UTF-8; since Export() is noexcept, that throw became
// std::terminate(). The exporter now tolerates it instead of crashing.
TEST(ElasticsearchLogsExporterTests, ExportingARecordWithInvalidUtf8DoesNotAbort)
{
  logs_exporter::ElasticsearchExporterOptions options;
  auto http_client = std::make_shared<FakeHttpClient>();
  auto exporter    = std::unique_ptr<sdklogs::LogRecordExporter>(
      new logs_exporter::ElasticsearchLogRecordExporter(options, http_client));

  auto record      = exporter->MakeRecordable();
  std::string body = "payload ";
  body += "\xC3\x28";  // a two byte sequence that is not valid UTF-8
  record->SetBody(body);

  auto result = exporter->Export(nostd::span<std::unique_ptr<sdklogs::Recordable>>(&record, 1));

  EXPECT_EQ(result, opentelemetry::sdk::common::ExportResult::kSuccess);
}

TEST(ElasticsearchLogsExporterTests, CustomClientConstructionSucceeds)
{
  logs_exporter::ElasticsearchExporterOptions opts;
  auto exporter = std::unique_ptr<sdklogs::LogRecordExporter>(
      new logs_exporter::ElasticsearchLogRecordExporter(opts));
  ASSERT_NE(exporter, nullptr);
}

// Attempt to write a log to an invalid host/port, test that the Export() returns failure
TEST(DISABLED_ElasticsearchLogsExporterTests, InvalidEndpoint)
{
  // Create invalid connection options for the elasticsearch exporter
  logs_exporter::ElasticsearchExporterOptions options("localhost", -1);

  // Create an elasticsearch exporter
  auto exporter = std::unique_ptr<sdklogs::LogRecordExporter>(
      new logs_exporter::ElasticsearchLogRecordExporter(options));

  // Create a log record and send to the exporter
  auto record = exporter->MakeRecordable();
  auto result = exporter->Export(nostd::span<std::unique_ptr<sdklogs::Recordable>>(&record, 1));

  // Ensure the return value is failure
  ASSERT_EQ(result, opentelemetry::sdk::common::ExportResult::kFailure);
}

// Test that when the exporter is shutdown, any call to Export should return failure
TEST(DISABLED_ElasticsearchLogsExporterTests, Shutdown)
{
  // Create an elasticsearch exporter and immediately shut it down
  auto exporter = std::unique_ptr<sdklogs::LogRecordExporter>(
      new logs_exporter::ElasticsearchLogRecordExporter);
  bool shutdownResult = exporter->Shutdown();
  ASSERT_TRUE(shutdownResult);

  // Write a log to the shutdown exporter
  auto record = exporter->MakeRecordable();
  auto result = exporter->Export(nostd::span<std::unique_ptr<sdklogs::Recordable>>(&record, 1));

  // Ensure the return value is failure
  ASSERT_EQ(result, opentelemetry::sdk::common::ExportResult::kFailure);
}

// Test the elasticsearch recordable object
TEST(DISABLED_ElasticsearchLogsExporterTests, RecordableCreation)
{
  // Create an elasticsearch exporter
  auto exporter = std::unique_ptr<sdklogs::LogRecordExporter>(
      new logs_exporter::ElasticsearchLogRecordExporter);

  // Create a recordable
  auto record = exporter->MakeRecordable();
  record->SetSeverity(logs_api::Severity::kFatal);
  record->SetTimestamp(std::chrono::system_clock::now());
  record->SetBody("Body of the log message");

  // Attributes and resource support different types
  record->SetAttribute("key0", false);
  record->SetAttribute("key1", "1");

  auto resource = opentelemetry::sdk::resource::Resource::Create({{"key2", 2}, {"key3", 3142}});
  record->SetResource(resource);

  exporter->Export(nostd::span<std::unique_ptr<sdklogs::Recordable>>(&record, 1));
}

TEST(ElasticsearchLogRecordableTests, BasicTests)
{
  const auto severity = logs_api::Severity::kFatal;
  const std::array<nostd::string_view, 2> stringlist{
      {nostd::string_view("string1"), nostd::string_view("string2")}};

  const std::int64_t expected_observed_ts = 1732063944999647774LL;
  const std::string expected_timestamp("2024-11-20T00:52:24.999647Z");
  const std::string expected_severity(
      opentelemetry::logs::SeverityNumToText[static_cast<std::size_t>(severity)]);
  const std::string expected_body("Body of the log message");
  const std::string expected_scope_name("scope_name");
  const bool expected_boolean  = false;
  const int expected_int       = 1;
  const double expected_double = 2.0;

  const nlohmann::json expected{
      {"@timestamp", expected_timestamp},
      {"boolean", expected_boolean},
      {"double", expected_double},
      {"ecs", {{"version", "8.11.0"}}},
      {"int", expected_int},
      {"log", {{"level", expected_severity}, {"logger", expected_scope_name}}},
      {"message", expected_body},
      {"observedtimestamp", expected_observed_ts},
      {"stringlist", {stringlist[0], stringlist[1]}}};

  const opentelemetry::common::SystemTimestamp now{std::chrono::nanoseconds(expected_observed_ts)};

  const auto scope =
      opentelemetry::sdk::instrumentationscope::InstrumentationScope::Create(expected_scope_name);

  opentelemetry::exporter::logs::ElasticSearchRecordable recordable;
  recordable.SetTimestamp(now);
  recordable.SetObservedTimestamp(now);
  recordable.SetSeverity(severity);
  recordable.SetBody(expected_body);
  recordable.SetInstrumentationScope(*scope);

  recordable.SetAttribute("boolean", expected_boolean);
  recordable.SetAttribute("int", expected_int);
  recordable.SetAttribute("double", expected_double);
  recordable.SetAttribute("stringlist", stringlist);

  const auto actual = recordable.GetJSON();

  EXPECT_EQ(actual, expected);
}
