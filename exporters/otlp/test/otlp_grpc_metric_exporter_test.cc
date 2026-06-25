// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef OPENTELEMETRY_STL_VERSION
// Unfortunately as of 04/27/2021 the fix is NOT in the vcpkg snapshot of Google Test.
// Remove above `#ifdef` once the GMock fix for C++20 is in the mainline.
//
// Please refer to this GitHub issue for additional details:
// https://github.com/google/googletest/issues/2914
// https://github.com/google/googletest/commit/61f010d703b32de9bfb20ab90ece38ab2f25977f
//
// If we compile using Visual Studio 2019 with `c++latest` (C++20) without the GMock fix,
// then the compilation here fails in `gmock-actions.h` from:
//   .\tools\vcpkg\installed\x64-windows\include\gmock\gmock-actions.h(819):
//   error C2653: 'result_of': is not a class or namespace name
//
// That is because `std::result_of` has been removed in C++20.

#  include "opentelemetry/exporters/otlp/otlp_grpc_metric_exporter.h"

#  include "opentelemetry/exporters/otlp/protobuf_include_prefix.h"

// Problematic code that pulls in Gmock and breaks with vs2019/c++latest :
#  include "opentelemetry/proto/collector/metrics/v1/metrics_service_mock.grpc.pb.h"

#  include "opentelemetry/exporters/otlp/protobuf_include_suffix.h"

#  include "opentelemetry/exporters/otlp/otlp_grpc_client.h"
#  include "opentelemetry/exporters/otlp/otlp_grpc_client_factory.h"

#  include "opentelemetry/common/timestamp.h"
#  include "opentelemetry/nostd/shared_ptr.h"
#  include "opentelemetry/sdk/common/global_log_handler.h"
#  include "opentelemetry/sdk/instrumentationscope/instrumentation_scope.h"
#  include "opentelemetry/sdk/metrics/data/metric_data.h"
#  include "opentelemetry/sdk/metrics/data/point_data.h"
#  include "opentelemetry/sdk/metrics/export/metric_producer.h"
#  include "opentelemetry/sdk/metrics/instruments.h"
#  include "opentelemetry/sdk/resource/resource.h"
#  include "opentelemetry/sdk/trace/simple_processor.h"
#  include "opentelemetry/sdk/trace/tracer_provider.h"
#  include "opentelemetry/test_common/sdk/common/scoped_test_log_handler.h"
#  include "opentelemetry/trace/provider.h"

#  include <grpcpp/grpcpp.h>
#  include <gtest/gtest.h>
#  include <algorithm>
#  include <functional>
#  include <utility>
#  include <vector>

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

namespace
{
class OtlpMockMetricsServiceStub : public proto::collector::metrics::v1::MockMetricsServiceStub
{
public:
// Some old toolchains can only use gRPC 1.33 and it's experimental.
#  if defined(GRPC_CPP_VERSION_MAJOR) && \
      (GRPC_CPP_VERSION_MAJOR * 1000 + GRPC_CPP_VERSION_MINOR) >= 1039
  using async_interface_base =
      proto::collector::metrics::v1::MetricsService::StubInterface::async_interface;
#  else
  using async_interface_base =
      proto::collector::metrics::v1::MetricsService::StubInterface::experimental_async_interface;
#  endif

  OtlpMockMetricsServiceStub() : async_interface_(this) {}

  class async_interface : public async_interface_base
  {
  public:
    async_interface(OtlpMockMetricsServiceStub *owner) : stub_(owner) {}

