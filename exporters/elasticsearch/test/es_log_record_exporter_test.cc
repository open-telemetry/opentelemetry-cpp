// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/elasticsearch/es_log_record_exporter.h"
#include "opentelemetry/common/timestamp.h"
#include "opentelemetry/exporters/elasticsearch/detail/es_bulk_response.h"
#include "opentelemetry/exporters/elasticsearch/es_log_recordable.h"
#include "opentelemetry/ext/http/client/http_client.h"
#include "opentelemetry/logs/severity.h"
#include "opentelemetry/nostd/function_ref.h"
#include "opentelemetry/nostd/shared_ptr.h"
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
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>
#include <utility>
#include <vector>
#include "nlohmann/json.hpp"

namespace sdklogs       = opentelemetry::sdk::logs;
namespace logs_api      = opentelemetry::logs;
namespace nostd         = opentelemetry::nostd;
namespace logs_exporter = opentelemetry::exporter::logs;

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

// The batch outcome is the top level "errors" field. Two success bodies that differ only in
// whitespace, so a check that depends on formatting cannot answer the same for both.
namespace
{
constexpr const char *kPrettySuccess =
    R"({"took":30,"errors":false,"items":[{"index":{"_index":"logs","_id":"1",)"
    R"("_shards":{"total":2,"successful":1,"failed" : 0},"status":201}}]})";
constexpr const char *kCompactSuccess =
    R"({"took":30,"errors":false,"items":[{"index":{"_index":"logs","_id":"1",)"
    R"("_shards":{"total":2,"successful":1,"failed":0},"status":201}}]})";
constexpr const char *kOneItemRejected =
    R"({"took":30,"errors":true,"items":[{"index":{"_index":"logs","_id":"1",)"
    R"("_shards":{"failed" : 0},"status":201}},{"index":{"_index":"logs","_id":"2",)"
    R"("status":400,"error":{"type":"mapper_parsing_exception","reason":"bad field"}}}]})";
}  // namespace

TEST(ElasticsearchBulkResponseTests, ReportsSuccessWhenErrorsIsFalse)
{
  std::string reason;
  EXPECT_TRUE(logs_exporter::detail::IsBulkResponseSuccessful(200, kPrettySuccess, 1, reason));
  EXPECT_TRUE(logs_exporter::detail::IsBulkResponseSuccessful(200, kCompactSuccess, 1, reason));
}

// A successful check must not leave a stale failure reason from a previous call behind.
TEST(ElasticsearchBulkResponseTests, ClearsFailureReasonOnSuccess)
{
  std::string reason = "stale";
  EXPECT_TRUE(logs_exporter::detail::IsBulkResponseSuccessful(200, kCompactSuccess, 1, reason));
  EXPECT_TRUE(reason.empty());
}

TEST(ElasticsearchBulkResponseTests, ReportsFailureWhenAnyItemWasRejected)
{
  std::string reason;
  EXPECT_FALSE(logs_exporter::detail::IsBulkResponseSuccessful(200, kOneItemRejected, 2, reason));
  EXPECT_NE(reason.find("mapper_parsing_exception"), std::string::npos);
}

TEST(ElasticsearchBulkResponseTests, ReportsFailureOnUnusableBody)
{
  std::string reason;
  EXPECT_FALSE(logs_exporter::detail::IsBulkResponseSuccessful(200, "not json at all", 1, reason));
  EXPECT_FALSE(reason.empty());

  EXPECT_FALSE(logs_exporter::detail::IsBulkResponseSuccessful(200, "", 1, reason));
  EXPECT_FALSE(reason.empty());

  // An array rather than the expected object.
  EXPECT_FALSE(logs_exporter::detail::IsBulkResponseSuccessful(200, "[1,2,3]", 1, reason));
  EXPECT_FALSE(reason.empty());
}

