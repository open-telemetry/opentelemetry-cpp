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
#include <functional>
#include <future>
#include <initializer_list>
#include <string>
#include <thread>
#include <utility>
#include "nlohmann/json.hpp"

namespace
{
namespace http_client = opentelemetry::ext::http::client;

// A response shaped like a successful Elasticsearch bulk reply: the exporter looks for
// `"failed" : 0` in the body (see ElasticsearchLogRecordExporter::Export) in addition to the
// status code before reporting success.
constexpr const char *kDefaultAcceptedBody = R"({"errors": false, "failed" : 0})";

class FakeResponse final : public http_client::Response
{
public:
  explicit FakeResponse(http_client::StatusCode status = 200,
                        const std::string &body        = kDefaultAcceptedBody)
      : status_(status), body_(body.begin(), body.end())
  {}

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

  http_client::StatusCode GetStatusCode() const noexcept override { return status_; }

private:
  http_client::StatusCode status_;
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

// What the client does with a request, called from inside SendRequest() so the exporter's own
// wait returns without needing a connection. The default answers once, successfully, which is
// what a case wants when the response is not the thing under test.
using EventScript = std::function<void(http_client::EventHandler &)>;

EventScript AnswerSuccessfully()
{
  return [](http_client::EventHandler &handler) {
    FakeResponse response;
    handler.OnResponse(response);
  };
}

class FakeSession final : public http_client::Session
{
public:
  explicit FakeSession(EventScript script = AnswerSuccessfully()) : script_(std::move(script)) {}

  std::shared_ptr<http_client::Request> CreateRequest() noexcept override
  {
    return std::make_shared<FakeRequest>();
  }

  void SendRequest(std::shared_ptr<http_client::EventHandler> handler) noexcept override
  {
    script_(*handler);
  }

  bool IsSessionActive() noexcept override { return true; }
  bool CancelSession() noexcept override { return true; }
  bool FinishSession() noexcept override { return true; }

private:
  EventScript script_;
};

class FakeHttpClient final : public http_client::HttpClient
{
public:
  explicit FakeHttpClient(EventScript script = AnswerSuccessfully()) : script_(std::move(script)) {}

  std::shared_ptr<http_client::Session> CreateSession(
      opentelemetry::nostd::string_view) noexcept override
  {
    return std::make_shared<FakeSession>(script_);
  }

  bool CancelAllSessions() noexcept override { return true; }
  bool FinishAllSessions() noexcept override { return true; }
  void SetMaxSessionsPerConnection(std::size_t) noexcept override {}

private:
  EventScript script_;
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

// Synchronous completion path. The fake client scripts its callbacks from inside SendRequest(),
// which runs before the exporter reaches waitForResponse(), so every case here also covers a
// completion recorded before the wait starts.
namespace
{
namespace http_client = opentelemetry::ext::http::client;

// Accepted by the substring check, by a top level "errors": false parse, and by one
// acknowledged operation result carrying a 2xx status, so these cases keep meaning the
// same thing whichever success check is in place.
constexpr const char *kAcceptedBody =
    R"({"took":30,"errors":false,"items":[{"index":{"_index":"logs","status":201,"_shards":{"failed" : 0}}}]})";

// Keeps the handler and returns, so the export reaches its wait with nothing recorded and only a
// notification can end it.
class DeferredSession : public http_client::Session
{
public:
  explicit DeferredSession(std::promise<std::shared_ptr<http_client::EventHandler>> *arrived)
      : arrived_(arrived)
  {}

