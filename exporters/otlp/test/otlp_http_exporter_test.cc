// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef OPENTELEMETRY_STL_VERSION

#  include <chrono>
#  include <thread>

#  include "opentelemetry/exporters/otlp/otlp_http_exporter.h"

#  include "opentelemetry/exporters/otlp/protobuf_include_prefix.h"

#  include "opentelemetry/proto/collector/trace/v1/trace_service.pb.h"

#  include "opentelemetry/exporters/otlp/protobuf_include_suffix.h"

#  include "opentelemetry/ext/http/client/http_client_factory.h"
#  include "opentelemetry/ext/http/server/http_server.h"
#  include "opentelemetry/sdk/trace/batch_span_processor.h"
#  include "opentelemetry/sdk/trace/batch_span_processor_options.h"
#  include "opentelemetry/sdk/trace/tracer_provider.h"
#  include "opentelemetry/test_common/ext/http/client/http_client_test_factory.h"
#  include "opentelemetry/test_common/ext/http/client/nosend/http_client_nosend.h"
#  include "opentelemetry/trace/provider.h"

#  include <google/protobuf/message_lite.h>
#  include <gtest/gtest.h>
#  include "gmock/gmock.h"

#  include "nlohmann/json.hpp"

#  if defined(_MSC_VER)
#    include "opentelemetry/sdk/common/env_variables.h"
using opentelemetry::sdk::common::setenv;
using opentelemetry::sdk::common::unsetenv;
#  endif
using namespace testing;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

namespace trace_api = opentelemetry::trace;
namespace resource  = opentelemetry::sdk::resource;

template <class T, size_t N>
static nostd::span<T, N> MakeSpan(T (&array)[N])
{
  return nostd::span<T, N>(array);
}

OtlpHttpClientOptions MakeOtlpHttpClientOptions(HttpRequestContentType content_type,
                                                bool async_mode)
{
  OtlpHttpExporterOptions options;
  options.content_type  = content_type;
  options.console_debug = true;
  options.timeout       = std::chrono::system_clock::duration::zero();
  options.http_headers.insert(
      std::make_pair<const std::string, std::string>("Custom-Header-Key", "Custom-Header-Value"));
  OtlpHttpClientOptions otlp_http_client_options(
      options.url, false,                 /* ssl_insecure_skip_verify */
      "", /* ssl_ca_cert_path */ "",      /* ssl_ca_cert_string */
      "",                                 /* ssl_client_key_path */
      "", /* ssl_client_key_string */ "", /* ssl_client_cert_path */
      "",                                 /* ssl_client_cert_string */
      "",                                 /* ssl_min_tls */
      "",                                 /* ssl_max_tls */
      "",                                 /* ssl_cipher */
      "",                                 /* ssl_cipher_suite */
      options.content_type, options.json_bytes_mapping, options.use_json_name,
      options.console_debug, options.timeout, options.http_headers);
  if (!async_mode)
  {
    otlp_http_client_options.max_concurrent_requests = 0;
  }
  return otlp_http_client_options;
}

namespace http_client = opentelemetry::ext::http::client;

class OtlpHttpExporterTestPeer : public ::testing::Test
{
public:
  std::unique_ptr<sdk::trace::SpanExporter> GetExporter(std::unique_ptr<OtlpHttpClient> http_client)
  {
    return std::unique_ptr<sdk::trace::SpanExporter>(new OtlpHttpExporter(std::move(http_client)));
  }

  // Get the options associated with the given exporter.
  const OtlpHttpExporterOptions &GetOptions(std::unique_ptr<OtlpHttpExporter> &exporter)
  {
    return exporter->options_;
  }

  static std::pair<OtlpHttpClient *, std::shared_ptr<http_client::HttpClient>>
  GetMockOtlpHttpClient(HttpRequestContentType content_type, bool async_mode = false)
  {
    auto http_client = http_client::HttpClientTestFactory::Create();
    return {new OtlpHttpClient(MakeOtlpHttpClientOptions(content_type, async_mode), http_client),
            http_client};
  }