TEST(ElasticsearchBulkResponseTests, ReportsFailureWhenErrorsFieldIsMissingOrNotBoolean)
{
  std::string reason;
  EXPECT_FALSE(
      logs_exporter::detail::IsBulkResponseSuccessful(200, R"({"took":30,"items":[]})", 1, reason));
  EXPECT_FALSE(logs_exporter::detail::IsBulkResponseSuccessful(
      200, R"({"errors":"false","items":[]})", 0, reason));
}

// A non-2xx status is a failure even when the body reports errors:false. This is the invariant
// the substring check and the body-only check both missed, on both the sync and async paths.
TEST(ElasticsearchBulkResponseTests, ReportsFailureOnNon2xxStatus)
{
  std::string reason;
  const char *ok_body = R"({"errors":false,"items":[]})";
  EXPECT_FALSE(logs_exporter::detail::IsBulkResponseSuccessful(500, ok_body, 0, reason));
  EXPECT_FALSE(logs_exporter::detail::IsBulkResponseSuccessful(429, ok_body, 0, reason));
  EXPECT_FALSE(logs_exporter::detail::IsBulkResponseSuccessful(400, ok_body, 0, reason));
  EXPECT_TRUE(logs_exporter::detail::IsBulkResponseSuccessful(200, ok_body, 0, reason));
}

// errors:true with no extractable item error still fails, with the generic reason.
TEST(ElasticsearchBulkResponseTests, ReportsFailureWhenErrorsTrueWithoutItemError)
{
  std::string reason;
  EXPECT_FALSE(logs_exporter::detail::IsBulkResponseSuccessful(200, R"({"errors":true,"items":[]})",
                                                               0, reason));
  EXPECT_FALSE(logs_exporter::detail::IsBulkResponseSuccessful(
      200, R"({"errors":true,"items":[null]})", 1, reason));
  EXPECT_FALSE(logs_exporter::detail::IsBulkResponseSuccessful(
      200, R"({"errors":true,"items":[{"index":42}]})", 1, reason));
}

// The 2xx range is the success band; the codes just outside it are failures. This pins the boundary
// so a later change to the status check cannot silently widen or narrow it.
TEST(ElasticsearchBulkResponseTests, TreatsThe2xxRangeAsTheSuccessBand)
{
  std::string reason;
  const char *ok_body = R"({"errors":false,"items":[]})";
  EXPECT_FALSE(logs_exporter::detail::IsBulkResponseSuccessful(199, ok_body, 0, reason));
  EXPECT_TRUE(logs_exporter::detail::IsBulkResponseSuccessful(200, ok_body, 0, reason));
  EXPECT_TRUE(logs_exporter::detail::IsBulkResponseSuccessful(299, ok_body, 0, reason));
  EXPECT_FALSE(logs_exporter::detail::IsBulkResponseSuccessful(300, ok_body, 0, reason));
}

// The exporter posts an unfiltered /_bulk, so Elasticsearch answers every submitted operation in
// "items". A body that does not is not an answer to that request, and treating it as success would
// hand the caller a write acknowledgement nobody made.
TEST(ElasticsearchBulkResponseTests, RequiresAnAcknowledgementForEverySubmittedOperation)
{
  std::string reason;
  constexpr const char *kNoItems   = R"({"errors":false})";
  constexpr const char *kNullItems = R"({"errors":false,"items":null})";
  constexpr const char *kOneItemBody =
      R"({"errors":false,"items":[{"index":{"_shards":{"failed":0},"status":201}}]})";
  constexpr const char *kTwoItemBody =
      R"({"errors":false,"items":[{"index":{"status":201}},{"index":{"status":201}}]})";

  EXPECT_FALSE(logs_exporter::detail::IsBulkResponseSuccessful(200, kNoItems, 1, reason));
  EXPECT_FALSE(logs_exporter::detail::IsBulkResponseSuccessful(200, kNullItems, 1, reason));
  EXPECT_FALSE(logs_exporter::detail::IsBulkResponseSuccessful(200, kOneItemBody, 2, reason))
      << "too few acknowledgements";
  EXPECT_FALSE(logs_exporter::detail::IsBulkResponseSuccessful(200, kTwoItemBody, 1, reason))
      << "more acknowledgements than operations";
  EXPECT_TRUE(logs_exporter::detail::IsBulkResponseSuccessful(200, kOneItemBody, 1, reason));
  EXPECT_TRUE(logs_exporter::detail::IsBulkResponseSuccessful(200, kTwoItemBody, 2, reason));
}

