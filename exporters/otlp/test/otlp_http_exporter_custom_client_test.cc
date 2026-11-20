// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <string>
#include <thread>
#include <utility>
#include "gmock/gmock.h"

#include "opentelemetry/exporters/otlp/otlp_http_client.h"
#include "opentelemetry/exporters/otlp/otlp_http_exporter.h"
#include "opentelemetry/exporters/otlp/otlp_http_exporter_factory.h"
#include "opentelemetry/exporters/otlp/otlp_http_exporter_options.h"
#include "opentelemetry/exporters/otlp/otlp_http_exporter_runtime_options.h"
#include "opentelemetry/ext/http/client/http_client.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/common/exporter_utils.h"
#include "opentelemetry/sdk/common/thread_instrumentation.h"
#include "opentelemetry/sdk/trace/batch_span_processor.h"
#include "opentelemetry/sdk/trace/batch_span_processor_options.h"
#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/processor.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/trace/span.h"
#include "opentelemetry/trace/tracer.h"
#include "opentelemetry/version.h"

// clang-format off
#include "opentelemetry/exporters/otlp/protobuf_include_prefix.h"  // IWYU pragma: keep
#include <google/protobuf/arena.h>
#include "opentelemetry/proto/collector/trace/v1/trace_service.pb.h"
#include "opentelemetry/exporters/otlp/protobuf_include_suffix.h"  // IWYU pragma: keep
// clang-format on

#include "opentelemetry/test_common/ext/http/client/http_client_test_factory.h"
#include "opentelemetry/test_common/ext/http/client/nosend/http_client_factory_nosend.h"
#include "opentelemetry/test_common/ext/http/client/nosend/http_client_nosend.h"

using namespace testing;

namespace google
{
namespace protobuf
{
class Message;
}
}  // namespace google

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

namespace http_client = opentelemetry::ext::http::client;
using NosendHttpClientFactory =
    opentelemetry::test_common::ext::http::client::nosend::HttpClientFactoryNosend;

static OtlpHttpClientOptions MakeOtlpHttpClientOptions(
    std::chrono::system_clock::duration timeout = std::chrono::system_clock::duration::zero())
{
  std::shared_ptr<opentelemetry::sdk::common::ThreadInstrumentation> not_instrumented;
  OtlpHttpExporterOptions options;
  options.console_debug                   = true;
  options.timeout                         = timeout;
  options.retry_policy_max_attempts       = 0U;
  options.retry_policy_initial_backoff    = std::chrono::duration<float>::zero();
  options.retry_policy_max_backoff        = std::chrono::duration<float>::zero();
  options.retry_policy_backoff_multiplier = 0.0f;
  OtlpHttpClientOptions otlp_http_client_options(
      options.url, false, /* ssl_insecure_skip_verify */
      "",                 /* ssl_ca_cert_path */
      "",                 /* ssl_ca_cert_string */
      "",                 /* ssl_client_key_path */
      "",                 /* ssl_client_key_string */
      "",                 /* ssl_client_cert_path */
      "",                 /* ssl_client_cert_string */
      "",                 /* ssl_min_tls */
      "",                 /* ssl_max_tls */
      "",                 /* ssl_cipher */
      "",                 /* ssl_cipher_suite */
      options.content_type, options.json_bytes_mapping, options.compression, options.use_json_name,
      options.console_debug, options.timeout, options.http_headers,
      options.retry_policy_max_attempts, options.retry_policy_initial_backoff,
      options.retry_policy_max_backoff, options.retry_policy_backoff_multiplier, not_instrumented);
  otlp_http_client_options.max_concurrent_requests = 0;
  return otlp_http_client_options;
}

class OtlpHttpExporterCustomClientTestPeer : public ::testing::Test
{
public:
  std::unique_ptr<sdk::trace::SpanExporter> GetExporter(std::unique_ptr<OtlpHttpClient> http_client)
  {
    return std::unique_ptr<sdk::trace::SpanExporter>(new OtlpHttpExporter(std::move(http_client)));
  }

  static std::pair<OtlpHttpClient *, std::shared_ptr<http_client::HttpClient>>
  GetMockOtlpHttpClient()
  {
    auto http_client = http_client::HttpClientTestFactory::Create();
    return {new OtlpHttpClient(MakeOtlpHttpClientOptions(), http_client), http_client};
  }

