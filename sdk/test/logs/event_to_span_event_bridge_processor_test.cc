// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "opentelemetry/context/context.h"
#include "opentelemetry/exporters/memory/in_memory_span_data.h"
#include "opentelemetry/exporters/memory/in_memory_span_exporter.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/sdk/logs/event_to_span_event_bridge_processor.h"
#include "opentelemetry/sdk/logs/read_write_log_record.h"
#include "opentelemetry/sdk/logs/recordable.h"
#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/processor.h"
#include "opentelemetry/sdk/trace/simple_processor.h"
#include "opentelemetry/sdk/trace/span_data.h"
#include "opentelemetry/sdk/trace/tracer.h"
#include "opentelemetry/sdk/trace/tracer_context.h"
#include "opentelemetry/trace/span.h"
#include "opentelemetry/trace/span_context.h"
#include "opentelemetry/trace/span_metadata.h"
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

// The case that motivated OnEmitWithContext: the record belongs to span_a, which is supplied
// explicitly, while an unrelated span_b is ambient. The event must land on span_a and only on
// span_a. Dispatching through OnEmit() (ambient only) cannot satisfy this.
TEST(EventToSpanEventBridgeProcessorTest, BridgesOntoExplicitContextSpanWhileDifferentSpanIsAmbient)
{
  std::shared_ptr<InMemorySpanData> span_data;
  auto tracer = MakeTracer(&span_data);

  auto span_a = tracer->StartSpan("span-a");
  auto span_b = tracer->StartSpan("span-b");

  // span_b is ambient; span_a is only reachable through the explicitly supplied context.
  auto scope = tracer->WithActiveSpan(span_b);

  opentelemetry::context::Context explicit_context =
      opentelemetry::context::Context().SetValue(trace_api::kSpanKey, span_a);

  EventToSpanEventBridgeProcessor processor;
  auto recordable  = processor.MakeRecordable();
  auto *log_record = static_cast<ReadWriteLogRecord *>(recordable.get());

  trace_api::SpanContext span_a_context = span_a->GetContext();
  log_record->SetTraceId(span_a_context.trace_id());
  log_record->SetSpanId(span_a_context.span_id());
  log_record->SetEventId(0, "my.event");
  log_record->SetAttribute("key1", "value1");

  const opentelemetry::nostd::variant<trace_api::SpanContext, opentelemetry::context::Context>
      resolved_context{explicit_context};
  processor.OnEmitWithContext(std::move(recordable), resolved_context);

  span_a->End();
  span_b->End();

  auto spans = span_data->GetSpans();
  ASSERT_EQ(2, spans.size());

  int spans_with_event = 0;
  for (auto &exported_span : spans)
  {
    if (exported_span->GetName() == "span-a")
    {
      ASSERT_EQ(1, exported_span->GetEvents().size());
      EXPECT_EQ("my.event", exported_span->GetEvents().at(0).GetName());
      ++spans_with_event;
    }
    else
    {
      EXPECT_EQ(0, exported_span->GetEvents().size());
    }
  }
  EXPECT_EQ(1, spans_with_event);
}

// An explicitly supplied context whose span does not match the record's ids must not bridge.
TEST(EventToSpanEventBridgeProcessorTest, IgnoresEventWhenExplicitContextSpanDoesNotMatch)
{
  std::shared_ptr<InMemorySpanData> span_data;
  auto tracer = MakeTracer(&span_data);

  auto span_a = tracer->StartSpan("span-a");
  auto span_b = tracer->StartSpan("span-b");

  opentelemetry::context::Context explicit_context =
      opentelemetry::context::Context().SetValue(trace_api::kSpanKey, span_b);

  EventToSpanEventBridgeProcessor processor;
  auto recordable  = processor.MakeRecordable();
  auto *log_record = static_cast<ReadWriteLogRecord *>(recordable.get());

  // Record carries span_a's ids, but span_b is the span in the resolved context.
  trace_api::SpanContext span_a_context = span_a->GetContext();
  log_record->SetTraceId(span_a_context.trace_id());
  log_record->SetSpanId(span_a_context.span_id());
  log_record->SetEventId(0, "my.event");

  const opentelemetry::nostd::variant<trace_api::SpanContext, opentelemetry::context::Context>
      resolved_context{explicit_context};
  processor.OnEmitWithContext(std::move(recordable), resolved_context);

  span_a->End();
  span_b->End();

  auto spans = span_data->GetSpans();
  ASSERT_EQ(2, spans.size());
  for (auto &exported_span : spans)
  {
    EXPECT_EQ(0, exported_span->GetEvents().size());
  }
}

// A bare SpanContext carries only ids, so the ambient span is the only writable span. It must
// still bridge when the ambient span is the one the ids refer to.
TEST(EventToSpanEventBridgeProcessorTest, BridgesOntoAmbientSpanForBareSpanContext)
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

  const opentelemetry::nostd::variant<trace_api::SpanContext, opentelemetry::context::Context>
      resolved_context{span_context};
  processor.OnEmitWithContext(std::move(recordable), resolved_context);

  span->End();

  auto spans = span_data->GetSpans();
  ASSERT_EQ(1, spans.size());
  ASSERT_EQ(1, spans.at(0)->GetEvents().size());
  EXPECT_EQ("my.event", spans.at(0)->GetEvents().at(0).GetName());
}

TEST(EventToSpanEventBridgeProcessorTest, ConsumesResolvedContext)
{
  EventToSpanEventBridgeProcessor processor;
  EXPECT_TRUE(processor.ConsumesResolvedContext());
}
