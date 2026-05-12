// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef OPENTELEMETRY_STL_VERSION

#  include <chrono>

#  include "opentelemetry/exporters/otlp/otlp_http_client.h"
#  include "opentelemetry/exporters/otlp/otlp_http_exporter.h"
#  include "opentelemetry/exporters/otlp/otlp_http_exporter_options.h"
#  include "opentelemetry/sdk/trace/batch_span_processor.h"
#  include "opentelemetry/sdk/trace/batch_span_processor_options.h"
#  include "opentelemetry/sdk/trace/tracer_provider.h"
#  include "opentelemetry/test_common/ext/http/client/http_client_test_factory.h"
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

static OtlpHttpClientOptions MakeOtlpHttpClientOptions()
{
  std::shared_ptr<opentelemetry::sdk::common::ThreadInstrumentation> not_instrumented;
  OtlpHttpExporterOptions options;
  options.console_debug                   = true;
  options.timeout                         = std::chrono::system_clock::duration::zero();
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
};

TEST_F(OtlpHttpExporterCustomClientTestPeer, ExportCallsSendRequest)
{
  auto mock_otlp_client = GetMockOtlpHttpClient();
  auto client           = mock_otlp_client.second;
  auto exporter         = GetExporter(std::unique_ptr<OtlpHttpClient>{mock_otlp_client.first});

  sdk::trace::BatchSpanProcessorOptions processor_opts;
  processor_opts.max_export_batch_size = 5;
  processor_opts.max_queue_size        = 5;
  processor_opts.schedule_delay_millis = std::chrono::milliseconds(256);

  auto processor = std::unique_ptr<sdk::trace::SpanProcessor>(
      new sdk::trace::BatchSpanProcessor(std::move(exporter), processor_opts));
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

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE

#endif  // OPENTELEMETRY_STL_VERSION