  void ExportJsonIntegrationTest()
  {
    auto mock_otlp_client =
        OtlpHttpExporterTestPeer::GetMockOtlpHttpClient(HttpRequestContentType::kJson);
    auto mock_otlp_http_client = mock_otlp_client.first;
    auto client                = mock_otlp_client.second;
    auto exporter = GetExporter(std::unique_ptr<OtlpHttpClient>{mock_otlp_http_client});

    resource::ResourceAttributes resource_attributes = {{"service.name", "unit_test_service"},
                                                        {"tenant.id", "test_user"}};
    resource_attributes["bool_value"]                = true;
    resource_attributes["int32_value"]               = static_cast<int32_t>(1);
    resource_attributes["uint32_value"]              = static_cast<uint32_t>(2);
    resource_attributes["int64_value"]               = static_cast<int64_t>(0x1100000000LL);
    resource_attributes["uint64_value"]              = static_cast<uint64_t>(0x1200000000ULL);
    resource_attributes["double_value"]              = static_cast<double>(3.1);
    resource_attributes["vec_bool_value"]            = std::vector<bool>{true, false, true};
    resource_attributes["vec_int32_value"]           = std::vector<int32_t>{1, 2};
    resource_attributes["vec_uint32_value"]          = std::vector<uint32_t>{3, 4};
    resource_attributes["vec_int64_value"]           = std::vector<int64_t>{5, 6};
    resource_attributes["vec_uint64_value"]          = std::vector<uint64_t>{7, 8};
    resource_attributes["vec_double_value"]          = std::vector<double>{3.2, 3.3};
    resource_attributes["vec_string_value"]          = std::vector<std::string>{"vector", "string"};
    auto resource = resource::Resource::Create(resource_attributes);

    auto processor_opts                  = sdk::trace::BatchSpanProcessorOptions();
    processor_opts.max_export_batch_size = 5;
    processor_opts.max_queue_size        = 5;
    processor_opts.schedule_delay_millis = std::chrono::milliseconds(256);

    auto processor = std::unique_ptr<sdk::trace::SpanProcessor>(
        new sdk::trace::BatchSpanProcessor(std::move(exporter), processor_opts));
    auto provider = nostd::shared_ptr<trace::TracerProvider>(
        new sdk::trace::TracerProvider(std::move(processor), resource));

    std::string report_trace_id;

    char trace_id_hex[2 * trace_api::TraceId::kSize] = {0};
    auto tracer                                      = provider->GetTracer("test");
    auto parent_span                                 = tracer->StartSpan("Test parent span");

    trace_api::StartSpanOptions child_span_opts = {};
    child_span_opts.parent                      = parent_span->GetContext();

    auto child_span = tracer->StartSpan("Test child span", child_span_opts);

    nostd::get<trace_api::SpanContext>(child_span_opts.parent)
        .trace_id()
        .ToLowerBase16(MakeSpan(trace_id_hex));
    report_trace_id.assign(trace_id_hex, sizeof(trace_id_hex));

    auto no_send_client = std::static_pointer_cast<http_client::nosend::HttpClient>(client);
    auto mock_session =
        std::static_pointer_cast<http_client::nosend::Session>(no_send_client->session_);
    EXPECT_CALL(*mock_session, SendRequest)
        .WillOnce([&mock_session, report_trace_id](
                      std::shared_ptr<opentelemetry::ext::http::client::EventHandler> callback) {
          auto check_json =
              nlohmann::json::parse(mock_session->GetRequest()->body_, nullptr, false);
          auto resource_span     = *check_json["resourceSpans"].begin();
          auto scope_span        = *resource_span["scopeSpans"].begin();
          auto span              = *scope_span["spans"].begin();
          auto received_trace_id = span["traceId"].get<std::string>();
          EXPECT_EQ(received_trace_id, report_trace_id);

          auto custom_header = mock_session->GetRequest()->headers_.find("Custom-Header-Key");
          ASSERT_TRUE(custom_header != mock_session->GetRequest()->headers_.end());
          if (custom_header != mock_session->GetRequest()->headers_.end())
          {
            EXPECT_EQ("Custom-Header-Value", custom_header->second);
          }

          auto user_agent_header = mock_session->GetRequest()->headers_.find("User-Agent");
          ASSERT_TRUE(user_agent_header != mock_session->GetRequest()->headers_.end());
          if (user_agent_header != mock_session->GetRequest()->headers_.end())
          {
            EXPECT_EQ(GetOtlpDefaultUserAgent(), user_agent_header->second);
          }

          // let the otlp_http_client to continue
          http_client::nosend::Response response;
          response.Finish(*callback.get());
        });

    child_span->End();
    parent_span->End();

    static_cast<sdk::trace::TracerProvider *>(provider.get())->ForceFlush();
  }

#  ifdef ENABLE_ASYNC_EXPORT
  void ExportJsonIntegrationTestAsync()
  {
    auto mock_otlp_client =
        OtlpHttpExporterTestPeer::GetMockOtlpHttpClient(HttpRequestContentType::kJson, true);
    auto mock_otlp_http_client = mock_otlp_client.first;
    auto client                = mock_otlp_client.second;
    auto exporter = GetExporter(std::unique_ptr<OtlpHttpClient>{mock_otlp_http_client});

    resource::ResourceAttributes resource_attributes = {{"service.name", "unit_test_service"},
                                                        {"tenant.id", "test_user"}};
    resource_attributes["bool_value"]                = true;
    resource_attributes["int32_value"]               = static_cast<int32_t>(1);
    resource_attributes["uint32_value"]              = static_cast<uint32_t>(2);
    resource_attributes["int64_value"]               = static_cast<int64_t>(0x1100000000LL);
    resource_attributes["uint64_value"]              = static_cast<uint64_t>(0x1200000000ULL);
    resource_attributes["double_value"]              = static_cast<double>(3.1);
    resource_attributes["vec_bool_value"]            = std::vector<bool>{true, false, true};
    resource_attributes["vec_int32_value"]           = std::vector<int32_t>{1, 2};
    resource_attributes["vec_uint32_value"]          = std::vector<uint32_t>{3, 4};
    resource_attributes["vec_int64_value"]           = std::vector<int64_t>{5, 6};
    resource_attributes["vec_uint64_value"]          = std::vector<uint64_t>{7, 8};
    resource_attributes["vec_double_value"]          = std::vector<double>{3.2, 3.3};
    resource_attributes["vec_string_value"]          = std::vector<std::string>{"vector", "string"};
    auto resource = resource::Resource::Create(resource_attributes);

    auto processor_opts                  = sdk::trace::BatchSpanProcessorOptions();
    processor_opts.max_export_batch_size = 5;
    processor_opts.max_queue_size        = 5;
    processor_opts.schedule_delay_millis = std::chrono::milliseconds(256);

    auto processor = std::unique_ptr<sdk::trace::SpanProcessor>(
        new sdk::trace::BatchSpanProcessor(std::move(exporter), processor_opts));
    auto provider = nostd::shared_ptr<trace::TracerProvider>(
        new sdk::trace::TracerProvider(std::move(processor), resource));

    std::string report_trace_id;

    char trace_id_hex[2 * trace_api::TraceId::kSize] = {0};
    auto tracer                                      = provider->GetTracer("test");
    auto parent_span                                 = tracer->StartSpan("Test parent span");

    trace_api::StartSpanOptions child_span_opts = {};
    child_span_opts.parent                      = parent_span->GetContext();

    auto child_span = tracer->StartSpan("Test child span", child_span_opts);

    nostd::get<trace_api::SpanContext>(child_span_opts.parent)
        .trace_id()
        .ToLowerBase16(MakeSpan(trace_id_hex));
    report_trace_id.assign(trace_id_hex, sizeof(trace_id_hex));

    auto no_send_client = std::static_pointer_cast<http_client::nosend::HttpClient>(client);
    auto mock_session =
        std::static_pointer_cast<http_client::nosend::Session>(no_send_client->session_);
    EXPECT_CALL(*mock_session, SendRequest)
        .WillOnce([&mock_session, report_trace_id](
                      std::shared_ptr<opentelemetry::ext::http::client::EventHandler> callback) {
          auto check_json =
              nlohmann::json::parse(mock_session->GetRequest()->body_, nullptr, false);
          auto resource_span     = *check_json["resourceSpans"].begin();
          auto scope_span        = *resource_span["scopeSpans"].begin();
          auto span              = *scope_span["spans"].begin();
          auto received_trace_id = span["traceId"].get<std::string>();
          EXPECT_EQ(received_trace_id, report_trace_id);

          auto custom_header = mock_session->GetRequest()->headers_.find("Custom-Header-Key");
          ASSERT_TRUE(custom_header != mock_session->GetRequest()->headers_.end());
          if (custom_header != mock_session->GetRequest()->headers_.end())
          {
            EXPECT_EQ("Custom-Header-Value", custom_header->second);
          }

          auto user_agent_header = mock_session->GetRequest()->headers_.find("User-Agent");
          ASSERT_TRUE(user_agent_header != mock_session->GetRequest()->headers_.end());
          if (user_agent_header != mock_session->GetRequest()->headers_.end())
          {
            EXPECT_EQ(GetOtlpDefaultUserAgent(), user_agent_header->second);
          }

          // let the otlp_http_client to continue
          std::thread async_finish{[callback]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            http_client::nosend::Response response;
            response.Finish(*callback.get());
          }};
          async_finish.detach();
        });

