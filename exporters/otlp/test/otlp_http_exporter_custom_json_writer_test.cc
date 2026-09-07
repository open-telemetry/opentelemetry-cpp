// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include "gmock/gmock.h"

#include "opentelemetry/exporters/otlp/detail/default_json_writer_factory.h"
#include "opentelemetry/exporters/otlp/otlp_http_client.h"
#include "opentelemetry/exporters/otlp/otlp_http_exporter.h"
#include "opentelemetry/exporters/otlp/otlp_http_exporter_factory.h"
#include "opentelemetry/exporters/otlp/otlp_http_exporter_options.h"
#include "opentelemetry/exporters/otlp/otlp_http_exporter_runtime_options.h"
#include "opentelemetry/exporters/otlp/otlp_json_writer.h"
#include "opentelemetry/exporters/otlp/otlp_json_writer_factory.h"
#include "opentelemetry/ext/http/client/http_client.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/common/exporter_utils.h"
#include "opentelemetry/sdk/trace/batch_span_processor.h"
#include "opentelemetry/sdk/trace/batch_span_processor_options.h"
#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/processor.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/trace/tracer.h"
#include "opentelemetry/version.h"
#include "otlp_marking_json_writer.h"

#include "opentelemetry/test_common/ext/http/client/http_client_test_factory.h"
#include "opentelemetry/test_common/ext/http/client/nosend/http_client_factory_nosend.h"
#include "opentelemetry/test_common/ext/http/client/nosend/http_client_nosend.h"

using namespace testing;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

namespace http_client = opentelemetry::ext::http::client;

class OtlpHttpExporterCustomJsonWriterTestPeer : public ::testing::Test
{};

TEST_F(OtlpHttpExporterCustomJsonWriterTestPeer, RuntimeOptionsInjectionCreatesExporter)
{
  OtlpHttpExporterOptions opts;
  OtlpHttpExporterRuntimeOptions runtime_opts;
  runtime_opts.json_writer_factory = std::make_shared<MarkingJsonWriterFactory>();
  auto exporter                    = OtlpHttpExporterFactory::Create(opts, runtime_opts);
  ASSERT_NE(exporter, nullptr);
}

TEST_F(OtlpHttpExporterCustomJsonWriterTestPeer, RuntimeOptionsWithHttpClientCreatesExporter)
{
  OtlpHttpExporterOptions opts;
  OtlpHttpExporterRuntimeOptions runtime_opts;
  runtime_opts.json_writer_factory = std::make_shared<MarkingJsonWriterFactory>();
  auto client                      = http_client::HttpClientTestFactory::Create();
  auto exporter = OtlpHttpExporterFactory::Create(opts, runtime_opts, std::move(client));
  ASSERT_NE(exporter, nullptr);
}

TEST_F(OtlpHttpExporterCustomJsonWriterTestPeer, ExportUsesTheInjectedJsonWriter)
{
  auto client         = http_client::HttpClientTestFactory::Create();
  auto no_send_client = std::static_pointer_cast<http_client::nosend::HttpClient>(client);
  auto session = std::static_pointer_cast<http_client::nosend::Session>(no_send_client->session_);

  OtlpHttpExporterOptions options;
  options.content_type = HttpRequestContentType::kJson;
  OtlpHttpExporterRuntimeOptions runtime_options;
  runtime_options.json_writer_factory = std::make_shared<MarkingJsonWriterFactory>();
  auto exporter = OtlpHttpExporterFactory::Create(options, runtime_options, std::move(client));

  std::string captured_body;
  EXPECT_CALL(*session, SendRequest)
      .WillRepeatedly(
          [&session, &captured_body](
              const std::shared_ptr<opentelemetry::ext::http::client::EventHandler> &callback) {
            captured_body = session->GetRequest()->body_.empty()
                                ? std::string()
                                : std::string(session->GetRequest()->body_.begin(),
                                              session->GetRequest()->body_.end());
            http_client::nosend::Response response;
            response.Finish(*callback);
          });

  sdk::trace::BatchSpanProcessorOptions processor_opts;
  processor_opts.max_export_batch_size = 5;
  processor_opts.max_queue_size        = 5;
  processor_opts.schedule_delay_millis = std::chrono::milliseconds(256);

  std::unique_ptr<sdk::trace::SpanProcessor> processor =
      std::make_unique<sdk::trace::BatchSpanProcessor>(std::move(exporter), processor_opts);
  auto provider = nostd::shared_ptr<sdk::trace::TracerProvider>(
      new sdk::trace::TracerProvider(std::move(processor)));

  auto tracer = provider->GetTracer("test");
  auto span   = tracer->StartSpan("custom-json-writer-span");
  span->End();

  provider->ForceFlush();

  ASSERT_FALSE(captured_body.empty());
  EXPECT_EQ(captured_body.rfind("/*custom-writer*/", 0), 0u)
      << "request body was not produced by the injected JsonWriter: " << captured_body;
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
