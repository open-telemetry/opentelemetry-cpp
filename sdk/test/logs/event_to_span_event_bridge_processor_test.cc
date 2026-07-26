// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <chrono>
#include <memory>
#include <string>
#include <vector>

#include "opentelemetry/exporters/memory/in_memory_span_data.h"
#include "opentelemetry/exporters/memory/in_memory_span_exporter.h"
#include "opentelemetry/sdk/logs/event_to_span_event_bridge_processor.h"
#include "opentelemetry/sdk/logs/read_write_log_record.h"
#include "opentelemetry/sdk/logs/recordable.h"
#include "opentelemetry/sdk/trace/simple_processor.h"
#include "opentelemetry/sdk/trace/tracer.h"
#include "opentelemetry/sdk/trace/tracer_context.h"
#include "opentelemetry/trace/scope.h"
#include "opentelemetry/trace/span.h"
#include "opentelemetry/trace/span_context.h"
#include "opentelemetry/trace/tracer.h"

using namespace opentelemetry::sdk::logs;
using opentelemetry::exporter::memory::InMemorySpanData;
using opentelemetry::exporter::memory::InMemorySpanExporter;

namespace trace_sdk = opentelemetry::sdk::trace;
namespace trace_api = opentelemetry::trace;

namespace
{

std::shared_ptr<trace_api::Tracer> MakeTracer(std::shared_ptr<InMemorySpanData> *out_span_data)
{
  InMemorySpanExporter *exporter = new InMemorySpanExporter();
  *out_span_data                 = exporter->GetData();

  auto processor = std::unique_ptr<trace_sdk::SpanProcessor>(
      new trace_sdk::SimpleSpanProcessor(std::unique_ptr<trace_sdk::SpanExporter>(exporter)));
  std::vector<std::unique_ptr<trace_sdk::SpanProcessor>> processors;
  processors.push_back(std::move(processor));
  auto context = std::make_shared<trace_sdk::TracerContext>(std::move(processors));
  return std::shared_ptr<trace_api::Tracer>(new trace_sdk::Tracer(context));
}

}  // namespace

TEST(EventToSpanEventBridgeProcessorTest, BridgesEventOntoCurrentMatchingSpan)
{
  std::shared_ptr<InMemorySpanData> span_data;
  auto tracer = MakeTracer(&span_data);

  auto span  = tracer->StartSpan("test-span");
  auto scope = tracer->WithActiveSpan(span);

  EventToSpanEventBridgeProcessor processor;
  auto recordable  = processor.MakeRecordable();
  auto *log_record = static_cast<ReadWriteLogRecord *>(recordable.get());

  trace_api::SpanContext span_context = span->GetContext();
  log_record->SetTraceId(span_context.trace_id());
  log_record->SetSpanId(span_context.span_id());
  log_record->SetEventId(0, "my.event");
  log_record->SetAttribute("key1", "value1");

  processor.OnEmit(std::move(recordable));

  span->End();

  auto spans = span_data->GetSpans();
  ASSERT_EQ(1, spans.size());
  ASSERT_EQ(1, spans.at(0)->GetEvents().size());
  EXPECT_EQ("my.event", spans.at(0)->GetEvents().at(0).GetName());
}

TEST(EventToSpanEventBridgeProcessorTest, IgnoresLogRecordWithoutEventName)
{
  std::shared_ptr<InMemorySpanData> span_data;
  auto tracer = MakeTracer(&span_data);

  auto span  = tracer->StartSpan("test-span");
  auto scope = tracer->WithActiveSpan(span);

  EventToSpanEventBridgeProcessor processor;
  auto recordable  = processor.MakeRecordable();
  auto *log_record = static_cast<ReadWriteLogRecord *>(recordable.get());

  trace_api::SpanContext span_context = span->GetContext();
  log_record->SetTraceId(span_context.trace_id());
  log_record->SetSpanId(span_context.span_id());

  processor.OnEmit(std::move(recordable));

  span->End();

  auto spans = span_data->GetSpans();
  ASSERT_EQ(1, spans.size());
  EXPECT_EQ(0, spans.at(0)->GetEvents().size());
}

TEST(EventToSpanEventBridgeProcessorTest, IgnoresEventFromDifferentSpan)
{
  std::shared_ptr<InMemorySpanData> span_data;
  auto tracer = MakeTracer(&span_data);

  auto span       = tracer->StartSpan("test-span");
  auto other_span = tracer->StartSpan("other-span");
  auto scope      = tracer->WithActiveSpan(span);

  EventToSpanEventBridgeProcessor processor;
  auto recordable  = processor.MakeRecordable();
  auto *log_record = static_cast<ReadWriteLogRecord *>(recordable.get());

  // The log record belongs to other_span, not the current active span.
  trace_api::SpanContext other_context = other_span->GetContext();
  log_record->SetTraceId(other_context.trace_id());
  log_record->SetSpanId(other_context.span_id());
  log_record->SetEventId(0, "my.event");

  processor.OnEmit(std::move(recordable));

  span->End();
  other_span->End();

  auto spans = span_data->GetSpans();
  ASSERT_EQ(2, spans.size());
  for (auto &exported_span : spans)
  {
    EXPECT_EQ(0, exported_span->GetEvents().size());
  }
}