  std::shared_ptr<http_client::Request> CreateRequest() noexcept override
  {
    return std::make_shared<FakeRequest>();
  }
  // The handler travels in the promise rather than beside it. A waiter that times out has not
  // observed the promise becoming ready and so is not synchronized with this thread, which would
  // make a handler read on that path a race with this write.
  void SendRequest(std::shared_ptr<http_client::EventHandler> handler) noexcept override
  {
    arrived_->set_value(std::move(handler));
  }
  bool IsSessionActive() noexcept override { return true; }
  bool CancelSession() noexcept override { return true; }
  bool FinishSession() noexcept override { return true; }

private:
  std::promise<std::shared_ptr<http_client::EventHandler>> *arrived_;
};

class DeferredHttpClient : public http_client::HttpClient
{
public:
  explicit DeferredHttpClient(std::promise<std::shared_ptr<http_client::EventHandler>> *arrived)
      : arrived_(arrived)
  {}

  std::shared_ptr<http_client::Session> CreateSession(nostd::string_view) noexcept override
  {
    return std::make_shared<DeferredSession>(arrived_);
  }
  bool CancelAllSessions() noexcept override { return true; }
  bool FinishAllSessions() noexcept override { return true; }
  void SetMaxSessionsPerConnection(std::size_t) noexcept override {}

private:
  std::promise<std::shared_ptr<http_client::EventHandler>> *arrived_;
};

opentelemetry::sdk::common::ExportResult ExportWith(EventScript script)
{
  auto client = std::make_shared<FakeHttpClient>(std::move(script));
  logs_exporter::ElasticsearchExporterOptions options;
  logs_exporter::ElasticsearchLogRecordExporter exporter(options, client);
  auto record = exporter.MakeRecordable();
  return exporter.Export(nostd::span<std::unique_ptr<sdklogs::Recordable>>(&record, 1));
}
}  // namespace

// The synchronous wait exists only when the exporter is built without async export, so these cases
// skip rather than compile out: gtest_add_tests reads the source, and a case that disappeared from
// the binary would still be registered with CTest. The skip goes in SetUp rather than at the top of
// each body, because GTEST_SKIP returns and leaves the rest of the body unreachable, which MSVC
// reports as C4702 and the maintainer mode jobs turn into an error.
namespace
{
class ElasticsearchLogsExporterSyncTests : public ::testing::Test
{
protected:
  void SetUp() override
  {
#ifdef ENABLE_ASYNC_EXPORT
    GTEST_SKIP() << "Export() returns without waiting when async export is enabled";
#endif
  }
};
}  // namespace

namespace
{
// Runs one export on another thread against a session that keeps its handler, and hands back the
// handler once the exporter has reached it. Everything is held by shared_ptr and the thread is
// detached, because waitForResponse has no deadline: if a wake-up stops working the export never
// returns, and joining it would hang the binary rather than fail the case.
struct ParkedExport
{
  std::shared_ptr<DeferredHttpClient> client;
  std::shared_ptr<logs_exporter::ElasticsearchLogRecordExporter> exporter;
  std::shared_ptr<http_client::EventHandler> handler;
  std::promise<std::shared_ptr<http_client::EventHandler>> arrived;
  std::promise<opentelemetry::sdk::common::ExportResult> done;
  std::future<opentelemetry::sdk::common::ExportResult> finished;
};

// Answers with nullptr rather than a half prepared handle when a precondition does not hold. Each
// step below is something a case needs before it can mean anything, and carrying on past one of
// them reads state the other thread is still writing.
std::shared_ptr<ParkedExport> StartParkedExport()
{
  auto parked    = std::make_shared<ParkedExport>();
  parked->client = std::make_shared<DeferredHttpClient>(&parked->arrived);

  logs_exporter::ElasticsearchExporterOptions options;
  parked->exporter =
      std::make_shared<logs_exporter::ElasticsearchLogRecordExporter>(options, parked->client);

  auto reached  = parked->arrived.get_future();
  auto finished = parked->done.get_future();

  std::thread([parked] {
    auto record = parked->exporter->MakeRecordable();
    parked->done.set_value(
        parked->exporter->Export(nostd::span<std::unique_ptr<sdklogs::Recordable>>(&record, 1)));
  }).detach();

  if (std::future_status::ready != reached.wait_for(std::chrono::seconds{5}))
  {
    ADD_FAILURE() << "the exporter never handed off the request handler";
    return nullptr;
  }

  parked->handler = reached.get();
  if (!parked->handler)
  {
    ADD_FAILURE() << "the session was handed a null handler";
    return nullptr;
  }

  if (std::future_status::timeout != finished.wait_for(std::chrono::milliseconds{100}))
  {
    ADD_FAILURE() << "the export returned before any callback was delivered";
    return nullptr;
  }

  parked->finished = std::move(finished);
  return parked;
}
}  // namespace

// A terminal error that arrives after the handler has been handed off, while the export is still
// running, has to end the wait, which is the half the notification is responsible for. Without
// these two, removing cv_.notify_all() keeps this file green.
//
// The handoff is what they hold, not the parking: the session publishes the handler before
// SendRequest() returns, so the export need not have reached cv_.wait() when the callback is
// delivered. Holding that would want a wait entry seam in production code, which is not worth the
// API it would add.
TEST_F(ElasticsearchLogsExporterSyncTests, AReadErrorAfterTheHandoffEndsTheExport)
{
  auto parked = StartParkedExport();
  ASSERT_NE(nullptr, parked);

  parked->handler->OnEvent(http_client::SessionState::ReadError, "");

  ASSERT_EQ(std::future_status::ready, parked->finished.wait_for(std::chrono::seconds{10}))
      << "the read error never woke the export";
  EXPECT_EQ(opentelemetry::sdk::common::ExportResult::kFailure, parked->finished.get());
}

// The same for the success half, which also holds that the wait is a wait: a waitForResponse that
// only read the current state would answer before this response arrives.
TEST_F(ElasticsearchLogsExporterSyncTests, AResponseAfterTheHandoffEndsTheExport)
{
  auto parked = StartParkedExport();
  ASSERT_NE(nullptr, parked);

  FakeResponse response(200, kAcceptedBody);
  parked->handler->OnResponse(response);

  ASSERT_EQ(std::future_status::ready, parked->finished.wait_for(std::chrono::seconds{10}))
      << "the response never woke the export";
  EXPECT_EQ(opentelemetry::sdk::common::ExportResult::kSuccess, parked->finished.get());
}

TEST_F(ElasticsearchLogsExporterSyncTests, ResponseRecordedBeforeTheWaitIsStillSeen)
{
  const auto result = ExportWith([](http_client::EventHandler &handler) {
    FakeResponse response(200, kAcceptedBody);
    handler.OnResponse(response);
  });
  EXPECT_EQ(result, opentelemetry::sdk::common::ExportResult::kSuccess);
}

TEST_F(ElasticsearchLogsExporterSyncTests, ReadErrorEndsTheWait)
{
  const auto result = ExportWith([](http_client::EventHandler &handler) {
    handler.OnEvent(http_client::SessionState::ReadError, "");
  });
  EXPECT_EQ(result, opentelemetry::sdk::common::ExportResult::kFailure);
}

// The whole contract in one place. Every state that ends a session has to leave a result behind,
// otherwise a client that emits it last strands the wait.
//
// A regression here surfaces as a CTest timeout rather than a failed assertion, because a state
// that stops being terminal leaves Export() waiting with nothing left to wake it.
TEST_F(ElasticsearchLogsExporterSyncTests, EveryTerminalStateEndsTheWaitInFailure)
{
  const http_client::SessionState terminal[] = {
      http_client::SessionState::CreateFailed, http_client::SessionState::ConnectFailed,
      http_client::SessionState::SendFailed,   http_client::SessionState::SSLHandshakeFailed,
      http_client::SessionState::TimedOut,     http_client::SessionState::NetworkError,
      http_client::SessionState::Cancelled,    http_client::SessionState::ReadError,
      http_client::SessionState::WriteError,   http_client::SessionState::Destroyed};

  for (const auto state : terminal)
  {
    SCOPED_TRACE(static_cast<int>(state));
    const auto result =
        ExportWith([state](http_client::EventHandler &handler) { handler.OnEvent(state, ""); });
    EXPECT_EQ(result, opentelemetry::sdk::common::ExportResult::kFailure);
  }
}

// The other side of the same contract: a state that only reports progress must not complete the
// export on its own, or a response that arrives afterwards is never consulted.
TEST_F(ElasticsearchLogsExporterSyncTests, ProgressStatesDoNotDecideTheResult)
{
  const auto result = ExportWith([](http_client::EventHandler &handler) {
    handler.OnEvent(http_client::SessionState::Created, "");
    handler.OnEvent(http_client::SessionState::Connecting, "");
    handler.OnEvent(http_client::SessionState::Connected, "");
    handler.OnEvent(http_client::SessionState::Sending, "");
    handler.OnEvent(http_client::SessionState::Response, "");
    FakeResponse response(200, kAcceptedBody);
    handler.OnResponse(response);
  });
  EXPECT_EQ(result, opentelemetry::sdk::common::ExportResult::kSuccess);
}

TEST_F(ElasticsearchLogsExporterSyncTests, WriteErrorEndsTheWait)
{
  const auto result = ExportWith([](http_client::EventHandler &handler) {
    handler.OnEvent(http_client::SessionState::WriteError, "");
  });
  EXPECT_EQ(result, opentelemetry::sdk::common::ExportResult::kFailure);
}

TEST_F(ElasticsearchLogsExporterSyncTests, SessionDestroyedWhilePendingEndsTheWait)
{
  const auto result = ExportWith([](http_client::EventHandler &handler) {
    handler.OnEvent(http_client::SessionState::Destroyed, "");
  });
  EXPECT_EQ(result, opentelemetry::sdk::common::ExportResult::kFailure);
}

// The first outcome recorded is the one reported, so tearing the session down after a response has
// arrived does not turn a successful export into a failure.
TEST_F(ElasticsearchLogsExporterSyncTests, SessionDestroyedAfterAResponseKeepsTheSuccess)
{
  const auto result = ExportWith([](http_client::EventHandler &handler) {
    FakeResponse response(200, kAcceptedBody);
    handler.OnResponse(response);
    handler.OnEvent(http_client::SessionState::Destroyed, "");
  });
  EXPECT_EQ(result, opentelemetry::sdk::common::ExportResult::kSuccess);
}

// The first outcome still wins once these two are terminal: reaching an I/O error after a
// response has been recorded must not turn a successful export into a failure.
TEST_F(ElasticsearchLogsExporterSyncTests, IoErrorAfterAResponseKeepsTheSuccess)
{
  for (const auto state :
       {http_client::SessionState::ReadError, http_client::SessionState::WriteError})
  {
    const auto result = ExportWith([state](http_client::EventHandler &handler) {
      FakeResponse response(200, kAcceptedBody);
      handler.OnResponse(response);
      handler.OnEvent(state, "");
    });
    EXPECT_EQ(result, opentelemetry::sdk::common::ExportResult::kSuccess);
  }
}

// The mirror image: the I/O error is recorded first, so a response arriving afterwards does
// not rescue the export.
TEST_F(ElasticsearchLogsExporterSyncTests, IoErrorBeforeAResponseKeepsTheFailure)
{
  for (const auto state :
       {http_client::SessionState::ReadError, http_client::SessionState::WriteError})
  {
    SCOPED_TRACE(static_cast<int>(state));
    const auto result = ExportWith([state](http_client::EventHandler &handler) {
      handler.OnEvent(state, "");
      FakeResponse response(200, kAcceptedBody);
      handler.OnResponse(response);
    });
    EXPECT_EQ(result, opentelemetry::sdk::common::ExportResult::kFailure);
  }
}
