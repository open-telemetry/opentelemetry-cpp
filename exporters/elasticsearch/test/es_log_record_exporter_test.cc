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
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <string>
#include <thread>
#include <utility>
#include <vector>
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

// A client that owns what it creates, the way the curl client does: CreateSession() keeps a
// reference in the client and only FinishSession() gives it back. CreateSession() also waits on
// a gate, so a case can run Shutdown() while an export sits between its two shutdown checks.
class RetainingSession final : public http_client::Session
{
public:
  std::shared_ptr<http_client::Request> CreateRequest() noexcept override
  {
    return std::make_shared<FakeRequest>();
  }

  void SendRequest(std::shared_ptr<http_client::EventHandler>) noexcept override { sent_ = true; }

  bool IsSessionActive() noexcept override { return finish_calls_ == 0; }
  bool CancelSession() noexcept override { return true; }
  // Counted rather than flagged: handing a session back twice is as wrong as not at all, and a
  // flag reads the same either way.
  bool FinishSession() noexcept override
  {
    ++finish_calls_;
    return true;
  }

  bool sent_                = false;
  std::size_t finish_calls_ = 0;
};

class RetainingHttpClient final : public http_client::HttpClient
{
public:
  std::shared_ptr<http_client::Session> CreateSession(
      opentelemetry::nostd::string_view) noexcept override
  {
    {
      std::unique_lock<std::mutex> lock{gate_m_};
      entered_ = true;
      gate_cv_.notify_all();
      gate_cv_.wait(lock, [this] { return released_; });
    }
    auto session = std::make_shared<RetainingSession>();
    std::lock_guard<std::mutex> lock{sessions_m_};
    sessions_.push_back(session);
    return session;
  }

  bool CancelAllSessions() noexcept override { return true; }
  bool FinishAllSessions() noexcept override { return true; }
  void SetMaxSessionsPerConnection(std::size_t) noexcept override {}

  void WaitUntilCreating()
  {
    std::unique_lock<std::mutex> lock{gate_m_};
    gate_cv_.wait(lock, [this] { return entered_; });
  }

  void Release()
  {
    {
      std::lock_guard<std::mutex> lock{gate_m_};
      released_ = true;
    }
    gate_cv_.notify_all();
  }

  std::size_t Sent()
  {
    std::lock_guard<std::mutex> lock{sessions_m_};
    std::size_t n = 0;
    for (const auto &s : sessions_)
    {
      n += s->sent_ ? 1 : 0;
    }
    return n;
  }

  std::size_t FinishCalls()
  {
    std::lock_guard<std::mutex> lock{sessions_m_};
    std::size_t n = 0;
    for (const auto &s : sessions_)
    {
      n += s->finish_calls_;
    }
    return n;
  }

  // What the client is still holding that nobody handed back.
  std::size_t Retained()
  {
    std::lock_guard<std::mutex> lock{sessions_m_};
    std::size_t n = 0;
    for (const auto &s : sessions_)
    {
      n += s->finish_calls_ == 0 ? 1 : 0;
    }
    return n;
  }

private:
  std::mutex gate_m_;
  std::condition_variable gate_cv_;
  bool entered_  = false;
  bool released_ = false;

  std::mutex sessions_m_;
  std::vector<std::shared_ptr<RetainingSession>> sessions_;
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

// A session that parks the handler it is given, in the test case rather than in the session
// itself, without ever calling back into it. The handler stays alive (so the export it
// represents keeps counting as outstanding) without creating a Session/AsyncResponseHandler
// reference cycle: AsyncResponseHandler holds a shared_ptr to its session, so a session that
// held the handler back would keep both alive for the exporter's own lifetime, which a leak
// sanitizer would flag.
//
// Only meaningful under ENABLE_ASYNC_EXPORT: that is the only build where ForceFlush() waits on
// anything at all (see ElasticsearchLogRecordExporter::ForceFlush).
#ifdef ENABLE_ASYNC_EXPORT
class HoldingSession final : public http_client::Session
{
public:
  explicit HoldingSession(std::shared_ptr<http_client::EventHandler> *parked) : parked_(parked) {}

  std::shared_ptr<http_client::Request> CreateRequest() noexcept override
  {
    return std::make_shared<FakeRequest>();
  }

  // Parked where the case can see it, not in this session: the handler owns its session, so a
  // session that owned the handler back would keep the pair alive.
  void SendRequest(std::shared_ptr<http_client::EventHandler> handler) noexcept override
  {
    *parked_ = std::move(handler);
  }

  bool IsSessionActive() noexcept override { return true; }
  bool CancelSession() noexcept override { return true; }
  bool FinishSession() noexcept override { return true; }

private:
  std::shared_ptr<http_client::EventHandler> *parked_;
};

class HoldingHttpClient final : public http_client::HttpClient
{
public:
  explicit HoldingHttpClient(std::shared_ptr<http_client::EventHandler> *parked) : parked_(parked)
  {}

  std::shared_ptr<http_client::Session> CreateSession(
      opentelemetry::nostd::string_view) noexcept override
  {
    return std::make_shared<HoldingSession>(parked_);
  }

