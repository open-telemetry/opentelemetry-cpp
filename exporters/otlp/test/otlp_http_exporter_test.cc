// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef HAVE_CPP_STDLIB

#  include "opentelemetry/exporters/otlp/otlp_http_exporter.h"

#  include "opentelemetry/exporters/otlp/protobuf_include_prefix.h"

#  include "opentelemetry/proto/collector/trace/v1/trace_service.pb.h"

#  include "opentelemetry/exporters/otlp/protobuf_include_suffix.h"

#  include "opentelemetry/ext/http/server/http_server.h"
#  include "opentelemetry/sdk/trace/batch_span_processor.h"
#  include "opentelemetry/sdk/trace/tracer_provider.h"
#  include "opentelemetry/trace/provider.h"

#  include <gtest/gtest.h>

#  include "nlohmann/json.hpp"

#  if defined(_MSC_VER)
#    define putenv _putenv
#  endif

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

class OtlpHttpExporterTestPeer : public ::testing::Test, public HTTP_SERVER_NS::HttpRequestCallback
{
protected:
  HTTP_SERVER_NS::HttpServer server_;
  std::string server_address_;
  std::atomic<bool> is_setup_;
  std::atomic<bool> is_running_;
  std::mutex mtx_requests;
  std::condition_variable cv_got_events;
  std::vector<nlohmann::json> received_requests_json_;
  std::vector<opentelemetry::proto::collector::trace::v1::ExportTraceServiceRequest>
      received_requests_binary_;
  std::map<std::string, std::string> received_requests_headers_;

public:
  OtlpHttpExporterTestPeer() : is_setup_(false), is_running_(false){};