// The right count of the wrong entries is still the wrong answer, and the verdict cannot depend on
// "errors", which the same responder controls.
TEST(ElasticsearchBulkResponseTests, RejectsItemsEntriesThatDoNotAcknowledgeAnIndexOperation)
{
  std::string reason;
  const auto rejected = [&reason](const char *body, std::size_t expected) {
    return !logs_exporter::detail::IsBulkResponseSuccessful(200, body, expected, reason);
  };

  EXPECT_TRUE(rejected(R"({"errors":false,"items":[null,null]})", 2));
  EXPECT_FALSE(reason.empty());
  EXPECT_TRUE(rejected(R"({"errors":false,"items":[1,2]})", 2));
  EXPECT_TRUE(rejected(R"({"errors":false,"items":[[]]})", 1));
  EXPECT_TRUE(rejected(R"({"errors":false,"items":[{}]})", 1)) << "no operation at all";
  EXPECT_TRUE(rejected(R"({"errors":false,"items":[{"unknown":{"status":201}}]})", 1))
      << "an operation the exporter never submitted";
  EXPECT_TRUE(rejected(R"({"errors":false,"items":[{"index":42}]})", 1))
      << "the index key holds no result object";
  EXPECT_TRUE(rejected(R"({"errors":false,"items":[{"index":{},"delete":{}}]})", 1))
      << "one entry cannot answer two operations";
  EXPECT_TRUE(rejected(R"({"errors":false,"items":[{"index":{"status":201},"delete":{}}]})", 1))
      << "a valid acknowledgement does not license a second member";
  EXPECT_TRUE(rejected(R"({"errors":false,"items":[{"index":{"_index":"logs"}}]})", 1))
      << "an index result carrying no status";
  EXPECT_TRUE(rejected(R"({"errors":true,"items":[null,null]})", 2))
      << "the same shape has to fail whichever way errors reads";

  // The shape a real acknowledgement has.
  EXPECT_FALSE(rejected(R"({"errors":false,"items":[{"index":{"status":201}}]})", 1));

  // Three shapes, three reasons: they are different things to go and look at.
  const auto reason_for = [&reason](const char *body) {
    logs_exporter::detail::IsBulkResponseSuccessful(200, body, 1, reason);
    return reason;
  };
  const std::string padded  = reason_for(R"({"errors":false,"items":[{"index":{},"delete":{}}]})");
  const std::string other   = reason_for(R"({"errors":false,"items":[{"unknown":{"status":1}}]})");
  const std::string no_stat = reason_for(R"({"errors":false,"items":[{"index":{"_index":"l"}}]})");
  EXPECT_NE(padded, other) << padded;
  EXPECT_NE(other, no_stat) << other;
  EXPECT_NE(padded, no_stat) << no_stat;
}

// The status says whether the operation applied, so the band has to hold for the number the server
// sent rather than for what it becomes on the way into an int.
TEST(ElasticsearchBulkResponseTests, RejectsAcknowledgementsOutsideTheSuccessBand)
{
  std::string reason;
  const auto applied = [&reason](const char *status) {
    const std::string body =
        std::string(R"({"errors":false,"items":[{"index":{"status":)") + status + R"(}}]})";
    return logs_exporter::detail::IsBulkResponseSuccessful(200, body, 1, reason);
  };

  EXPECT_TRUE(applied("200"));
  EXPECT_TRUE(applied("201"));
  EXPECT_TRUE(applied("299"));

  EXPECT_FALSE(applied("0")) << "zero is a status, not the absence of one";
  EXPECT_FALSE(reason.empty());
  EXPECT_FALSE(applied("-0"));
  EXPECT_FALSE(applied("-1"));
  EXPECT_FALSE(applied("199"));
  EXPECT_FALSE(applied("300"));
  EXPECT_FALSE(applied("4294967496")) << "2^32 + 200, which lands on 200 in a 32 bit int";
  EXPECT_FALSE(applied("18446744073709551615")) << "no signed type holds it";
  EXPECT_FALSE(applied("200.5")) << "not an integer";
  EXPECT_FALSE(applied(R"("200")")) << "a string is not a status";
  EXPECT_FALSE(applied("null"));

  // The value reaches the log as it arrived, so a reader sees what the server said.
  applied("4294967496");
  EXPECT_NE(reason.find("4294967496"), std::string::npos) << reason;
}

