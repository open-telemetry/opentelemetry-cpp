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
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <mutex>
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
// Snapshot ordering, for the cases that need an export to start after a flush has taken its
// watermark. That order decides what the flush is waiting on, so it has to be a fact rather
// than a sleep: on a loaded runner the other order runs instead, and in that order even the
// counting model these cases exist to rule out would report a pass.
// ---------------------------------------------------------------------------
#ifdef ENABLE_ASYNC_EXPORT
OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace logs
{
class ElasticsearchExporterTestPeer
{
public:
  static std::uint64_t WatermarksTaken(ElasticsearchLogRecordExporter &exporter)
  {
    std::lock_guard<std::mutex> lock_guard{exporter.synchronization_data_->force_flush_cv_m};
    return exporter.synchronization_data_->watermarks_taken;
  }
};
}  // namespace logs
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE

namespace
{
// Bounded, and the count only goes up, so a wait that expires means the flush never got there
// rather than that the case missed it.
//
// Well inside the bound CTest puts on the whole binary. Matching the two would mean a case that
// never takes its watermark is killed from the outside before this returns, so it would never
// reach its own assertion, its join, or its cleanup, and the report would say the suite timed out
// rather than which case failed and why.
bool WaitForWatermarks(logs_exporter::ElasticsearchLogRecordExporter &exporter,
                       std::uint64_t wanted)
{
  const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds{5};
  while (std::chrono::steady_clock::now() < deadline)
  {
    if (logs_exporter::ElasticsearchExporterTestPeer::WatermarksTaken(exporter) >= wanted)
    {
      return true;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds{1});
  }
  return false;
}
}  // namespace
#else
namespace
{
// The cases that call this skip in SetUp when there is no wait to observe, but they are compiled
// in both configurations on purpose: a case removed from the binary stays registered with CTest
// and reports a pass without running. So the helper has to exist in both too.
inline bool WaitForWatermarks(logs_exporter::ElasticsearchLogRecordExporter &, std::uint64_t)
{
  return false;
}
}  // namespace
#endif  // ENABLE_ASYNC_EXPORT

// ---------------------------------------------------------------------------
// ForceFlush deadline. Only built with async export, which is the only
// configuration where the wait exists.
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

// The wait these cases describe exists only in an async build, so they skip elsewhere rather than
// compile out: gtest_add_tests reads the source, and a case missing from the binary is still
// registered with CTest, where it then reports a pass without having run. The skip goes in SetUp
// because GTEST_SKIP returns, and a skip at the top of each body would leave the rest of that body
// unreachable, which MSVC reports as C4702.
namespace
{
class ElasticsearchForceFlushTests : public ::testing::Test
{
protected:
  void SetUp() override
  {
#ifndef ENABLE_ASYNC_EXPORT
    GTEST_SKIP() << "ForceFlush has nothing to wait for when async export is disabled";
#endif
  }
};
}  // namespace

// The session never calls back, so the flush cannot complete. It has to say so, and it has to say
// so when the caller's deadline runs out rather than when the response timeout does.
TEST_F(ElasticsearchForceFlushTests, ReportsFailureWhenTheFlushDoesNotComplete)
{
  // The handler is kept because a dropped one now reports a failure from its destructor, which
  // would finish the session and leave nothing for the flush to wait on. The real curl operation
  // owns the handler until the request ends, so this is also the truer shape.
  std::vector<std::shared_ptr<http_client::EventHandler>> kept;
  auto fixture = MakeExporter([&kept](const std::shared_ptr<http_client::EventHandler> &handler) {
    kept.push_back(handler);
  });
  ExportOnce(*fixture.exporter);

  const auto start   = std::chrono::steady_clock::now();
  const bool flushed = fixture.exporter->ForceFlush(std::chrono::milliseconds{20});
  const auto elapsed = std::chrono::steady_clock::now() - start;

  EXPECT_FALSE(flushed);
  EXPECT_LT(std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count(),
            kShortResponseTimeoutSeconds * 1000);
}

