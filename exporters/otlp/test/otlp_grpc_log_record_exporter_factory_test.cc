// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_LOGS_PREVIEW

#  include <gtest/gtest.h>

#  include "opentelemetry/exporters/otlp/otlp_grpc_exporter_options.h"
#  include "opentelemetry/exporters/otlp/otlp_grpc_log_record_exporter_factory.h"

/*
  Make sure OtlpGrpcLogRecordExporterFactory does not require,
  even indirectly, protobuf headers.
*/
#  ifdef GOOGLE_PROTOBUF_VERSION
#    error "protobuf should not be included"
#  endif

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

TEST(OtlpGrpcLogRecordExporterFactoryTest, BuildTest)
{
  OtlpGrpcExporterOptions opts;
  opts.endpoint = "localhost:45454";

  std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> exporter =
      OtlpGrpcLogRecordExporterFactory::Create(opts);

  EXPECT_TRUE(exporter != nullptr);
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE

#endif  // ENABLE_LOGS_PREVIEW