    void Export(
        ::grpc::ClientContext *context,
        const ::opentelemetry::proto::collector::metrics::v1::ExportMetricsServiceRequest *request,
        ::opentelemetry::proto::collector::metrics::v1::ExportMetricsServiceResponse *response,
        std::function<void(::grpc::Status)> callback) override
    {
      stub_->last_async_status_ = stub_->Export(context, *request, response);
      callback(stub_->last_async_status_);
    }

// Some old toolchains can only use gRPC 1.33 and it's experimental.
#  if defined(GRPC_CPP_VERSION_MAJOR) &&                                      \
          (GRPC_CPP_VERSION_MAJOR * 1000 + GRPC_CPP_VERSION_MINOR) >= 1039 || \
      defined(GRPC_CALLBACK_API_NONEXPERIMENTAL)
    void Export(
        ::grpc::ClientContext * /*context*/,
        const ::opentelemetry::proto::collector::metrics::v1::ExportMetricsServiceRequest
            * /*request*/,
        ::opentelemetry::proto::collector::metrics::v1::ExportMetricsServiceResponse * /*response*/,
        ::grpc::ClientUnaryReactor * /*reactor*/) override
    {}
#  else
    void Export(
        ::grpc::ClientContext * /*context*/,
        const ::opentelemetry::proto::collector::metrics::v1::ExportMetricsServiceRequest
            * /*request*/,
        ::opentelemetry::proto::collector::metrics::v1::ExportMetricsServiceResponse * /*response*/,
        ::grpc::experimental::ClientUnaryReactor * /*reactor*/)
    {}
#  endif

  private:
    OtlpMockMetricsServiceStub *stub_;
  };

  async_interface_base *async() override { return &async_interface_; }

  ::grpc::Status GetLastAsyncStatus() const noexcept { return last_async_status_; }

private:
  ::grpc::Status last_async_status_;
  async_interface async_interface_;
};

using opentelemetry::test_common::ScopedTestLogHandler;
}  // namespace

class OtlpGrpcMetricExporterTestPeer : public ::testing::Test
{
public:
  std::unique_ptr<sdk::metrics::PushMetricExporter> GetExporter(
      const OtlpGrpcMetricExporterOptions &options)
  {
    return std::make_unique<OtlpGrpcMetricExporter>(options);
  }

  std::unique_ptr<sdk::metrics::PushMetricExporter> GetExporter(
      std::unique_ptr<proto::collector::metrics::v1::MetricsService::StubInterface> stub_interface)
  {
    return std::unique_ptr<sdk::metrics::PushMetricExporter>(
        new OtlpGrpcMetricExporter(std::move(stub_interface)));
  }

  std::unique_ptr<sdk::metrics::PushMetricExporter> GetExporter(
      std::unique_ptr<proto::collector::metrics::v1::MetricsService::StubInterface> stub_interface,
      const std::shared_ptr<OtlpGrpcClient> &client)
  {
    return std::unique_ptr<sdk::metrics::PushMetricExporter>(
        new OtlpGrpcMetricExporter(std::move(stub_interface), client));
  }

  std::unique_ptr<sdk::metrics::PushMetricExporter> GetExporter(
      const OtlpGrpcMetricExporterOptions &options,
      const std::shared_ptr<OtlpGrpcClient> &client)
  {
    return std::unique_ptr<sdk::metrics::PushMetricExporter>(
        new OtlpGrpcMetricExporter(options, client));
  }

  // Get the options associated with the given exporter.
  const OtlpGrpcMetricExporterOptions &GetOptions(std::unique_ptr<OtlpGrpcMetricExporter> &exporter)
  {
    return exporter->options_;
  }
};

// Test exporter configuration options
TEST_F(OtlpGrpcMetricExporterTestPeer, ConfigTest)
{
  OtlpGrpcMetricExporterOptions opts;
  opts.endpoint                                    = "localhost:45454";
  std::unique_ptr<OtlpGrpcMetricExporter> exporter = std::make_unique<OtlpGrpcMetricExporter>(opts);
  EXPECT_EQ(GetOptions(exporter).endpoint, "localhost:45454");
}

