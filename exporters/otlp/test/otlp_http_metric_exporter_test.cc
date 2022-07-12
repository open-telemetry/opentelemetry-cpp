// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW

#  include <chrono>
#  include <thread>

#  include "opentelemetry/exporters/otlp/otlp_http_metric_exporter.h"

#  include "opentelemetry/exporters/otlp/protobuf_include_prefix.h"

#  include "opentelemetry/proto/collector/metrics/v1/metrics_service.pb.h"

#  include "opentelemetry/exporters/otlp/protobuf_include_suffix.h"

#  include "opentelemetry/common/key_value_iterable_view.h"
#  include "opentelemetry/ext/http/client/http_client_factory.h"
#  include "opentelemetry/ext/http/client/nosend/http_client_nosend.h"
#  include "opentelemetry/ext/http/server/http_server.h"
#  include "opentelemetry/sdk/metrics/aggregation/default_aggregation.h"
#  include "opentelemetry/sdk/metrics/aggregation/histogram_aggregation.h"
#  include "opentelemetry/sdk/metrics/data/metric_data.h"
#  include "opentelemetry/sdk/metrics/instruments.h"
#  include "opentelemetry/sdk/resource/resource.h"

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

template <class T, size_t N>
static nostd::span<T, N> MakeSpan(T (&array)[N])
{
  return nostd::span<T, N>(array);
}

OtlpHttpClientOptions MakeOtlpHttpClientOptions(HttpRequestContentType content_type,
                                                bool async_mode)
{
  OtlpHttpMetricExporterOptions options;
  options.content_type  = content_type;
  options.console_debug = true;
  options.http_headers.insert(
      std::make_pair<const std::string, std::string>("Custom-Header-Key", "Custom-Header-Value"));
  OtlpHttpClientOptions otlp_http_client_options(
      options.url, options.content_type, options.json_bytes_mapping, options.use_json_name,
      options.console_debug, options.timeout, options.http_headers);
  if (!async_mode)
  {
    otlp_http_client_options.max_concurrent_requests = 0;
  }
  return otlp_http_client_options;
}

namespace http_client = opentelemetry::ext::http::client;

class OtlpHttpMetricExporterTestPeer : public ::testing::Test
{
public:
  std::unique_ptr<opentelemetry::sdk::metrics::MetricExporter> GetExporter(
      std::unique_ptr<OtlpHttpClient> http_client)
  {
    return std::unique_ptr<opentelemetry::sdk::metrics::MetricExporter>(
        new OtlpHttpMetricExporter(std::move(http_client)));
  }

  // Get the options associated with the given exporter.
  const OtlpHttpMetricExporterOptions &GetOptions(std::unique_ptr<OtlpHttpMetricExporter> &exporter)
  {
    return exporter->options_;
  }
  static std::pair<OtlpHttpClient *, std::shared_ptr<http_client::HttpClient>>
  GetMockOtlpHttpClient(HttpRequestContentType content_type, bool async_mode = false)
  {
    auto http_client = http_client::HttpClientFactory::CreateNoSend();
    return {new OtlpHttpClient(MakeOtlpHttpClientOptions(content_type, async_mode), http_client),
            http_client};
  }

