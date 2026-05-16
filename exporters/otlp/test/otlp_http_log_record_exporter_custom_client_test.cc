// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef OPENTELEMETRY_STL_VERSION

#  include "opentelemetry/exporters/otlp/otlp_http_log_record_exporter_factory.h"
#  include "opentelemetry/exporters/otlp/otlp_http_log_record_exporter_options.h"
#  include "opentelemetry/exporters/otlp/otlp_http_log_record_exporter_runtime_options.h"
#  include "opentelemetry/ext/http/client/http_client_factory.h"
#  include "opentelemetry/test_common/ext/http/client/http_client_test_factory.h"

#  include <gtest/gtest.h>

namespace http_client = opentelemetry::ext::http::client;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

TEST(OtlpHttpLogRecordExporterCustomClientTest, FactoryInjectionCreatesExporter)
{
  OtlpHttpLogRecordExporterOptions opts;
  auto factory  = http_client::GetDefaultHttpClientFactory();
  auto exporter = OtlpHttpLogRecordExporterFactory::Create(opts, std::move(factory));
  ASSERT_NE(exporter, nullptr);
}

TEST(OtlpHttpLogRecordExporterCustomClientTest, HttpClientInjectionCreatesExporter)
{
  OtlpHttpLogRecordExporterOptions opts;
  auto client   = http_client::HttpClientTestFactory::Create();
  auto exporter = OtlpHttpLogRecordExporterFactory::Create(opts, std::move(client));
  ASSERT_NE(exporter, nullptr);
}

TEST(OtlpHttpLogRecordExporterCustomClientTest, RuntimeOptionsWithFactoryCreatesExporter)
{
  OtlpHttpLogRecordExporterOptions opts;
  OtlpHttpLogRecordExporterRuntimeOptions runtime_opts;
  auto factory  = http_client::GetDefaultHttpClientFactory();
  auto exporter = OtlpHttpLogRecordExporterFactory::Create(opts, runtime_opts, std::move(factory));
  ASSERT_NE(exporter, nullptr);
}

TEST(OtlpHttpLogRecordExporterCustomClientTest, RuntimeOptionsWithHttpClientCreatesExporter)
{
  OtlpHttpLogRecordExporterOptions opts;
  OtlpHttpLogRecordExporterRuntimeOptions runtime_opts;
  auto client   = http_client::HttpClientTestFactory::Create();
  auto exporter = OtlpHttpLogRecordExporterFactory::Create(opts, runtime_opts, std::move(client));
  ASSERT_NE(exporter, nullptr);
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE

#endif  // OPENTELEMETRY_STL_VERSION