// A false flag claims every operation was applied. No conforming server sends it alongside a
// rejection, so holding the items to the claim rejects nothing the flag alone would have taken.
TEST(ElasticsearchBulkResponseTests, RejectsAnAcknowledgedFailureUnderErrorsFalse)
{
  std::string reason;
  EXPECT_FALSE(logs_exporter::detail::IsBulkResponseSuccessful(
      200, R"({"errors":false,"items":[{"index":{"status":400}}]})", 1, reason));
  EXPECT_NE(reason.find("400"), std::string::npos) << "the reason names the status: " << reason;

  EXPECT_FALSE(logs_exporter::detail::IsBulkResponseSuccessful(
      200, R"({"errors":false,"items":[{"index":{"status":201}},{"index":{"status":429}}]})", 2,
      reason))
      << "one rejected operation among several is still a rejection";

  // The whole 2xx band is a success, the same band the response's own HTTP status uses.
  EXPECT_TRUE(logs_exporter::detail::IsBulkResponseSuccessful(
      200, R"({"errors":false,"items":[{"index":{"status":200}},{"index":{"status":299}}]})", 2,
      reason));
  EXPECT_FALSE(logs_exporter::detail::IsBulkResponseSuccessful(
      200, R"({"errors":false,"items":[{"index":{"status":300}}]})", 1, reason));
}

// An operation that did not apply says so through its status and through an "error" member. The
// errors:true path already reads the second one, so reading it here as well is what stops the same
// body from being accepted or rejected according to a flag the responder also controls.
TEST(ElasticsearchBulkResponseTests, RejectsAnItemErrorUnderErrorsFalse)
{
  std::string reason;
  EXPECT_FALSE(logs_exporter::detail::IsBulkResponseSuccessful(
      200,
      R"({"errors":false,"items":[{"index":{"_index":"logs","status":201,)"
      R"("error":{"type":"mapper_parsing_exception","reason":"rejected"}}}]})",
      1, reason));
  EXPECT_NE(reason.find("mapper_parsing_exception"), std::string::npos)
      << "the reason names what the server said: " << reason;

  // A conforming server sends the flag and the member together, and that reads the same way.
  EXPECT_FALSE(logs_exporter::detail::IsBulkResponseSuccessful(
      200,
      R"({"errors":true,"items":[{"index":{"_index":"logs","status":400,)"
      R"("error":{"type":"mapper_parsing_exception"}}}]})",
      1, reason));
  EXPECT_NE(reason.find("mapper_parsing_exception"), std::string::npos) << reason;

  // Only the member decides. An acknowledgement without one stays a success.
  EXPECT_TRUE(logs_exporter::detail::IsBulkResponseSuccessful(
      200, R"({"errors":false,"items":[{"index":{"_index":"logs","status":201}}]})", 1, reason));
}
// ---------------------------------------------------------------------------
// The response travelling from the HTTP callback to the verdict.
//
// The cases above call IsBulkResponseSuccessful() directly, so they cannot show
// that the status and the body actually reach it. A fake HTTP client drives the
// callbacks from inside SendRequest(), which runs before Export() reaches the
// wait, and the assertions are on the ExportResult.
//
// This fixture is the same one #4331 adds to this file. Whichever lands first,
// the other drops the duplicate when it rebases.
// ---------------------------------------------------------------------------
namespace
{
namespace http_client = opentelemetry::ext::http::client;

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

using EventScript = std::function<void(http_client::EventHandler &)>;

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
    script_(*handler);
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
    return std::make_shared<FakeSession>(script_);
  }
  bool CancelAllSessions() noexcept override { return true; }
  bool FinishAllSessions() noexcept override { return true; }
  void SetMaxSessionsPerConnection(std::size_t) noexcept override {}

