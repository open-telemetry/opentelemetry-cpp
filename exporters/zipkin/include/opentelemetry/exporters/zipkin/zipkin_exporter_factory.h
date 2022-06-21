// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/nostd/unique_ptr.h"
#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/exporters/zipkin/zipkin_exporter_options.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace zipkin
{

class ZipkinExporterFactory
{
public:
  /**
   * Create a ZipkinExporter using all default options.
   */
  static nostd::unique_ptr<opentelemetry::sdk::trace::SpanExporter> Build();

  /**
   * Create a ZipkinExporter using the given options.
   */
  static nostd::unique_ptr<opentelemetry::sdk::trace::SpanExporter> Build(const ZipkinExporterOptions &options);
};

}  // namespace zipkin
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
