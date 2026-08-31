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
#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/instrumentationscope/instrumentation_scope.h"
#include "opentelemetry/sdk/logs/exporter.h"
#include "opentelemetry/sdk/logs/recordable.h"
#include "opentelemetry/sdk/resource/resource.h"

#include <gtest/gtest.h>
#include <array>
#include <atomic>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <nlohmann/detail/json_ref.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <thread>
#include <utility>
#include <vector>
#include "nlohmann/json.hpp"

namespace sdklogs       = opentelemetry::sdk::logs;
namespace logs_api      = opentelemetry::logs;
namespace nostd         = opentelemetry::nostd;
namespace logs_exporter = opentelemetry::exporter::logs;
namespace internal_log  = opentelemetry::sdk::common::internal_log;

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

// ---------------------------------------------------------------------------
// ForceFlush deadline.
// ---------------------------------------------------------------------------
namespace
{
namespace http_client = opentelemetry::ext::http::client;

// Accepted by the substring check, by a top level "errors": false parse, and by one
// acknowledged operation result carrying a 2xx status, so these cases keep meaning the
// same thing whichever success check is in place.
constexpr const char *kAcceptedBody =
    R"({"took":30,"errors":false,"items":[{"index":{"status":201,"_shards":{"failed" : 0}}}]})";

class FakeResponse : public http_client::Response
{
public:
  FakeResponse(http_client::StatusCode status, const std::string &body)
      : status_(status), body_(body.begin(), body.end())
  {}
  const http_client::Body &GetBody() const noexcept override { return body_; }
  bool ForEachHeader(
      nostd::function_ref<bool(nostd::string_view, nostd::string_view)>) const noexcept override
  {
    return true;
  }
  bool ForEachHeader(
      const nostd::string_view &,
      nostd::function_ref<bool(nostd::string_view, nostd::string_view)>) const noexcept override
  {
    return true;
  }
  http_client::StatusCode GetStatusCode() const noexcept override { return status_; }

private:
  http_client::StatusCode status_;
  http_client::Body body_;
};

class FakeRequest : public http_client::Request
{
public:
  void SetMethod(http_client::Method) noexcept override {}
  void SetUri(nostd::string_view) noexcept override {}
  void SetSslOptions(const http_client::HttpSslOptions &) noexcept override {}
  void SetBody(http_client::Body &) noexcept override {}
  void AddHeader(nostd::string_view, nostd::string_view) noexcept override {}
  void ReplaceHeader(nostd::string_view, nostd::string_view) noexcept override {}
  void SetTimeoutMs(std::chrono::milliseconds) noexcept override {}
  void SetCompression(const http_client::Compression &) noexcept override {}
  void EnableLogging(bool) noexcept override {}
  void SetRetryPolicy(const http_client::RetryPolicy &) noexcept override {}
};

using EventScript = std::function<void(const std::shared_ptr<http_client::EventHandler> &)>;

class FakeSession : public http_client::Session
{
public:
  explicit FakeSession(EventScript script) : script_(std::move(script)) {}
  std::shared_ptr<http_client::Request> CreateRequest() noexcept override
  {
    return std::make_shared<FakeRequest>();
  }
  void SendRequest(std::shared_ptr<http_client::EventHandler> handler) noexcept override
  {
    script_(handler);
  }
  bool IsSessionActive() noexcept override { return false; }
  bool CancelSession() noexcept override { return true; }
  bool FinishSession() noexcept override { return true; }

private:
  EventScript script_;
};

class FakeHttpClient : public http_client::HttpClient
{
public:
  explicit FakeHttpClient(EventScript script) : script_(std::move(script)) {}
  std::shared_ptr<http_client::Session> CreateSession(nostd::string_view) noexcept override
  {
    if (on_create_session)
    {
      on_create_session();
    }
    return std::make_shared<FakeSession>(script_);
  }

  // Runs inside Export(), after the records have been handed over and before the request exists.
  std::function<void()> on_create_session;

  // Runs inside Shutdown(). A real client answers its outstanding sessions here, so a case that
  // needs a flush to be woken by the shutdown rather than by its own bound sets this; one that
  // leaves it unset is a client that goes quiet instead, which is the case the bound exists for.
  std::function<void()> on_cancel_all;

