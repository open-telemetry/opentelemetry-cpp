// Import for CMake
// #include "../include/opentelemetry/exporters/memory/in_memory_span_data.h"

// Import for Bazel
#include "opentelemetry/exporters/memory/in_memory_span_exporter.h"

#include "opentelemetry/nostd/span.h"
#include "opentelemetry/sdk/trace/span_data.h"

#include <gtest/gtest.h>

using opentelemetry::exporter::memory::InMemorySpanExporter;
using opentelemetry::sdk::trace::Recordable;
using opentelemetry::sdk::trace::SpanData;

TEST(InMemorySpanExporter, ExportBatch)
{
  InMemorySpanExporter exporter;

  ASSERT_EQ(0, exporter.GetData().get()->GetSpans().size());

  opentelemetry::nostd::span<std::unique_ptr<Recordable>> batch(
      new std::unique_ptr<Recordable>(new SpanData), 1);

  exporter.Export(batch);

  ASSERT_EQ(1, exporter.GetData().get()->GetSpans().size());

  // Consumes all spans in exporter
  ASSERT_EQ(0, exporter.GetData().get()->GetSpans().size());
}
