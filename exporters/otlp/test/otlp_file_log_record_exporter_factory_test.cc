// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <memory>

#include "opentelemetry/exporters/otlp/otlp_file_log_record_exporter_factory.h"
#include "opentelemetry/exporters/otlp/otlp_file_log_record_exporter_options.h"
#include "opentelemetry/sdk/logs/exporter.h"
#include "opentelemetry/version.h"

/*
  Make sure OtlpFileExporterFactory does not require,
  even indirectly, nlohmann/json headers.
*/
#ifdef NLOHMANN_JSON_VERSION_MAJOR
#  error "nlohmann/json should not be included"
#endif /* NLOHMANN_JSON_VERSION_MAJOR */

/*
  Make sure OtlpFileExporterFactory does not require,
  even indirectly, protobuf headers.
*/
#ifdef GOOGLE_PROTOBUF_VERSION
#  error "protobuf should not be included"
#endif

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