// Test exporter configuration options with use_ssl_credentials
TEST_F(OtlpGrpcMetricExporterTestPeer, ConfigSslCredentialsTest)
{
  std::string cacert_str = "--begin and end fake cert--";
  OtlpGrpcMetricExporterOptions opts;
  opts.use_ssl_credentials                         = true;
  opts.ssl_credentials_cacert_as_string            = cacert_str;
  std::unique_ptr<OtlpGrpcMetricExporter> exporter = std::make_unique<OtlpGrpcMetricExporter>(opts);
  ;
  EXPECT_EQ(GetOptions(exporter).ssl_credentials_cacert_as_string, cacert_str);
  EXPECT_EQ(GetOptions(exporter).use_ssl_credentials, true);
}

#  ifndef NO_GETENV
// Test exporter configuration options with use_ssl_credentials
TEST_F(OtlpGrpcMetricExporterTestPeer, ConfigFromEnv)
{
  const std::string cacert_str = "--begin and end fake cert--";
  setenv("OTEL_EXPORTER_OTLP_CERTIFICATE_STRING", cacert_str.c_str(), 1);
  setenv("OTEL_EXPORTER_OTLP_SSL_ENABLE", "True", 1);
  const std::string endpoint = "https://localhost:9999";
  setenv("OTEL_EXPORTER_OTLP_ENDPOINT", endpoint.c_str(), 1);
  setenv("OTEL_EXPORTER_OTLP_TIMEOUT", "20050ms", 1);
  setenv("OTEL_EXPORTER_OTLP_HEADERS", "k1=v1,k2=v2", 1);
  setenv("OTEL_EXPORTER_OTLP_METRICS_HEADERS", "k1=v3,k1=v4", 1);

  std::unique_ptr<OtlpGrpcMetricExporter> exporter = std::make_unique<OtlpGrpcMetricExporter>();
  ;
  EXPECT_EQ(GetOptions(exporter).ssl_credentials_cacert_as_string, cacert_str);
  EXPECT_EQ(GetOptions(exporter).use_ssl_credentials, true);
  EXPECT_EQ(GetOptions(exporter).endpoint, endpoint);
  EXPECT_EQ(GetOptions(exporter).timeout.count(),
            std::chrono::duration_cast<std::chrono::system_clock::duration>(
                std::chrono::milliseconds{20050})
                .count());
  EXPECT_EQ(GetOptions(exporter).metadata.size(), 3);
  {
    // Test k2
    auto range = GetOptions(exporter).metadata.equal_range("k2");
    EXPECT_TRUE(range.first != range.second);
    EXPECT_EQ(range.first->second, std::string("v2"));
    ++range.first;
    EXPECT_TRUE(range.first == range.second);
  }
  {
    // Test k1
    auto range = GetOptions(exporter).metadata.equal_range("k1");
    EXPECT_TRUE(range.first != range.second);
    EXPECT_EQ(range.first->second, std::string("v3"));
    ++range.first;
    EXPECT_EQ(range.first->second, std::string("v4"));
    ++range.first;
    EXPECT_TRUE(range.first == range.second);
  }

  unsetenv("OTEL_EXPORTER_OTLP_ENDPOINT");
  unsetenv("OTEL_EXPORTER_OTLP_CERTIFICATE_STRING");
  unsetenv("OTEL_EXPORTER_OTLP_SSL_ENABLE");
  unsetenv("OTEL_EXPORTER_OTLP_TIMEOUT");
  unsetenv("OTEL_EXPORTER_OTLP_HEADERS");
  unsetenv("OTEL_EXPORTER_OTLP_METRICS_HEADERS");
}

// Test exporter configuration options with use_ssl_credentials
TEST_F(OtlpGrpcMetricExporterTestPeer, ConfigHttpsSecureFromEnv)
{
  // https takes precedence over insecure
  const std::string endpoint = "https://localhost:9999";
  setenv("OTEL_EXPORTER_OTLP_ENDPOINT", endpoint.c_str(), 1);
  setenv("OTEL_EXPORTER_OTLP_METRICS_INSECURE", "true", 1);

  std::unique_ptr<OtlpGrpcMetricExporter> exporter = std::make_unique<OtlpGrpcMetricExporter>();
  ;
  EXPECT_EQ(GetOptions(exporter).use_ssl_credentials, true);
  EXPECT_EQ(GetOptions(exporter).endpoint, endpoint);

  unsetenv("OTEL_EXPORTER_OTLP_ENDPOINT");
  unsetenv("OTEL_EXPORTER_OTLP_METRICS_INSECURE");
}

