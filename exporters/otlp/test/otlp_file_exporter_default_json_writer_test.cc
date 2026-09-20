// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <memory>

#include "opentelemetry/exporters/otlp/otlp_file_exporter_factory.h"
#include "opentelemetry/exporters/otlp/otlp_file_exporter_options.h"
#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

TEST(OtlpFileExporterFactoryTest, BuildTest)
{
  OtlpFileExporterOptions opts;
  std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> exporter =
      OtlpFileExporterFactory::Create(opts);

  EXPECT_TRUE(exporter != nullptr);
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
