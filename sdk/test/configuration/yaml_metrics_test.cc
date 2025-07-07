// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <memory>
#include <string>

#include "opentelemetry/sdk/configuration/configuration.h"
#include "opentelemetry/sdk/configuration/console_push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/otlp_file_push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/otlp_grpc_push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/otlp_http_encoding.h"
#include "opentelemetry/sdk/configuration/otlp_http_push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/periodic_metric_reader_configuration.h"
#include "opentelemetry/sdk/configuration/prometheus_pull_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/pull_metric_reader_configuration.h"
#include "opentelemetry/sdk/configuration/yaml_configuration_parser.h"

static std::unique_ptr<opentelemetry::sdk::configuration::Configuration> DoParse(
    const std::string &yaml)
{
  static const std::string source("test");
  return opentelemetry::sdk::configuration::YamlConfigurationParser::ParseString(source, yaml);
}

TEST(YamlMetrics, no_readers)
{
  std::string yaml = R"(
file_format: xx.yy
meter_provider:
)";

  auto config = DoParse(yaml);
  ASSERT_EQ(config, nullptr);
}

TEST(YamlMetrics, empty_readers)
{
  std::string yaml = R"(
file_format: xx.yy
meter_provider:
  readers:
)";

  auto config = DoParse(yaml);
  ASSERT_EQ(config, nullptr);
}

TEST(YamlMetrics, many_readers)
{
  std::string yaml = R"(
file_format: xx.yy
meter_provider:
  readers:
    - periodic:
        exporter:
          console:
    - periodic:
        exporter:
          console:
)";

  auto config = DoParse(yaml);
  ASSERT_NE(config, nullptr);
  ASSERT_NE(config->meter_provider, nullptr);
  ASSERT_EQ(config->meter_provider->readers.size(), 2);
}

TEST(YamlMetrics, default_periodic_reader)
{
  std::string yaml = R"(
file_format: xx.yy
meter_provider:
  readers:
    - periodic:
        exporter:
          console:
)";

  auto config = DoParse(yaml);
  ASSERT_NE(config, nullptr);
  ASSERT_NE(config->meter_provider, nullptr);
  ASSERT_EQ(config->meter_provider->readers.size(), 1);
  auto *reader = config->meter_provider->readers[0].get();
  ASSERT_NE(reader, nullptr);
  auto *periodic =
      reinterpret_cast<opentelemetry::sdk::configuration::PeriodicMetricReaderConfiguration *>(
          reader);
  ASSERT_EQ(periodic->interval, 5000);
  ASSERT_EQ(periodic->timeout, 30000);
  ASSERT_NE(periodic->exporter, nullptr);
  auto *exporter = periodic->exporter.get();
  ASSERT_NE(exporter, nullptr);
}

TEST(YamlMetrics, periodic_reader)
{
  std::string yaml = R"(
file_format: xx.yy
meter_provider:
  readers:
    - periodic:
        interval: 2500
        timeout: 15000
        exporter:
          console:
)";

  auto config = DoParse(yaml);
  ASSERT_NE(config, nullptr);
  ASSERT_NE(config->meter_provider, nullptr);
  ASSERT_EQ(config->meter_provider->readers.size(), 1);
  auto *reader = config->meter_provider->readers[0].get();
  ASSERT_NE(reader, nullptr);
  auto *periodic =
      reinterpret_cast<opentelemetry::sdk::configuration::PeriodicMetricReaderConfiguration *>(
          reader);
  ASSERT_EQ(periodic->interval, 2500);
  ASSERT_EQ(periodic->timeout, 15000);
  ASSERT_NE(periodic->exporter, nullptr);
  auto *exporter = periodic->exporter.get();
  ASSERT_NE(exporter, nullptr);
}

TEST(YamlMetrics, pull_reader)
{
  std::string yaml = R"(
file_format: xx.yy
meter_provider:
  readers:
    - pull:
        exporter:
          prometheus:
)";

  auto config = DoParse(yaml);
  ASSERT_NE(config, nullptr);
  ASSERT_NE(config->meter_provider, nullptr);
  ASSERT_EQ(config->meter_provider->readers.size(), 1);
  auto *reader = config->meter_provider->readers[0].get();
  ASSERT_NE(reader, nullptr);
  auto *pull =
      reinterpret_cast<opentelemetry::sdk::configuration::PullMetricReaderConfiguration *>(reader);
  ASSERT_NE(pull->exporter, nullptr);
  auto *exporter = pull->exporter.get();
  ASSERT_NE(exporter, nullptr);
}