// Test exporter configuration options with use_ssl_credentials
TEST_F(OtlpGrpcMetricExporterTestPeer, ConfigHttpInsecureFromEnv)
{
  // http takes precedence over secure
  const std::string endpoint = "http://localhost:9999";
  setenv("OTEL_EXPORTER_OTLP_ENDPOINT", endpoint.c_str(), 1);
  setenv("OTEL_EXPORTER_OTLP_METRICS_INSECURE", "false", 1);

  std::unique_ptr<OtlpGrpcMetricExporter> exporter = std::make_unique<OtlpGrpcMetricExporter>();
  ;
  EXPECT_EQ(GetOptions(exporter).use_ssl_credentials, false);
  EXPECT_EQ(GetOptions(exporter).endpoint, endpoint);

  unsetenv("OTEL_EXPORTER_OTLP_ENDPOINT");
  unsetenv("OTEL_EXPORTER_OTLP_METRICS_INSECURE");
}

// Test exporter configuration options with use_ssl_credentials
TEST_F(OtlpGrpcMetricExporterTestPeer, ConfigUnknownSecureFromEnv)
{
  const std::string endpoint = "localhost:9999";
  setenv("OTEL_EXPORTER_OTLP_ENDPOINT", endpoint.c_str(), 1);
  setenv("OTEL_EXPORTER_OTLP_METRICS_INSECURE", "false", 1);

  std::unique_ptr<OtlpGrpcMetricExporter> exporter = std::make_unique<OtlpGrpcMetricExporter>();
  ;
  EXPECT_EQ(GetOptions(exporter).use_ssl_credentials, true);
  EXPECT_EQ(GetOptions(exporter).endpoint, endpoint);

  unsetenv("OTEL_EXPORTER_OTLP_ENDPOINT");
  unsetenv("OTEL_EXPORTER_OTLP_METRICS_INSECURE");
}

// Test exporter configuration options with use_ssl_credentials
TEST_F(OtlpGrpcMetricExporterTestPeer, ConfigUnknownInsecureFromEnv)
{
  const std::string endpoint = "localhost:9999";
  setenv("OTEL_EXPORTER_OTLP_ENDPOINT", endpoint.c_str(), 1);
  setenv("OTEL_EXPORTER_OTLP_METRICS_INSECURE", "true", 1);

  std::unique_ptr<OtlpGrpcMetricExporter> exporter = std::make_unique<OtlpGrpcMetricExporter>();
  ;
  EXPECT_EQ(GetOptions(exporter).use_ssl_credentials, false);
  EXPECT_EQ(GetOptions(exporter).endpoint, endpoint);

  unsetenv("OTEL_EXPORTER_OTLP_ENDPOINT");
  unsetenv("OTEL_EXPORTER_OTLP_METRICS_INSECURE");
}

TEST_F(OtlpGrpcMetricExporterTestPeer, ConfigRetryDefaultValues)
{
  std::unique_ptr<OtlpGrpcMetricExporter> exporter = std::make_unique<OtlpGrpcMetricExporter>();
  ;
  const auto options = GetOptions(exporter);
  ASSERT_EQ(options.retry_policy_max_attempts, 5);
  ASSERT_FLOAT_EQ(options.retry_policy_initial_backoff.count(), 1.0f);
  ASSERT_FLOAT_EQ(options.retry_policy_max_backoff.count(), 5.0f);
  ASSERT_FLOAT_EQ(options.retry_policy_backoff_multiplier, 1.5f);
}

