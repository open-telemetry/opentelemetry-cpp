// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/memory/in_memory_data.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/sdk/trace/span_data.h"

#include <gtest/gtest.h>

using opentelemetry::exporter::memory::InMemoryData;
using opentelemetry::sdk::trace::Recordable;
using opentelemetry::sdk::trace::SpanData;

TEST(InMemoryData, AddRecordable)
{
  InMemoryData<SpanData> data(100);

  ASSERT_EQ(0, data.Get().size());

  std::unique_ptr<SpanData> spandata(new SpanData());

  data.Add(std::move(spandata));

  // Consumes all spans in exporter
  ASSERT_EQ(1, data.Get().size());

  ASSERT_EQ(0, data.Get().size());
}
