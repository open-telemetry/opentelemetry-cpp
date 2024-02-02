// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/zipkin/zipkin_exporter_factory.h"
#include "opentelemetry/exporters/zipkin/zipkin_exporter.h"
#include "opentelemetry/exporters/zipkin/zipkin_exporter_options.h"
#include "opentelemetry/exporters/zipkin/zipkin_builder.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace zipkin
{

static ZipkinBuilder singleton;

void ZipkinBuilder::Register(opentelemetry::sdk::init::Registry *registry)
{
  opentelemetry::sdk::init::Registry::SetZipkinBuilder(& singleton);
}

std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> ZipkinBuilder::Build(
const opentelemetry::sdk::configuration::ZipkinSpanExporterConfiguration *model) const
{
  // FIXME: from model
  ZipkinExporterOptions options;
  return ZipkinExporterFactory::Create(options);
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