TEST_F(OtlpGrpcMetricExporterTestPeer, ConfigRetryValuesFromEnv)
{
  setenv("OTEL_CPP_EXPORTER_OTLP_METRICS_RETRY_MAX_ATTEMPTS", "123", 1);
  setenv("OTEL_CPP_EXPORTER_OTLP_METRICS_RETRY_INITIAL_BACKOFF", "4.5", 1);
  setenv("OTEL_CPP_EXPORTER_OTLP_METRICS_RETRY_MAX_BACKOFF", "6.7", 1);
  setenv("OTEL_CPP_EXPORTER_OTLP_METRICS_RETRY_BACKOFF_MULTIPLIER", "8.9", 1);

  std::unique_ptr<OtlpGrpcMetricExporter> exporter = std::make_unique<OtlpGrpcMetricExporter>();
  ;
  const auto options = GetOptions(exporter);
  ASSERT_EQ(options.retry_policy_max_attempts, 123);
  ASSERT_FLOAT_EQ(options.retry_policy_initial_backoff.count(), 4.5f);
  ASSERT_FLOAT_EQ(options.retry_policy_max_backoff.count(), 6.7f);
  ASSERT_FLOAT_EQ(options.retry_policy_backoff_multiplier, 8.9f);

  unsetenv("OTEL_CPP_EXPORTER_OTLP_METRICS_RETRY_MAX_ATTEMPTS");
  unsetenv("OTEL_CPP_EXPORTER_OTLP_METRICS_RETRY_INITIAL_BACKOFF");
  unsetenv("OTEL_CPP_EXPORTER_OTLP_METRICS_RETRY_MAX_BACKOFF");
  unsetenv("OTEL_CPP_EXPORTER_OTLP_METRICS_RETRY_BACKOFF_MULTIPLIER");
}

TEST_F(OtlpGrpcMetricExporterTestPeer, ConfigRetryGenericValuesFromEnv)
{
  setenv("OTEL_CPP_EXPORTER_OTLP_RETRY_MAX_ATTEMPTS", "321", 1);
  setenv("OTEL_CPP_EXPORTER_OTLP_RETRY_INITIAL_BACKOFF", "5.4", 1);
  setenv("OTEL_CPP_EXPORTER_OTLP_RETRY_MAX_BACKOFF", "7.6", 1);
  setenv("OTEL_CPP_EXPORTER_OTLP_RETRY_BACKOFF_MULTIPLIER", "9.8", 1);

  std::unique_ptr<OtlpGrpcMetricExporter> exporter = std::make_unique<OtlpGrpcMetricExporter>();
  ;
  const auto options = GetOptions(exporter);
  ASSERT_EQ(options.retry_policy_max_attempts, 321);
  ASSERT_FLOAT_EQ(options.retry_policy_initial_backoff.count(), 5.4f);
  ASSERT_FLOAT_EQ(options.retry_policy_max_backoff.count(), 7.6f);
  ASSERT_FLOAT_EQ(options.retry_policy_backoff_multiplier, 9.8f);

  unsetenv("OTEL_CPP_EXPORTER_OTLP_RETRY_MAX_ATTEMPTS");
  unsetenv("OTEL_CPP_EXPORTER_OTLP_RETRY_INITIAL_BACKOFF");
  unsetenv("OTEL_CPP_EXPORTER_OTLP_RETRY_MAX_BACKOFF");
  unsetenv("OTEL_CPP_EXPORTER_OTLP_RETRY_BACKOFF_MULTIPLIER");
}
#  endif  // NO_GETENV

