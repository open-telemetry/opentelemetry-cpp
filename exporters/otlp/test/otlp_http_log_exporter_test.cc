// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef HAVE_CPP_STDLIB
#  ifdef ENABLE_LOGS_PREVIEW

#    include "opentelemetry/exporters/otlp/otlp_http_log_exporter.h"

#    include "opentelemetry/exporters/otlp/protobuf_include_prefix.h"

#    include "opentelemetry/proto/collector/logs/v1/logs_service.pb.h"

#    include "opentelemetry/exporters/otlp/protobuf_include_suffix.h"

#    include "opentelemetry/common/key_value_iterable_view.h"
#    include "opentelemetry/ext/http/client/http_client_factory.h"
#    include "opentelemetry/ext/http/client/nosend/http_client_nosend.h"
#    include "opentelemetry/ext/http/server/http_server.h"
#    include "opentelemetry/logs/provider.h"
#    include "opentelemetry/sdk/logs/batch_log_processor.h"
#    include "opentelemetry/sdk/logs/exporter.h"
#    include "opentelemetry/sdk/logs/log_record.h"
#    include "opentelemetry/sdk/logs/logger_provider.h"
#    include "opentelemetry/sdk/resource/resource.h"

#    include <gtest/gtest.h>
#    include "gmock/gmock.h"

#    include "nlohmann/json.hpp"

#    if defined(_MSC_VER)
#      include "opentelemetry/sdk/common/env_variables.h"
using opentelemetry::sdk::common::setenv;
using opentelemetry::sdk::common::unsetenv;
#    endif

using namespace testing;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

template <class T, size_t N>
static nostd::span<T, N> MakeSpan(T (&array)[N])
{
  return nostd::span<T, N>(array);
}

OtlpHttpClientOptions MakeOtlpHttpClientOptions(HttpRequestContentType content_type)
{
  OtlpHttpLogExporterOptions options;
  options.content_type  = content_type;
  options.console_debug = true;
  options.http_headers.insert(
      std::make_pair<const std::string, std::string>("Custom-Header-Key", "Custom-Header-Value"));
  OtlpHttpClientOptions otlp_http_client_options(
      options.url, options.content_type, options.json_bytes_mapping, options.use_json_name,
      options.console_debug, options.timeout, options.http_headers);
  return otlp_http_client_options;
}

namespace http_client = opentelemetry::ext::http::client;

class OtlpHttpLogExporterTestPeer : public ::testing::Test
{
public:
  std::unique_ptr<sdk::logs::LogExporter> GetExporter(std::unique_ptr<OtlpHttpClient> http_client)
  {
    return std::unique_ptr<sdk::logs::LogExporter>(new OtlpHttpLogExporter(std::move(http_client)));
  }

  // Get the options associated with the given exporter.
  const OtlpHttpLogExporterOptions &GetOptions(std::unique_ptr<OtlpHttpLogExporter> &exporter)
  {
    return exporter->options_;
  }
  static std::pair<OtlpHttpClient *, std::shared_ptr<http_client::HttpClient>>
  GetMockOtlpHttpClient(HttpRequestContentType content_type)
  {
    auto http_client = http_client::HttpClientFactory::CreateNoSend();
    return {new OtlpHttpClient(MakeOtlpHttpClientOptions(content_type), http_client), http_client};
  }
};

