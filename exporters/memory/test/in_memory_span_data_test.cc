// Copyright 2021, OpenTelemetry Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "opentelemetry/exporters/memory/in_memory_span_data.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/sdk/trace/span_data.h"

#include <gtest/gtest.h>

using opentelemetry::exporter::memory::InMemorySpanData;
using opentelemetry::sdk::trace::Recordable;
using opentelemetry::sdk::trace::SpanData;

TEST(InMemorySpanData, AddRecordable)
{
  InMemorySpanData data(100);

  ASSERT_EQ(0, data.GetSpans().size());

  std::unique_ptr<SpanData> spandata(new SpanData());

  data.Add(std::move(spandata));

  // Consumes all spans in exporter
  ASSERT_EQ(1, data.GetSpans().size());

  ASSERT_EQ(0, data.GetSpans().size());
}