TEST(YamlMetrics, default_otlp_http)
{
  std::string yaml = R"(
file_format: xx.yy
meter_provider:
  readers:
    - periodic:
        exporter:
          otlp_http:
            endpoint: "somewhere"
)";

  auto config = DoParse(yaml);
  ASSERT_NE(config, nullptr);
  ASSERT_NE(config->meter_provider, nullptr);
  ASSERT_EQ(config->meter_provider->readers.size(), 1);
  auto *reader = config->meter_provider->readers[0].get();
  ASSERT_NE(reader, nullptr);
  auto *periodic =
      reinterpret_cast<opentelemetry::sdk::configuration::PeriodicMetricReaderConfiguration *>(
          reader);
  ASSERT_NE(periodic->exporter, nullptr);
  auto *exporter = periodic->exporter.get();
  ASSERT_NE(exporter, nullptr);
  auto *otlp_http = reinterpret_cast<
      opentelemetry::sdk::configuration::OtlpHttpPushMetricExporterConfiguration *>(exporter);
  ASSERT_EQ(otlp_http->endpoint, "somewhere");
  ASSERT_EQ(otlp_http->certificate_file, "");
  ASSERT_EQ(otlp_http->client_key_file, "");
  ASSERT_EQ(otlp_http->client_certificate_file, "");
  ASSERT_EQ(otlp_http->headers, nullptr);
  ASSERT_EQ(otlp_http->headers_list, "");
  ASSERT_EQ(otlp_http->compression, "");
  ASSERT_EQ(otlp_http->timeout, 10000);
  ASSERT_EQ(otlp_http->temporality_preference,
            opentelemetry::sdk::configuration::TemporalityPreference::cumulative);
  ASSERT_EQ(
      otlp_http->default_histogram_aggregation,
      opentelemetry::sdk::configuration::DefaultHistogramAggregation::explicit_bucket_histogram);
  ASSERT_EQ(otlp_http->encoding, opentelemetry::sdk::configuration::OtlpHttpEncoding::protobuf);
}

TEST(YamlMetrics, otlp_http)
{
  std::string yaml = R"(
file_format: xx.yy
meter_provider:
  readers:
    - periodic:
        exporter:
          otlp_http:
            endpoint: "somewhere"
            certificate_file: "certificate_file"
            client_key_file: "client_key_file"
            client_certificate_file: "client_certificate_file"
            headers:
              - name: foo
                value: "123"
              - name: bar
                value: "456"
            headers_list: "baz=789"
            compression: "compression"
            timeout: 5000
            temporality_preference: delta
            default_histogram_aggregation: base2_exponential_bucket_histogram
            encoding: json
)";

  auto config = DoParse(yaml);
  ASSERT_NE(config, nullptr);
  ASSERT_NE(config->meter_provider, nullptr);
  ASSERT_EQ(config->meter_provider->readers.size(), 1);
  auto *reader = config->meter_provider->readers[0].get();
  ASSERT_NE(reader, nullptr);
  auto *periodic =
      reinterpret_cast<opentelemetry::sdk::configuration::PeriodicMetricReaderConfiguration *>(
          reader);
  ASSERT_NE(periodic->exporter, nullptr);
  auto *exporter = periodic->exporter.get();
  ASSERT_NE(exporter, nullptr);
  auto *otlp_http = reinterpret_cast<
      opentelemetry::sdk::configuration::OtlpHttpPushMetricExporterConfiguration *>(exporter);
  ASSERT_EQ(otlp_http->endpoint, "somewhere");
  ASSERT_EQ(otlp_http->certificate_file, "certificate_file");
  ASSERT_EQ(otlp_http->client_key_file, "client_key_file");
  ASSERT_EQ(otlp_http->client_certificate_file, "client_certificate_file");
  ASSERT_NE(otlp_http->headers, nullptr);
  ASSERT_EQ(otlp_http->headers->kv_map.size(), 2);
  ASSERT_EQ(otlp_http->headers->kv_map["foo"], "123");
  ASSERT_EQ(otlp_http->headers->kv_map["bar"], "456");
  ASSERT_EQ(otlp_http->headers_list, "baz=789");
  ASSERT_EQ(otlp_http->compression, "compression");
  ASSERT_EQ(otlp_http->timeout, 5000);
  ASSERT_EQ(otlp_http->temporality_preference,
            opentelemetry::sdk::configuration::TemporalityPreference::delta);
  ASSERT_EQ(otlp_http->default_histogram_aggregation,
            opentelemetry::sdk::configuration::DefaultHistogramAggregation::
                base2_exponential_bucket_histogram);
  ASSERT_EQ(otlp_http->encoding, opentelemetry::sdk::configuration::OtlpHttpEncoding::json);
}

