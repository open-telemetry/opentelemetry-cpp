// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef OPENTELEMETRY_STL_VERSION

#  include "opentelemetry/exporters/otlp/otlp_http_metric_exporter_factory.h"
#  include "opentelemetry/exporters/otlp/otlp_http_metric_exporter_options.h"
#  include "opentelemetry/exporters/otlp/otlp_http_metric_exporter_runtime_options.h"
#  include "opentelemetry/ext/http/client/http_client_factory.h"
#  include "opentelemetry/test_common/ext/http/client/http_client_test_factory.h"
#  include "opentelemetry/test_common/ext/http/client/nosend/http_client_factory_nosend.h"

#  include <gtest/gtest.h>

namespace http_client = opentelemetry::ext::http::client;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

using NosendHttpClientFactory =
    opentelemetry::test_common::ext::http::client::nosend::HttpClientFactoryNosend;

TEST(OtlpHttpMetricExporterCustomClientTest, FactoryInjectionCreatesExporter)
{
  OtlpHttpMetricExporterOptions opts;
  auto factory  = std::make_shared<NosendHttpClientFactory>();
  auto exporter = OtlpHttpMetricExporterFactory::Create(opts, std::move(factory));
  ASSERT_NE(exporter, nullptr);
}

TEST(OtlpHttpMetricExporterCustomClientTest, HttpClientInjectionCreatesExporter)
{
  OtlpHttpMetricExporterOptions opts;
  auto client   = http_client::HttpClientTestFactory::Create();
  auto exporter = OtlpHttpMetricExporterFactory::Create(opts, std::move(client));
  ASSERT_NE(exporter, nullptr);
}

TEST(OtlpHttpMetricExporterCustomClientTest, RuntimeOptionsWithFactoryCreatesExporter)
{
  OtlpHttpMetricExporterOptions opts;
  OtlpHttpMetricExporterRuntimeOptions runtime_opts;
  auto factory  = std::make_shared<NosendHttpClientFactory>();
  auto exporter = OtlpHttpMetricExporterFactory::Create(opts, runtime_opts, std::move(factory));
  ASSERT_NE(exporter, nullptr);
}

TEST(OtlpHttpMetricExporterCustomClientTest, RuntimeOptionsWithHttpClientCreatesExporter)
{
  OtlpHttpMetricExporterOptions opts;
  OtlpHttpMetricExporterRuntimeOptions runtime_opts;
  auto client   = http_client::HttpClientTestFactory::Create();
  auto exporter = OtlpHttpMetricExporterFactory::Create(opts, runtime_opts, std::move(client));
  ASSERT_NE(exporter, nullptr);
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE

#endif  // OPENTELEMETRY_STL_VERSION
