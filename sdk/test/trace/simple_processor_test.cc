#include "opentelemetry/sdk/trace/simple_processor.h"
#include "../../../exporters/memory/include/opentelemetry/exporters/memory/in_memory_span_exporter.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/sdk/trace/span_data.h"

#include <gtest/gtest.h>

using namespace opentelemetry::sdk::trace;
using opentelemetry::exporter::memory::InMemorySpanExporter;

TEST(SimpleSpanProcessor, ToInMemorySpanExporter)
{
  std::unique_ptr<SpanExporter> exporter(new InMemorySpanExporter());
  SimpleSpanProcessor processor(std::move(exporter));

  auto recordable = processor.MakeRecordable();

  processor.OnStart(*recordable);
  ASSERT_EQ(0, spans_received->size());

  processor.OnEnd(std::move(recordable));
  ASSERT_EQ(1, spans_received->size());

  processor.Shutdown();
  ASSERT_TRUE(*shutdown_called);
}