    child_span->End();
    parent_span->End();

    static_cast<sdk::trace::TracerProvider *>(provider.get())->ForceFlush();
  }
#  endif

  void ExportBinaryIntegrationTest()
  {
    auto mock_otlp_client =
        OtlpHttpExporterTestPeer::GetMockOtlpHttpClient(HttpRequestContentType::kBinary);
    auto mock_otlp_http_client = mock_otlp_client.first;
    auto client                = mock_otlp_client.second;
    auto exporter = GetExporter(std::unique_ptr<OtlpHttpClient>{mock_otlp_http_client});

    resource::ResourceAttributes resource_attributes = {{"service.name", "unit_test_service"},
                                                        {"tenant.id", "test_user"}};
    resource_attributes["bool_value"]                = true;
    resource_attributes["int32_value"]               = static_cast<int32_t>(1);
    resource_attributes["uint32_value"]              = static_cast<uint32_t>(2);
    resource_attributes["int64_value"]               = static_cast<int64_t>(0x1100000000LL);
    resource_attributes["uint64_value"]              = static_cast<uint64_t>(0x1200000000ULL);
    resource_attributes["double_value"]              = static_cast<double>(3.1);
    resource_attributes["vec_bool_value"]            = std::vector<bool>{true, false, true};
    resource_attributes["vec_int32_value"]           = std::vector<int32_t>{1, 2};
    resource_attributes["vec_uint32_value"]          = std::vector<uint32_t>{3, 4};
    resource_attributes["vec_int64_value"]           = std::vector<int64_t>{5, 6};
    resource_attributes["vec_uint64_value"]          = std::vector<uint64_t>{7, 8};
    resource_attributes["vec_double_value"]          = std::vector<double>{3.2, 3.3};
    resource_attributes["vec_string_value"]          = std::vector<std::string>{"vector", "string"};
    auto resource = resource::Resource::Create(resource_attributes);

    auto processor_opts                  = sdk::trace::BatchSpanProcessorOptions();
    processor_opts.max_export_batch_size = 5;
    processor_opts.max_queue_size        = 5;
    processor_opts.schedule_delay_millis = std::chrono::milliseconds(256);

    auto processor = std::unique_ptr<sdk::trace::SpanProcessor>(
        new sdk::trace::BatchSpanProcessor(std::move(exporter), processor_opts));
    auto provider = nostd::shared_ptr<trace::TracerProvider>(
        new sdk::trace::TracerProvider(std::move(processor), resource));

    std::string report_trace_id;

    uint8_t trace_id_binary[trace_api::TraceId::kSize] = {0};
    auto tracer                                        = provider->GetTracer("test");
    auto parent_span                                   = tracer->StartSpan("Test parent span");

    trace_api::StartSpanOptions child_span_opts = {};
    child_span_opts.parent                      = parent_span->GetContext();

    auto child_span = tracer->StartSpan("Test child span", child_span_opts);
    nostd::get<trace_api::SpanContext>(child_span_opts.parent)
        .trace_id()
        .CopyBytesTo(MakeSpan(trace_id_binary));
    report_trace_id.assign(reinterpret_cast<char *>(trace_id_binary), sizeof(trace_id_binary));

    auto no_send_client = std::static_pointer_cast<http_client::nosend::HttpClient>(client);
    auto mock_session =
        std::static_pointer_cast<http_client::nosend::Session>(no_send_client->session_);
    EXPECT_CALL(*mock_session, SendRequest)
        .WillOnce([&mock_session, report_trace_id](
                      std::shared_ptr<opentelemetry::ext::http::client::EventHandler> callback) {
          opentelemetry::proto::collector::trace::v1::ExportTraceServiceRequest request_body;
          request_body.ParseFromArray(&mock_session->GetRequest()->body_[0],
                                      static_cast<int>(mock_session->GetRequest()->body_.size()));
          auto received_trace_id =
              request_body.resource_spans(0).scope_spans(0).spans(0).trace_id();
          EXPECT_EQ(received_trace_id, report_trace_id);

          auto custom_header = mock_session->GetRequest()->headers_.find("Custom-Header-Key");
          ASSERT_TRUE(custom_header != mock_session->GetRequest()->headers_.end());
          if (custom_header != mock_session->GetRequest()->headers_.end())
          {
            EXPECT_EQ("Custom-Header-Value", custom_header->second);
          }

          http_client::nosend::Response response;
          response.Finish(*callback.get());
        });

    child_span->End();
    parent_span->End();

    static_cast<sdk::trace::TracerProvider *>(provider.get())->ForceFlush();
  }

