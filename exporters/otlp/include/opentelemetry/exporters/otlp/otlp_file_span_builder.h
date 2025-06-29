// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "opentelemetry/sdk/configuration/otlp_file_span_exporter_configuration.h"
#include "opentelemetry/sdk/init/otlp_file_span_exporter_builder.h"
#include "opentelemetry/sdk/init/registry.h"
#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

class OPENTELEMETRY_EXPORT OtlpFileSpanBuilder
    : public opentelemetry::sdk::init::OtlpFileSpanExporterBuilder
{
public:
  static void Register(opentelemetry::sdk::init::Registry *registry);

  OtlpFileSpanBuilder()           = default;
  ~OtlpFileSpanBuilder() override = default;

  std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> Build(
      const opentelemetry::sdk::configuration::OtlpFileSpanExporterConfiguration *model)
      const override;
};

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