TEST(YamlMetrics, default_otlp_grpc)
{
  std::string yaml = R"(
file_format: xx.yy
meter_provider:
  readers:
    - periodic:
        exporter:
          otlp_grpc:
            endpoint: "somewhere"
)";

  auto config = DoParse(yaml);
  ASSERT_NE(config, nullptr);
  ASSERT_NE(config->meter_provider, nullptr);
  ASSERT_EQ(config->meter_provider->readers.size(), 1);
  auto *reader = config->meter_provider->readers[0].get();
  ASSERT_NE(reader, nullptr);
  auto *periodic =
      reinterpret_cast<opentelemetry::sdk::configuration::PeriodicMetricReaderConfiguration *>(
          reader);
  ASSERT_NE(periodic->exporter, nullptr);
  auto *exporter = periodic->exporter.get();
  ASSERT_NE(exporter, nullptr);
  auto *otlp_grpc = reinterpret_cast<
      opentelemetry::sdk::configuration::OtlpGrpcPushMetricExporterConfiguration *>(exporter);
  ASSERT_EQ(otlp_grpc->endpoint, "somewhere");
  ASSERT_EQ(otlp_grpc->certificate_file, "");
  ASSERT_EQ(otlp_grpc->client_key_file, "");
  ASSERT_EQ(otlp_grpc->client_certificate_file, "");
  ASSERT_EQ(otlp_grpc->headers, nullptr);
  ASSERT_EQ(otlp_grpc->headers_list, "");
  ASSERT_EQ(otlp_grpc->compression, "");
  ASSERT_EQ(otlp_grpc->timeout, 10000);
  ASSERT_EQ(otlp_grpc->temporality_preference,
            opentelemetry::sdk::configuration::TemporalityPreference::cumulative);
  ASSERT_EQ(
      otlp_grpc->default_histogram_aggregation,
      opentelemetry::sdk::configuration::DefaultHistogramAggregation::explicit_bucket_histogram);
  ASSERT_EQ(otlp_grpc->insecure, false);
}

TEST(YamlMetrics, otlp_grpc)
{
  std::string yaml = R"(
file_format: xx.yy
meter_provider:
  readers:
    - periodic:
        exporter:
          otlp_grpc:
            endpoint: "somewhere"
            certificate_file: "certificate_file"
            client_key_file: "client_key_file"
            client_certificate_file: "client_certificate_file"
            headers:
              - name: foo
                value: "123"
              - name: bar
                value: "456"
            headers_list: "baz=789"
            compression: "compression"
            timeout: 5000
            temporality_preference: delta
            default_histogram_aggregation: base2_exponential_bucket_histogram
            insecure: true
)";

  auto config = DoParse(yaml);
  ASSERT_NE(config, nullptr);
  ASSERT_NE(config->meter_provider, nullptr);
  ASSERT_EQ(config->meter_provider->readers.size(), 1);
  auto *reader = config->meter_provider->readers[0].get();
  ASSERT_NE(reader, nullptr);
  auto *periodic =
      reinterpret_cast<opentelemetry::sdk::configuration::PeriodicMetricReaderConfiguration *>(
          reader);
  ASSERT_NE(periodic->exporter, nullptr);
  auto *exporter = periodic->exporter.get();
  ASSERT_NE(exporter, nullptr);
  auto *otlp_grpc = reinterpret_cast<
      opentelemetry::sdk::configuration::OtlpGrpcPushMetricExporterConfiguration *>(exporter);
  ASSERT_EQ(otlp_grpc->endpoint, "somewhere");
  ASSERT_EQ(otlp_grpc->certificate_file, "certificate_file");
  ASSERT_EQ(otlp_grpc->client_key_file, "client_key_file");
  ASSERT_EQ(otlp_grpc->client_certificate_file, "client_certificate_file");
  ASSERT_NE(otlp_grpc->headers, nullptr);
  ASSERT_EQ(otlp_grpc->headers->kv_map.size(), 2);
  ASSERT_EQ(otlp_grpc->headers->kv_map["foo"], "123");
  ASSERT_EQ(otlp_grpc->headers->kv_map["bar"], "456");
  ASSERT_EQ(otlp_grpc->headers_list, "baz=789");
  ASSERT_EQ(otlp_grpc->compression, "compression");
  ASSERT_EQ(otlp_grpc->timeout, 5000);
  ASSERT_EQ(otlp_grpc->temporality_preference,
            opentelemetry::sdk::configuration::TemporalityPreference::delta);
  ASSERT_EQ(otlp_grpc->default_histogram_aggregation,
            opentelemetry::sdk::configuration::DefaultHistogramAggregation::
                base2_exponential_bucket_histogram);
  ASSERT_EQ(otlp_grpc->insecure, true);
}