  bool CancelAllSessions() noexcept override
  {
    if (on_cancel_all)
    {
      on_cancel_all();
    }
    return true;
  }
  bool FinishAllSessions() noexcept override { return true; }
  void SetMaxSessionsPerConnection(std::size_t) noexcept override {}

private:
  EventScript script_;
};

}  // namespace

// ---------------------------------------------------------------------------
// A fake HTTP client, and an exporter built on it, shared by the cases below.
// ---------------------------------------------------------------------------
namespace
{
// A response timeout short enough that a wait bounded by it instead of by the caller's deadline
// is visible in the elapsed time.
constexpr int kShortResponseTimeoutSeconds = 2;

struct FlushFixture
{
  std::shared_ptr<FakeHttpClient> client;
  std::unique_ptr<logs_exporter::ElasticsearchLogRecordExporter> exporter;
};

FlushFixture MakeExporter(EventScript script)
{
  FlushFixture fixture;
  fixture.client = std::make_shared<FakeHttpClient>(std::move(script));
  logs_exporter::ElasticsearchExporterOptions options;
  options.response_timeout_ = kShortResponseTimeoutSeconds;
  fixture.exporter          = std::unique_ptr<logs_exporter::ElasticsearchLogRecordExporter>(
      new logs_exporter::ElasticsearchLogRecordExporter(options, fixture.client));
  return fixture;
}

void ExportOnce(logs_exporter::ElasticsearchLogRecordExporter &exporter)
{
  auto record = exporter.MakeRecordable();
  exporter.Export(nostd::span<std::unique_ptr<sdklogs::Recordable>>(&record, 1));
}
}  // namespace

// ---------------------------------------------------------------------------
// Exactly-once accounting for the async handler, which exists only in an async build, so
// these cases skip there rather than compile out.
// ---------------------------------------------------------------------------

namespace
{
// The completion callback logs one line per invocation and names the verdict in it, so these
// count the callback and say which result it carried.
//
// Session tracking cannot stand in for this: ids are erased, and erasing one that has already gone
// is a no-op, so ForceFlush() reports the same thing whether the callback ran once or three times.
class CompletionCountingLogHandler : public internal_log::LogHandler
{
public:
  void Handle(internal_log::LogLevel /* level */,
              const char * /* file */,
              int /* line */,
              const char *msg,
              const opentelemetry::sdk::common::AttributeMap & /* attributes */) noexcept override
  {
    if (msg == nullptr)
    {
      return;
    }
    lines_.fetch_add(1, std::memory_order_relaxed);

    const std::string text(msg);
    if (text.find("log record(s) success") != std::string::npos)
    {
      successes_.fetch_add(1, std::memory_order_relaxed);
    }
    else if (text.find("log record(s) error") != std::string::npos)
    {
      failures_.fetch_add(1, std::memory_order_relaxed);
    }
  }

  int successes() const noexcept { return successes_.load(std::memory_order_relaxed); }
  int failures() const noexcept { return failures_.load(std::memory_order_relaxed); }
  int completions() const noexcept { return successes() + failures(); }

  // Everything the handler was given, not only the completions. What a session says on its way to
  // an outcome is as much a part of the contract as what it says at the end of one.
  int lines() const noexcept { return lines_.load(std::memory_order_relaxed); }

private:
  std::atomic<int> successes_{0};
  std::atomic<int> failures_{0};
  std::atomic<int> lines_{0};
};

class ElasticsearchAsyncCompletionTests : public ::testing::Test
{
protected:
  void SetUp() override
  {
#if !defined(ENABLE_ASYNC_EXPORT)
    GTEST_SKIP() << "the async handler does not exist when async export is disabled";
#elif OTEL_INTERNAL_LOG_LEVEL < OTEL_INTERNAL_LOG_LEVEL_DEBUG
    GTEST_SKIP() << "the success half of the completion callback is compiled out below debug level";
#else
    // One skip point, because GTEST_SKIP returns and a second one after it would leave the rest of
    // this body unreachable, which MSVC reports as C4702 under maintainer mode.
    previous_handler_ = internal_log::GlobalLogHandler::GetLogHandler();
    handler_ = nostd::shared_ptr<internal_log::LogHandler>(new CompletionCountingLogHandler());
    internal_log::GlobalLogHandler::SetLogHandler(handler_);
    previous_level_ = internal_log::GlobalLogHandler::GetLogLevel();
    internal_log::GlobalLogHandler::SetLogLevel(internal_log::LogLevel::Debug);
#endif
  }