  void ExportJsonIntegrationTestExportSumPointData(
#  ifdef ENABLE_ASYNC_EXPORT
      bool async_mode
#  endif
  )
  {
    auto mock_otlp_client =
        OtlpHttpMetricExporterTestPeer::GetMockOtlpHttpClient(HttpRequestContentType::kJson
#  ifdef ENABLE_ASYNC_EXPORT
                                                              ,
                                                              async_mode
#  endif
        );
    auto mock_otlp_http_client = mock_otlp_client.first;
    auto client                = mock_otlp_client.second;
    auto exporter = GetExporter(std::unique_ptr<OtlpHttpClient>{mock_otlp_http_client});

    opentelemetry::sdk::metrics::SumPointData sum_point_data{};
    sum_point_data.value_ = 10.0;
    opentelemetry::sdk::metrics::SumPointData sum_point_data2{};
    sum_point_data2.value_ = 20.0;
    opentelemetry::sdk::metrics::ResourceMetrics data;
    auto resource = opentelemetry::sdk::resource::Resource::Create(
        opentelemetry::sdk::resource::ResourceAttributes{});
    data.resource_ = &resource;
    auto instrumentation_library =
        opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary::Create("library_name",
                                                                                   "1.5.0");
    opentelemetry::sdk::metrics::MetricData metric_data{
        opentelemetry::sdk::metrics::InstrumentDescriptor{
            "metrics_library_name", "metrics_description", "metrics_unit",
            opentelemetry::sdk::metrics::InstrumentType::kCounter,
            opentelemetry::sdk::metrics::InstrumentValueType::kDouble},
        opentelemetry::sdk::metrics::AggregationTemporality::kDelta,
        opentelemetry::common::SystemTimestamp{}, opentelemetry::common::SystemTimestamp{},
        std::vector<opentelemetry::sdk::metrics::PointDataAttributes>{
            {opentelemetry::sdk::metrics::PointAttributes{{"a1", "b1"}}, sum_point_data},
            {opentelemetry::sdk::metrics::PointAttributes{{"a2", "b2"}}, sum_point_data2}}};
    data.instrumentation_info_metric_data_ =
        std::vector<opentelemetry::sdk::metrics::InstrumentationInfoMetrics>{
            {instrumentation_library.get(),
             std::vector<opentelemetry::sdk::metrics::MetricData>{metric_data}}};

    auto no_send_client = std::static_pointer_cast<http_client::nosend::HttpClient>(client);
    auto mock_session =
        std::static_pointer_cast<http_client::nosend::Session>(no_send_client->session_);
    EXPECT_CALL(*mock_session, SendRequest)
        .WillOnce([&mock_session, &data, &resource](
                      std::shared_ptr<opentelemetry::ext::http::client::EventHandler> callback) {
          auto check_json =
              nlohmann::json::parse(mock_session->GetRequest()->body_, nullptr, false);

          auto resource_metrics = *check_json["resource_metrics"].begin();
          // auto scope_metrics    = *resource_metrics["scope_metrics"].begin();
          // auto scope = scope_metrics["scope"];
          auto instrumentation_library_metrics =
              *resource_metrics["instrumentation_library_metrics"].begin();
          auto scope = instrumentation_library_metrics["instrumentation_library"];
          EXPECT_EQ("library_name", scope["name"].get<std::string>());
          EXPECT_EQ("1.5.0", scope["version"].get<std::string>());

          // auto metric = *scope_metrics["metrics"].begin();
          auto metric = *instrumentation_library_metrics["metrics"].begin();
          EXPECT_EQ("metrics_library_name", metric["name"].get<std::string>());
          EXPECT_EQ("metrics_description", metric["description"].get<std::string>());
          EXPECT_EQ("metrics_unit", metric["unit"].get<std::string>());

          auto data_points = metric["sum"]["data_points"];
          EXPECT_EQ(10.0, data_points[0]["as_double"].get<double>());
          EXPECT_EQ(20.0, data_points[1]["as_double"].get<double>());

          auto custom_header = mock_session->GetRequest()->headers_.find("Custom-Header-Key");
          ASSERT_TRUE(custom_header != mock_session->GetRequest()->headers_.end());
          if (custom_header != mock_session->GetRequest()->headers_.end())
          {
            EXPECT_EQ("Custom-Header-Value", custom_header->second);
          }

          http_client::nosend::Response response;
          response.Finish(*callback.get());
        });

    auto result = exporter->Export(data);
    EXPECT_EQ(result, opentelemetry::sdk::common::ExportResult::kSuccess);

    exporter->ForceFlush();
  }

