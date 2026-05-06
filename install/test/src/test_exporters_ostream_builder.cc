// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>

#include <opentelemetry/exporters/ostream/console_log_record_builder.h>
#include <opentelemetry/exporters/ostream/console_push_metric_builder.h>
#include <opentelemetry/exporters/ostream/console_span_builder.h>

TEST(ExportersOstreamBuilderInstall, ConsoleSpanBuilder)
{
  auto builder = std::make_unique<opentelemetry::exporter::trace::ConsoleSpanBuilder>();
  ASSERT_TRUE(builder != nullptr);
  auto exporter = builder->Build(nullptr);
  ASSERT_TRUE(exporter != nullptr);
}

TEST(ExportersOstreamBuilderInstall, ConsolePushMetricBuilder)
{
  auto builder = std::make_unique<opentelemetry::exporter::metrics::ConsolePushMetricBuilder>();
  ASSERT_TRUE(builder != nullptr);

  opentelemetry::sdk::configuration::ConsolePushMetricExporterConfiguration model;
  model.temporality_preference =
      opentelemetry::sdk::configuration::TemporalityPreference::cumulative;
  model.default_histogram_aggregation =
      opentelemetry::sdk::configuration::DefaultHistogramAggregation::explicit_bucket_histogram;

  auto exporter = builder->Build(&model);
  ASSERT_TRUE(exporter != nullptr);
}

TEST(ExportersOstreamBuilderInstall, ConsoleLogRecordBuilder)
{
  auto builder = std::make_unique<opentelemetry::exporter::logs::ConsoleLogRecordBuilder>();
  ASSERT_TRUE(builder != nullptr);
  auto exporter = builder->Build(nullptr);
  ASSERT_TRUE(exporter != nullptr);
}