TEST(YamlMetrics, default_otlp_file)
{
  std::string yaml = R"(
file_format: xx.yy
meter_provider:
  readers:
    - periodic:
        exporter:
          otlp_file/development:
)";

  auto config = DoParse(yaml);
  ASSERT_NE(config, nullptr);
  ASSERT_NE(config->meter_provider, nullptr);
  ASSERT_EQ(config->meter_provider->readers.size(), 1);
  auto *reader = config->meter_provider->readers[0].get();
  ASSERT_NE(reader, nullptr);
  auto *periodic =
      reinterpret_cast<opentelemetry::sdk::configuration::PeriodicMetricReaderConfiguration *>(
          reader);
  ASSERT_NE(periodic->exporter, nullptr);
  auto *exporter = periodic->exporter.get();
  ASSERT_NE(exporter, nullptr);
  auto *otlp_file = reinterpret_cast<
      opentelemetry::sdk::configuration::OtlpFilePushMetricExporterConfiguration *>(exporter);
  ASSERT_EQ(otlp_file->output_stream, "");
  ASSERT_EQ(otlp_file->temporality_preference,
            opentelemetry::sdk::configuration::TemporalityPreference::cumulative);
  ASSERT_EQ(
      otlp_file->default_histogram_aggregation,
      opentelemetry::sdk::configuration::DefaultHistogramAggregation::explicit_bucket_histogram);
}

TEST(YamlMetrics, otlp_file)
{
  std::string yaml = R"(
file_format: xx.yy
meter_provider:
  readers:
    - periodic:
        exporter:
          otlp_file/development:
            output_stream: "somewhere"
            temporality_preference: delta
            default_histogram_aggregation: base2_exponential_bucket_histogram
)";

  auto config = DoParse(yaml);
  ASSERT_NE(config, nullptr);
  ASSERT_NE(config->meter_provider, nullptr);
  ASSERT_EQ(config->meter_provider->readers.size(), 1);
  auto *reader = config->meter_provider->readers[0].get();
  ASSERT_NE(reader, nullptr);
  auto *periodic =
      reinterpret_cast<opentelemetry::sdk::configuration::PeriodicMetricReaderConfiguration *>(
          reader);
  ASSERT_NE(periodic->exporter, nullptr);
  auto *exporter = periodic->exporter.get();
  ASSERT_NE(exporter, nullptr);
  auto *otlp_file = reinterpret_cast<
      opentelemetry::sdk::configuration::OtlpFilePushMetricExporterConfiguration *>(exporter);
  ASSERT_EQ(otlp_file->output_stream, "somewhere");
  ASSERT_EQ(otlp_file->temporality_preference,
            opentelemetry::sdk::configuration::TemporalityPreference::delta);
  ASSERT_EQ(otlp_file->default_histogram_aggregation,
            opentelemetry::sdk::configuration::DefaultHistogramAggregation::
                base2_exponential_bucket_histogram);
}

TEST(YamlMetrics, default_console)
{
  std::string yaml = R"(
file_format: xx.yy
meter_provider:
  readers:
    - periodic:
        exporter:
          console:
)";

  auto config = DoParse(yaml);
  ASSERT_NE(config, nullptr);
  ASSERT_NE(config->meter_provider, nullptr);
  ASSERT_EQ(config->meter_provider->readers.size(), 1);
  auto *reader = config->meter_provider->readers[0].get();
  ASSERT_NE(reader, nullptr);
  auto *periodic =
      reinterpret_cast<opentelemetry::sdk::configuration::PeriodicMetricReaderConfiguration *>(
          reader);
  ASSERT_NE(periodic->exporter, nullptr);
  auto *exporter = periodic->exporter.get();
  ASSERT_NE(exporter, nullptr);
  auto *console =
      reinterpret_cast<opentelemetry::sdk::configuration::ConsolePushMetricExporterConfiguration *>(
          exporter);

  // FIXME-CONFIG: https://github.com/open-telemetry/opentelemetry-configuration/issues/242

