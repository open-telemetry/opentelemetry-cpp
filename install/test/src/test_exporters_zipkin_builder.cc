// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>

#include <opentelemetry/exporters/zipkin/zipkin_builder.h>

TEST(ExportersZipkinBuilderInstall, ZipkinBuilder)
{
  auto builder = std::make_unique<opentelemetry::exporter::zipkin::ZipkinBuilder>();
  ASSERT_TRUE(builder != nullptr);

  opentelemetry::sdk::configuration::ZipkinSpanExporterConfiguration model;
  model.endpoint = "localhost";

  auto exporter = builder->Build(&model);
  ASSERT_TRUE(exporter != nullptr);
}