// Nothing outstanding, so there is nothing to wait for.
TEST_F(ElasticsearchForceFlushTests, ReturnsImmediatelyWithNothingInFlight)
{
  auto fixture = MakeExporter([](const std::shared_ptr<http_client::EventHandler> &) {});

  const auto start = std::chrono::steady_clock::now();
  EXPECT_TRUE(fixture.exporter->ForceFlush(std::chrono::milliseconds{20}));
  EXPECT_LT(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() -
                                                                  start)
                .count(),
            kShortResponseTimeoutSeconds * 1000);
}

// The session completes inside SendRequest(), before the flush is even asked for, so the
// completion is already published when the predicate is first evaluated.
TEST_F(ElasticsearchForceFlushTests, SucceedsWhenTheSessionFinishedBeforeTheWait)
{
  auto fixture = MakeExporter([](const std::shared_ptr<http_client::EventHandler> &handler) {
    FakeResponse response(200, kAcceptedBody);
    handler->OnResponse(response);
  });
  ExportOnce(*fixture.exporter);

  EXPECT_TRUE(fixture.exporter->ForceFlush(std::chrono::milliseconds{20}));
}

// Two exports, one of which never finishes. Reporting success here would tell the caller data was
// flushed that is still in flight.
TEST_F(ElasticsearchForceFlushTests, PartialCompletionIsNotSuccess)
{
  bool respond = true;
  std::vector<std::shared_ptr<http_client::EventHandler>> kept;
  auto fixture =
      MakeExporter([&respond, &kept](const std::shared_ptr<http_client::EventHandler> &handler) {
        kept.push_back(handler);  // the second session has to stay unfinished
        if (respond)
        {
          FakeResponse response(200, kAcceptedBody);
          handler->OnResponse(response);
        }
      });
  ExportOnce(*fixture.exporter);
  respond = false;
  ExportOnce(*fixture.exporter);

  EXPECT_FALSE(fixture.exporter->ForceFlush(std::chrono::milliseconds{20}));
}

// A completion delivered from another thread after the waiter has parked has to wake it. This is
// the half the inline scripts above cannot reach.
TEST_F(ElasticsearchForceFlushTests, ACompletionAfterTheWaiterParksWakesIt)
{
  // Held by the test, not by the fakes: a handler owns its session, so a session that also owned
  // its handler would be a reference cycle and leak.
  std::shared_ptr<http_client::EventHandler> captured;
  auto fixture =
      MakeExporter([&captured](const std::shared_ptr<http_client::EventHandler> &handler) {
        captured = handler;
      });
  ExportOnce(*fixture.exporter);
  ASSERT_NE(captured, nullptr);

  bool parked = false;
  std::thread responder([&fixture, &captured, &parked] {
    parked = WaitForWatermarks(*fixture.exporter, 1);
    FakeResponse response(200, kAcceptedBody);
    captured->OnResponse(response);
  });

  const bool flushed = fixture.exporter->ForceFlush(std::chrono::seconds{5});
  responder.join();

  EXPECT_TRUE(parked) << "the flush never took a watermark, so it was not waiting for this";
  EXPECT_TRUE(flushed);
}

// A session started after the flush was asked for belongs to a later batch, so its completion is
// not evidence about the batch the caller is waiting on. Counting completions rather than
// identifying them let one stand in for the other: the flush reported success with the original
// export still in flight.
//
// The ordering the case needs is that the flush takes its snapshot before the second export
// starts. ForceFlush() is called with nothing between it and the thread that starts, and the other
// side sleeps first, so the window is four orders of magnitude wider than the race. Reverting the
// production change makes this fail, which is what shows the ordering held.
TEST_F(ElasticsearchForceFlushTests, ANewerSessionDoesNotStandInForAnOlderOne)
{
  std::vector<std::shared_ptr<http_client::EventHandler>> kept;
  std::shared_ptr<http_client::EventHandler> latest;
  auto fixture =
      MakeExporter([&kept, &latest](const std::shared_ptr<http_client::EventHandler> &handler) {
        kept.push_back(handler);
        latest = handler;
      });

  // The batch the caller waits for. It never finishes.
  ExportOnce(*fixture.exporter);

  bool snapshotted = false;
  std::thread newer([&fixture, &latest, &snapshotted] {
    snapshotted = WaitForWatermarks(*fixture.exporter, 1);
    ExportOnce(*fixture.exporter);
    FakeResponse response(200, kAcceptedBody);
    latest->OnResponse(response);
  });

  const bool flushed = fixture.exporter->ForceFlush(std::chrono::milliseconds{500});
  newer.join();

  EXPECT_TRUE(snapshotted)
      << "the second export did not start after the snapshot, so it is not a later batch";
  EXPECT_FALSE(flushed) << "a later batch's completion flushed an export that is still in flight";
}