  // Records the outcome. A request that never settles calls back no times and one that settles
  // twice calls back twice, so both failures show in the count rather than only in a timeout.
  static void ExportOneRequest(OtlpHttpClient &otlp_client,
                               const std::shared_ptr<std::atomic<int>> &calls,
                               const std::shared_ptr<sdk::common::ExportResult> &result)
  {
    auto arena = std::make_unique<google::protobuf::Arena>();
    auto *request =
        google::protobuf::Arena::Create<proto::collector::trace::v1::ExportTraceServiceRequest>(
            arena.get());
    auto *response =
        google::protobuf::Arena::Create<proto::collector::trace::v1::ExportTraceServiceResponse>(
            arena.get());

    otlp_client.Export(
        *request, std::move(arena), response,
        [calls, result](opentelemetry::sdk::common::ExportResult outcome,
                        google::protobuf::Message *) {
          calls->fetch_add(1, std::memory_order_release);
          *result = outcome;
          return true;
        },
        1);
  }

  // Friendship is granted to this fixture and does not extend to the class TEST_F derives from
  // it, so the two private things a case needs are read through these.
  static std::uint64_t FlushCutoff(const OtlpHttpClient &otlp_client)
  {
    return otlp_client.start_session_counter_.load(std::memory_order_acquire);
  }

  static bool SnapshotSatisfied(OtlpHttpClient &otlp_client, std::uint64_t cutoff)
  {
    return otlp_client.allSessionsBeforeFinished(cutoff);
  }

  // A non-zero request budget keeps the export asynchronous, so it returns while the session runs.
  // The budget has to cover every session still registered, not just this one: Export registers
  // first and then waits for the registry to fall back inside the budget, so too small a budget
  // makes the call run the client timeout down and report failure.
  static opentelemetry::sdk::common::ExportResult ExportOneRequest(
      OtlpHttpClient &otlp_client,
      std::size_t max_running_requests = 1)
  {
    auto arena = std::make_unique<google::protobuf::Arena>();
    auto *request =
        google::protobuf::Arena::Create<proto::collector::trace::v1::ExportTraceServiceRequest>(
            arena.get());
    auto *response =
        google::protobuf::Arena::Create<proto::collector::trace::v1::ExportTraceServiceResponse>(
            arena.get());

    return otlp_client.Export(
        *request, std::move(arena), response,
        [](opentelemetry::sdk::common::ExportResult, google::protobuf::Message *) { return true; },
        max_running_requests);
  }
};

TEST_F(OtlpHttpExporterCustomClientTestPeer, FactoryInjectionCreatesExporter)
{
  OtlpHttpExporterOptions opts;
  auto factory  = std::make_shared<NosendHttpClientFactory>();
  auto exporter = OtlpHttpExporterFactory::Create(opts, std::move(factory));
  ASSERT_NE(exporter, nullptr);
}

TEST_F(OtlpHttpExporterCustomClientTestPeer, HttpClientInjectionCreatesExporter)
{
  OtlpHttpExporterOptions opts;
  auto client   = http_client::HttpClientTestFactory::Create();
  auto exporter = OtlpHttpExporterFactory::Create(opts, std::move(client));
  ASSERT_NE(exporter, nullptr);
}

TEST_F(OtlpHttpExporterCustomClientTestPeer, RuntimeOptionsWithFactoryCreatesExporter)
{
  OtlpHttpExporterOptions opts;
  OtlpHttpExporterRuntimeOptions runtime_opts;
  auto factory  = std::make_shared<NosendHttpClientFactory>();
  auto exporter = OtlpHttpExporterFactory::Create(opts, runtime_opts, std::move(factory));
  ASSERT_NE(exporter, nullptr);
}

TEST_F(OtlpHttpExporterCustomClientTestPeer, RuntimeOptionsWithHttpClientCreatesExporter)
{
  OtlpHttpExporterOptions opts;
  OtlpHttpExporterRuntimeOptions runtime_opts;
  auto client   = http_client::HttpClientTestFactory::Create();
  auto exporter = OtlpHttpExporterFactory::Create(opts, runtime_opts, std::move(client));
  ASSERT_NE(exporter, nullptr);
}

