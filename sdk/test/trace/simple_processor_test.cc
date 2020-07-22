#include "opentelemetry/sdk/trace/simple_processor.h"
#include "opentelemetry/exporters/mock/mock_span_exporter.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/sdk/trace/span_data.h"

#include <gtest/gtest.h>

using namespace opentelemetry::sdk::trace;
using opentelemetry::exporter::mock::MockSpanExporter;

TEST(SimpleSpanProcessor, ToMockSpanExporter)
{
  std::shared_ptr<bool> shutdown_called(new bool(false));

  std::shared_ptr<std::vector<std::unique_ptr<SpanData>>> spans_received(
      new std::vector<std::unique_ptr<SpanData>>);

  std::unique_ptr<SpanExporter> exporter(new MockSpanExporter(spans_received, shutdown_called));
  SimpleSpanProcessor processor(std::move(exporter));

  auto recordable = processor.MakeRecordable();

  processor.OnStart(*recordable);
  ASSERT_EQ(0, spans_received->size());

  processor.OnEnd(std::move(recordable));
  ASSERT_EQ(1, spans_received->size());

  processor.Shutdown();
  ASSERT_TRUE(*shutdown_called);
}