// A batch counts as received when Export() is entered, not when its request has finished being
// built, so a flush asked while the body is still being serialised has to wait for it. Registering
// the session after the request was built left a window where the flush snapshotted past it and
// reported it as already flushed. CreateSession() stands in for the serialisation: it runs after
// the records were handed over and before the request exists.
TEST_F(ElasticsearchForceFlushTests, AnExportAlreadyUnderWayIsSomethingToWaitFor)
{
  std::vector<std::shared_ptr<http_client::EventHandler>> kept;
  auto fixture = MakeExporter([&kept](const std::shared_ptr<http_client::EventHandler> &handler) {
    kept.push_back(handler);
  });

  std::mutex mutex;
  std::condition_variable cv;
  bool inside_export                = false;
  bool release                      = false;
  fixture.client->on_create_session = [&mutex, &cv, &inside_export, &release] {
    {
      std::lock_guard<std::mutex> lock(mutex);
      inside_export = true;
    }
    cv.notify_all();
    std::unique_lock<std::mutex> lock(mutex);
    cv.wait(lock, [&release] { return release; });
  };

  std::thread exporting([&fixture] { ExportOnce(*fixture.exporter); });
  {
    std::unique_lock<std::mutex> lock(mutex);
    cv.wait(lock, [&inside_export] { return inside_export; });
  }

  const bool flushed = fixture.exporter->ForceFlush(std::chrono::milliseconds{200});

  {
    std::lock_guard<std::mutex> lock(mutex);
    release = true;
  }
  cv.notify_all();
  exporting.join();

  EXPECT_FALSE(flushed) << "the flush passed over a batch Export() had already been handed";
}

// The same substitution with the count actually reaching the snapshot, which is what makes a
// counter look right while being wrong. Two exports are outstanding when the flush is asked, so it
// waits for two completions; a third export starts after that, and it plus one of the originals
// deliver two completions. The export the caller is waiting on has still not finished.
TEST_F(ElasticsearchForceFlushTests, LaterCompletionsCannotCoverAnOlderOutstandingExport)
{
  std::vector<std::shared_ptr<http_client::EventHandler>> kept;
  std::shared_ptr<http_client::EventHandler> latest;
  auto fixture =
      MakeExporter([&kept, &latest](const std::shared_ptr<http_client::EventHandler> &handler) {
        kept.push_back(handler);
        latest = handler;
      });

  ExportOnce(*fixture.exporter);  // never finishes
  ExportOnce(*fixture.exporter);
  auto second = latest;

  bool snapshotted = false;
  std::thread worker([&fixture, &second, &latest, &snapshotted] {
    snapshotted = WaitForWatermarks(*fixture.exporter, 1);
    ExportOnce(*fixture.exporter);  // starts after the flush took its snapshot
    const auto &third = latest;
    FakeResponse response(200, kAcceptedBody);
    second->OnResponse(response);
    third->OnResponse(response);
  });

  const bool flushed = fixture.exporter->ForceFlush(std::chrono::milliseconds{500});
  worker.join();

  EXPECT_TRUE(snapshotted)
      << "the third export did not start after the snapshot, so it is not a later batch";
  EXPECT_FALSE(flushed) << "two completions arrived, but not the one the caller was waiting for";
}

