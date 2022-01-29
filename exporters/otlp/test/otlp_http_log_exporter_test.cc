// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef HAVE_CPP_STDLIB
#  ifdef ENABLE_LOGS_PREVIEW

#    include "opentelemetry/exporters/otlp/otlp_http_log_exporter.h"

#    include "opentelemetry/exporters/otlp/protobuf_include_prefix.h"

#    include "opentelemetry/proto/collector/logs/v1/logs_service.pb.h"

#    include "opentelemetry/exporters/otlp/protobuf_include_suffix.h"

#    include "opentelemetry/common/key_value_iterable_view.h"
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
};

class MockOtlpHttpClient : public OtlpHttpClient
{
public:
  MockOtlpHttpClient(OtlpHttpClientOptions &&options) : OtlpHttpClient(std::move(options)) {}
  MOCK_METHOD(sdk::common::ExportResult,
              Export,
              (const google::protobuf::Message &),
              (noexcept, override));
};

MockOtlpHttpClient *GetMockOtlpHttpClient(HttpRequestContentType content_type)
{
  OtlpHttpLogExporterOptions options;
  options.content_type  = content_type;
  options.console_debug = true;
  options.http_headers.insert(
      std::make_pair<const std::string, std::string>("Custom-Header-Key", "Custom-Header-Value"));
  OtlpHttpClientOptions otlpHttpClientOptions(
      options.url, options.content_type, options.json_bytes_mapping, options.use_json_name,
      options.console_debug, options.timeout, options.http_headers);
  return new MockOtlpHttpClient(std::move(otlpHttpClientOptions));
}

// Create log records, let processor call Export()
TEST_F(OtlpHttpLogExporterTestPeer, ExportJsonIntegrationTest)
{
  auto mockOtlpHttpClient = GetMockOtlpHttpClient(HttpRequestContentType::kJson);
  auto exporter           = GetExporter(std::unique_ptr<OtlpHttpClient>{mockOtlpHttpClient});

  bool attribute_storage_bool_value[]          = {true, false, true};
  int32_t attribute_storage_int32_value[]      = {1, 2};
  uint32_t attribute_storage_uint32_value[]    = {3, 4};
  int64_t attribute_storage_int64_value[]      = {5, 6};
  uint64_t attribute_storage_uint64_value[]    = {7, 8};
  double attribute_storage_double_value[]      = {3.2, 3.3};
  std::string attribute_storage_string_value[] = {"vector", "string"};

  auto provider = nostd::shared_ptr<sdk::logs::LoggerProvider>(new sdk::logs::LoggerProvider());
  provider->AddProcessor(std::unique_ptr<sdk::logs::LogProcessor>(
      new sdk::logs::BatchLogProcessor(std::move(exporter), 5, std::chrono::milliseconds(256), 1)));

  std::string report_trace_id;
  std::string report_span_id;
  {
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
    EXPECT_CALL(*mockOtlpHttpClient, Export(_))
        .Times(Exactly(1))
        .WillOnce(Return(sdk::common::ExportResult::kSuccess));
    logger->Log(opentelemetry::logs::Severity::kInfo, "Log name", "Log message",
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
  }
}

// Create log records, let processor call Export()
TEST_F(OtlpHttpLogExporterTestPeer, ExportBinaryIntegrationTest)
{
  auto mockOtlpHttpClient = GetMockOtlpHttpClient(HttpRequestContentType::kJson);
  auto exporter           = GetExporter(std::unique_ptr<OtlpHttpClient>{mockOtlpHttpClient});

  bool attribute_storage_bool_value[]          = {true, false, true};
  int32_t attribute_storage_int32_value[]      = {1, 2};
  uint32_t attribute_storage_uint32_value[]    = {3, 4};
  int64_t attribute_storage_int64_value[]      = {5, 6};
  uint64_t attribute_storage_uint64_value[]    = {7, 8};
  double attribute_storage_double_value[]      = {3.2, 3.3};
  std::string attribute_storage_string_value[] = {"vector", "string"};

  auto provider = nostd::shared_ptr<sdk::logs::LoggerProvider>(new sdk::logs::LoggerProvider());
  provider->AddProcessor(std::unique_ptr<sdk::logs::LogProcessor>(
      new sdk::logs::BatchLogProcessor(std::move(exporter), 5, std::chrono::milliseconds(256), 1)));

  std::string report_trace_id;
  std::string report_span_id;
  {
    uint8_t trace_id_bin[opentelemetry::trace::TraceId::kSize] = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
    opentelemetry::trace::TraceId trace_id{trace_id_bin};
    uint8_t span_id_bin[opentelemetry::trace::SpanId::kSize] = {'7', '6', '5', '4',
                                                                '3', '2', '1', '0'};
    opentelemetry::trace::SpanId span_id{span_id_bin};

    const std::string schema_url{"https://opentelemetry.io/schemas/1.2.0"};
    auto logger = provider->GetLogger("test", "", "opentelelemtry_library", "", schema_url);
    EXPECT_CALL(*mockOtlpHttpClient, Export(_))
        .Times(Exactly(1))
        .WillOnce(Return(sdk::common::ExportResult::kSuccess));
    logger->Log(opentelemetry::logs::Severity::kInfo, "Log name", "Log message",
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
  }
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