#if 0
  ASSERT_EQ(console->temporality_preference,
            opentelemetry::sdk::configuration::TemporalityPreference::cumulative);
  ASSERT_EQ(
      console->default_histogram_aggregation,
      opentelemetry::sdk::configuration::DefaultHistogramAggregation::explicit_bucket_histogram);
#endif
}

TEST(YamlMetrics, console)
{
  std::string yaml = R"(
file_format: xx.yy
meter_provider:
  readers:
    - periodic:
        exporter:
          console:
            temporality_preference: delta
            default_histogram_aggregation: base2_exponential_bucket_histogram
)";

  auto config = DoParse(yaml);
  ASSERT_NE(config, nullptr);
  ASSERT_NE(config->meter_provider, nullptr);
  ASSERT_EQ(config->meter_provider->readers.size(), 1);
  auto *reader = config->meter_provider->readers[0].get();
  ASSERT_NE(reader, nullptr);
  auto *periodic =
      reinterpret_cast<opentelemetry::sdk::configuration::PeriodicMetricReaderConfiguration *>(
          reader);
  ASSERT_NE(periodic->exporter, nullptr);
  auto *exporter = periodic->exporter.get();
  ASSERT_NE(exporter, nullptr);
  auto *console =
      reinterpret_cast<opentelemetry::sdk::configuration::ConsolePushMetricExporterConfiguration *>(
          exporter);

  // FIXME-CONFIG: https://github.com/open-telemetry/opentelemetry-configuration/issues/242

#if 0
  ASSERT_EQ(console->temporality_preference,
            opentelemetry::sdk::configuration::TemporalityPreference::cumulative);
  ASSERT_EQ(
      console->default_histogram_aggregation,
      opentelemetry::sdk::configuration::DefaultHistogramAggregation::explicit_bucket_histogram);
#endif
}

TEST(YamlMetrics, default_prometheus)
{
  std::string yaml = R"(
file_format: xx.yy
meter_provider:
  readers:
    - pull:
        exporter:
          prometheus:
)";

  auto config = DoParse(yaml);
  ASSERT_NE(config, nullptr);
  ASSERT_NE(config->meter_provider, nullptr);
  ASSERT_EQ(config->meter_provider->readers.size(), 1);
  auto *reader = config->meter_provider->readers[0].get();
  ASSERT_NE(reader, nullptr);
  auto *pull =
      reinterpret_cast<opentelemetry::sdk::configuration::PullMetricReaderConfiguration *>(reader);
  ASSERT_NE(pull->exporter, nullptr);
  auto *exporter = pull->exporter.get();
  ASSERT_NE(exporter, nullptr);
  auto *prometheus = reinterpret_cast<
      opentelemetry::sdk::configuration::PrometheusPullMetricExporterConfiguration *>(exporter);
  ASSERT_EQ(prometheus->host, "localhost");
  ASSERT_EQ(prometheus->port, 9464);
  ASSERT_EQ(prometheus->without_units, false);
  ASSERT_EQ(prometheus->without_type_suffix, false);
  ASSERT_EQ(prometheus->without_scope_info, false);
}

TEST(YamlMetrics, prometheus)
{
  std::string yaml = R"(
file_format: xx.yy
meter_provider:
  readers:
    - pull:
        exporter:
          prometheus:
            host: "prometheus"
            port: 1234
            without_units: true
            without_type_suffix: true
            without_scope_info: true
)";

  auto config = DoParse(yaml);
  ASSERT_NE(config, nullptr);
  ASSERT_NE(config->meter_provider, nullptr);
  ASSERT_EQ(config->meter_provider->readers.size(), 1);
  auto *reader = config->meter_provider->readers[0].get();
  ASSERT_NE(reader, nullptr);
  auto *pull =
      reinterpret_cast<opentelemetry::sdk::configuration::PullMetricReaderConfiguration *>(reader);
  ASSERT_NE(pull->exporter, nullptr);
  auto *exporter = pull->exporter.get();
  ASSERT_NE(exporter, nullptr);
  auto *prometheus = reinterpret_cast<
      opentelemetry::sdk::configuration::PrometheusPullMetricExporterConfiguration *>(exporter);
  ASSERT_EQ(prometheus->host, "prometheus");
  ASSERT_EQ(prometheus->port, 1234);
  ASSERT_EQ(prometheus->without_units, true);
  ASSERT_EQ(prometheus->without_type_suffix, true);
  ASSERT_EQ(prometheus->without_scope_info, true);
}

// TODO: views