// A second caller gets its own deadline. Serialising the calls is fine, making the second one
// wait out the first one's is not, since its timeout would mean nothing.
TEST_F(ElasticsearchForceFlushTests, AConcurrentFlushKeepsItsOwnDeadline)
{
  std::vector<std::shared_ptr<http_client::EventHandler>> kept;
  auto fixture = MakeExporter([&kept](const std::shared_ptr<http_client::EventHandler> &handler) {
    kept.push_back(handler);
  });
  ExportOnce(*fixture.exporter);

  // The first caller waits well past the bound asserted below, so a second caller that queued
  // behind it could not come in under that bound by accident.
  std::thread slow([&fixture] { fixture.exporter->ForceFlush(std::chrono::milliseconds{1500}); });

  // Recorded rather than asserted: that thread is still running, and a fatal assertion here
  // would destroy it while it is joinable, which ends the process.
  const bool first_waiting = WaitForWatermarks(*fixture.exporter, 1);

  const auto start          = std::chrono::steady_clock::now();
  const bool second_flushed = fixture.exporter->ForceFlush(std::chrono::milliseconds{20});
  const auto ms             = std::chrono::duration_cast<std::chrono::milliseconds>(
                      std::chrono::steady_clock::now() - start)
                      .count();
  slow.join();

  EXPECT_TRUE(first_waiting) << "the first caller never reached its wait, so nothing was queued";
  EXPECT_FALSE(second_flushed);
  EXPECT_LT(ms, 700) << "waited behind the first caller instead of its own deadline";
}

// The default argument is microseconds::max(), which AdjustWaitForTimeout maps to the sentinel for
// no deadline. That branch takes the lock outright and waits on the predicate, so it needs a case
// where the predicate already holds or the test would never return.
// The indefinite wait, actually waited on. The case above it reaches the same branch but the
// fake answers from inside SendRequest(), so the session is already gone and the predicate holds
// before ForceFlush() is called. Nothing there would notice the wait being replaced by one
// evaluation of the predicate.
TEST_F(ElasticsearchForceFlushTests, AnIndefiniteFlushParksUntilTheOutcomeArrives)
{
  std::shared_ptr<http_client::EventHandler> captured;
  auto fixture =
      MakeExporter([&captured](const std::shared_ptr<http_client::EventHandler> &handler) {
        captured = handler;
      });
  ExportOnce(*fixture.exporter);
  ASSERT_NE(captured, nullptr);

  std::atomic<bool> returned{false};
  bool flushed = false;
  std::thread waiter([&fixture, &returned, &flushed] {
    flushed = fixture.exporter->ForceFlush();
    returned.store(true, std::memory_order_release);
  });

  // Recorded rather than asserted here. A fatal assertion between starting that thread and
  // joining it leaves it joinable, and a joinable thread being destroyed ends the process, which
  // takes the rest of the binary with it instead of reporting one case.
  const bool snapshotted    = WaitForWatermarks(*fixture.exporter, 1);
  const bool returned_early = returned.load(std::memory_order_acquire);

  FakeResponse response(200, kAcceptedBody);
  captured->OnResponse(response);
  waiter.join();

  EXPECT_TRUE(snapshotted) << "the flush never took a watermark, so it never reached the wait";
  EXPECT_FALSE(returned_early) << "the flush returned with its session still outstanding";
  EXPECT_TRUE(flushed) << "the flush did not report the completion it was waiting for";
}

// The other boundary from the substitution cases. Those hold that a newer completion cannot
// finish an older flush; this holds that a newer export still running cannot keep that flush
// open. Without it the predicate could become running.empty(), or compare with > instead of >=,
// and nothing would report it.
TEST_F(ElasticsearchForceFlushTests, ANewerSessionDoesNotHoldAnOlderFlushOpen)
{
  std::vector<std::shared_ptr<http_client::EventHandler>> kept;
  auto fixture = MakeExporter([&kept](const std::shared_ptr<http_client::EventHandler> &handler) {
    kept.push_back(handler);
  });

  ExportOnce(*fixture.exporter);
  ASSERT_EQ(kept.size(), static_cast<std::size_t>(1));

  std::atomic<bool> returned{false};
  bool flushed = false;
  std::thread waiter([&fixture, &returned, &flushed] {
    flushed = fixture.exporter->ForceFlush(std::chrono::seconds{5});
    returned.store(true, std::memory_order_release);
  });

  // Recorded rather than asserted, for the same reason as the case above: nothing fatal may
  // happen while that thread is still running.
  const bool snapshotted = WaitForWatermarks(*fixture.exporter, 1);

  // Started after the snapshot and never answered, so it is outside what this flush waits on.
  ExportOnce(*fixture.exporter);
  const std::size_t exports_started = kept.size();
  const bool returned_early         = returned.load(std::memory_order_acquire);

  FakeResponse response(200, kAcceptedBody);
  kept.front()->OnResponse(response);
  waiter.join();

  EXPECT_TRUE(snapshotted)
      << "the flush never took a watermark, so the second export is not newer than one";
  EXPECT_EQ(exports_started, static_cast<std::size_t>(2));
  EXPECT_FALSE(returned_early)
      << "the flush returned before the export it snapshotted had answered";
  EXPECT_TRUE(flushed) << "the flush waited out its deadline on an export that started after it";
}

