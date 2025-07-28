// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <memory>
#include <string>
#include <utility>

#include "opentelemetry/exporters/zipkin/zipkin_builder.h"
#include "opentelemetry/exporters/zipkin/zipkin_exporter_factory.h"
#include "opentelemetry/exporters/zipkin/zipkin_exporter_options.h"
#include "opentelemetry/sdk/configuration/registry.h"
#include "opentelemetry/sdk/configuration/zipkin_span_exporter_builder.h"
#include "opentelemetry/sdk/configuration/zipkin_span_exporter_configuration.h"
#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace zipkin
{

void ZipkinBuilder::Register(opentelemetry::sdk::configuration::Registry *registry)
{
  auto builder = std::make_unique<ZipkinBuilder>();
  registry->SetZipkinSpanBuilder(std::move(builder));
}

std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> ZipkinBuilder::Build(
    const opentelemetry::sdk::configuration::ZipkinSpanExporterConfiguration *model) const
{
  ZipkinExporterOptions options(nullptr);

  options.endpoint = model->endpoint;

  return ZipkinExporterFactory::Create(options);
}

}  // namespace zipkin
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
