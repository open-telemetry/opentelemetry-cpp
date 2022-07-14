// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

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

  ASSERT_EQ(0, exporter.GetData()->Get().size());

  std::unique_ptr<Recordable> spandata(new SpanData());
  opentelemetry::nostd::span<std::unique_ptr<Recordable>> batch(&spandata, 1);

  exporter.Export(batch);

  ASSERT_EQ(1, exporter.GetData()->Get().size());

  // Consumes all spans in exporter
  ASSERT_EQ(0, exporter.GetData()->Get().size());
}
