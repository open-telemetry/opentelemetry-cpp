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

#  include "opentelemetry/sdk/trace/simple_processor.h"
#  include "opentelemetry/sdk/trace/tracer_provider.h"
#  include "opentelemetry/trace/provider.h"

#  include <gtest/gtest.h>

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

class OtlpGrpcMetricExporterTestPeer : public ::testing::Test
{
public:
  std::unique_ptr<sdk::metrics::PushMetricExporter> GetExporter(
      std::unique_ptr<proto::collector::metrics::v1::MetricsService::StubInterface> &stub_interface)
  {
    return std::unique_ptr<sdk::metrics::PushMetricExporter>(
        new OtlpGrpcMetricExporter(std::move(stub_interface)));
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
#  endif

#  ifndef NO_GETENV
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
#  endif

#  ifndef NO_GETENV
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
#  endif

#  ifndef NO_GETENV
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
#  endif

#  ifndef NO_GETENV
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
#  endif

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
#endif /* OPENTELEMETRY_STL_VERSION */
