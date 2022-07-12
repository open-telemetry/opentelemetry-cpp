// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

/*
  Cripple the build environment on purpose.
  This is to make sure that if protobuf headers
  are included by OtlpGrpcExporterFactory,
  even indirectly, the build will fail.
*/
#define PROTOBUF_VERSION 6666666

#include <gtest/gtest.h>

#include "opentelemetry/exporters/otlp/otlp_grpc_exporter_factory.h"
#include "opentelemetry/exporters/otlp/otlp_grpc_exporter_options.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

TEST(OtlpGrpcExporterFactoryTest, BuildTest)
{
  OtlpGrpcExporterOptions opts;
  opts.endpoint = "localhost:45454";

  std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> exporter =
      OtlpGrpcExporterFactory::Create(opts);

  EXPECT_TRUE(exporter != nullptr);
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
