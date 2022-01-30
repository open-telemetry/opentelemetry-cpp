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
#  include "gmock/gmock.h"

#  include <iostream>
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
  OtlpHttpExporterOptions options;
  options.content_type  = content_type;
  options.console_debug = true;
  options.http_headers.insert(
      std::make_pair<const std::string, std::string>("Custom-Header-Key", "Custom-Header-Value"));
  OtlpHttpClientOptions otlpHttpClientOptions(
      options.url, options.content_type, options.json_bytes_mapping, options.use_json_name,
      options.console_debug, options.timeout, options.http_headers);
  return new MockOtlpHttpClient(std::move(otlpHttpClientOptions));
}

class IsValidMessageMatcher
{
public:
  IsValidMessageMatcher(const std::string &trace_id) : trace_id_(trace_id) {}
  template <typename T>
  bool MatchAndExplain(const T &p, MatchResultListener * /* listener */) const
  {
    OtlpHttpExporterOptions options;
    options.content_type  = HttpRequestContentType::kJson;
    options.console_debug = true;
    options.http_headers.insert(
        std::make_pair<const std::string, std::string>("Custom-Header-Key", "Custom-Header-Value"));
    OtlpHttpClientOptions otlpHttpClientOptions(
        options.url, options.content_type, options.json_bytes_mapping, options.use_json_name,
        options.console_debug, options.timeout, options.http_headers);
    nlohmann::json check_json;
    OtlpHttpClient::ConvertGenericMessageToJson(check_json, p, otlpHttpClientOptions);
    auto resource_span                = *check_json["resource_spans"].begin();
    auto instrumentation_library_span = *resource_span["instrumentation_library_spans"].begin();
    auto span                         = *instrumentation_library_span["spans"].begin();
    auto received_trace_id            = span["trace_id"].get<std::string>();

    if (trace_id_ != received_trace_id)
    {
      opentelemetry::ext::http::client::Body body;
      OtlpHttpClient::SerializeToHttpBody(body, p);
      // received_trace_id = body.resource_spans(0)
      //                      .instrumentation_library_spans(0)
      //                      .spans(0)
      //                      .trace_id();

      auto msg = std::string(body.begin(), body.end());

      std::puts(received_trace_id.c_str());
      std::puts(msg.c_str());
      std::puts("######");
    }
    return trace_id_ == received_trace_id;
  }

  // Describes the property of a value matching this matcher.
  void DescribeTo(std::ostream *os) const { *os << "is not NULL"; }

  // Describes the property of a value NOT matching this matcher.
  void DescribeNegationTo(std::ostream *os) const { *os << "is NULL"; }

private:
  std::string trace_id_;
};

// To construct a polymorphic matcher, pass an instance of the class
// to MakePolymorphicMatcher().  Note the return type.
PolymorphicMatcher<IsValidMessageMatcher> IsValidMessage(const std::string &trace_id)
{
  return MakePolymorphicMatcher(IsValidMessageMatcher(trace_id));
}

// Create spans, let processor call Export()
TEST_F(OtlpHttpExporterTestPeer, ExportJsonIntegrationTest)
{
  auto mockOtlpHttpClient = GetMockOtlpHttpClient(HttpRequestContentType::kJson);
  auto exporter           = GetExporter(std::unique_ptr<OtlpHttpClient>{mockOtlpHttpClient});

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
  EXPECT_CALL(*mockOtlpHttpClient, Export(IsValidMessage(report_trace_id)))
      .Times(Exactly(1))
      .WillOnce(Return(sdk::common::ExportResult::kSuccess));
}

// Create spans, let processor call Export()
TEST_F(OtlpHttpExporterTestPeer, ExportBinaryIntegrationTest)
{
  auto mockOtlpHttpClient = GetMockOtlpHttpClient(HttpRequestContentType::kBinary);
  auto exporter           = GetExporter(std::unique_ptr<OtlpHttpClient>{mockOtlpHttpClient});

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
  child_span->End();
  parent_span->End();

  nostd::get<trace_api::SpanContext>(child_span_opts.parent)
      .trace_id()
      .ToLowerBase16(MakeSpan(trace_id_hex));
  report_trace_id.assign(reinterpret_cast<char *>(trace_id_hex), sizeof(trace_id_hex));
  EXPECT_CALL(*mockOtlpHttpClient, Export(IsValidMessage(report_trace_id)))
      .Times(Exactly(1))
      .WillOnce(Return(sdk::common::ExportResult::kSuccess));
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

  unsetenv("OTEL_EXPORTER_OTLP_TRACES_ENDPOINT");
  unsetenv("OTEL_EXPORTER_OTLP_TIMEOUT");
  unsetenv("OTEL_EXPORTER_OTLP_HEADERS");
  unsetenv("OTEL_EXPORTER_OTLP_TRACES_HEADERS");
}
#  endif

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
#endif
