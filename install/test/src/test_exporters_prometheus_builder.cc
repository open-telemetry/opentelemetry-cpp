// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>

#include <opentelemetry/exporters/prometheus/prometheus_pull_builder.h>

TEST(ExportersPrometheusBuilderInstall, PrometheusPullBuilder)
{
  auto builder = std::make_unique<opentelemetry::exporter::metrics::PrometheusPullBuilder>();
  ASSERT_TRUE(builder != nullptr);

  opentelemetry::sdk::configuration::PrometheusPullMetricExporterConfiguration model;
  model.host               = "localhost";
  model.port               = 1234;
  model.without_scope_info = false;

  auto exporter = builder->Build(&model);
  ASSERT_TRUE(exporter != nullptr);
}
