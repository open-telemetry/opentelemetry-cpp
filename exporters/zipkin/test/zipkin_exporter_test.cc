// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef HAVE_CPP_STDLIB

#  include "opentelemetry/exporters/zipkin/zipkin_exporter.h"
#  include <string>
#  include "opentelemetry/ext/http/client/curl/http_client_curl.h"
#  include "opentelemetry/ext/http/server/http_server.h"
#  include "opentelemetry/sdk/trace/batch_span_processor.h"
#  include "opentelemetry/sdk/trace/tracer_provider.h"
#  include "opentelemetry/trace/provider.h"

#  include <gtest/gtest.h>
#  include <gmock/gmock.h>

#  include "nlohmann/json.hpp"

#  if defined(_MSC_VER)
#    include "opentelemetry/sdk/common/env_variables.h"
using opentelemetry::sdk::common::setenv;
using opentelemetry::sdk::common::unsetenv;
#  endif
namespace sdk_common = opentelemetry::sdk::common;
using namespace testing;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace zipkin
{

namespace trace_api = opentelemetry::trace;
namespace resource  = opentelemetry::sdk::resource;

template <class T, size_t N>
static nostd::span<T, N> MakeSpan(T (&array)[N])
{
  return nostd::span<T, N>(array);
}

class ZipkinExporterTestPeer : public ::testing::Test, HTTP_SERVER_NS::HttpRequestCallback
{
protected:
  HTTP_SERVER_NS::HttpServer server_;
  std::string server_address_;
  std::atomic<bool> is_setup_;
  std::atomic<bool> is_running_;
  std::mutex mtx_requests;
  std::condition_variable cv_got_events;
  std::vector<nlohmann::json> received_requests_json_;
  std::map<std::string, std::string> received_requests_headers_;

public:
  ZipkinExporterTestPeer() : is_setup_(false), is_running_(false){};

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
    std::string kHttpJsonContentType{"application/json"};
    if (request.uri == "/v1/traces")
    {
      response.headers["Content-Type"] = kHttpJsonContentType;
      std::unique_lock<std::mutex> lk(mtx_requests);
      if (nullptr != request_content_type && *request_content_type == kHttpJsonContentType)
      {
        auto json                        = nlohmann::json::parse(request.content, nullptr, false);
        response.headers["Content-Type"] = kHttpJsonContentType;
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

  size_t getCurrentRequestCount() const { return received_requests_json_.size(); }

public:
  std::unique_ptr<sdk::trace::SpanExporter> GetExporter()
  {
    ZipkinExporterOptions opts;
    opts.endpoint = server_address_;
    opts.headers.insert(
        std::make_pair<const std::string, std::string>("Custom-Header-Key", "Custom-Header-Value"));
    return std::unique_ptr<sdk::trace::SpanExporter>(new ZipkinExporter(opts));
  }

  std::unique_ptr<sdk::trace::SpanExporter> GetExporter(
      std::shared_ptr<opentelemetry::ext::http::client::HttpClientSync> http_client)
  {
    return std::unique_ptr<sdk::trace::SpanExporter>(new ZipkinExporter(http_client));
  }

  // Get the options associated with the given exporter.
  const ZipkinExporterOptions &GetOptions(std::unique_ptr<ZipkinExporter> &exporter)
  {
    return exporter->options_;
  }
};

class MockHttpClient : public opentelemetry::ext::http::client::HttpClientSync
{
public:
  MOCK_METHOD(ext::http::client::Result,
              Post,
              (const nostd::string_view &,
               const ext::http::client::Body &,
               const ext::http::client::Headers &),
              (noexcept, override));
  MOCK_METHOD(ext::http::client::Result,
              Get,
              (const nostd::string_view &, const ext::http::client::Headers &),
              (noexcept, override));
};

// Create spans, let processor call Export()
TEST_F(ZipkinExporterTestPeer, ExportJsonIntegrationTest)
{
  size_t old_count = getCurrentRequestCount();
  auto exporter    = GetExporter();

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
  auto processor                       = std::unique_ptr<sdk::trace::SpanProcessor>(
      new sdk::trace::BatchSpanProcessor(std::move(exporter), processor_opts));
  auto provider = nostd::shared_ptr<trace::TracerProvider>(
      new sdk::trace::TracerProvider(std::move(processor), resource));

  std::string report_trace_id;
  {
    char trace_id_hex[2 * trace_api::TraceId::kSize] = {0};
    auto tracer                                      = provider->GetTracer("test");
    auto parent_span                                 = tracer->StartSpan("Test parent span");

    trace_api::StartSpanOptions child_span_opts = {};
    child_span_opts.parent                      = parent_span->GetContext();

    auto child_span = tracer->StartSpan("Test child span", child_span_opts);
    child_span->End();
    parent_span->End();

    nostd::get<trace_api::SpanContext>(child_span_opts.parent)
        .trace_id()
        .ToLowerBase16(MakeSpan(trace_id_hex));
    report_trace_id.assign(trace_id_hex, sizeof(trace_id_hex));
  }

  ASSERT_TRUE(waitForRequests(30, old_count + 1));
  auto check_json        = received_requests_json_.back();
  auto trace_id_kv       = check_json.at(0).find("traceId");
  auto received_trace_id = trace_id_kv.value().get<std::string>();
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
TEST_F(ZipkinExporterTestPeer, ShutdownTest)
{
  auto mock_http_client = new MockHttpClient;
  // Leave a comment line here or different version of clang-format has a different result here
  auto exporter = GetExporter(
      std::shared_ptr<opentelemetry::ext::http::client::HttpClientSync>{mock_http_client});
  auto recordable_1 = exporter->MakeRecordable();
  recordable_1->SetName("Test span 1");
  auto recordable_2 = exporter->MakeRecordable();
  recordable_2->SetName("Test span 2");

  // exporter shuold not be shutdown by default
  nostd::span<std::unique_ptr<sdk::trace::Recordable>> batch_1(&recordable_1, 1);
  EXPECT_CALL(*mock_http_client, Post(_, _, _))
      .Times(Exactly(1))
      .WillOnce(Return(ByMove(std::move(ext::http::client::Result{
          std::unique_ptr<ext::http::client::Response>{new ext::http::client::curl::Response()},
          ext::http::client::SessionState::Response}))));
  auto result = exporter->Export(batch_1);
  EXPECT_EQ(sdk_common::ExportResult::kSuccess, result);

  exporter->Shutdown();

  nostd::span<std::unique_ptr<sdk::trace::Recordable>> batch_2(&recordable_2, 1);
  result = exporter->Export(batch_2);
  EXPECT_EQ(sdk_common::ExportResult::kFailure, result);
}

// Test exporter configuration options
TEST_F(ZipkinExporterTestPeer, ConfigTest)
{
  ZipkinExporterOptions opts;
  opts.endpoint = "http://localhost:45455/v1/traces";
  std::unique_ptr<ZipkinExporter> exporter(new ZipkinExporter(opts));
  EXPECT_EQ(GetOptions(exporter).endpoint, "http://localhost:45455/v1/traces");
}

#  ifndef NO_GETENV
// Test exporter configuration options from env
TEST_F(ZipkinExporterTestPeer, ConfigFromEnv)
{
  const std::string endpoint = "http://localhost:9999/v1/traces";
  setenv("OTEL_EXPORTER_ZIPKIN_ENDPOINT", endpoint.c_str(), 1);

  std::unique_ptr<ZipkinExporter> exporter(new ZipkinExporter());
  EXPECT_EQ(GetOptions(exporter).endpoint, endpoint);

  unsetenv("OTEL_EXPORTER_ZIPKIN_ENDPOINT");
}

#  endif  // NO_GETENV

}  // namespace zipkin
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
#endif  // HAVE_CPP_STDLIB