  bool CancelAllSessions() noexcept override { return true; }
  bool FinishAllSessions() noexcept override { return true; }
  void SetMaxSessionsPerConnection(std::size_t) noexcept override {}

private:
  std::shared_ptr<http_client::EventHandler> *parked_;
};
#endif  // ENABLE_ASYNC_EXPORT

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

// Regression test: Shutdown() used to ignore its timeout parameter entirely and always return
// true, whether or not anything had actually flushed. It should now report what flushing (via
// ForceFlush) actually found. With FakeHttpClient, every export completes synchronously inside
// Export() itself, so there is nothing left pending by the time Shutdown() runs.
TEST(ElasticsearchLogsExporterTests, ShutdownReportsFlushCompletion)
{
  logs_exporter::ElasticsearchExporterOptions options;
  auto http_client = std::make_shared<FakeHttpClient>();
  auto exporter    = std::unique_ptr<sdklogs::LogRecordExporter>(
      new logs_exporter::ElasticsearchLogRecordExporter(options, http_client));

  auto record = exporter->MakeRecordable();
  record->SetBody("shutdown regression test");
  auto export_result =
      exporter->Export(nostd::span<std::unique_ptr<sdklogs::Recordable>>(&record, 1));
  ASSERT_EQ(export_result, opentelemetry::sdk::common::ExportResult::kSuccess);

  EXPECT_TRUE(exporter->Shutdown(std::chrono::seconds(1)));
}

// Regression test: ForceFlush()'s wait loop always waited the full response_timeout_ on its
// condition variable, regardless of how much of the caller's own timeout was left. With an
// export still outstanding and nothing to notify the condition variable, Shutdown(1us) waited
// the full response_timeout_ (30s by default) instead of returning after about 1us. The wait
// is now clamped to whatever remains of the caller's deadline, matching the pattern already
// used by the OTLP HTTP client's ForceFlush().
#ifdef ENABLE_ASYNC_EXPORT
TEST(ElasticsearchLogsExporterTests, ShutdownClampsWaitToCallerTimeoutWhenExportIsOutstanding)
{
  // Declared first so it outlives the client and the session that point at it.
  std::shared_ptr<http_client::EventHandler> parked;
  logs_exporter::ElasticsearchExporterOptions options;
  auto http_client = std::make_shared<HoldingHttpClient>(&parked);
  auto exporter    = std::unique_ptr<sdklogs::LogRecordExporter>(
      new logs_exporter::ElasticsearchLogRecordExporter(options, http_client));

  auto record = exporter->MakeRecordable();
  record->SetBody("this export never completes");
  auto export_result =
      exporter->Export(nostd::span<std::unique_ptr<sdklogs::Recordable>>(&record, 1));
  ASSERT_EQ(export_result, opentelemetry::sdk::common::ExportResult::kSuccess);

  auto start   = std::chrono::steady_clock::now();
  bool result  = exporter->Shutdown(std::chrono::microseconds(1));
  auto elapsed = std::chrono::steady_clock::now() - start;

  EXPECT_FALSE(result);
  EXPECT_LT(elapsed, std::chrono::seconds(1));

  // Let the export finish now that the assertions are done.
  parked.reset();
}
#endif  // ENABLE_ASYNC_EXPORT

// Regression test: once Shutdown() has been called, any later Export() must fail rather than
// silently trying to register a session against an exporter that is already tearing down.
// The case is only meaningful where the second shutdown check exists, but it is registered in
// both builds: gtest_add_tests reads the source, so a case behind #ifdef is still handed to CTest
// in the build that does not compile it and reports a pass it never ran.
TEST(ElasticsearchLogsExporterTests, ARejectedExportHandsItsSessionBack)
{
#ifndef ENABLE_ASYNC_EXPORT
  GTEST_SKIP() << "the shutdown re-check this covers is compiled only with async export";
#else
  auto client = std::make_shared<RetainingHttpClient>();
  logs_exporter::ElasticsearchExporterOptions options;
  auto exporter = std::unique_ptr<sdklogs::LogRecordExporter>(
      new logs_exporter::ElasticsearchLogRecordExporter(options, client));

  auto record = exporter->MakeRecordable();
  record->SetBody("a record the exporter will refuse");
  std::array<std::unique_ptr<sdklogs::Recordable>, 1> batch = {std::move(record)};

  auto result = opentelemetry::sdk::common::ExportResult::kSuccess;
  std::thread exporting([&] {
    result = exporter->Export(
        nostd::span<std::unique_ptr<sdklogs::Recordable>>(batch.data(), batch.size()));
  });

  // Shutdown lands while the export is inside CreateSession, so it sees no registered session
  // and returns, and the export then meets the second check on its way back.
  client->WaitUntilCreating();
  exporter->Shutdown();
  client->Release();
  exporting.join();

  EXPECT_EQ(opentelemetry::sdk::common::ExportResult::kFailure, result);
  EXPECT_EQ(0u, client->Sent()) << "a rejected export must not send";
  EXPECT_EQ(0u, client->Retained())
      << "the rejected session is still held by the client, so nothing will call FinishSession";
  EXPECT_EQ(1u, client->FinishCalls()) << "handed back exactly once, not twice";
#endif
}

TEST(ElasticsearchLogsExporterTests, ExportAfterShutdownFails)
{
  logs_exporter::ElasticsearchExporterOptions options;
  auto http_client = std::make_shared<FakeHttpClient>();
  auto exporter    = std::unique_ptr<sdklogs::LogRecordExporter>(
      new logs_exporter::ElasticsearchLogRecordExporter(options, http_client));

  ASSERT_TRUE(exporter->Shutdown(std::chrono::seconds(1)));

  auto record = exporter->MakeRecordable();
  auto result = exporter->Export(nostd::span<std::unique_ptr<sdklogs::Recordable>>(&record, 1));

  EXPECT_EQ(result, opentelemetry::sdk::common::ExportResult::kFailure);
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
