// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "opentelemetry/sdk/configuration/zipkin_span_exporter_configuration.h"
#include "opentelemetry/sdk/init/registry.h"
#include "opentelemetry/sdk/init/zipkin_span_exporter_builder.h"
#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace zipkin
{

class OPENTELEMETRY_EXPORT ZipkinBuilder
    : public opentelemetry::sdk::init::ZipkinSpanExporterBuilder
{
public:
  static void Register(opentelemetry::sdk::init::Registry *registry);

  ZipkinBuilder()           = default;
  ~ZipkinBuilder() override = default;

  std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> Build(
      const opentelemetry::sdk::configuration::ZipkinSpanExporterConfiguration *model)
      const override;
};

}  // namespace zipkin
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