private:
  EventScript script_;
};

// The response has to answer one operation per record, so a case using a body with N items has to
// export N records or it would be rejected on the count before reaching what it means to test.
opentelemetry::sdk::common::ExportResult ExportWith(EventScript script, std::size_t records = 1)
{
  auto client = std::make_shared<FakeHttpClient>(std::move(script));
  logs_exporter::ElasticsearchExporterOptions options;
  logs_exporter::ElasticsearchLogRecordExporter exporter(options, client);

  std::vector<std::unique_ptr<sdklogs::Recordable>> batch;
  batch.reserve(records);
  for (std::size_t i = 0; i < records; ++i)
  {
    batch.push_back(exporter.MakeRecordable());
  }
  return exporter.Export(
      nostd::span<std::unique_ptr<sdklogs::Recordable>>(batch.data(), batch.size()));
}
}  // namespace

// The synchronous wait exists only when the exporter is built without async export, so these cases
// skip rather than compile out: gtest_add_tests reads the source, and a case that disappeared from
// the binary would still be registered with CTest. The skip goes in SetUp rather than at the top of
// each body, because GTEST_SKIP returns and leaves the rest of the body unreachable, which MSVC
// reports as C4702 and the maintainer mode jobs turn into an error.
namespace
{
class ElasticsearchLogsExporterWiringTests : public ::testing::Test
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

// A body the parser accepts, through the whole path rather than through the helper alone.
TEST_F(ElasticsearchLogsExporterWiringTests, AcceptedBulkResponseIsASuccessfulExport)
{
  const auto result = ExportWith([](http_client::EventHandler &handler) {
    FakeResponse response(200, kPrettySuccess);
    handler.OnResponse(response);
  });
  EXPECT_EQ(result, opentelemetry::sdk::common::ExportResult::kSuccess);
}

// The first response decides the outcome, so it has to be the one the verdict is read from. A
// client that answers twice would otherwise leave the decision with the first and the evidence
// for it with the second.
TEST_F(ElasticsearchLogsExporterWiringTests, ASecondResponseDoesNotReplaceTheOneThatDecided)
{
  const auto result = ExportWith([](http_client::EventHandler &handler) {
    FakeResponse rejected(500, kPrettySuccess);
    handler.OnResponse(rejected);
    FakeResponse accepted(200, kPrettySuccess);
    handler.OnResponse(accepted);
  });
  EXPECT_EQ(result, opentelemetry::sdk::common::ExportResult::kFailure)
      << "the 500 that decided the outcome was read back as the 200 that followed it";
}

// The case #4295 reports: a 200 whose rejected item still leaves a shard counter reading
// "failed" : 0, so the body looks successful to anything that reads it as text.
TEST_F(ElasticsearchLogsExporterWiringTests, RejectedItemIsAFailedExport)
{
  // Two records, because kOneItemRejected answers two operations. Exporting one would be rejected
  // on the acknowledgement count and the case would stop testing what it is named for.
  const auto result = ExportWith(
      [](http_client::EventHandler &handler) {
        FakeResponse response(200, kOneItemRejected);
        handler.OnResponse(response);
      },
      2);
  EXPECT_EQ(result, opentelemetry::sdk::common::ExportResult::kFailure);
}

// The status has to reach the parser, not just the body. A handler that stored a fixed status,
// or an Export() that never asked for it, would still pass every case above.
TEST_F(ElasticsearchLogsExporterWiringTests, ServerErrorIsAFailedExportEvenWithAnAcceptedBody)
{
  const auto result = ExportWith([](http_client::EventHandler &handler) {
    FakeResponse response(500, kPrettySuccess);
    handler.OnResponse(response);
  });
  EXPECT_EQ(result, opentelemetry::sdk::common::ExportResult::kFailure);
}

// ---------------------------------------------------------------------------
// Asynchronous path.
//
// Export() hands the body to AsyncResponseHandler and returns before any
// response arrives, so the parsed result decides nothing the caller can see.
// It decides which internal log line is written, which is what these cases
// read. The fake delivers the response from inside SendRequest(), so the line
// is already written by the time Export() returns.
// ---------------------------------------------------------------------------
namespace
{
struct CapturingLogHandler : public opentelemetry::sdk::common::internal_log::LogHandler
{
  void Handle(opentelemetry::sdk::common::internal_log::LogLevel level,
              const char * /* file */,
              int /* line */,
              const char *msg,
              const opentelemetry::sdk::common::AttributeMap & /* attributes */) noexcept override
  {
    messages.emplace_back(level, std::string{msg});
  }