TEST_F(ElasticsearchForceFlushTests, AnIndefiniteFlushReturnsOnceEverythingIsFinished)
{
  auto fixture = MakeExporter([](const std::shared_ptr<http_client::EventHandler> &handler) {
    FakeResponse response(200, kAcceptedBody);
    handler->OnResponse(response);
  });
  ExportOnce(*fixture.exporter);

  EXPECT_TRUE(fixture.exporter->ForceFlush());
}

// What true means, pinned so that it cannot drift unnoticed: the snapshotted exports have
// reported an outcome, not that their batches were delivered. Elasticsearch rejected this one
// and the flush still reports success. Surfacing an export failure through this return value is
// https://github.com/open-telemetry/opentelemetry-cpp/issues/3075, which is a repository wide
// decision rather than one for this exporter: the OTLP HTTP client's ForceFlush reports the same
// way today, and changing one of them alone would leave the two disagreeing.
TEST_F(ElasticsearchForceFlushTests, AFailedExportSettlesItsSessionAndIsReportedAsFlushed)
{
  auto fixture = MakeExporter([](const std::shared_ptr<http_client::EventHandler> &handler) {
    FakeResponse response(200, R"({"took":1,"errors":true,"items":[]})");
    handler->OnResponse(response);
  });
  ExportOnce(*fixture.exporter);

  EXPECT_TRUE(fixture.exporter->ForceFlush(std::chrono::milliseconds{20}));
}

// A caller with no deadline of its own still gets one. Nothing obliges an HTTP client to call
// back, and a flush that waits for a callback that never comes would take the caller down with
// it, so the exporter bounds the wait by its own response timeout and reports that it did not
// finish. The elapsed time is asserted from below as well: a bound of zero would also return
// false here, and would return it immediately.
TEST_F(ElasticsearchForceFlushTests, ANoDeadlineFlushGivesUpAtTheResponseTimeout)
{
  // Kept for the same reason as the case at the top of this file: a dropped handler reports a
  // failure from its destructor, which would retire the session this case needs left outstanding.
  std::vector<std::shared_ptr<http_client::EventHandler>> kept;
  auto fixture = MakeExporter([&kept](const std::shared_ptr<http_client::EventHandler> &handler) {
    kept.push_back(handler);
  });
  ExportOnce(*fixture.exporter);

  const auto start   = std::chrono::steady_clock::now();
  const bool flushed = fixture.exporter->ForceFlush();
  const auto elapsed = std::chrono::steady_clock::now() - start;
  const auto ms      = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();

  EXPECT_FALSE(flushed) << "reported a completion for a session that never answered";
  EXPECT_GE(ms, 1000) << "returned without waiting, so the bound is not the response timeout";
}

// ForceFlush() and Shutdown() are required to be safe to call concurrently, and this PR took out
// the lock that used to serialise them, so the overlap is deterministic here rather than left to
// chance: the flush is parked on its session before the shutdown starts. A real client answers
// its outstanding sessions while shutting down, which is what has to wake the waiter.
TEST_F(ElasticsearchForceFlushTests, AParkedFlushIsWokenByTheShutdownThatCompletesItsSession)
{
  std::shared_ptr<http_client::EventHandler> captured;
  auto fixture =
      MakeExporter([&captured](const std::shared_ptr<http_client::EventHandler> &handler) {
        captured = handler;
      });
  ExportOnce(*fixture.exporter);
  ASSERT_NE(captured, nullptr);

  fixture.client->on_cancel_all = [&captured] {
    captured->OnEvent(http_client::SessionState::Cancelled, "");
  };

  std::atomic<bool> returned{false};
  bool flushed = false;
  std::thread waiter([&fixture, &returned, &flushed] {
    flushed = fixture.exporter->ForceFlush();
    returned.store(true, std::memory_order_release);
  });

  // Recorded rather than asserted, for the reason the cases above give: nothing fatal may happen
  // while that thread is still joinable.
  const bool snapshotted    = WaitForWatermarks(*fixture.exporter, 1);
  const bool returned_early = returned.load(std::memory_order_acquire);

  const auto start = std::chrono::steady_clock::now();
  const bool down  = fixture.exporter->Shutdown();
  waiter.join();
  const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                      std::chrono::steady_clock::now() - start)
                      .count();

  EXPECT_TRUE(snapshotted) << "the flush never took a watermark, so it never reached the wait";
  EXPECT_FALSE(returned_early) << "the flush returned with its session still outstanding";
  EXPECT_TRUE(down);
  EXPECT_TRUE(flushed) << "the shutdown settled the session and the flush still reported failure";
  EXPECT_LT(ms, kShortResponseTimeoutSeconds * 1000)
      << "the flush waited out its own bound instead of being woken by the shutdown";
}