TEST_F(OtlpHttpExporterCustomClientTestPeer, ExportCallsSendRequest)
{
  auto mock_otlp_client = GetMockOtlpHttpClient();
  auto client           = mock_otlp_client.second;
  auto exporter         = GetExporter(std::unique_ptr<OtlpHttpClient>{mock_otlp_client.first});

  sdk::trace::BatchSpanProcessorOptions processor_opts;
  processor_opts.max_export_batch_size = 5;
  processor_opts.max_queue_size        = 5;
  processor_opts.schedule_delay_millis = std::chrono::milliseconds(256);

  std::unique_ptr<sdk::trace::SpanProcessor> processor =
      std::make_unique<sdk::trace::BatchSpanProcessor>(std::move(exporter), processor_opts);
  auto provider = nostd::shared_ptr<sdk::trace::TracerProvider>(
      new sdk::trace::TracerProvider(std::move(processor)));

  auto tracer = provider->GetTracer("test");
  auto span   = tracer->StartSpan("custom-client-span");
  span->End();

  auto no_send_client = std::static_pointer_cast<http_client::nosend::HttpClient>(client);
  EXPECT_CALL(*std::static_pointer_cast<http_client::nosend::Session>(no_send_client->session_),
              SendRequest)
      .WillRepeatedly(
          [](const std::shared_ptr<opentelemetry::ext::http::client::EventHandler> &callback) {
            http_client::nosend::Response response;
            response.Finish(*callback);
          });

  provider->ForceFlush();
}

TEST_F(OtlpHttpExporterCustomClientTestPeer, ForceFlushReturnsWithinTheCallerDeadline)
{
  auto client         = http_client::HttpClientTestFactory::Create();
  auto no_send_client = std::static_pointer_cast<http_client::nosend::HttpClient>(client);
  auto session = std::static_pointer_cast<http_client::nosend::Session>(no_send_client->session_);

  // Hold the request open, so ForceFlush has something to wait for.
  std::shared_ptr<opentelemetry::ext::http::client::EventHandler> pending;
  EXPECT_CALL(*session, SendRequest)
      .WillRepeatedly(
          [&pending](std::shared_ptr<opentelemetry::ext::http::client::EventHandler> callback) {
            pending = std::move(callback);
          });

  // A client timeout far longer than the deadline ForceFlush is given below.
  OtlpHttpClient otlp_client(MakeOtlpHttpClientOptions(std::chrono::seconds{30}), client);

  ExportOneRequest(otlp_client);
  ASSERT_NE(pending, nullptr);

  const auto started = std::chrono::steady_clock::now();
  const bool flushed = otlp_client.ForceFlush(std::chrono::milliseconds{50});
  const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                           std::chrono::steady_clock::now() - started)
                           .count();

  EXPECT_FALSE(flushed);
  EXPECT_LT(elapsed, 1000) << "waited on the client timeout rather than the one it was given";

  // Complete the export, or the client destructor waits on it.
  http_client::nosend::Response sent;
  sent.Finish(*pending);
}

// The result is the finished counter alone, so a flush with nothing left to wait for has to say
// so. Guards the counter against the session bookkeeping it is derived from.
TEST_F(OtlpHttpExporterCustomClientTestPeer, ForceFlushReportsSuccessOnceTheSessionIsDone)
{
  auto client         = http_client::HttpClientTestFactory::Create();
  auto no_send_client = std::static_pointer_cast<http_client::nosend::HttpClient>(client);
  auto session = std::static_pointer_cast<http_client::nosend::Session>(no_send_client->session_);

  std::shared_ptr<opentelemetry::ext::http::client::EventHandler> pending;
  EXPECT_CALL(*session, SendRequest)
      .WillRepeatedly(
          [&pending](std::shared_ptr<opentelemetry::ext::http::client::EventHandler> callback) {
            pending = std::move(callback);
          });

  OtlpHttpClient otlp_client(MakeOtlpHttpClientOptions(std::chrono::seconds{30}), client);

  ExportOneRequest(otlp_client);
  ASSERT_NE(pending, nullptr);

  http_client::nosend::Response sent;
  sent.Finish(*pending);

  EXPECT_TRUE(otlp_client.ForceFlush(std::chrono::milliseconds{50}));
}