  void TearDown() override
  {
    if (handler_)
    {
      internal_log::GlobalLogHandler::SetLogLevel(previous_level_);
      internal_log::GlobalLogHandler::SetLogHandler(previous_handler_);
    }
  }

  const CompletionCountingLogHandler &Counter() const
  {
    return *static_cast<CompletionCountingLogHandler *>(handler_.get());
  }

  int Completions() const { return Counter().completions(); }
  int Lines() const { return Counter().lines(); }

  nostd::shared_ptr<internal_log::LogHandler> handler_;
  nostd::shared_ptr<internal_log::LogHandler> previous_handler_;
  internal_log::LogLevel previous_level_ = internal_log::LogLevel::Warning;
};
}  // namespace
// The orderings a real session can produce, each of which reported twice before the guard.
TEST_F(ElasticsearchAsyncCompletionTests, TerminalOrderingsReportExactlyOnce)
{
  using State = http_client::SessionState;
  struct Case
  {
    const char *name;
    State first;
    State second;
  };
  const Case cases[] = {
      {"connect then create", State::ConnectFailed, State::CreateFailed},
      {"read error then destroyed", State::ReadError, State::Destroyed},
      {"write error then destroyed", State::WriteError, State::Destroyed},
      {"timed out then network error", State::TimedOut, State::NetworkError},
      {"cancelled then destroyed", State::Cancelled, State::Destroyed},
  };

  for (const auto &test_case : cases)
  {
    SCOPED_TRACE(test_case.name);
    std::vector<std::shared_ptr<http_client::EventHandler>> kept;
    auto fixture = MakeExporter(
        [&kept, &test_case](const std::shared_ptr<http_client::EventHandler> &handler) {
          kept.push_back(handler);
          handler->OnEvent(test_case.first, "");
          handler->OnEvent(test_case.second, "");
        });

    const int before = Completions();
    ExportOnce(*fixture.exporter);
    EXPECT_EQ(Completions() - before, 1);

    // The handler is still alive at the check above, and its destructor reports when nothing
    // else has. Letting it go here is what makes the two together exactly one rather than the
    // callback alone.
    kept.clear();
    EXPECT_EQ(Completions() - before, 1) << "destroying the handler reported a second time";
  }
}

// A response decides the outcome, and a teardown event arriving after it must not report again.
// The other order is the case below, because the first verdict is the one that has to survive
// either way round.
TEST_F(ElasticsearchAsyncCompletionTests, AResponseAndATeardownEventReportOnce)
{
  for (const auto state :
       {http_client::SessionState::Destroyed, http_client::SessionState::Cancelled,
        http_client::SessionState::TimedOut})
  {
    SCOPED_TRACE(static_cast<int>(state));
    std::vector<std::shared_ptr<http_client::EventHandler>> kept;
    auto fixture =
        MakeExporter([&kept, state](const std::shared_ptr<http_client::EventHandler> &handler) {
          kept.push_back(handler);
          FakeResponse response(200, kAcceptedBody);
          handler->OnResponse(response);
          handler->OnEvent(state, "");
        });

    const int before          = Completions();
    const int failures_before = Counter().failures();
    ExportOnce(*fixture.exporter);

    EXPECT_EQ(Completions() - before, 1);
    EXPECT_EQ(Counter().failures() - failures_before, 0)
        << "the teardown verdict replaced the response's";

    kept.clear();
    EXPECT_EQ(Completions() - before, 1) << "destroying the handler reported a second time";
  }
}

// The other order, and the contract it settles. A read or write error ends the export here: the
// exporter treats it as the outcome, and a response arriving afterwards is ignored rather than
// replacing it. EventHandler does not say whether either state can be followed by a response, so
// this is the choice this exporter makes, written down where a change to it would be visible.
TEST_F(ElasticsearchAsyncCompletionTests, ATeardownEventAndALaterResponseReportOnce)
{
  for (const auto state :
       {http_client::SessionState::ReadError, http_client::SessionState::WriteError,
        http_client::SessionState::Destroyed, http_client::SessionState::TimedOut,
        http_client::SessionState::NetworkError, http_client::SessionState::Cancelled})
  {
    SCOPED_TRACE(static_cast<int>(state));
    std::vector<std::shared_ptr<http_client::EventHandler>> kept;
    auto fixture =
        MakeExporter([&kept, state](const std::shared_ptr<http_client::EventHandler> &handler) {
          kept.push_back(handler);
          handler->OnEvent(state, "");
          FakeResponse response(200, kAcceptedBody);
          handler->OnResponse(response);
        });

    const int before          = Completions();
    const int failures_before = Counter().failures();
    ExportOnce(*fixture.exporter);

    EXPECT_EQ(Completions() - before, 1);
    EXPECT_EQ(Counter().failures() - failures_before, 1)
        << "a response after the failure replaced the verdict that had already been reported";

    kept.clear();
    EXPECT_EQ(Completions() - before, 1) << "destroying the handler reported a second time";
  }
}