// The same overlap against a client that answers nothing on the way down. There is no event to
// wake the waiter here, so what is being pinned is that the flush still ends: without a bound on
// the no-deadline wait this case does not fail, it stops.
TEST_F(ElasticsearchForceFlushTests, AParkedFlushEndsEvenIfTheShutdownSettlesNothing)
{
  std::vector<std::shared_ptr<http_client::EventHandler>> kept;
  auto fixture = MakeExporter([&kept](const std::shared_ptr<http_client::EventHandler> &handler) {
    kept.push_back(handler);
  });
  ExportOnce(*fixture.exporter);

  bool flushed = false;
  std::thread waiter([&fixture, &flushed] { flushed = fixture.exporter->ForceFlush(); });

  const bool snapshotted = WaitForWatermarks(*fixture.exporter, 1);
  const bool down        = fixture.exporter->Shutdown();
  waiter.join();

  EXPECT_TRUE(snapshotted) << "the flush never took a watermark, so it never reached the wait";
  EXPECT_TRUE(down);
  EXPECT_FALSE(flushed) << "reported a completion for a session nothing ever settled";
}
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
// Every terminal state has to finish the session, or a flush waits on a request that can never
// complete. Every progress state has to leave it running.
TEST_F(ElasticsearchAsyncCompletionTests, EverySessionStateIsClassifiedAndReportsAtMostOnce)
{
  struct Case
  {
    http_client::SessionState state;
    bool terminal;
  };

  // Every member of the enum as it stands. An array cannot notice that the enum grew, so what
  // catches a new state is the switch in the exporter having no default, under the -Wswitch that
  // maintainer mode turns into an error. This table is the second half of that.
  const Case cases[] = {
      {http_client::SessionState::CreateFailed, true},
      {http_client::SessionState::Created, false},
      {http_client::SessionState::Destroyed, true},
      {http_client::SessionState::Connecting, false},
      {http_client::SessionState::ConnectFailed, true},
      {http_client::SessionState::Connected, false},
      {http_client::SessionState::Sending, false},
      {http_client::SessionState::SendFailed, true},
      {http_client::SessionState::Response, false},
      {http_client::SessionState::SSLHandshakeFailed, true},
      {http_client::SessionState::TimedOut, true},
      {http_client::SessionState::NetworkError, true},
      {http_client::SessionState::ReadError, true},
      {http_client::SessionState::WriteError, true},
      {http_client::SessionState::Cancelled, true},
  };

  for (const auto &test_case : cases)
  {
    SCOPED_TRACE(static_cast<int>(test_case.state));
    std::vector<std::shared_ptr<http_client::EventHandler>> kept;
    auto fixture = MakeExporter(
        [&kept, &test_case](const std::shared_ptr<http_client::EventHandler> &handler) {
          kept.push_back(handler);  // so the event, not the destructor, is what decides
          handler->OnEvent(test_case.state, "");
          handler->OnEvent(test_case.state, "");  // a repeat must not report a second time
        });

    // Counted per iteration: the previous fixture's handler reports from its destructor as it goes
    // out of scope, which lands in the same counter.
    const int before       = Completions();
    const int lines_before = Lines();
    ExportOnce(*fixture.exporter);

    EXPECT_EQ(Completions() - before, test_case.terminal ? 1 : 0);
    EXPECT_EQ(fixture.exporter->ForceFlush(std::chrono::milliseconds{20}), test_case.terminal);

    // Nothing on the way to an outcome may reach the log handler at all. The handler is
    // replaceable, this session is registered before the request is handed to the client, and no
    // progress event has retired it, so a handler that flushed from one would be waiting for the
    // export whose call stack it is standing in. A terminal state is reported, and by then the
    // session has already been let go.
    if (test_case.terminal)
    {
      EXPECT_GT(Lines() - lines_before, 0) << "a terminal state said nothing";
    }
    else
    {
      EXPECT_EQ(Lines() - lines_before, 0) << "a progress event reached the log handler";
    }
  }
}

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

