// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <utility>

#include "opentelemetry/exporters/otlp/otlp_http_metric_exporter_factory.h"
#include "opentelemetry/exporters/otlp/otlp_http_metric_exporter_options.h"
#include "opentelemetry/exporters/otlp/otlp_http_metric_exporter_runtime_options.h"
#include "opentelemetry/sdk/metrics/push_metric_exporter.h"
#include "opentelemetry/version.h"

#include "opentelemetry/test_common/ext/http/client/http_client_test_factory.h"

#include "otlp_stub_json_writer.h"

namespace http_client = opentelemetry::ext::http::client;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

TEST(OtlpHttpMetricExporterCustomJsonWriterTest, RuntimeOptionsInjectionCreatesExporter)
{
  OtlpHttpMetricExporterOptions opts;
  OtlpHttpMetricExporterRuntimeOptions runtime_opts;
  runtime_opts.json_writer_factory = std::make_shared<test::StubJsonWriterFactory>();
  auto exporter                    = OtlpHttpMetricExporterFactory::Create(opts, runtime_opts);
  ASSERT_NE(exporter, nullptr);
}

TEST(OtlpHttpMetricExporterCustomJsonWriterTest, RuntimeOptionsWithHttpClientCreatesExporter)
{
  OtlpHttpMetricExporterOptions opts;
  OtlpHttpMetricExporterRuntimeOptions runtime_opts;
  runtime_opts.json_writer_factory = std::make_shared<test::StubJsonWriterFactory>();
  auto client                      = http_client::HttpClientTestFactory::Create();
  auto exporter = OtlpHttpMetricExporterFactory::Create(opts, runtime_opts, std::move(client));
  ASSERT_NE(exporter, nullptr);
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