  virtual void SetUp() override
  {
    if (is_setup_.exchange(true))
    {
      return;
    }
    int port = server_.addListeningPort(14371);
    std::ostringstream os;
    os << "localhost:" << port;
    server_address_ = "http://" + os.str() + "/v1/traces";
    server_.setServerName(os.str());
    server_.setKeepalive(false);
    server_.addHandler("/v1/traces", *this);
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

    if (request.uri == "/v1/traces")
    {
      response.headers["Content-Type"] = "application/json";
      std::unique_lock<std::mutex> lk(mtx_requests);
      if (nullptr != request_content_type && *request_content_type == kHttpBinaryContentType)
      {
        opentelemetry::proto::collector::trace::v1::ExportTraceServiceRequest request_body;
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
  std::unique_ptr<sdk::trace::SpanExporter> GetExporter(HttpRequestContentType content_type)
  {
    OtlpHttpExporterOptions opts;
    opts.url           = server_address_;
    opts.content_type  = content_type;
    opts.console_debug = true;
    opts.http_headers.insert(
        std::make_pair<const std::string, std::string>("Custom-Header-Key", "Custom-Header-Value"));
    return std::unique_ptr<sdk::trace::SpanExporter>(new OtlpHttpExporter(opts));
  }

  // Get the options associated with the given exporter.
  const OtlpHttpExporterOptions &GetOptions(std::unique_ptr<OtlpHttpExporter> &exporter)
  {
    return exporter->options_;
  }
};

// Create spans, let processor call Export()
TEST_F(OtlpHttpExporterTestPeer, ExportJsonIntegrationTest)
{
  size_t old_count = getCurrentRequestCount();
  auto exporter    = GetExporter(HttpRequestContentType::kJson);

  opentelemetry::sdk::resource::ResourceAttributes resource_attributes = {
      {"service.name", "unit_test_service"}, {"tenant.id", "test_user"}};
  resource_attributes["bool_value"]       = true;
  resource_attributes["int32_value"]      = static_cast<int32_t>(1);
  resource_attributes["uint32_value"]     = static_cast<uint32_t>(2);
  resource_attributes["int64_value"]      = static_cast<int64_t>(0x1100000000LL);
  resource_attributes["uint64_value"]     = static_cast<uint64_t>(0x1200000000ULL);
  resource_attributes["double_value"]     = static_cast<double>(3.1);
  resource_attributes["vec_bool_value"]   = std::vector<bool>{true, false, true};
  resource_attributes["vec_int32_value"]  = std::vector<int32_t>{1, 2};
  resource_attributes["vec_uint32_value"] = std::vector<uint32_t>{3, 4};
  resource_attributes["vec_int64_value"]  = std::vector<int64_t>{5, 6};
  resource_attributes["vec_uint64_value"] = std::vector<uint64_t>{7, 8};
  resource_attributes["vec_double_value"] = std::vector<double>{3.2, 3.3};
  resource_attributes["vec_string_value"] = std::vector<std::string>{"vector", "string"};
  auto resource = opentelemetry::sdk::resource::Resource::Create(resource_attributes);

  auto processor_opts                  = sdk::trace::BatchSpanProcessorOptions();
  processor_opts.max_export_batch_size = 5;
  processor_opts.max_queue_size        = 5;
  processor_opts.schedule_delay_millis = std::chrono::milliseconds(256);
  auto processor                       = std::unique_ptr<sdk::trace::SpanProcessor>(
      new sdk::trace::BatchSpanProcessor(std::move(exporter), processor_opts));
  auto provider = nostd::shared_ptr<trace::TracerProvider>(
      new sdk::trace::TracerProvider(std::move(processor), resource));

  std::string report_trace_id;
  {
    char trace_id_hex[2 * opentelemetry::trace::TraceId::kSize] = {0};
    auto tracer                                                 = provider->GetTracer("test");
    auto parent_span = tracer->StartSpan("Test parent span");

    opentelemetry::trace::StartSpanOptions child_span_opts = {};
    child_span_opts.parent                                 = parent_span->GetContext();

    auto child_span = tracer->StartSpan("Test child span", child_span_opts);
    child_span->End();
    parent_span->End();

    nostd::get<opentelemetry::trace::SpanContext>(child_span_opts.parent)
        .trace_id()
        .ToLowerBase16(MakeSpan(trace_id_hex));
    report_trace_id.assign(trace_id_hex, sizeof(trace_id_hex));
  }

  ASSERT_TRUE(waitForRequests(2, old_count + 1));
  auto check_json                   = received_requests_json_.back();
  auto resource_span                = *check_json["resource_spans"].begin();
  auto instrumentation_library_span = *resource_span["instrumentation_library_spans"].begin();
  auto span                         = *instrumentation_library_span["spans"].begin();
  auto received_trace_id            = span["trace_id"].get<std::string>();
  EXPECT_EQ(received_trace_id, report_trace_id);
  {
    auto custom_header = received_requests_headers_.find("Custom-Header-Key");
    ASSERT_TRUE(custom_header != received_requests_headers_.end());
    if (custom_header != received_requests_headers_.end())
    {
      EXPECT_EQ("Custom-Header-Value", custom_header->second);
    }
  }
}

// Create spans, let processor call Export()
TEST_F(OtlpHttpExporterTestPeer, ExportBinaryIntegrationTest)
{
  size_t old_count = getCurrentRequestCount();

  auto exporter = GetExporter(HttpRequestContentType::kBinary);

  opentelemetry::sdk::resource::ResourceAttributes resource_attributes = {
      {"service.name", "unit_test_service"}, {"tenant.id", "test_user"}};
  resource_attributes["bool_value"]       = true;
  resource_attributes["int32_value"]      = static_cast<int32_t>(1);
  resource_attributes["uint32_value"]     = static_cast<uint32_t>(2);
  resource_attributes["int64_value"]      = static_cast<int64_t>(0x1100000000LL);
  resource_attributes["uint64_value"]     = static_cast<uint64_t>(0x1200000000ULL);
  resource_attributes["double_value"]     = static_cast<double>(3.1);
  resource_attributes["vec_bool_value"]   = std::vector<bool>{true, false, true};
  resource_attributes["vec_int32_value"]  = std::vector<int32_t>{1, 2};
  resource_attributes["vec_uint32_value"] = std::vector<uint32_t>{3, 4};
  resource_attributes["vec_int64_value"]  = std::vector<int64_t>{5, 6};
  resource_attributes["vec_uint64_value"] = std::vector<uint64_t>{7, 8};
  resource_attributes["vec_double_value"] = std::vector<double>{3.2, 3.3};
  resource_attributes["vec_string_value"] = std::vector<std::string>{"vector", "string"};
  auto resource = opentelemetry::sdk::resource::Resource::Create(resource_attributes);

  auto processor_opts                  = sdk::trace::BatchSpanProcessorOptions();
  processor_opts.max_export_batch_size = 5;
  processor_opts.max_queue_size        = 5;
  processor_opts.schedule_delay_millis = std::chrono::milliseconds(256);

  auto processor = std::unique_ptr<sdk::trace::SpanProcessor>(
      new sdk::trace::BatchSpanProcessor(std::move(exporter), processor_opts));
  auto provider = nostd::shared_ptr<trace::TracerProvider>(
      new sdk::trace::TracerProvider(std::move(processor), resource));

  std::string report_trace_id;
  {
    uint8_t trace_id_binary[opentelemetry::trace::TraceId::kSize] = {0};
    auto tracer                                                   = provider->GetTracer("test");
    auto parent_span = tracer->StartSpan("Test parent span");

    opentelemetry::trace::StartSpanOptions child_span_opts = {};
    child_span_opts.parent                                 = parent_span->GetContext();

    auto child_span = tracer->StartSpan("Test child span", child_span_opts);
    child_span->End();
    parent_span->End();

    nostd::get<opentelemetry::trace::SpanContext>(child_span_opts.parent)
        .trace_id()
        .CopyBytesTo(MakeSpan(trace_id_binary));
    report_trace_id.assign(reinterpret_cast<char *>(trace_id_binary), sizeof(trace_id_binary));
  }

  ASSERT_TRUE(waitForRequests(2, old_count + 1));

  auto received_trace_id = received_requests_binary_.back()
                               .resource_spans(0)
                               .instrumentation_library_spans(0)
                               .spans(0)
                               .trace_id();
  EXPECT_EQ(received_trace_id, report_trace_id);
}

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
  putenv("OTEL_EXPORTER_OTLP_ENDPOINT=http://localhost:9999");
  putenv("OTEL_EXPORTER_OTLP_TIMEOUT=20s");
  putenv("OTEL_EXPORTER_OTLP_HEADERS=k1=v1,k2=v2");
  putenv("OTEL_EXPORTER_OTLP_TRACES_HEADERS=k1=v3,k1=v4");

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
#    if defined(_MSC_VER)
  putenv("OTEL_EXPORTER_OTLP_ENDPOINT=");
  putenv("OTEL_EXPORTER_OTLP_TIMEOUT=");
  putenv("OTEL_EXPORTER_OTLP_HEADERS=");
  putenv("OTEL_EXPORTER_OTLP_TRACES_HEADERS=");

#    else
  unsetenv("OTEL_EXPORTER_OTLP_ENDPOINT");
  unsetenv("OTEL_EXPORTER_OTLP_TIMEOUT");
  unsetenv("OTEL_EXPORTER_OTLP_HEADERS");
  unsetenv("OTEL_EXPORTER_OTLP_TRACES_HEADERS");

#    endif
}

TEST_F(OtlpHttpExporterTestPeer, ConfigFromTracesEnv)
{
  const std::string url = "http://localhost:9999/v1/traces";
  putenv("OTEL_EXPORTER_OTLP_TRACES_ENDPOINT=http://localhost:9999/v1/traces");
  putenv("OTEL_EXPORTER_OTLP_TIMEOUT=20s");
  putenv("OTEL_EXPORTER_OTLP_HEADERS=k1=v1,k2=v2");
  putenv("OTEL_EXPORTER_OTLP_TRACES_HEADERS=k1=v3,k1=v4");

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
#    if defined(_MSC_VER)
  putenv("OTEL_EXPORTER_OTLP_TRACES_ENDPOINT=");
  putenv("OTEL_EXPORTER_OTLP_TIMEOUT=");
  putenv("OTEL_EXPORTER_OTLP_HEADERS=");
  putenv("OTEL_EXPORTER_OTLP_TRACES_HEADERS=");

#    else
  unsetenv("OTEL_EXPORTER_OTLP_TRACES_ENDPOINT");
  unsetenv("OTEL_EXPORTER_OTLP_TIMEOUT");
  unsetenv("OTEL_EXPORTER_OTLP_HEADERS");
  unsetenv("OTEL_EXPORTER_OTLP_TRACES_HEADERS");

#    endif
}
#  endif

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
#endif
