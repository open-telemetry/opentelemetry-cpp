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

class OtlpHttpLogExporterTestPeer : public ::testing::Test,
                                    public HTTP_SERVER_NS::HttpRequestCallback
{
protected:
  HTTP_SERVER_NS::HttpServer server_;
  std::string server_address_;
  std::atomic<bool> is_setup_;
  std::atomic<bool> is_running_;
  std::mutex mtx_requests;
  std::condition_variable cv_got_events;
  std::vector<nlohmann::json> received_requests_json_;
  std::vector<opentelemetry::proto::collector::logs::v1::ExportLogsServiceRequest>
      received_requests_binary_;
  std::map<std::string, std::string> received_requests_headers_;

public:
  OtlpHttpLogExporterTestPeer() : is_setup_(false), is_running_(false){};

  virtual void SetUp() override
  {
    if (is_setup_.exchange(true))
    {
      return;
    }
    int port = server_.addListeningPort(14372);
    std::ostringstream os;
    os << "localhost:" << port;
    server_address_ = "http://" + os.str() + "/v1/logs";
    server_.setServerName(os.str());
    server_.setKeepalive(false);
    server_.addHandler("/v1/logs", *this);
    server_.start();
    is_running_ = true;
  }

  virtual void TearDown() override
  {
    if (!is_setup_.exchange(false))
      return;
    server_.stop();
    is_running_ = false;
  }

  virtual int onHttpRequest(HTTP_SERVER_NS::HttpRequest const &request,
                            HTTP_SERVER_NS::HttpResponse &response) override
  {
    const std::string *request_content_type = nullptr;
    {
      auto it = request.headers.find("Content-Type");
      if (it != request.headers.end())
      {
        request_content_type = &it->second;
      }
    }
    received_requests_headers_ = request.headers;

    int response_status = 0;

    if (request.uri == "/v1/logs")
    {
      response.headers["Content-Type"] = "application/json";
      std::unique_lock<std::mutex> lk(mtx_requests);
      if (nullptr != request_content_type && *request_content_type == kHttpBinaryContentType)
      {
        opentelemetry::proto::collector::logs::v1::ExportLogsServiceRequest request_body;
        if (request_body.ParseFromArray(&request.content[0],
                                        static_cast<int>(request.content.size())))
        {
          received_requests_binary_.push_back(request_body);
          response.body = "{\"code\": 0, \"message\": \"success\"}";
        }
        else
        {
          response.body   = "{\"code\": 400, \"message\": \"Parse binary failed\"}";
          response_status = 400;
        }
      }
      else if (nullptr != request_content_type && *request_content_type == kHttpJsonContentType)
      {
        auto json                        = nlohmann::json::parse(request.content, nullptr, false);
        response.headers["Content-Type"] = "application/json";
        if (json.is_discarded())
        {
          response.body   = "{\"code\": 400, \"message\": \"Parse json failed\"}";
          response_status = 400;
        }
        else
        {
          received_requests_json_.push_back(json);
          response.body = "{\"code\": 0, \"message\": \"success\"}";
        }
      }
      else
      {
        response.body   = "{\"code\": 400, \"message\": \"Unsupported content type\"}";
        response_status = 400;
      }

      response_status = 200;
    }
    else
    {
      std::unique_lock<std::mutex> lk(mtx_requests);
      response.headers["Content-Type"] = "text/plain";
      response.body                    = "404 Not Found";
      response_status                  = 200;
    }

    cv_got_events.notify_one();

    return response_status;
  }

  bool waitForRequests(unsigned timeOutSec, size_t expected_count = 1)
  {
    std::unique_lock<std::mutex> lk(mtx_requests);
    if (cv_got_events.wait_for(lk, std::chrono::milliseconds(1000 * timeOutSec),
                               [&] { return getCurrentRequestCount() >= expected_count; }))
    {
      return true;
    }
    return false;
  }

  size_t getCurrentRequestCount() const
  {
    return received_requests_json_.size() + received_requests_binary_.size();
  }

public:
  std::unique_ptr<sdk::logs::LogExporter> GetExporter(HttpRequestContentType content_type)
  {
    OtlpHttpLogExporterOptions opts;
    opts.url           = server_address_;
    opts.content_type  = content_type;
    opts.console_debug = true;
    opts.http_headers.insert(
        std::make_pair<const std::string, std::string>("Custom-Header-Key", "Custom-Header-Value"));
    return std::unique_ptr<sdk::logs::LogExporter>(new OtlpHttpLogExporter(opts));
  }

  // Get the options associated with the given exporter.
  const OtlpHttpLogExporterOptions &GetOptions(std::unique_ptr<OtlpHttpLogExporter> &exporter)
  {
    return exporter->options_;
  }
};