// Two terminal events delivered at the same time. The inline scripts above cannot reach the race
// the compare-exchange exists for.
TEST_F(ElasticsearchAsyncCompletionTests, ConcurrentTerminalEventsReportOnce)
{
  std::shared_ptr<http_client::EventHandler> captured;
  auto fixture =
      MakeExporter([&captured](const std::shared_ptr<http_client::EventHandler> &handler) {
        captured = handler;
      });
  ExportOnce(*fixture.exporter);
  ASSERT_NE(captured, nullptr);

  std::atomic<bool> go{false};
  std::thread first([&captured, &go] {
    while (!go.load(std::memory_order_acquire))
    {
      std::this_thread::yield();
    }
    captured->OnEvent(http_client::SessionState::ConnectFailed, "");
  });
  std::thread second([&captured, &go] {
    while (!go.load(std::memory_order_acquire))
    {
      std::this_thread::yield();
    }
    captured->OnEvent(http_client::SessionState::NetworkError, "");
  });
  go.store(true, std::memory_order_release);
  first.join();
  second.join();

  EXPECT_EQ(Completions(), 1);
}

// A response and a terminal event delivered at the same time. Whichever wins, there is one report.
TEST_F(ElasticsearchAsyncCompletionTests, AConcurrentResponseAndTerminalEventReportOnce)
{
  std::shared_ptr<http_client::EventHandler> captured;
  auto fixture =
      MakeExporter([&captured](const std::shared_ptr<http_client::EventHandler> &handler) {
        captured = handler;
      });
  ExportOnce(*fixture.exporter);
  ASSERT_NE(captured, nullptr);

  std::atomic<bool> go{false};
  std::thread responder([&captured, &go] {
    while (!go.load(std::memory_order_acquire))
    {
      std::this_thread::yield();
    }
    FakeResponse response(200, kAcceptedBody);
    captured->OnResponse(response);
  });
  std::thread failer([&captured, &go] {
    while (!go.load(std::memory_order_acquire))
    {
      std::this_thread::yield();
    }
    captured->OnEvent(http_client::SessionState::TimedOut, "");
  });
  go.store(true, std::memory_order_release);
  responder.join();
  failer.join();

  EXPECT_EQ(Completions(), 1);
}

// What Session::SendRequest does when HttpOperation::SendAsync fails to set up: the operation
// dispatches ConnectFailed and returns non-OK, then SendRequest dispatches CreateFailed for the
// same handler. One export, so one finished session, not two.
namespace
{
// Calls back into the exporter from inside the log handler, which is what an application can
// install through GlobalLogHandler::SetLogHandler().
class FlushingLogHandler : public internal_log::LogHandler
{
public:
  // The needle picks which diagnostic re-enters the exporter, because the two paths that log
  // one describe it differently.
  void Watch(logs_exporter::ElasticsearchLogRecordExporter *exporter,
             const char *needle = "Logs were not written") noexcept
  {
    exporter_ = exporter;
    needle_   = needle;
  }

  void Handle(internal_log::LogLevel /* level */,
              const char * /* file */,
              int /* line */,
              const char *msg,
              const opentelemetry::sdk::common::AttributeMap & /* attributes */) noexcept override
  {
    if (msg == nullptr || exporter_ == nullptr)
    {
      return;
    }
    if (std::string(msg).find(needle_) == std::string::npos)
    {
      return;
    }
    lines_.fetch_add(1, std::memory_order_relaxed);
    if (reentered_.exchange(true, std::memory_order_relaxed))
    {
      return;
    }
    flushed_.store(exporter_->ForceFlush(std::chrono::milliseconds{20}), std::memory_order_relaxed);
  }

