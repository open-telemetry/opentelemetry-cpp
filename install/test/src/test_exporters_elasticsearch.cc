// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>

#include <opentelemetry/exporters/elasticsearch/es_log_record_exporter.h>

TEST(ExportersElasticSearchInstall, ElasticsearchLogRecordExporter)
{
  auto options  = opentelemetry::exporter::logs::ElasticsearchExporterOptions();
  auto exporter = opentelemetry::exporter::logs::ElasticsearchLogRecordExporter(options);
}