  std::vector<std::pair<opentelemetry::sdk::common::internal_log::LogLevel, std::string>> messages;
};

class ElasticsearchLogsExporterAsyncTests : public ::testing::Test
{
protected:
  void SetUp() override
  {
#ifndef ENABLE_ASYNC_EXPORT
    GTEST_SKIP() << "Export() takes the synchronous path when async export is disabled";
#endif
    handler_ = nostd::shared_ptr<opentelemetry::sdk::common::internal_log::LogHandler>(
        new CapturingLogHandler());
    opentelemetry::sdk::common::internal_log::GlobalLogHandler::SetLogHandler(handler_);
  }

  void TearDown() override
  {
    opentelemetry::sdk::common::internal_log::GlobalLogHandler::SetLogHandler(
        nostd::shared_ptr<opentelemetry::sdk::common::internal_log::LogHandler>(
            new opentelemetry::sdk::common::internal_log::DefaultLogHandler()));
  }

  bool LoggedAnExportFailure() const
  {
    const auto &messages = static_cast<CapturingLogHandler *>(handler_.get())->messages;
    for (const auto &message : messages)
    {
      if (message.second.find("Logs were not written to Elasticsearch correctly") !=
          std::string::npos)
      {
        return true;
      }
    }
    return false;
  }

  nostd::shared_ptr<opentelemetry::sdk::common::internal_log::LogHandler> handler_;
};
}  // namespace

TEST_F(ElasticsearchLogsExporterAsyncTests, AnAcceptedResponseIsNotReportedAsAFailure)
{
  EXPECT_EQ(ExportWith([](http_client::EventHandler &handler) {
              FakeResponse response(200, kPrettySuccess);
              handler.OnResponse(response);
            }),
            opentelemetry::sdk::common::ExportResult::kSuccess);

#if OTEL_INTERNAL_LOG_LEVEL >= OTEL_INTERNAL_LOG_LEVEL_ERROR
  EXPECT_FALSE(LoggedAnExportFailure());
#endif
}

// The count reaches the parser on this path through the handler rather than through Export(), so
// a handler that dropped it would still satisfy every synchronous case.
TEST_F(ElasticsearchLogsExporterAsyncTests, ARejectedResponseIsReportedAsAFailure)
{
  // Export() reports success on this path whatever the response says, which is why the outcome has
  // to be read from the log below. Asserting it here also keeps this case from having no assertion
  // at all where the internal log level excludes the one that follows.
  EXPECT_EQ(ExportWith(
                [](http_client::EventHandler &handler) {
                  FakeResponse response(200, kOneItemRejected);
                  handler.OnResponse(response);
                },
                2),
            opentelemetry::sdk::common::ExportResult::kSuccess);

#if OTEL_INTERNAL_LOG_LEVEL >= OTEL_INTERNAL_LOG_LEVEL_ERROR
  EXPECT_TRUE(LoggedAnExportFailure());
#endif
}