  bool reentered() const noexcept { return reentered_.load(std::memory_order_relaxed); }
  bool flushed() const noexcept { return flushed_.load(std::memory_order_relaxed); }
  int lines() const noexcept { return lines_.load(std::memory_order_relaxed); }

private:
  logs_exporter::ElasticsearchLogRecordExporter *exporter_{nullptr};
  const char *needle_{"Logs were not written"};
  std::atomic<bool> reentered_{false};
  std::atomic<bool> flushed_{false};
  std::atomic<int> lines_{0};
};
}  // namespace

// The session has to be retired before anything replaceable is called, or a handler that flushes
// waits for the export whose completion is calling it.
TEST_F(ElasticsearchAsyncCompletionTests, AFlushFromInsideTheLogHandlerDoesNotWaitForItsOwnSession)
{
  auto fixture = MakeExporter([](const std::shared_ptr<http_client::EventHandler> &handler) {
    FakeResponse response(200, R"({"took":1,"errors":true,"items":[]})");
    handler->OnResponse(response);
  });

  auto watcher = nostd::shared_ptr<internal_log::LogHandler>(new FlushingLogHandler());
  auto *raw    = static_cast<FlushingLogHandler *>(watcher.get());
  raw->Watch(fixture.exporter.get());
  internal_log::GlobalLogHandler::SetLogHandler(watcher);

  ExportOnce(*fixture.exporter);

  ASSERT_TRUE(raw->reentered()) << "the failure never reached the log handler";
  EXPECT_TRUE(raw->flushed()) << "the flush waited for the session that was reporting itself";
  raw->Watch(nullptr);
}

// The same rule on the path that refuses the batch. The export is registered before the shutdown
// check, so reporting the refusal before retiring it makes a flushing handler wait for the
// Export() that is calling it, and the refusal is described twice.
TEST_F(ElasticsearchAsyncCompletionTests, AFlushFromTheShutdownErrorDoesNotWaitForItsOwnExport)
{
  auto fixture = MakeExporter([](const std::shared_ptr<http_client::EventHandler> &) {});
  ASSERT_TRUE(fixture.exporter->Shutdown());

  auto watcher = nostd::shared_ptr<internal_log::LogHandler>(new FlushingLogHandler());
  auto *raw    = static_cast<FlushingLogHandler *>(watcher.get());
  raw->Watch(fixture.exporter.get(), "exporter is shutdown");
  internal_log::GlobalLogHandler::SetLogHandler(watcher);

  ExportOnce(*fixture.exporter);

  ASSERT_TRUE(raw->reentered()) << "the shutdown refusal never reached the log handler";
  EXPECT_TRUE(raw->flushed()) << "the flush waited for the export that was refusing itself";
  EXPECT_EQ(1, raw->lines()) << "one refusal was described " << raw->lines() << " times";
  raw->Watch(nullptr);
}

// Two responses for one request write the same body and race for the same outcome. The body is a
// local so there is nothing shared to tear, and the exchange decides which one reports.
TEST_F(ElasticsearchAsyncCompletionTests, TwoConcurrentResponsesReportOnce)
{
  std::shared_ptr<http_client::EventHandler> captured;
  auto fixture =
      MakeExporter([&captured](const std::shared_ptr<http_client::EventHandler> &handler) {
        captured = handler;
      });
  ExportOnce(*fixture.exporter);
  ASSERT_NE(captured, nullptr);

  std::atomic<bool> go{false};
  const auto deliver = [&captured, &go](const char *body) {
    while (!go.load(std::memory_order_acquire))
    {
      std::this_thread::yield();
    }
    FakeResponse response(200, body);
    captured->OnResponse(response);
  };
  std::thread first(deliver, kAcceptedBody);
  std::thread second(deliver, R"({"took":2,"errors":true,"items":[]})");
  go.store(true, std::memory_order_release);
  first.join();
  second.join();

  EXPECT_EQ(Completions(), 1) << "one request, one outcome, whichever response won";
}

// A handler destroyed without ever reporting still has to finish its session.
TEST_F(ElasticsearchAsyncCompletionTests, AHandlerDestroyedWithoutAnOutcomeStillFinishes)
{
  auto fixture = MakeExporter([](const std::shared_ptr<http_client::EventHandler> &) {});
  ExportOnce(*fixture.exporter);
  EXPECT_TRUE(fixture.exporter->ForceFlush(std::chrono::milliseconds{20}));
}