#  ifdef ENABLE_ASYNC_EXPORT
  void ExportBinaryIntegrationTestAsync()
  {
    auto mock_otlp_client =
        OtlpHttpExporterTestPeer::GetMockOtlpHttpClient(HttpRequestContentType::kBinary, true);
    auto mock_otlp_http_client = mock_otlp_client.first;
    auto client                = mock_otlp_client.second;
    auto exporter = GetExporter(std::unique_ptr<OtlpHttpClient>{mock_otlp_http_client});

    resource::ResourceAttributes resource_attributes = {{"service.name", "unit_test_service"},
                                                        {"tenant.id", "test_user"}};
    resource_attributes["bool_value"]                = true;
    resource_attributes["int32_value"]               = static_cast<int32_t>(1);
    resource_attributes["uint32_value"]              = static_cast<uint32_t>(2);
    resource_attributes["int64_value"]               = static_cast<int64_t>(0x1100000000LL);
    resource_attributes["uint64_value"]              = static_cast<uint64_t>(0x1200000000ULL);
    resource_attributes["double_value"]              = static_cast<double>(3.1);
    resource_attributes["vec_bool_value"]            = std::vector<bool>{true, false, true};
    resource_attributes["vec_int32_value"]           = std::vector<int32_t>{1, 2};
    resource_attributes["vec_uint32_value"]          = std::vector<uint32_t>{3, 4};
    resource_attributes["vec_int64_value"]           = std::vector<int64_t>{5, 6};
    resource_attributes["vec_uint64_value"]          = std::vector<uint64_t>{7, 8};
    resource_attributes["vec_double_value"]          = std::vector<double>{3.2, 3.3};
    resource_attributes["vec_string_value"]          = std::vector<std::string>{"vector", "string"};
    auto resource = resource::Resource::Create(resource_attributes);

    auto processor_opts                  = sdk::trace::BatchSpanProcessorOptions();
    processor_opts.max_export_batch_size = 5;
    processor_opts.max_queue_size        = 5;
    processor_opts.schedule_delay_millis = std::chrono::milliseconds(256);

    auto processor = std::unique_ptr<sdk::trace::SpanProcessor>(
        new sdk::trace::BatchSpanProcessor(std::move(exporter), processor_opts));
    auto provider = nostd::shared_ptr<trace::TracerProvider>(
        new sdk::trace::TracerProvider(std::move(processor), resource));

    std::string report_trace_id;

    uint8_t trace_id_binary[trace_api::TraceId::kSize] = {0};
    auto tracer                                        = provider->GetTracer("test");
    auto parent_span                                   = tracer->StartSpan("Test parent span");

    trace_api::StartSpanOptions child_span_opts = {};
    child_span_opts.parent                      = parent_span->GetContext();

    auto child_span = tracer->StartSpan("Test child span", child_span_opts);
    nostd::get<trace_api::SpanContext>(child_span_opts.parent)
        .trace_id()
        .CopyBytesTo(MakeSpan(trace_id_binary));
    report_trace_id.assign(reinterpret_cast<char *>(trace_id_binary), sizeof(trace_id_binary));

    auto no_send_client = std::static_pointer_cast<http_client::nosend::HttpClient>(client);
    auto mock_session =
        std::static_pointer_cast<http_client::nosend::Session>(no_send_client->session_);
    EXPECT_CALL(*mock_session, SendRequest)
        .WillOnce([&mock_session, report_trace_id](
                      std::shared_ptr<opentelemetry::ext::http::client::EventHandler> callback) {
          opentelemetry::proto::collector::trace::v1::ExportTraceServiceRequest request_body;
          request_body.ParseFromArray(&mock_session->GetRequest()->body_[0],
                                      static_cast<int>(mock_session->GetRequest()->body_.size()));
          auto received_trace_id =
              request_body.resource_spans(0).scope_spans(0).spans(0).trace_id();
          EXPECT_EQ(received_trace_id, report_trace_id);

          auto custom_header = mock_session->GetRequest()->headers_.find("Custom-Header-Key");
          ASSERT_TRUE(custom_header != mock_session->GetRequest()->headers_.end());
          if (custom_header != mock_session->GetRequest()->headers_.end())
          {
            EXPECT_EQ("Custom-Header-Value", custom_header->second);
          }

          // let the otlp_http_client to continue
          std::thread async_finish{[callback]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            http_client::nosend::Response response;
            response.Finish(*callback.get());
          }};
          async_finish.detach();
        });

    child_span->End();
    parent_span->End();

    static_cast<sdk::trace::TracerProvider *>(provider.get())->ForceFlush();
  }
