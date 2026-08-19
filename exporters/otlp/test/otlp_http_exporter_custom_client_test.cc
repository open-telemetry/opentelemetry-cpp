// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef OPENTELEMETRY_STL_VERSION

#  include <atomic>
#  include <chrono>
#  include <memory>
#  include <utility>

#  include "opentelemetry/exporters/otlp/otlp_http_client.h"
#  include "opentelemetry/exporters/otlp/otlp_http_exporter.h"
#  include "opentelemetry/exporters/otlp/otlp_http_exporter_factory.h"
#  include "opentelemetry/exporters/otlp/otlp_http_exporter_options.h"
#  include "opentelemetry/exporters/otlp/otlp_http_exporter_runtime_options.h"

#  include "opentelemetry/exporters/otlp/protobuf_include_prefix.h"

#  include <google/protobuf/arena.h>
#  include "opentelemetry/proto/collector/trace/v1/trace_service.pb.h"

#  include "opentelemetry/exporters/otlp/protobuf_include_suffix.h"

#  include "opentelemetry/ext/http/client/http_client.h"
#  include "opentelemetry/ext/http/client/http_client_factory.h"
#  include "opentelemetry/sdk/common/exporter_utils.h"
#  include "opentelemetry/sdk/trace/batch_span_processor.h"
#  include "opentelemetry/sdk/trace/batch_span_processor_options.h"
#  include "opentelemetry/sdk/trace/tracer_provider.h"
#  include "opentelemetry/test_common/ext/http/client/http_client_test_factory.h"
#  include "opentelemetry/test_common/ext/http/client/nosend/http_client_factory_nosend.h"
#  include "opentelemetry/test_common/ext/http/client/nosend/http_client_nosend.h"

#  include <gtest/gtest.h>
#  include "gmock/gmock.h"

using namespace testing;

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

  // The same, with the outcome recorded. A request that never settles calls this no times, and one
  // that settles twice calls it twice, so both failures the terminal states can produce are
  // visible in the count rather than only in a timeout.
  static void ExportOneRequest(OtlpHttpClient &otlp_client,
                               std::shared_ptr<std::atomic<int>> calls,
                               std::shared_ptr<sdk::common::ExportResult> result)
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

  // A non-zero request budget keeps the export asynchronous, so it returns while the session runs.
  static void ExportOneRequest(OtlpHttpClient &otlp_client)
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
        [](opentelemetry::sdk::common::ExportResult, google::protobuf::Message *) { return true; },
        1);
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

// Every state the client can finish a request on has to end that request. These three used to be
// logged and then dropped: the switch that decides whether the request is over did not name them,
// so a client that ended a transfer this way left the caller waiting for a callback that was not
// coming, and ForceFlush and Shutdown could only give up on their own deadlines.
class OtlpHttpTerminalStateTest : public OtlpHttpExporterCustomClientTestPeer,
                                  public ::testing::WithParamInterface<http_client::SessionState>
{};

TEST_P(OtlpHttpTerminalStateTest, ATerminalClientEventEndsTheRequest)
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

  auto calls  = std::make_shared<std::atomic<int>>(0);
  auto result = std::make_shared<sdk::common::ExportResult>(sdk::common::ExportResult::kSuccess);
  ExportOneRequest(otlp_client, calls, result);
  ASSERT_NE(pending, nullptr);

  pending->OnEvent(GetParam(), "");

  EXPECT_EQ(1, calls->load(std::memory_order_acquire))
      << "the request was not ended by the state the client finished on";
  EXPECT_EQ(sdk::common::ExportResult::kFailure, *result);

  // The deadline is short on purpose. Without the request being ended above this waits it out.
  EXPECT_TRUE(otlp_client.ForceFlush(std::chrono::milliseconds{50}));
}

INSTANTIATE_TEST_SUITE_P(TerminalStates,
                         OtlpHttpTerminalStateTest,
                         ::testing::Values(http_client::SessionState::ReadError,
                                           http_client::SessionState::WriteError,
                                           http_client::SessionState::Destroyed));

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

#endif  // OPENTELEMETRY_STL_VERSION
