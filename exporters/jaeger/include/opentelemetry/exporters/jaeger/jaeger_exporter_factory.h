// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <opentelemetry/exporters/jaeger/jaeger_exporter_options.h>
#include <opentelemetry/sdk/trace/exporter.h>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace jaeger
{

class JaegerExporterFactory
{
public:
  static std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> Build();
  static std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> Build(
      const JaegerExporterOptions &options);
};

}  // namespace jaeger
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