// ForceFlush reports on two lifetime totals, started and finished, so it asks whether as many
// requests have finished as had started when it was entered, not whether the ones it is waiting
// for are among them. A request that starts after the call and finishes before the pending one
// satisfies that count and the flush reports success with the first request still in flight.
//
// Nothing between starting the flush thread and joining it may assert. A gtest failure returns
// from the case, a joinable std::thread terminates the process on destruction, and
// ~OtlpHttpClient waits for the registry to empty with no deadline. So both cases below complete
// every request and join, and only then assert.
TEST_F(OtlpHttpExporterCustomClientTestPeer, ForceFlushDoesNotAcceptALaterRequestInPlaceOfItsOwn)
{
  auto client         = http_client::HttpClientTestFactory::Create();
  auto no_send_client = std::static_pointer_cast<http_client::nosend::HttpClient>(client);
  auto first = std::static_pointer_cast<http_client::nosend::Session>(no_send_client->session_);

  std::shared_ptr<opentelemetry::ext::http::client::EventHandler> first_pending;
  EXPECT_CALL(*first, SendRequest)
      .WillRepeatedly(
          [&first_pending](std::shared_ptr<opentelemetry::ext::http::client::EventHandler> cb) {
            first_pending = std::move(cb);
          });

  // A short client timeout, so the bounded wait re-reads its predicate often.
  OtlpHttpClient otlp_client(MakeOtlpHttpClientOptions(std::chrono::milliseconds{10}), client);

  const auto first_result = ExportOneRequest(otlp_client, 2);

  // Entered with the first request pending, so this is the request it has to wait for. How long it
  // waits is the other half of the answer: ending early is what accepting the wrong completion
  // looks like, so the duration is asserted as well as the result.
  bool flushed = true;
  std::chrono::steady_clock::duration waited{};
  std::thread flusher([&otlp_client, &flushed, &waited] {
    const auto entered = std::chrono::steady_clock::now();
    flushed            = otlp_client.ForceFlush(std::chrono::milliseconds{500});
    waited             = std::chrono::steady_clock::now() - entered;
  });
  std::this_thread::sleep_for(std::chrono::milliseconds{200});

  // A second request, started after the flush. The nosend client hands out one session, and the
  // running sessions are keyed by session pointer, so a second session object is what makes this a
  // second entry rather than an overwrite of the first.
  auto second = std::make_shared<http_client::nosend::Session>(*no_send_client);
  std::shared_ptr<opentelemetry::ext::http::client::EventHandler> second_pending;
  EXPECT_CALL(*second, SendRequest)
      .WillRepeatedly(
          [&second_pending](std::shared_ptr<opentelemetry::ext::http::client::EventHandler> cb) {
            second_pending = std::move(cb);
          });
  no_send_client->session_ = second;

  const auto second_result = ExportOneRequest(otlp_client, 2);

  if (second_pending)
  {
    http_client::nosend::Response second_sent;
    second_sent.Finish(*second_pending);
  }

  flusher.join();
  if (first_pending)
  {
    http_client::nosend::Response first_sent;
    first_sent.Finish(*first_pending);
  }

  const auto waited_ms = std::chrono::duration_cast<std::chrono::milliseconds>(waited).count();
  EXPECT_EQ(opentelemetry::sdk::common::ExportResult::kSuccess, first_result);
  EXPECT_EQ(opentelemetry::sdk::common::ExportResult::kSuccess, second_result);
  EXPECT_NE(first_pending, nullptr);
  EXPECT_NE(second_pending, nullptr);
  EXPECT_FALSE(flushed) << "reported success while the request pending when it was entered was "
                           "still in flight";
  EXPECT_GE(waited_ms, 400) << "ended on the second request's completion rather than waiting for "
                               "the first";
}

// Finishes whatever a case left registered, so that a fatal assertion ends the case instead of
// hanging it. ~OtlpHttpClient waits for the registry to empty with no deadline, and the nosend
// client delivers no completion of its own, so an early return with a session still registered
// never comes back. Finishing an already finished handler does nothing, since ResponseHandler
// clears its owner and session before releasing. Declare after the client, so this runs first.
namespace
{
class FinishPendingOnExit
{
public:
  FinishPendingOnExit(
      std::shared_ptr<opentelemetry::ext::http::client::EventHandler> &first,
      std::shared_ptr<opentelemetry::ext::http::client::EventHandler> &second) noexcept
      : handlers_{&first, &second}
  {}

  ~FinishPendingOnExit()
  {
    for (auto *handler : handlers_)
    {
      if (*handler)
      {
        http_client::nosend::Response done;
        done.Finish(**handler);
      }
    }
  }

  FinishPendingOnExit(const FinishPendingOnExit &)            = delete;
  FinishPendingOnExit &operator=(const FinishPendingOnExit &) = delete;
  FinishPendingOnExit(FinishPendingOnExit &&)                 = delete;
  FinishPendingOnExit &operator=(FinishPendingOnExit &&)      = delete;

private:
  std::shared_ptr<opentelemetry::ext::http::client::EventHandler> *handlers_[2]{};
};
}  // namespace

