// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <memory>
#include <string>

#include "opentelemetry/exporters/otlp/otlp_file_log_record_exporter_factory.h"
#include "opentelemetry/exporters/otlp/otlp_file_log_record_exporter_options.h"
#include "opentelemetry/sdk/logs/exporter.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

TEST(OtlpFileLogRecordExporterFactoryTest, BuildTest)
{
  OtlpFileLogRecordExporterOptions opts;
  std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> exporter =
      OtlpFileLogRecordExporterFactory::Create(opts);

  EXPECT_TRUE(exporter != nullptr);
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
