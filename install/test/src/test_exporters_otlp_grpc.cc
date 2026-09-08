// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>

#include <opentelemetry/exporters/otlp/otlp_grpc_client_options.h>
#include <opentelemetry/exporters/otlp/otlp_grpc_exporter_options.h>
#include <opentelemetry/exporters/otlp/otlp_grpc_log_record_exporter_options.h>
#include <opentelemetry/exporters/otlp/otlp_grpc_metric_exporter_options.h>

#include <opentelemetry/exporters/otlp/otlp_grpc_client_factory.h>
#include <opentelemetry/exporters/otlp/otlp_grpc_exporter_factory.h>
#include <opentelemetry/exporters/otlp/otlp_grpc_log_record_exporter_factory.h>
#include <opentelemetry/exporters/otlp/otlp_grpc_metric_exporter_factory.h>

#include <opentelemetry/exporters/otlp/otlp_grpc_log_record_builder.h>
#include <opentelemetry/exporters/otlp/otlp_grpc_push_metric_builder.h>
#include <opentelemetry/exporters/otlp/otlp_grpc_span_builder.h>

TEST(ExportersOtlpGrpcInstall, OtlpGrpcClient)
{
  auto options = opentelemetry::exporter::otlp::OtlpGrpcExporterOptions();
  auto client  = opentelemetry::exporter::otlp::OtlpGrpcClientFactory::Create(options);
  ASSERT_TRUE(client != nullptr);
}

TEST(ExportersOtlpGrpcInstall, OtlpGrpcExporter)
{
  auto options  = opentelemetry::exporter::otlp::OtlpGrpcExporterOptions();
  auto exporter = opentelemetry::exporter::otlp::OtlpGrpcExporterFactory::Create(options);
  ASSERT_TRUE(exporter != nullptr);
}

TEST(ExportersOtlpGrpcInstall, OtlpGrpcLogRecordExporter)
{
  auto options  = opentelemetry::exporter::otlp::OtlpGrpcLogRecordExporterOptions();
  auto exporter = opentelemetry::exporter::otlp::OtlpGrpcLogRecordExporterFactory::Create(options);
  ASSERT_TRUE(exporter != nullptr);
}

TEST(ExportersOtlpGrpcInstall, OtlpGrpcMetricExporter)
{
  auto options  = opentelemetry::exporter::otlp::OtlpGrpcMetricExporterOptions();
  auto exporter = opentelemetry::exporter::otlp::OtlpGrpcMetricExporterFactory::Create(options);
  ASSERT_TRUE(exporter != nullptr);
}

TEST(ExportersOtlpGrpcBuilderInstall, OtlpGrpcSpanBuilder)
{
  auto builder = std::make_unique<opentelemetry::exporter::otlp::OtlpGrpcSpanBuilder>();
  ASSERT_TRUE(builder != nullptr);

  opentelemetry::sdk::configuration::OtlpGrpcSpanExporterConfiguration model;
  model.tls = std::make_unique<opentelemetry::sdk::configuration::GrpcTlsConfiguration>();

  model.endpoint      = "http://localhost:4317";
  model.tls->insecure = false;
  model.timeout       = 12;
  model.compression   = "none";

  auto exporter = builder->Build(&model);
  ASSERT_TRUE(exporter != nullptr);
}

TEST(ExportersOtlpGrpcBuilderInstall, OtlpGrpcPushMetricBuilder)
{
  auto builder = std::make_unique<opentelemetry::exporter::otlp::OtlpGrpcPushMetricBuilder>();
  ASSERT_TRUE(builder != nullptr);

  opentelemetry::sdk::configuration::OtlpGrpcPushMetricExporterConfiguration model;
  model.tls = std::make_unique<opentelemetry::sdk::configuration::GrpcTlsConfiguration>();

  model.endpoint      = "http://localhost:4317";
  model.tls->insecure = false;
  model.timeout       = 12;
  model.compression   = "none";
  model.temporality_preference =
      opentelemetry::sdk::configuration::TemporalityPreference::cumulative;

  auto exporter = builder->Build(&model);
  ASSERT_TRUE(exporter != nullptr);
}

TEST(ExportersOtlpGrpcBuilderInstall, OtlpGrpcLogRecordBuilder)
{
  auto builder = std::make_unique<opentelemetry::exporter::otlp::OtlpGrpcLogRecordBuilder>();
  ASSERT_TRUE(builder != nullptr);

  opentelemetry::sdk::configuration::OtlpGrpcLogRecordExporterConfiguration model;
  model.tls = std::make_unique<opentelemetry::sdk::configuration::GrpcTlsConfiguration>();

  model.endpoint      = "http://localhost:4317";
  model.tls->insecure = false;
  model.timeout       = 12;
  model.compression   = "none";

  auto exporter = builder->Build(&model);
  ASSERT_TRUE(exporter != nullptr);
}
