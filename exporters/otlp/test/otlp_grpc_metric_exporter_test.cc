// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <stdlib.h>
#include <chrono>
#include <map>
#include <memory>
#include <string>
#include <utility>

#include "opentelemetry/exporters/otlp/otlp_grpc_client.h"
#include "opentelemetry/exporters/otlp/otlp_grpc_client_factory.h"
#include "opentelemetry/exporters/otlp/otlp_grpc_metric_exporter.h"
#include "opentelemetry/exporters/otlp/otlp_grpc_metric_exporter_options.h"
#include "opentelemetry/exporters/otlp/otlp_preferred_temporality.h"
#include "opentelemetry/sdk/metrics/instruments.h"
#include "opentelemetry/sdk/metrics/push_metric_exporter.h"
#include "opentelemetry/version.h"

// clang-format off
#  include "opentelemetry/exporters/otlp/protobuf_include_prefix.h" // IWYU pragma: keep
#  include "opentelemetry/proto/collector/metrics/v1/metrics_service.grpc.pb.h"
#  include "opentelemetry/exporters/otlp/protobuf_include_suffix.h" // IWYU pragma: keep
// clang-format on

#if defined(_MSC_VER)
#  include "opentelemetry/sdk/common/env_variables.h"
using opentelemetry::sdk::common::setenv;
using opentelemetry::sdk::common::unsetenv;
#endif

using namespace testing;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

class OtlpGrpcMetricExporterTestPeer : public ::testing::Test
{
public:
  std::unique_ptr<sdk::metrics::PushMetricExporter> GetExporter(
      const OtlpGrpcMetricExporterOptions &options)
  {
    return std::unique_ptr<sdk::metrics::PushMetricExporter>(new OtlpGrpcMetricExporter(options));
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
  opts.endpoint = "localhost:45454";
  std::unique_ptr<OtlpGrpcMetricExporter> exporter(new OtlpGrpcMetricExporter(opts));
  EXPECT_EQ(GetOptions(exporter).endpoint, "localhost:45454");
}

// Test exporter configuration options with use_ssl_credentials
TEST_F(OtlpGrpcMetricExporterTestPeer, ConfigSslCredentialsTest)
{
  std::string cacert_str = "--begin and end fake cert--";
  OtlpGrpcMetricExporterOptions opts;
  opts.use_ssl_credentials              = true;
  opts.ssl_credentials_cacert_as_string = cacert_str;
  std::unique_ptr<OtlpGrpcMetricExporter> exporter(new OtlpGrpcMetricExporter(opts));
  EXPECT_EQ(GetOptions(exporter).ssl_credentials_cacert_as_string, cacert_str);
  EXPECT_EQ(GetOptions(exporter).use_ssl_credentials, true);
}

#ifndef NO_GETENV
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

  std::unique_ptr<OtlpGrpcMetricExporter> exporter(new OtlpGrpcMetricExporter());
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

  std::unique_ptr<OtlpGrpcMetricExporter> exporter(new OtlpGrpcMetricExporter());
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

  std::unique_ptr<OtlpGrpcMetricExporter> exporter(new OtlpGrpcMetricExporter());
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

  std::unique_ptr<OtlpGrpcMetricExporter> exporter(new OtlpGrpcMetricExporter());
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

  std::unique_ptr<OtlpGrpcMetricExporter> exporter(new OtlpGrpcMetricExporter());
  EXPECT_EQ(GetOptions(exporter).use_ssl_credentials, false);
  EXPECT_EQ(GetOptions(exporter).endpoint, endpoint);

  unsetenv("OTEL_EXPORTER_OTLP_ENDPOINT");
  unsetenv("OTEL_EXPORTER_OTLP_METRICS_INSECURE");
}

TEST_F(OtlpGrpcMetricExporterTestPeer, ConfigRetryDefaultValues)
{
  std::unique_ptr<OtlpGrpcMetricExporter> exporter(new OtlpGrpcMetricExporter());
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

  std::unique_ptr<OtlpGrpcMetricExporter> exporter(new OtlpGrpcMetricExporter());
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

  std::unique_ptr<OtlpGrpcMetricExporter> exporter(new OtlpGrpcMetricExporter());
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
#endif  // NO_GETENV

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

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