TEST_F(ElasticsearchAsyncCompletionTests, TwoTerminalEventsCountAsOneSession)
{
  // The real curl operation owns the handler until the request finishes, so a fake that lets it
  // go would make every request complete the moment SendRequest returns. The test owns them
  // instead of the fakes, since a handler owns its session and the reverse would be a cycle.
  std::vector<std::shared_ptr<http_client::EventHandler>> kept;
  int session = 0;
  auto fixture =
      MakeExporter([&kept, &session](const std::shared_ptr<http_client::EventHandler> &handler) {
        kept.push_back(handler);
        if (++session == 1)
        {
          handler->OnEvent(http_client::SessionState::ConnectFailed, "");
          handler->OnEvent(http_client::SessionState::CreateFailed, "");
        }
      });

  ExportOnce(*fixture.exporter);  // reports twice before the fix
  ExportOnce(*fixture.exporter);  // never calls back

  EXPECT_EQ(Completions(), 1) << "the first session reported more than once";
  EXPECT_FALSE(fixture.exporter->ForceFlush(std::chrono::milliseconds{20}))
      << "the second session is still in flight";
}

// The curl client's completion lambda tests the response first and the abort in an else, so this
// ordering no longer comes from it. It still comes from anywhere: EventHandler promises callers
// nothing about how many terminal events arrive, and #4360 is open on exactly that.
TEST_F(ElasticsearchAsyncCompletionTests, CancelledThenAResponseCountsOnce)
{
  std::vector<std::shared_ptr<http_client::EventHandler>> kept;
  int session = 0;
  auto fixture =
      MakeExporter([&kept, &session](const std::shared_ptr<http_client::EventHandler> &handler) {
        kept.push_back(handler);
        if (++session == 1)
        {
          handler->OnEvent(http_client::SessionState::Cancelled, "");
          FakeResponse response(200, kAcceptedBody);
          handler->OnResponse(response);
        }
      });

  ExportOnce(*fixture.exporter);
  ExportOnce(*fixture.exporter);

  EXPECT_EQ(Completions(), 1) << "the late response reported on top of the cancellation";
  EXPECT_EQ(Counter().successes(), 0) << "the late response replaced the cancellation verdict";
  EXPECT_FALSE(fixture.exporter->ForceFlush(std::chrono::milliseconds{20}));
}

// A response followed by the session being torn down is the ordinary successful shape.
TEST_F(ElasticsearchAsyncCompletionTests, ResponseThenDestroyedCountsOnce)
{
  std::vector<std::shared_ptr<http_client::EventHandler>> kept;
  int session = 0;
  auto fixture =
      MakeExporter([&kept, &session](const std::shared_ptr<http_client::EventHandler> &handler) {
        kept.push_back(handler);
        if (++session == 1)
        {
          FakeResponse response(200, kAcceptedBody);
          handler->OnResponse(response);
          handler->OnEvent(http_client::SessionState::Destroyed, "");
        }
      });

  ExportOnce(*fixture.exporter);
  ExportOnce(*fixture.exporter);

  EXPECT_EQ(Completions(), 1) << "the teardown reported on top of the response";
  EXPECT_FALSE(fixture.exporter->ForceFlush(std::chrono::milliseconds{20}));
}

// A handler destroyed without ever reporting still has to finish its session.
TEST_F(ElasticsearchAsyncCompletionTests, AHandlerDestroyedWithoutAnOutcomeStillFinishes)
{
  auto fixture = MakeExporter([](const std::shared_ptr<http_client::EventHandler> &) {});
  ExportOnce(*fixture.exporter);
  EXPECT_TRUE(fixture.exporter->ForceFlush(std::chrono::milliseconds{20}));
}
