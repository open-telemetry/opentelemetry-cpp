// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <chrono>
#include <cstdlib>
#include <string>

#include "opentelemetry/exporters/otlp/otlp_grpc_client_options.h"
#include "opentelemetry/exporters/otlp/otlp_grpc_exporter_options.h"
#include "opentelemetry/exporters/otlp/otlp_grpc_log_record_exporter_options.h"
#include "opentelemetry/exporters/otlp/otlp_grpc_metric_exporter_options.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

namespace
{
class ScopedEnvVar
{
public:
  ScopedEnvVar(const char *name, const char *value) : name_(name)
  {
#ifdef _MSC_VER
    _putenv_s(name_.c_str(), value);
#else
    setenv(name_.c_str(), value, 1);
#endif
  }

  ~ScopedEnvVar()
  {
#ifdef _MSC_VER
    _putenv_s(name_.c_str(), "");
#else
    unsetenv(name_.c_str());
#endif
  }

private:
  std::string name_;
};
}  // namespace

TEST(OtlpGrpcClientOptionsTest, DefaultConstructorPopulatesSpecDefaults)
{
  OtlpGrpcClientOptions options;

  EXPECT_EQ(options.endpoint, "http://localhost:4317");
  EXPECT_FALSE(options.use_ssl_credentials);
  EXPECT_EQ(options.compression, "none");
  EXPECT_EQ(options.retry_policy_max_attempts, 5U);
  EXPECT_EQ(options.retry_policy_backoff_multiplier, 1.5f);
}

TEST(OtlpGrpcClientOptionsTest, VoidPointerConstructorSkipsDefaults)
{
  OtlpGrpcClientOptions options(static_cast<void *>(nullptr));

  EXPECT_TRUE(options.endpoint.empty());
  EXPECT_TRUE(options.use_ssl_credentials);
}

TEST(OtlpGrpcClientOptionsTest, EndpointHonorsGenericEnvVar)
{
  ScopedEnvVar env("OTEL_EXPORTER_OTLP_ENDPOINT", "https://collector.example.com:4317");

  OtlpGrpcClientOptions options;
  EXPECT_EQ(options.endpoint, "https://collector.example.com:4317");
  EXPECT_TRUE(options.use_ssl_credentials);
}

TEST(OtlpGrpcClientOptionsTest, DerivedSignalOptionsCopyFixedFieldsFromSharedClient)
{
  OtlpGrpcClientOptions client_options;
  client_options.endpoint                  = "https://shared-collector.example.com:4317";
  client_options.compression               = "gzip";
  client_options.max_threads               = 7;
  client_options.retry_policy_max_attempts = 3;

  OtlpGrpcExporterOptions trace_options(client_options);
  EXPECT_EQ(trace_options.endpoint, client_options.endpoint);
  EXPECT_EQ(trace_options.compression, client_options.compression);
  EXPECT_EQ(trace_options.max_threads, client_options.max_threads);
  EXPECT_EQ(trace_options.retry_policy_max_attempts, client_options.retry_policy_max_attempts);

  OtlpGrpcMetricExporterOptions metric_options(client_options);
  EXPECT_EQ(metric_options.endpoint, client_options.endpoint);
  EXPECT_EQ(metric_options.compression, client_options.compression);

  OtlpGrpcLogRecordExporterOptions log_options(client_options);
  EXPECT_EQ(log_options.endpoint, client_options.endpoint);
  EXPECT_EQ(log_options.compression, client_options.compression);
}

TEST(OtlpGrpcClientOptionsTest, SignalSpecificTimeoutOverridesSharedClientTimeout)
{
  ScopedEnvVar generic_timeout("OTEL_EXPORTER_OTLP_TIMEOUT", "20s");
  ScopedEnvVar traces_timeout("OTEL_EXPORTER_OTLP_TRACES_TIMEOUT", "99s");

  OtlpGrpcClientOptions client_options;
  EXPECT_EQ(client_options.timeout, std::chrono::seconds(20));

  OtlpGrpcExporterOptions trace_options(client_options);
  EXPECT_EQ(trace_options.timeout, std::chrono::seconds(99));
}

TEST(OtlpGrpcClientOptionsTest, SignalWithoutSpecificOverrideFallsBackToSharedClientTimeout)
{
  ScopedEnvVar generic_timeout("OTEL_EXPORTER_OTLP_TIMEOUT", "20s");

  OtlpGrpcClientOptions client_options;
  EXPECT_EQ(client_options.timeout, std::chrono::seconds(20));

  OtlpGrpcMetricExporterOptions metric_options(client_options);
  EXPECT_EQ(metric_options.timeout, std::chrono::seconds(20));
}

TEST(OtlpGrpcClientOptionsTest, DerivedVoidPointerConstructorsSkipDefaults)
{
  ScopedEnvVar env("OTEL_EXPORTER_OTLP_ENDPOINT", "https://collector.example.com:4317");

  OtlpGrpcExporterOptions trace_options(static_cast<void *>(nullptr));
  EXPECT_TRUE(trace_options.endpoint.empty());

  OtlpGrpcMetricExporterOptions metric_options(static_cast<void *>(nullptr));
  EXPECT_TRUE(metric_options.endpoint.empty());

  OtlpGrpcLogRecordExporterOptions log_options(static_cast<void *>(nullptr));
  EXPECT_TRUE(log_options.endpoint.empty());
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
