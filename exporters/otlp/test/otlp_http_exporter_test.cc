// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef HAVE_CPP_STDLIB

#  include "opentelemetry/exporters/otlp/otlp_http_exporter.h"

#  include "opentelemetry/exporters/otlp/protobuf_include_prefix.h"

#  include "opentelemetry/proto/collector/trace/v1/trace_service.pb.h"

#  include "opentelemetry/exporters/otlp/protobuf_include_suffix.h"

#  include "opentelemetry/ext/http/server/http_server.h"
#  include "opentelemetry/sdk/trace/simple_processor.h"
#  include "opentelemetry/sdk/trace/tracer_provider.h"
#  include "opentelemetry/trace/provider.h"

#  include <gtest/gtest.h>

#  include "nlohmann/json.hpp"

using namespace testing;

namespace http_client = opentelemetry::ext::http::client;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

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
    server_address_ = "http://" + os.str() + kDefaultTracePath;
    server_.setServerName(os.str());
    server_.setKeepalive(false);
    server_.addHandler(kDefaultTracePath, *this);
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
    if (request.uri == kDefaultTracePath)
    {
      response.headers["Content-Type"] = "application/json";
      std::unique_lock<std::mutex> lk(mtx_requests);
      if (request.headers["Content-Type"] == kHttpBinaryContentType)
      {
        opentelemetry::proto::collector::trace::v1::ExportTraceServiceRequest request_body;
        if (request_body.ParseFromArray(&request.content[0], request.content.size()))
        {
          received_requests_binary_.push_back(request_body);
          response.body = "{\"code\": 0, \"message\": \"success\"}";
        }
        else
        {
          response.body = "{\"code\": 400, \"message\": \"Parse binary failed\"}";
          return 400;
        }
      }
      else if (request.headers["Content-Type"] == kHttpJsonContentType)
      {
        auto json                        = nlohmann::json::parse(request.content, nullptr, false);
        response.headers["Content-Type"] = "application/json";
        if (json.is_discarded())
        {
          response.body = "{\"code\": 400, \"message\": \"Parse json failed\"}";
          return 400;
        }
        else
        {
          received_requests_json_.push_back(json);
          response.body = "{\"code\": 0, \"message\": \"success\"}";
        }
      }
      else
      {
        response.body = "{\"code\": 400, \"message\": \"Unsupported content type\"}";
        return 400;
      }

      return 200;
    }
    else
    {
      std::unique_lock<std::mutex> lk(mtx_requests);
      response.headers["Content-Type"] = "text/plain";
      response.body                    = "404 Not Found";
      return 200;
    }
  }

  bool waitForRequests(unsigned timeOutSec, unsigned expected_count = 1)
  {
    std::unique_lock<std::mutex> lk(mtx_requests);
    if (cv_got_events.wait_for(lk, std::chrono::milliseconds(1000 * timeOutSec), [&] {
          return received_requests_json_.size() + received_requests_binary_.size() >=
                 expected_count;
        }))
    {
      return true;
    }
    return false;
  }

public:
  std::unique_ptr<sdk::trace::SpanExporter> GetExporter(HttpRequestContentType content_type)
  {
    OtlpHttpExporterOptions opts;
    opts.url          = server_address_;
    opts.content_type = content_type;
    return std::unique_ptr<sdk::trace::SpanExporter>(new OtlpHttpExporter(opts));
  }

  // Get the options associated with the given exporter.
  const OtlpHttpExporterOptions &GetOptions(std::unique_ptr<OtlpHttpExporter> &exporter)
  {
    return exporter->options_;
  }
};

// Call Export() directly
TEST_F(OtlpHttpExporterTestPeer, ExportUnitTest)
{
  auto exporter = GetExporter(HttpRequestContentType::kJson);

  auto recordable_1 = exporter->MakeRecordable();
  recordable_1->SetName("Test span 1");
  auto recordable_2 = exporter->MakeRecordable();
  recordable_2->SetName("Test span 2");

  // Test successful RPC
  nostd::span<std::unique_ptr<sdk::trace::Recordable>> batch_1(&recordable_1, 1);
  EXPECT_CALL(*mock_stub, Export(_, _, _)).Times(Exactly(1)).WillOnce(Return(grpc::Status::OK));
  auto result = exporter->Export(batch_1);
  EXPECT_EQ(sdk::common::ExportResult::kSuccess, result);

  // Test failed RPC
  nostd::span<std::unique_ptr<sdk::trace::Recordable>> batch_2(&recordable_2, 1);
  EXPECT_CALL(*mock_stub, Export(_, _, _))
      .Times(Exactly(1))
      .WillOnce(Return(grpc::Status::CANCELLED));
  result = exporter->Export(batch_2);
  EXPECT_EQ(sdk::common::ExportResult::kFailure, result);
}

// Create spans, let processor call Export()
TEST_F(OtlpHttpExporterTestPeer, ExportJsonIntegrationTest)
{
  auto exporter = GetExporter(HttpRequestContentType::kJson);

  opentelemetry::sdk::resource::ResourceAttributes resource_attributes = {
      {"service.name", 'unit_test_service'}, {"tenant.id", 'test_user'}};
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

  auto processor = std::unique_ptr<sdk::trace::SpanProcessor>(
      new sdk::trace::SimpleSpanProcessor(std::move(exporter)));
  auto provider = nostd::shared_ptr<trace::TracerProvider>(
      new sdk::trace::TracerProvider(std::move(processor), resource));
  auto tracer = provider->GetTracer("test");

  EXPECT_CALL(*mock_stub, Export(_, _, _))
      .Times(AtLeast(1))
      .WillRepeatedly(Return(grpc::Status::OK));

  auto parent_span = tracer->StartSpan("Test parent span");
  auto child_span  = tracer->StartSpan("Test child span");
  child_span->End();
  parent_span->End();
}

// Create spans, let processor call Export()
TEST_F(OtlpHttpExporterTestPeer, ExportBinaryIntegrationTest)
{
  auto exporter = GetExporter(HttpRequestContentType::kBinary);

  opentelemetry::sdk::resource::ResourceAttributes resource_attributes = {
      {"service.name", 'unit_test_service'}, {"tenant.id", 'test_user'}};
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

  auto processor = std::unique_ptr<sdk::trace::SpanProcessor>(
      new sdk::trace::SimpleSpanProcessor(std::move(exporter)));
  auto provider = nostd::shared_ptr<trace::TracerProvider>(
      new sdk::trace::TracerProvider(std::move(processor), resource));
  auto tracer = provider->GetTracer("test");

  EXPECT_CALL(*mock_stub, Export(_, _, _))
      .Times(AtLeast(1))
      .WillRepeatedly(Return(grpc::Status::OK));

  auto parent_span = tracer->StartSpan("Test parent span");
  auto child_span  = tracer->StartSpan("Test child span");
  child_span->End();
  parent_span->End();
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
TEST_F(OtlpHttpExporterTestPeer, ConfigUseJsonNameTest)
{
  OtlpHttpExporterOptions opts;
  opts.json_bytes_mapping = JsonBytesMappingKind::kHex;
  std::unique_ptr<OtlpHttpExporter> exporter(new OtlpHttpExporter(opts));
  EXPECT_EQ(GetOptions(exporter).json_bytes_mapping, JsonBytesMappingKind::kHex);
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
#endif