  void ExportBinaryIntegrationTestExportSumPointData(
#  ifdef ENABLE_ASYNC_EXPORT
      bool async_mode
#  endif
  )
  {
    auto mock_otlp_client =
        OtlpHttpMetricExporterTestPeer::GetMockOtlpHttpClient(HttpRequestContentType::kBinary
#  ifdef ENABLE_ASYNC_EXPORT
                                                              ,
                                                              async_mode
#  endif
        );
    auto mock_otlp_http_client = mock_otlp_client.first;
    auto client                = mock_otlp_client.second;
    auto exporter = GetExporter(std::unique_ptr<OtlpHttpClient>{mock_otlp_http_client});

    bool attribute_storage_bool_value[]          = {true, false, true};
    int32_t attribute_storage_int32_value[]      = {1, 2};
    uint32_t attribute_storage_uint32_value[]    = {3, 4};
    int64_t attribute_storage_int64_value[]      = {5, 6};
    uint64_t attribute_storage_uint64_value[]    = {7, 8};
    double attribute_storage_double_value[]      = {3.2, 3.3};
    std::string attribute_storage_string_value[] = {"vector", "string"};

    opentelemetry::sdk::metrics::SumPointData sum_point_data{};
    sum_point_data.value_ = 10.0;
    opentelemetry::sdk::metrics::SumPointData sum_point_data2{};
    sum_point_data2.value_ = 20.0;
    opentelemetry::sdk::metrics::ResourceMetrics data;
    auto resource = opentelemetry::sdk::resource::Resource::Create(
        opentelemetry::sdk::resource::ResourceAttributes{});
    data.resource_ = &resource;
    auto instrumentation_library =
        opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary::Create("library_name",
                                                                                   "1.5.0");
    opentelemetry::sdk::metrics::MetricData metric_data{
        opentelemetry::sdk::metrics::InstrumentDescriptor{
            "metrics_library_name", "metrics_description", "metrics_unit",
            opentelemetry::sdk::metrics::InstrumentType::kCounter,
            opentelemetry::sdk::metrics::InstrumentValueType::kDouble},
        opentelemetry::sdk::metrics::AggregationTemporality::kDelta,
        opentelemetry::common::SystemTimestamp{}, opentelemetry::common::SystemTimestamp{},
        std::vector<opentelemetry::sdk::metrics::PointDataAttributes>{
            {opentelemetry::sdk::metrics::PointAttributes{{"a1", "b1"}}, sum_point_data},
            {opentelemetry::sdk::metrics::PointAttributes{{"a2", "b2"}}, sum_point_data2}}};
    data.instrumentation_info_metric_data_ =
        std::vector<opentelemetry::sdk::metrics::InstrumentationInfoMetrics>{
            {instrumentation_library.get(),
             std::vector<opentelemetry::sdk::metrics::MetricData>{metric_data}}};

    auto no_send_client = std::static_pointer_cast<http_client::nosend::HttpClient>(client);
    auto mock_session =
        std::static_pointer_cast<http_client::nosend::Session>(no_send_client->session_);

    EXPECT_CALL(*mock_session, SendRequest)
        .WillOnce([&mock_session, &data, &resource](
                      std::shared_ptr<opentelemetry::ext::http::client::EventHandler> callback) {
          opentelemetry::proto::collector::metrics::v1::ExportMetricsServiceRequest request_body;
          request_body.ParseFromArray(&mock_session->GetRequest()->body_[0],
                                      static_cast<int>(mock_session->GetRequest()->body_.size()));
          // auto &scope_metrics = request_body.resource_metrics(0).scope_metrics(0);
          // auto &scope = instrumentation_library_metrics.scope();
          auto &instrumentation_library_metrics =
              request_body.resource_metrics(0).instrumentation_library_metrics(0);
          auto &scope = instrumentation_library_metrics.instrumentation_library();
          EXPECT_EQ("library_name", scope.name());
          EXPECT_EQ("1.5.0", scope.version());

          // auto metric = *scope_metrics["metrics"].begin();
          auto &metric = instrumentation_library_metrics.metrics(0);
          EXPECT_EQ("metrics_library_name", metric.name());
          EXPECT_EQ("metrics_description", metric.description());
          EXPECT_EQ("metrics_unit", metric.unit());

          auto &data_points = metric.sum().data_points();
          EXPECT_EQ(10.0, data_points.Get(0).as_double());
          bool has_attributes = false;
          for (auto &kv : data_points.Get(0).attributes())
          {
            if (kv.key() == "a1")
            {
              EXPECT_EQ("b1", kv.value().string_value());
              has_attributes = true;
            }
          }
          EXPECT_TRUE(has_attributes);

          EXPECT_EQ(20.0, data_points.Get(1).as_double());
          has_attributes = false;
          for (auto &kv : data_points.Get(1).attributes())
          {
            if (kv.key() == "a2")
            {
              EXPECT_EQ("b2", kv.value().string_value());
              has_attributes = true;
            }
          }
          EXPECT_TRUE(has_attributes);

          http_client::nosend::Response response;
          response.Finish(*callback.get());
        });

    auto result = exporter->Export(data);
    EXPECT_EQ(result, opentelemetry::sdk::common::ExportResult::kSuccess);

    exporter->ForceFlush();
  }
};

TEST(OtlpHttpMetricExporterTest, Shutdown)
{
  auto exporter =
      std::unique_ptr<opentelemetry::sdk::metrics::MetricExporter>(new OtlpHttpMetricExporter());
  ASSERT_TRUE(exporter->Shutdown());
  auto result = exporter->Export(opentelemetry::sdk::metrics::ResourceMetrics{});
  EXPECT_EQ(result, opentelemetry::sdk::common::ExportResult::kFailure);
}

