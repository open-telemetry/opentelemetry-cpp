// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <type_traits>

#define max(a, b) (((a) > (b)) ? (a) : (b))
#include "opentelemetry/exporters/otlp/otlp_file_exporter_options.h"
#include "opentelemetry/exporters/otlp/otlp_grpc_exporter_options.h"
#include "opentelemetry/exporters/otlp/otlp_http_exporter_options.h"
#include "opentelemetry/exporters/otlp/otlp_recordable.h"
#undef max

#include <gtest/gtest.h>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

TEST(OtlpWindowsMaxMacroCompileTest, PublicHeadersCompile)
{
  EXPECT_TRUE((std::is_class<OtlpFileExporterOptions>::value));
  EXPECT_TRUE((std::is_class<OtlpGrpcExporterOptions>::value));
  EXPECT_TRUE((std::is_class<OtlpHttpExporterOptions>::value));
  EXPECT_TRUE((std::is_class<OtlpRecordable>::value));
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