TEST_F(OtlpGrpcMetricExporterTestPeer, CheckGetAggregationTemporality)
{
  auto options                    = OtlpGrpcMetricExporterOptions();
  options.aggregation_temporality = PreferredAggregationTemporality::kCumulative;

  auto client = OtlpGrpcClientFactory::Create(options);

  auto exporter0 = GetExporter(options);
  auto exporter1 = GetExporter(client->MakeMetricsServiceStub());
  auto exporter2 = GetExporter(options, client);
  auto exporter3 = GetExporter(client->MakeMetricsServiceStub(), client);

  EXPECT_EQ(
      opentelemetry::sdk::metrics::AggregationTemporality::kCumulative,
      exporter0->GetAggregationTemporality(opentelemetry::sdk::metrics::InstrumentType::kCounter));

  EXPECT_EQ(
      opentelemetry::sdk::metrics::AggregationTemporality::kCumulative,
      exporter1->GetAggregationTemporality(opentelemetry::sdk::metrics::InstrumentType::kCounter));

  EXPECT_EQ(
      opentelemetry::sdk::metrics::AggregationTemporality::kCumulative,
      exporter2->GetAggregationTemporality(opentelemetry::sdk::metrics::InstrumentType::kCounter));

  EXPECT_EQ(
      opentelemetry::sdk::metrics::AggregationTemporality::kCumulative,
      exporter3->GetAggregationTemporality(opentelemetry::sdk::metrics::InstrumentType::kCounter));
}

// Exporter logs the rejection on partial_success.
TEST_F(OtlpGrpcMetricExporterTestPeer, ExportPartialSuccess)
{
  ScopedTestLogHandler log{sdk::common::internal_log::LogLevel::Error};

  auto mock_stub = new OtlpMockMetricsServiceStub();
  std::unique_ptr<proto::collector::metrics::v1::MetricsService::StubInterface> stub_interface(
      mock_stub);
  auto exporter = GetExporter(std::move(stub_interface));

  opentelemetry::sdk::metrics::SumPointData sum_point_data{};
  sum_point_data.value_ = 10.0;
  opentelemetry::sdk::metrics::ResourceMetrics data;
  auto resource = opentelemetry::sdk::resource::Resource::Create(
      opentelemetry::sdk::resource::ResourceAttributes{});
  data.resource_ = &resource;
  auto scope     = opentelemetry::sdk::instrumentationscope::InstrumentationScope::Create(
      "library_name", "1.5.0");
  opentelemetry::sdk::metrics::MetricData metric_data{
      opentelemetry::sdk::metrics::InstrumentDescriptor{
          "metrics_name", "description", "unit",
          opentelemetry::sdk::metrics::InstrumentType::kCounter,
          opentelemetry::sdk::metrics::InstrumentValueType::kDouble},
      opentelemetry::sdk::metrics::AggregationTemporality::kDelta,
      opentelemetry::common::SystemTimestamp{}, opentelemetry::common::SystemTimestamp{},
      std::vector<opentelemetry::sdk::metrics::PointDataAttributes>{
          {opentelemetry::sdk::metrics::PointAttributes{}, sum_point_data}}};
  data.scope_metric_data_ = std::vector<opentelemetry::sdk::metrics::ScopeMetrics>{
      {scope.get(), std::vector<opentelemetry::sdk::metrics::MetricData>{metric_data}}};

  EXPECT_CALL(*mock_stub, Export(_, _, _))
      .Times(Exactly(1))
      .WillOnce(Invoke([](grpc::ClientContext *,
                          const proto::collector::metrics::v1::ExportMetricsServiceRequest &,
                          proto::collector::metrics::v1::ExportMetricsServiceResponse *response)
                           -> grpc::Status {
        response->mutable_partial_success()->set_rejected_data_points(21);
        response->mutable_partial_success()->set_error_message("too many data points!!");
        return grpc::Status::OK;
      }));

  EXPECT_EQ(opentelemetry::sdk::common::ExportResult::kSuccess, exporter->Export(data));
  exporter->ForceFlush();

  auto entries  = log.Drain();
  auto contains = [&](const std::string &needle) {
    return std::any_of(entries.begin(), entries.end(), [&](const ScopedTestLogHandler::Entry &e) {
      return e.msg.find(needle) != std::string::npos;
    });
  };
  EXPECT_TRUE(contains("partial success"));
  EXPECT_TRUE(contains("21 data point(s) rejected"));
  EXPECT_TRUE(contains("too many data points!!"));
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
#endif /* OPENTELEMETRY_STL_VERSION */