// The cutoff a flush takes on entry, asked about directly. Driving these through ForceFlush needs
// the second request to register after the flush has read the cutoff, and nothing a test can
// observe says when that happened, so the order here is sequential rather than arranged with a
// sleep. Two cases, because the two directions cannot share one sequence: whichever request is
// completed first is no longer in the registry for the other question.
TEST_F(OtlpHttpExporterCustomClientTestPeer, ACompletionFromALaterRequestDoesNotSatisfyTheSnapshot)
{
  auto client         = http_client::HttpClientTestFactory::Create();
  auto no_send_client = std::static_pointer_cast<http_client::nosend::HttpClient>(client);
  auto first = std::static_pointer_cast<http_client::nosend::Session>(no_send_client->session_);

  std::shared_ptr<opentelemetry::ext::http::client::EventHandler> first_pending;
  std::shared_ptr<opentelemetry::ext::http::client::EventHandler> second_pending;
  EXPECT_CALL(*first, SendRequest)
      .WillRepeatedly(
          [&first_pending](std::shared_ptr<opentelemetry::ext::http::client::EventHandler> cb) {
            first_pending = std::move(cb);
          });

  OtlpHttpClient otlp_client(MakeOtlpHttpClientOptions(std::chrono::milliseconds{10}), client);
  FinishPendingOnExit finish_pending{first_pending, second_pending};

  ASSERT_EQ(opentelemetry::sdk::common::ExportResult::kSuccess, ExportOneRequest(otlp_client, 2));
  ASSERT_NE(first_pending, nullptr);

  // What ForceFlush would read on entry, with the first request already registered.
  const std::uint64_t cutoff = FlushCutoff(otlp_client);

  auto second = std::make_shared<http_client::nosend::Session>(*no_send_client);
  EXPECT_CALL(*second, SendRequest)
      .WillRepeatedly(
          [&second_pending](std::shared_ptr<opentelemetry::ext::http::client::EventHandler> cb) {
            second_pending = std::move(cb);
          });
  no_send_client->session_ = second;

  ASSERT_EQ(opentelemetry::sdk::common::ExportResult::kSuccess, ExportOneRequest(otlp_client, 2));
  ASSERT_NE(second_pending, nullptr);

  ASSERT_FALSE(SnapshotSatisfied(otlp_client, cutoff))
      << "the request below the cutoff is still in flight";

  http_client::nosend::Response second_sent;
  second_sent.Finish(*second_pending);
  EXPECT_FALSE(SnapshotSatisfied(otlp_client, cutoff))
      << "a completion from a request started after the cutoff satisfied the snapshot";

  http_client::nosend::Response first_sent;
  first_sent.Finish(*first_pending);
}

// The direction that a flush waiting for the whole registry to empty would get wrong, which is why
// the later request is left in flight for the assertion rather than completed first.
TEST_F(OtlpHttpExporterCustomClientTestPeer, ALaterRequestStillInFlightDoesNotHoldTheSnapshotBack)
{
  auto client         = http_client::HttpClientTestFactory::Create();
  auto no_send_client = std::static_pointer_cast<http_client::nosend::HttpClient>(client);
  auto first = std::static_pointer_cast<http_client::nosend::Session>(no_send_client->session_);

  std::shared_ptr<opentelemetry::ext::http::client::EventHandler> first_pending;
  std::shared_ptr<opentelemetry::ext::http::client::EventHandler> second_pending;
  EXPECT_CALL(*first, SendRequest)
      .WillRepeatedly(
          [&first_pending](std::shared_ptr<opentelemetry::ext::http::client::EventHandler> cb) {
            first_pending = std::move(cb);
          });

  OtlpHttpClient otlp_client(MakeOtlpHttpClientOptions(std::chrono::milliseconds{10}), client);
  FinishPendingOnExit finish_pending{first_pending, second_pending};

  ASSERT_EQ(opentelemetry::sdk::common::ExportResult::kSuccess, ExportOneRequest(otlp_client, 2));
  ASSERT_NE(first_pending, nullptr);

  // What ForceFlush would read on entry, with the first request already registered.
  const std::uint64_t cutoff = FlushCutoff(otlp_client);

  auto second = std::make_shared<http_client::nosend::Session>(*no_send_client);
  EXPECT_CALL(*second, SendRequest)
      .WillRepeatedly(
          [&second_pending](std::shared_ptr<opentelemetry::ext::http::client::EventHandler> cb) {
            second_pending = std::move(cb);
          });
  no_send_client->session_ = second;

  ASSERT_EQ(opentelemetry::sdk::common::ExportResult::kSuccess, ExportOneRequest(otlp_client, 2));
  ASSERT_NE(second_pending, nullptr);

  ASSERT_FALSE(SnapshotSatisfied(otlp_client, cutoff))
      << "the request below the cutoff is still in flight";

  http_client::nosend::Response first_sent;
  first_sent.Finish(*first_pending);
  EXPECT_TRUE(SnapshotSatisfied(otlp_client, cutoff))
      << "the snapshot was not satisfied once the request below the cutoff finished, with a later "
         "one still in flight";
  EXPECT_NE(second_pending, nullptr);

  http_client::nosend::Response second_sent;
  second_sent.Finish(*second_pending);
}

