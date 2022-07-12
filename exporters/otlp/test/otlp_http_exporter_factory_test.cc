// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

/*
  Cripple the build environment on purpose.
  This is to make sure that if protobuf headers
  are included by OtlpHttpExporterFactory,
  even indirectly, the build will fail.
*/
#define PROTOBUF_VERSION 6666666

#include <gtest/gtest.h>

#include "opentelemetry/exporters/otlp/otlp_http_exporter_factory.h"
#include "opentelemetry/exporters/otlp/otlp_http_exporter_options.h"

/*
  Make sure OtlpHttpExporterFactory does not leak nlohmann/json.hpp,
  even indirectly.
*/
#ifdef NLOHMANN_JSON_VERSION_MAJOR
#  error "OtlpHttpExporterFactory should not expose nlohmann/json.hpp"
#endif /* NLOHMANN_JSON_VERSION_MAJOR */

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

TEST(OtlpHttpExporterFactoryTest, BuildTest)
{
  OtlpHttpExporterOptions opts;
  opts.url = "localhost:45454";

  std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> exporter =
      OtlpHttpExporterFactory::Create(opts);

  EXPECT_TRUE(exporter != nullptr);
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
