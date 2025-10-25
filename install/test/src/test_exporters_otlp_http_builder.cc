// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>

#include "opentelemetry/exporters/otlp/otlp_http_log_record_builder.h"
#include "opentelemetry/exporters/otlp/otlp_http_push_metric_builder.h"
#include "opentelemetry/exporters/otlp/otlp_http_span_builder.h"

TEST(ExportersOtlpHttpBuilderInstall, OtlpHttpSpanBuilder)
{
  auto builder = std::make_unique<opentelemetry::exporter::otlp::OtlpHttpSpanBuilder>();
  ASSERT_TRUE(builder != nullptr);

  opentelemetry::sdk::configuration::OtlpHttpSpanExporterConfiguration model;
  model.endpoint    = "http://localhost:4318";
  model.encoding    = opentelemetry::sdk::configuration::OtlpHttpEncoding::json;
  model.timeout     = 12;
  model.compression = "none";

  auto exporter = builder->Build(&model);
  ASSERT_TRUE(exporter != nullptr);
}

TEST(ExportersOtlpHttpBuilderInstall, OtlpHttpPushMetricBuilder)
{
  auto builder = std::make_unique<opentelemetry::exporter::otlp::OtlpHttpPushMetricBuilder>();
  ASSERT_TRUE(builder != nullptr);

  opentelemetry::sdk::configuration::OtlpHttpPushMetricExporterConfiguration model;
  model.endpoint    = "http://localhost:4318";
  model.encoding    = opentelemetry::sdk::configuration::OtlpHttpEncoding::json;
  model.timeout     = 12;
  model.compression = "none";
  model.temporality_preference =
      opentelemetry::sdk::configuration::TemporalityPreference::cumulative;

  auto exporter = builder->Build(&model);
  ASSERT_TRUE(exporter != nullptr);
}

TEST(ExportersOtlpHttpBuilderInstall, OtlpHttpLogRecordBuilder)
{
  auto builder = std::make_unique<opentelemetry::exporter::otlp::OtlpHttpLogRecordBuilder>();
  ASSERT_TRUE(builder != nullptr);

  opentelemetry::sdk::configuration::OtlpHttpLogRecordExporterConfiguration model;
  model.endpoint    = "http://localhost:4318";
  model.encoding    = opentelemetry::sdk::configuration::OtlpHttpEncoding::json;
  model.timeout     = 12;
  model.compression = "none";

  auto exporter = builder->Build(&model);
  ASSERT_TRUE(exporter != nullptr);
}
