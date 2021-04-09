// Copyright 2020, OpenTelemetry Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "opentelemetry/exporters/jaeger/recordable.h"
#include "opentelemetry/sdk/trace/simple_processor.h"
#include "opentelemetry/sdk/trace/span_data.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/trace/provider.h"

#include <gtest/gtest.h>

namespace trace    = opentelemetry::trace;
namespace nostd    = opentelemetry::nostd;
namespace sdktrace = opentelemetry::sdk::trace;

using namespace jaegertracing;

TEST(JaegerExporterRecordable, SetIds)
{
  opentelemetry::exporter::jaeger::Recordable rec;

  int64_t trace_id_val[2]    = {0x0000000000000000, 0x1000000000000000};
  int64_t span_id_val        = 0x2000000000000000;
  int64_t parent_span_id_val = 0x3000000000000000;

  const trace::TraceId trace_id{
      nostd::span<uint8_t, 16>(reinterpret_cast<uint8_t *>(trace_id_val), 16)};

  const trace::SpanId span_id(
      nostd::span<uint8_t, 8>(reinterpret_cast<uint8_t *>(&span_id_val), 8));

  const trace::SpanId parent_span_id(
      nostd::span<uint8_t, 8>(reinterpret_cast<uint8_t *>(&parent_span_id_val), 8));

  rec.SetIds(trace_id, span_id, parent_span_id);

  std::unique_ptr<thrift::Span> span{rec.Span()};

  EXPECT_EQ(span->traceIdLow, trace_id_val[0]);
  EXPECT_EQ(span->traceIdHigh, trace_id_val[1]);
  EXPECT_EQ(span->spanId, span_id_val);
  EXPECT_EQ(span->parentSpanId, parent_span_id_val);
}
