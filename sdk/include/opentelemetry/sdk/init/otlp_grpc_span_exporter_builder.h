// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/sdk/configuration/otlp_grpc_span_exporter_configuration.h"
#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace init
{

class OtlpGrpcSpanExporterBuilder
{
public:
  OtlpGrpcSpanExporterBuilder()                                                    = default;
  OtlpGrpcSpanExporterBuilder(OtlpGrpcSpanExporterBuilder &&)                      = default;
  OtlpGrpcSpanExporterBuilder(const OtlpGrpcSpanExporterBuilder &)                 = default;
  OtlpGrpcSpanExporterBuilder &operator=(OtlpGrpcSpanExporterBuilder &&)           = default;
  OtlpGrpcSpanExporterBuilder &operator=(const OtlpGrpcSpanExporterBuilder &other) = default;
  virtual ~OtlpGrpcSpanExporterBuilder()                                           = default;

  virtual std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> Build(
      const opentelemetry::sdk::configuration::OtlpGrpcSpanExporterConfiguration *model) const = 0;
};

}  // namespace init
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
