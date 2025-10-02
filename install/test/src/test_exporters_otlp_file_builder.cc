// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>

#include "opentelemetry/exporters/otlp/otlp_file_log_record_builder.h"
#include "opentelemetry/exporters/otlp/otlp_file_push_metric_builder.h"
#include "opentelemetry/exporters/otlp/otlp_file_span_builder.h"

TEST(ExportersOtlpFileBuilderInstall, OtlpFileSpanBuilder)
{
  auto builder = std::make_unique<opentelemetry::exporter::otlp::OtlpFileSpanBuilder>();
  ASSERT_TRUE(builder != nullptr);
  auto exporter = builder->Build(nullptr);
  ASSERT_TRUE(exporter != nullptr);
}

TEST(ExportersOtlpFileBuilderInstall, OtlpFilePushMetricBuilder)
{
  auto builder = std::make_unique<opentelemetry::exporter::otlp::OtlpFilePushMetricBuilder>();
  ASSERT_TRUE(builder != nullptr);

  opentelemetry::sdk::configuration::OtlpFilePushMetricExporterConfiguration model;
  model.temporality_preference =
      opentelemetry::sdk::configuration::TemporalityPreference::cumulative;

  auto exporter = builder->Build(&model);
  ASSERT_TRUE(exporter != nullptr);
}

TEST(ExportersOtlpFileBuilderInstall, OtlpFileLogRecordBuilder)
{
  auto builder = std::make_unique<opentelemetry::exporter::otlp::OtlpFileLogRecordBuilder>();
  ASSERT_TRUE(builder != nullptr);
  auto exporter = builder->Build(nullptr);
  ASSERT_TRUE(exporter != nullptr);
}