#  endif
};

TEST(OtlpHttpExporterTest, Shutdown)
{
  auto exporter = std::unique_ptr<opentelemetry::sdk::trace::SpanExporter>(new OtlpHttpExporter());
  ASSERT_TRUE(exporter->Shutdown());

  nostd::span<std::unique_ptr<opentelemetry::sdk::trace::Recordable>> spans = {};

  auto result = exporter->Export(spans);
  EXPECT_EQ(result, opentelemetry::sdk::common::ExportResult::kFailure);
}

// Create spans, let processor call Export()
TEST_F(OtlpHttpExporterTestPeer, ExportJsonIntegrationTestSync)
{
  ExportJsonIntegrationTest();
}

#  ifdef ENABLE_ASYNC_EXPORT
TEST_F(OtlpHttpExporterTestPeer, ExportJsonIntegrationTestAsync)
{
  ExportJsonIntegrationTestAsync();
  google::protobuf::ShutdownProtobufLibrary();
}
#  endif

// Create spans, let processor call Export()
TEST_F(OtlpHttpExporterTestPeer, ExportBinaryIntegrationTestSync)
{
  ExportBinaryIntegrationTest();
}

#  ifdef ENABLE_ASYNC_EXPORT
TEST_F(OtlpHttpExporterTestPeer, ExportBinaryIntegrationTestAsync)
{
  ExportBinaryIntegrationTestAsync();
}
#  endif