// Create log records, let processor call Export()
TEST_F(OtlpHttpLogExporterTestPeer, ExportJsonIntegrationTest)
{
  auto mock_otlp_client =
      OtlpHttpLogExporterTestPeer::GetMockOtlpHttpClient(HttpRequestContentType::kJson);
  auto mock_otlp_http_client = mock_otlp_client.first;
  auto client                = mock_otlp_client.second;
  auto exporter              = GetExporter(std::unique_ptr<OtlpHttpClient>{mock_otlp_http_client});

  bool attribute_storage_bool_value[]          = {true, false, true};
  int32_t attribute_storage_int32_value[]      = {1, 2};
  uint32_t attribute_storage_uint32_value[]    = {3, 4};
  int64_t attribute_storage_int64_value[]      = {5, 6};
  uint64_t attribute_storage_uint64_value[]    = {7, 8};
  double attribute_storage_double_value[]      = {3.2, 3.3};
  std::string attribute_storage_string_value[] = {"vector", "string"};

  auto provider = nostd::shared_ptr<sdk::logs::LoggerProvider>(new sdk::logs::LoggerProvider());
  provider->AddProcessor(std::unique_ptr<sdk::logs::LogProcessor>(
      new sdk::logs::BatchLogProcessor(std::move(exporter), 5, std::chrono::milliseconds(256), 5)));

  std::string report_trace_id;
  std::string report_span_id;
  uint8_t trace_id_bin[opentelemetry::trace::TraceId::kSize] = {
      '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
  char trace_id_hex[2 * opentelemetry::trace::TraceId::kSize] = {0};
  opentelemetry::trace::TraceId trace_id{trace_id_bin};
  uint8_t span_id_bin[opentelemetry::trace::SpanId::kSize]  = {'7', '6', '5', '4',
                                                              '3', '2', '1', '0'};
  char span_id_hex[2 * opentelemetry::trace::SpanId::kSize] = {0};
  opentelemetry::trace::SpanId span_id{span_id_bin};

  const std::string schema_url{"https://opentelemetry.io/schemas/1.2.0"};
  auto logger = provider->GetLogger("test", "", "opentelelemtry_library", "", schema_url);
  logger->Log(opentelemetry::logs::Severity::kInfo, "Log message",
              {{"service.name", "unit_test_service"},
               {"tenant.id", "test_user"},
               {"bool_value", true},
               {"int32_value", static_cast<int32_t>(1)},
               {"uint32_value", static_cast<uint32_t>(2)},
               {"int64_value", static_cast<int64_t>(0x1100000000LL)},
               {"uint64_value", static_cast<uint64_t>(0x1200000000ULL)},
               {"double_value", static_cast<double>(3.1)},
               {"vec_bool_value", attribute_storage_bool_value},
               {"vec_int32_value", attribute_storage_int32_value},
               {"vec_uint32_value", attribute_storage_uint32_value},
               {"vec_int64_value", attribute_storage_int64_value},
               {"vec_uint64_value", attribute_storage_uint64_value},
               {"vec_double_value", attribute_storage_double_value},
               {"vec_string_value", attribute_storage_string_value}},
              trace_id, span_id,
              opentelemetry::trace::TraceFlags{opentelemetry::trace::TraceFlags::kIsSampled},
              std::chrono::system_clock::now());

  trace_id.ToLowerBase16(MakeSpan(trace_id_hex));
  report_trace_id.assign(trace_id_hex, sizeof(trace_id_hex));

  span_id.ToLowerBase16(MakeSpan(span_id_hex));
  report_span_id.assign(span_id_hex, sizeof(span_id_hex));

  auto no_send_client = std::static_pointer_cast<http_client::nosend::HttpClient>(client);
  auto mock_session =
      std::static_pointer_cast<http_client::nosend::Session>(no_send_client->session_);
  EXPECT_CALL(*mock_session, SendRequest)
      .WillOnce([&mock_session, report_trace_id,
                 report_span_id](opentelemetry::ext::http::client::EventHandler &callback) {
        auto check_json = nlohmann::json::parse(mock_session->GetRequest()->body_, nullptr, false);
        auto resource_logs     = *check_json["resource_logs"].begin();
        auto scope_logs        = *resource_logs["scope_logs"].begin();
        auto log               = *scope_logs["log_records"].begin();
        auto received_trace_id = log["trace_id"].get<std::string>();
        auto received_span_id  = log["span_id"].get<std::string>();
        EXPECT_EQ(received_trace_id, report_trace_id);
        EXPECT_EQ(received_span_id, report_span_id);
        EXPECT_EQ("Log message", log["body"]["string_value"].get<std::string>());
        EXPECT_LE(15, log["attributes"].size());
        auto custom_header = mock_session->GetRequest()->headers_.find("Custom-Header-Key");
        ASSERT_TRUE(custom_header != mock_session->GetRequest()->headers_.end());
        if (custom_header != mock_session->GetRequest()->headers_.end())
        {
          EXPECT_EQ("Custom-Header-Value", custom_header->second);
        }
        // let the otlp_http_client to continue
        http_client::nosend::Response response;
        callback.OnResponse(response);
      });
}

// Create log records, let processor call Export()
TEST_F(OtlpHttpLogExporterTestPeer, ExportBinaryIntegrationTest)
{
  auto mock_otlp_client =
      OtlpHttpLogExporterTestPeer::GetMockOtlpHttpClient(HttpRequestContentType::kBinary);
  auto mock_otlp_http_client = mock_otlp_client.first;
  auto client                = mock_otlp_client.second;
  auto exporter              = GetExporter(std::unique_ptr<OtlpHttpClient>{mock_otlp_http_client});

  bool attribute_storage_bool_value[]          = {true, false, true};
  int32_t attribute_storage_int32_value[]      = {1, 2};
  uint32_t attribute_storage_uint32_value[]    = {3, 4};
  int64_t attribute_storage_int64_value[]      = {5, 6};
  uint64_t attribute_storage_uint64_value[]    = {7, 8};
  double attribute_storage_double_value[]      = {3.2, 3.3};
  std::string attribute_storage_string_value[] = {"vector", "string"};

  auto provider = nostd::shared_ptr<sdk::logs::LoggerProvider>(new sdk::logs::LoggerProvider());
  provider->AddProcessor(std::unique_ptr<sdk::logs::LogProcessor>(
      new sdk::logs::BatchLogProcessor(std::move(exporter), 5, std::chrono::milliseconds(256), 5)));

  std::string report_trace_id;
  std::string report_span_id;
  uint8_t trace_id_bin[opentelemetry::trace::TraceId::kSize] = {
      '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
  opentelemetry::trace::TraceId trace_id{trace_id_bin};
  uint8_t span_id_bin[opentelemetry::trace::SpanId::kSize] = {'7', '6', '5', '4',
                                                              '3', '2', '1', '0'};
  opentelemetry::trace::SpanId span_id{span_id_bin};

  const std::string schema_url{"https://opentelemetry.io/schemas/1.2.0"};
  auto logger = provider->GetLogger("test", "", "opentelelemtry_library", "", schema_url);
  logger->Log(opentelemetry::logs::Severity::kInfo, "Log message",
              {{"service.name", "unit_test_service"},
               {"tenant.id", "test_user"},
               {"bool_value", true},
               {"int32_value", static_cast<int32_t>(1)},
               {"uint32_value", static_cast<uint32_t>(2)},
               {"int64_value", static_cast<int64_t>(0x1100000000LL)},
               {"uint64_value", static_cast<uint64_t>(0x1200000000ULL)},
               {"double_value", static_cast<double>(3.1)},
               {"vec_bool_value", attribute_storage_bool_value},
               {"vec_int32_value", attribute_storage_int32_value},
               {"vec_uint32_value", attribute_storage_uint32_value},
               {"vec_int64_value", attribute_storage_int64_value},
               {"vec_uint64_value", attribute_storage_uint64_value},
               {"vec_double_value", attribute_storage_double_value},
               {"vec_string_value", attribute_storage_string_value}},
              trace_id, span_id,
              opentelemetry::trace::TraceFlags{opentelemetry::trace::TraceFlags::kIsSampled},
              std::chrono::system_clock::now());

  report_trace_id.assign(reinterpret_cast<const char *>(trace_id_bin), sizeof(trace_id_bin));
  report_span_id.assign(reinterpret_cast<const char *>(span_id_bin), sizeof(span_id_bin));

  auto no_send_client = std::static_pointer_cast<http_client::nosend::HttpClient>(client);
  auto mock_session =
      std::static_pointer_cast<http_client::nosend::Session>(no_send_client->session_);
  EXPECT_CALL(*mock_session, SendRequest)
      .WillOnce([&mock_session, report_trace_id,
                 report_span_id](opentelemetry::ext::http::client::EventHandler &callback) {
        opentelemetry::proto::collector::logs::v1::ExportLogsServiceRequest request_body;
        request_body.ParseFromArray(&mock_session->GetRequest()->body_[0],
                                    static_cast<int>(mock_session->GetRequest()->body_.size()));
        auto &received_log = request_body.resource_logs(0).scope_logs(0).log_records(0);
        EXPECT_EQ(received_log.trace_id(), report_trace_id);
        EXPECT_EQ(received_log.span_id(), report_span_id);
        EXPECT_EQ("Log message", received_log.body().string_value());
        EXPECT_LE(15, received_log.attributes_size());
        bool check_service_name = false;
        for (auto &attribute : received_log.attributes())
        {
          if ("service.name" == attribute.key())
          {
            check_service_name = true;
            EXPECT_EQ("unit_test_service", attribute.value().string_value());
          }
        }
        ASSERT_TRUE(check_service_name);
        http_client::nosend::Response response;
        callback.OnResponse(response);
      });
}

// Test exporter configuration options
TEST_F(OtlpHttpLogExporterTestPeer, ConfigTest)
{
  OtlpHttpLogExporterOptions opts;
  opts.url = "http://localhost:45456/v1/logs";
  std::unique_ptr<OtlpHttpLogExporter> exporter(new OtlpHttpLogExporter(opts));
  EXPECT_EQ(GetOptions(exporter).url, "http://localhost:45456/v1/logs");
}

// Test exporter configuration options with use_json_name
TEST_F(OtlpHttpLogExporterTestPeer, ConfigUseJsonNameTest)
{
  OtlpHttpLogExporterOptions opts;
  opts.use_json_name = true;
  std::unique_ptr<OtlpHttpLogExporter> exporter(new OtlpHttpLogExporter(opts));
  EXPECT_EQ(GetOptions(exporter).use_json_name, true);
}

// Test exporter configuration options with json_bytes_mapping=JsonBytesMappingKind::kHex
TEST_F(OtlpHttpLogExporterTestPeer, ConfigJsonBytesMappingTest)
{
  OtlpHttpLogExporterOptions opts;
  opts.json_bytes_mapping = JsonBytesMappingKind::kHex;
  std::unique_ptr<OtlpHttpLogExporter> exporter(new OtlpHttpLogExporter(opts));
  EXPECT_EQ(GetOptions(exporter).json_bytes_mapping, JsonBytesMappingKind::kHex);
}

#    ifndef NO_GETENV
// Test exporter configuration options with use_ssl_credentials
TEST_F(OtlpHttpLogExporterTestPeer, ConfigFromEnv)
{
  const std::string url = "http://localhost:9999/v1/logs";
  setenv("OTEL_EXPORTER_OTLP_ENDPOINT", "http://localhost:9999", 1);
  setenv("OTEL_EXPORTER_OTLP_TIMEOUT", "20s", 1);
  setenv("OTEL_EXPORTER_OTLP_HEADERS", "k1=v1,k2=v2", 1);
  setenv("OTEL_EXPORTER_OTLP_LOGS_HEADERS", "k1=v3,k1=v4", 1);

  std::unique_ptr<OtlpHttpLogExporter> exporter(new OtlpHttpLogExporter());
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
  unsetenv("OTEL_EXPORTER_OTLP_LOGS_HEADERS");
}

TEST_F(OtlpHttpLogExporterTestPeer, ConfigFromLogsEnv)
{
  const std::string url = "http://localhost:9999/v1/logs";
  setenv("OTEL_EXPORTER_OTLP_LOGS_ENDPOINT", url.c_str(), 1);
  setenv("OTEL_EXPORTER_OTLP_TIMEOUT", "20s", 1);
  setenv("OTEL_EXPORTER_OTLP_HEADERS", "k1=v1,k2=v2", 1);
  setenv("OTEL_EXPORTER_OTLP_LOGS_HEADERS", "k1=v3,k1=v4", 1);

  std::unique_ptr<OtlpHttpLogExporter> exporter(new OtlpHttpLogExporter());
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

  unsetenv("OTEL_EXPORTER_OTLP_LOGS_ENDPOINT");
  unsetenv("OTEL_EXPORTER_OTLP_TIMEOUT");
  unsetenv("OTEL_EXPORTER_OTLP_HEADERS");
  unsetenv("OTEL_EXPORTER_OTLP_LOGS_HEADERS");
}

TEST_F(OtlpHttpLogExporterTestPeer, DefaultEndpoint)
{
  EXPECT_EQ("http://localhost:4318/v1/logs", GetOtlpDefaultHttpLogEndpoint());
  EXPECT_EQ("http://localhost:4318/v1/traces", GetOtlpDefaultHttpEndpoint());
  EXPECT_EQ("http://localhost:4317", GetOtlpDefaultGrpcEndpoint());
}

#    endif

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
#  endif
#endif