// Create log records, let processor call Export()
TEST_F(OtlpHttpLogExporterTestPeer, ExportJsonIntegrationTest)
{
  size_t old_count = getCurrentRequestCount();
  auto exporter    = GetExporter(HttpRequestContentType::kJson);

  bool resource_storage_bool_value[]          = {true, false, true};
  int32_t resource_storage_int32_value[]      = {1, 2};
  uint32_t resource_storage_uint32_value[]    = {3, 4};
  int64_t resource_storage_int64_value[]      = {5, 6};
  uint64_t resource_storage_uint64_value[]    = {7, 8};
  double resource_storage_double_value[]      = {3.2, 3.3};
  std::string resource_storage_string_value[] = {"vector", "string"};

  auto provider = nostd::shared_ptr<sdk::logs::LoggerProvider>(new sdk::logs::LoggerProvider());
  provider->SetProcessor(std::unique_ptr<sdk::logs::LogProcessor>(
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

    auto logger = provider->GetLogger("test");
    logger->Log(opentelemetry::logs::Severity::kInfo, "Log name", "Log message",
                {{"service.name", "unit_test_service"},
                 {"tenant.id", "test_user"},
                 {"bool_value", true},
                 {"int32_value", static_cast<int32_t>(1)},
                 {"uint32_value", static_cast<uint32_t>(2)},
                 {"int64_value", static_cast<int64_t>(0x1100000000LL)},
                 {"uint64_value", static_cast<uint64_t>(0x1200000000ULL)},
                 {"double_value", static_cast<double>(3.1)},
                 {"vec_bool_value", resource_storage_bool_value},
                 {"vec_int32_value", resource_storage_int32_value},
                 {"vec_uint32_value", resource_storage_uint32_value},
                 {"vec_int64_value", resource_storage_int64_value},
                 {"vec_uint64_value", resource_storage_uint64_value},
                 {"vec_double_value", resource_storage_double_value},
                 {"vec_string_value", resource_storage_string_value}},
                {{"log_attribute", "test_value"}}, trace_id, span_id,
                opentelemetry::trace::TraceFlags{opentelemetry::trace::TraceFlags::kIsSampled},
                std::chrono::system_clock::now());

    trace_id.ToLowerBase16(MakeSpan(trace_id_hex));
    report_trace_id.assign(trace_id_hex, sizeof(trace_id_hex));

    span_id.ToLowerBase16(MakeSpan(span_id_hex));
    report_span_id.assign(span_id_hex, sizeof(span_id_hex));
  }

  ASSERT_TRUE(waitForRequests(2, old_count + 1));
  auto check_json                   = received_requests_json_.back();
  auto resource_logs                = *check_json["resource_logs"].begin();
  auto instrumentation_library_span = *resource_logs["instrumentation_library_logs"].begin();
  auto log                          = *instrumentation_library_span["logs"].begin();
  auto received_trace_id            = log["trace_id"].get<std::string>();
  auto received_span_id             = log["span_id"].get<std::string>();
  EXPECT_EQ(received_trace_id, report_trace_id);
  EXPECT_EQ(received_span_id, report_span_id);
  EXPECT_EQ("Log name", log["name"].get<std::string>());
  EXPECT_EQ("Log message", log["body"]["string_value"].get<std::string>());
  EXPECT_EQ("test_value", (*log["attributes"].begin())["value"]["string_value"].get<std::string>());
  EXPECT_LE(15, resource_logs["resource"]["attributes"].size());
  bool check_service_name = false;
  for (auto attribute : resource_logs["resource"]["attributes"])
  {
    if ("service.name" == attribute["key"].get<std::string>())
    {
      check_service_name = true;
      EXPECT_EQ("unit_test_service", attribute["value"]["string_value"].get<std::string>());
    }
  }
  ASSERT_TRUE(check_service_name);

  {
    auto custom_header = received_requests_headers_.find("Custom-Header-Key");
    ASSERT_TRUE(custom_header != received_requests_headers_.end());
    if (custom_header != received_requests_headers_.end())
    {
      EXPECT_EQ("Custom-Header-Value", custom_header->second);
    }
  }
}

// Create log records, let processor call Export()
TEST_F(OtlpHttpLogExporterTestPeer, ExportBinaryIntegrationTest)
{
  size_t old_count = getCurrentRequestCount();

  auto exporter = GetExporter(HttpRequestContentType::kBinary);

  bool resource_storage_bool_value[]          = {true, false, true};
  int32_t resource_storage_int32_value[]      = {1, 2};
  uint32_t resource_storage_uint32_value[]    = {3, 4};
  int64_t resource_storage_int64_value[]      = {5, 6};
  uint64_t resource_storage_uint64_value[]    = {7, 8};
  double resource_storage_double_value[]      = {3.2, 3.3};
  std::string resource_storage_string_value[] = {"vector", "string"};

  auto provider = nostd::shared_ptr<sdk::logs::LoggerProvider>(new sdk::logs::LoggerProvider());
  provider->SetProcessor(std::unique_ptr<sdk::logs::LogProcessor>(
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

    auto logger = provider->GetLogger("test");
    logger->Log(opentelemetry::logs::Severity::kInfo, "Log name", "Log message",
                {{"service.name", "unit_test_service"},
                 {"tenant.id", "test_user"},
                 {"bool_value", true},
                 {"int32_value", static_cast<int32_t>(1)},
                 {"uint32_value", static_cast<uint32_t>(2)},
                 {"int64_value", static_cast<int64_t>(0x1100000000LL)},
                 {"uint64_value", static_cast<uint64_t>(0x1200000000ULL)},
                 {"double_value", static_cast<double>(3.1)},
                 {"vec_bool_value", resource_storage_bool_value},
                 {"vec_int32_value", resource_storage_int32_value},
                 {"vec_uint32_value", resource_storage_uint32_value},
                 {"vec_int64_value", resource_storage_int64_value},
                 {"vec_uint64_value", resource_storage_uint64_value},
                 {"vec_double_value", resource_storage_double_value},
                 {"vec_string_value", resource_storage_string_value}},
                {{"log_attribute", "test_value"}}, trace_id, span_id,
                opentelemetry::trace::TraceFlags{opentelemetry::trace::TraceFlags::kIsSampled},
                std::chrono::system_clock::now());

    report_trace_id.assign(reinterpret_cast<const char *>(trace_id_bin), sizeof(trace_id_bin));
    report_span_id.assign(reinterpret_cast<const char *>(span_id_bin), sizeof(span_id_bin));
  }

  ASSERT_TRUE(waitForRequests(2, old_count + 1));
  auto received_log =
      received_requests_binary_.back().resource_logs(0).instrumentation_library_logs(0).logs(0);
  EXPECT_EQ(received_log.trace_id(), report_trace_id);
  EXPECT_EQ(received_log.span_id(), report_span_id);
  EXPECT_EQ("Log name", received_log.name());
  EXPECT_EQ("Log message", received_log.body().string_value());
  EXPECT_EQ("test_value", received_log.attributes(0).value().string_value());
  EXPECT_LE(15, received_requests_binary_.back().resource_logs(0).resource().attributes_size());
  bool check_service_name = false;
  for (auto &attribute : received_requests_binary_.back().resource_logs(0).resource().attributes())
  {
    if ("service.name" == attribute.key())
    {
      check_service_name = true;
      EXPECT_EQ("unit_test_service", attribute.value().string_value());
    }
  }
  ASSERT_TRUE(check_service_name);
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