// Test exporter configuration options
TEST_F(OtlpHttpExporterTestPeer, ConfigTest)
{
  OtlpHttpExporterOptions opts;
  opts.url = "http://localhost:45455/v1/traces";
  std::unique_ptr<OtlpHttpExporter> exporter(new OtlpHttpExporter(opts));
  EXPECT_EQ(GetOptions(exporter).url, "http://localhost:45455/v1/traces");
}

// Test exporter configuration options with use_json_name
TEST_F(OtlpHttpExporterTestPeer, ConfigUseJsonNameTest)
{
  OtlpHttpExporterOptions opts;
  opts.use_json_name = true;
  std::unique_ptr<OtlpHttpExporter> exporter(new OtlpHttpExporter(opts));
  EXPECT_EQ(GetOptions(exporter).use_json_name, true);
}

// Test exporter configuration options with json_bytes_mapping=JsonBytesMappingKind::kHex
TEST_F(OtlpHttpExporterTestPeer, ConfigJsonBytesMappingTest)
{
  OtlpHttpExporterOptions opts;
  opts.json_bytes_mapping = JsonBytesMappingKind::kHex;
  std::unique_ptr<OtlpHttpExporter> exporter(new OtlpHttpExporter(opts));
  EXPECT_EQ(GetOptions(exporter).json_bytes_mapping, JsonBytesMappingKind::kHex);
}

