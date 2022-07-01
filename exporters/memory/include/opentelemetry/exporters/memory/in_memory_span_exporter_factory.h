// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/exporters/memory/in_memory_span_data.h"
#include "opentelemetry/sdk/trace/exporter.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace memory
{

class InMemorySpanExporterFactory
{
public:
  static std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> Build(
      std::shared_ptr<InMemorySpanData> &data);
  static std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> Build(
      std::shared_ptr<InMemorySpanData> &data,
      size_t buffer_size);
};

}  // namespace memory
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