#  ifdef ENABLE_ASYNC_EXPORT
TEST_F(OtlpHttpMetricExporterTestPeer, ExportJsonIntegrationTestSumPointDataAsync)
{
  ExportJsonIntegrationTestExportSumPointData(true);
}
TEST_F(OtlpHttpMetricExporterTestPeer, ExportJsonIntegrationTestSumPointDataSync)
{
  ExportJsonIntegrationTestExportSumPointData(false);
}
TEST_F(OtlpHttpMetricExporterTestPeer, ExportBinaryIntegrationTestSumPointDataAsync)
{
  ExportBinaryIntegrationTestExportSumPointData(true);
}
TEST_F(OtlpHttpMetricExporterTestPeer, ExportBinaryIntegrationTestSumPointDataSync)
{
  ExportBinaryIntegrationTestExportSumPointData(false);
}
#  else
TEST_F(OtlpHttpMetricExporterTestPeer, ExportJsonIntegrationTestSumPointData)
{
  ExportJsonIntegrationTestExportSumPointData();
}
TEST_F(OtlpHttpMetricExporterTestPeer, ExportBinaryIntegrationTestSumPointData)
{
  ExportBinaryIntegrationTestExportSumPointData();
}
#  endif

// Test exporter configuration options
TEST_F(OtlpHttpMetricExporterTestPeer, ConfigTest)
{
  OtlpHttpMetricExporterOptions opts;
  opts.url = "http://localhost:45456/v1/metrics";
  std::unique_ptr<OtlpHttpMetricExporter> exporter(new OtlpHttpMetricExporter(opts));
  EXPECT_EQ(GetOptions(exporter).url, "http://localhost:45456/v1/metrics");
}

// Test exporter configuration options with use_json_name
TEST_F(OtlpHttpMetricExporterTestPeer, ConfigUseJsonNameTest)
{
  OtlpHttpMetricExporterOptions opts;
  opts.use_json_name = true;
  std::unique_ptr<OtlpHttpMetricExporter> exporter(new OtlpHttpMetricExporter(opts));
  EXPECT_EQ(GetOptions(exporter).use_json_name, true);
}

// Test exporter configuration options with json_bytes_mapping=JsonBytesMappingKind::kHex
TEST_F(OtlpHttpMetricExporterTestPeer, ConfigJsonBytesMappingTest)
{
  OtlpHttpMetricExporterOptions opts;
  opts.json_bytes_mapping = JsonBytesMappingKind::kHex;
  std::unique_ptr<OtlpHttpMetricExporter> exporter(new OtlpHttpMetricExporter(opts));
  EXPECT_EQ(GetOptions(exporter).json_bytes_mapping, JsonBytesMappingKind::kHex);
}

#  ifndef NO_GETENV
// Test exporter configuration options with use_ssl_credentials
TEST_F(OtlpHttpMetricExporterTestPeer, ConfigFromEnv)
{
  const std::string url = "http://localhost:9999/v1/metrics";
  setenv("OTEL_EXPORTER_OTLP_ENDPOINT", "http://localhost:9999", 1);
  setenv("OTEL_EXPORTER_OTLP_TIMEOUT", "20s", 1);
  setenv("OTEL_EXPORTER_OTLP_HEADERS", "k1=v1,k2=v2", 1);
  setenv("OTEL_EXPORTER_OTLP_METRICS_HEADERS", "k1=v3,k1=v4", 1);

  std::unique_ptr<OtlpHttpMetricExporter> exporter(new OtlpHttpMetricExporter());
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
  unsetenv("OTEL_EXPORTER_OTLP_METRICS_HEADERS");
}

TEST_F(OtlpHttpMetricExporterTestPeer, ConfigFromMetricsEnv)
{
  const std::string url = "http://localhost:9999/v1/metrics";
  setenv("OTEL_EXPORTER_OTLP_METRICS_ENDPOINT", url.c_str(), 1);
  setenv("OTEL_EXPORTER_OTLP_TIMEOUT", "20s", 1);
  setenv("OTEL_EXPORTER_OTLP_HEADERS", "k1=v1,k2=v2", 1);
  setenv("OTEL_EXPORTER_OTLP_METRICS_HEADERS", "k1=v3,k1=v4", 1);

  std::unique_ptr<OtlpHttpMetricExporter> exporter(new OtlpHttpMetricExporter());
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

  unsetenv("OTEL_EXPORTER_OTLP_METRICS_ENDPOINT");
  unsetenv("OTEL_EXPORTER_OTLP_TIMEOUT");
  unsetenv("OTEL_EXPORTER_OTLP_HEADERS");
  unsetenv("OTEL_EXPORTER_OTLP_METRICS_HEADERS");
}

TEST_F(OtlpHttpMetricExporterTestPeer, DefaultEndpoint)
{
  EXPECT_EQ("http://localhost:4318/v1/metrics", GetOtlpDefaultHttpMetricEndpoint());
}

#  endif

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE

#endif