#  ifndef NO_GETENV
// Test exporter configuration options with use_ssl_credentials
TEST_F(OtlpHttpExporterTestPeer, ConfigFromEnv)
{
  const std::string url = "http://localhost:9999/v1/traces";
  setenv("OTEL_EXPORTER_OTLP_ENDPOINT", "http://localhost:9999", 1);
  setenv("OTEL_EXPORTER_OTLP_TIMEOUT", "20s", 1);
  setenv("OTEL_EXPORTER_OTLP_HEADERS", "k1=v1,k2=v2", 1);
  setenv("OTEL_EXPORTER_OTLP_TRACES_HEADERS", "k1=v3,k1=v4", 1);

  std::unique_ptr<OtlpHttpExporter> exporter(new OtlpHttpExporter());
  EXPECT_EQ(GetOptions(exporter).url, url);
  EXPECT_EQ(
      GetOptions(exporter).timeout.count(),
      std::chrono::duration_cast<std::chrono::system_clock::duration>(std::chrono::seconds{20})
          .count());
  EXPECT_EQ(GetOptions(exporter).http_headers.size(), 3);
  {
    // Test k2
    auto range = GetOptions(exporter).http_headers.equal_range("k2");
    EXPECT_TRUE(range.first != range.second);
    EXPECT_EQ(range.first->second, std::string("v2"));
    ++range.first;
    EXPECT_TRUE(range.first == range.second);
  }
  {
    // k1
    auto range = GetOptions(exporter).http_headers.equal_range("k1");
    EXPECT_TRUE(range.first != range.second);
    EXPECT_EQ(range.first->second, std::string("v3"));
    ++range.first;
    EXPECT_EQ(range.first->second, std::string("v4"));
    ++range.first;
    EXPECT_TRUE(range.first == range.second);
  }

  unsetenv("OTEL_EXPORTER_OTLP_ENDPOINT");
  unsetenv("OTEL_EXPORTER_OTLP_TIMEOUT");
  unsetenv("OTEL_EXPORTER_OTLP_HEADERS");
  unsetenv("OTEL_EXPORTER_OTLP_TRACES_HEADERS");
}

TEST_F(OtlpHttpExporterTestPeer, ConfigFromTracesEnv)
{
  const std::string url = "http://localhost:9999/v1/traces";
  setenv("OTEL_EXPORTER_OTLP_TRACES_ENDPOINT", url.c_str(), 1);
  setenv("OTEL_EXPORTER_OTLP_TRACES_TIMEOUT", "1eternity", 1);
  setenv("OTEL_EXPORTER_OTLP_HEADERS", "k1=v1,k2=v2", 1);
  setenv("OTEL_EXPORTER_OTLP_TRACES_HEADERS", "k1=v3,k1=v4", 1);

  std::unique_ptr<OtlpHttpExporter> exporter(new OtlpHttpExporter());
  EXPECT_EQ(GetOptions(exporter).url, url);
  EXPECT_EQ(
      GetOptions(exporter).timeout.count(),
      std::chrono::duration_cast<std::chrono::system_clock::duration>(std::chrono::seconds{10})
          .count());
  EXPECT_EQ(GetOptions(exporter).http_headers.size(), 3);
  {
    // Test k2
    auto range = GetOptions(exporter).http_headers.equal_range("k2");
    EXPECT_TRUE(range.first != range.second);
    EXPECT_EQ(range.first->second, std::string("v2"));
    ++range.first;
    EXPECT_TRUE(range.first == range.second);
  }
  {
    // k1
    auto range = GetOptions(exporter).http_headers.equal_range("k1");
    EXPECT_TRUE(range.first != range.second);
    EXPECT_EQ(range.first->second, std::string("v3"));
    ++range.first;
    EXPECT_EQ(range.first->second, std::string("v4"));
    ++range.first;
    EXPECT_TRUE(range.first == range.second);
  }

  unsetenv("OTEL_EXPORTER_OTLP_TRACES_ENDPOINT");
  unsetenv("OTEL_EXPORTER_OTLP_TRACES_TIMEOUT");
  unsetenv("OTEL_EXPORTER_OTLP_HEADERS");
  unsetenv("OTEL_EXPORTER_OTLP_TRACES_HEADERS");
}
#  endif

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
#endif /* OPENTELEMETRY_STL_VERSION */