// A client that finishes a transfer on ReadError, WriteError or Destroyed ends the request: the
// result callback runs once, and ForceFlush returns rather than waiting out its deadline.
TEST_F(OtlpHttpExporterCustomClientTestPeer, ATerminalClientEventEndsTheRequest)
{
  for (const auto state :
       {http_client::SessionState::ReadError, http_client::SessionState::WriteError,
        http_client::SessionState::Destroyed})
  {
    SCOPED_TRACE(static_cast<int>(state));

    auto client         = http_client::HttpClientTestFactory::Create();
    auto no_send_client = std::static_pointer_cast<http_client::nosend::HttpClient>(client);
    auto session = std::static_pointer_cast<http_client::nosend::Session>(no_send_client->session_);

    std::shared_ptr<opentelemetry::ext::http::client::EventHandler> pending;
    EXPECT_CALL(*session, SendRequest)
        .WillRepeatedly(
            [&pending](std::shared_ptr<opentelemetry::ext::http::client::EventHandler> callback) {
              pending = std::move(callback);
            });

    OtlpHttpClient otlp_client(MakeOtlpHttpClientOptions(std::chrono::seconds{30}), client);

    auto calls  = std::make_shared<std::atomic<int>>(0);
    auto result = std::make_shared<sdk::common::ExportResult>(sdk::common::ExportResult::kSuccess);
    ExportOneRequest(otlp_client, calls, result);
    ASSERT_NE(pending, nullptr);

    pending->OnEvent(state, "");

    EXPECT_EQ(1, calls->load(std::memory_order_acquire))
        << "the request was not ended by the state the client finished on";
    EXPECT_EQ(sdk::common::ExportResult::kFailure, *result);

    // The deadline is short on purpose. Without the request being ended above this waits it out.
    EXPECT_TRUE(otlp_client.ForceFlush(std::chrono::milliseconds{50}));
  }
}

// The other direction. A client is free to report one of those states after it has already
// delivered a response, and the outcome the caller was given must not be replaced or repeated.
TEST_F(OtlpHttpExporterCustomClientTestPeer, ALateTerminalEventDoesNotReportASecondTime)
{
  for (const auto state :
       {http_client::SessionState::ReadError, http_client::SessionState::WriteError,
        http_client::SessionState::Destroyed})
  {
    SCOPED_TRACE(static_cast<int>(state));

    auto client         = http_client::HttpClientTestFactory::Create();
    auto no_send_client = std::static_pointer_cast<http_client::nosend::HttpClient>(client);
    auto session = std::static_pointer_cast<http_client::nosend::Session>(no_send_client->session_);

    std::shared_ptr<opentelemetry::ext::http::client::EventHandler> pending;
    EXPECT_CALL(*session, SendRequest)
        .WillRepeatedly(
            [&pending](std::shared_ptr<opentelemetry::ext::http::client::EventHandler> callback) {
              pending = std::move(callback);
            });

    OtlpHttpClient otlp_client(MakeOtlpHttpClientOptions(std::chrono::seconds{30}), client);

    auto calls  = std::make_shared<std::atomic<int>>(0);
    auto result = std::make_shared<sdk::common::ExportResult>(sdk::common::ExportResult::kFailure);
    ExportOneRequest(otlp_client, calls, result);
    ASSERT_NE(pending, nullptr);

    http_client::nosend::Response sent;
    sent.Finish(*pending);
    ASSERT_EQ(1, calls->load(std::memory_order_acquire));

    pending->OnEvent(state, "");

    EXPECT_EQ(1, calls->load(std::memory_order_acquire))
        << "a state arriving after the response reported the request a second time";
  }
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
