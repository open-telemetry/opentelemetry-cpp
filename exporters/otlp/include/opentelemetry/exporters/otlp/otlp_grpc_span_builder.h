// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "opentelemetry/sdk/configuration/otlp_grpc_span_exporter_configuration.h"
#include "opentelemetry/sdk/init/otlp_grpc_span_exporter_builder.h"
#include "opentelemetry/sdk/init/registry.h"
#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

class OPENTELEMETRY_EXPORT OtlpGrpcSpanBuilder
    : public opentelemetry::sdk::init::OtlpGrpcSpanExporterBuilder
{
public:
  static void Register(opentelemetry::sdk::init::Registry *registry);

  OtlpGrpcSpanBuilder()           = default;
  ~OtlpGrpcSpanBuilder() override = default;

  std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> Build(
      const opentelemetry::sdk::configuration::OtlpGrpcSpanExporterConfiguration *model)
      const override;
};

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
