#include "opentelemetry/sdk/trace/simple_processor.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/sdk/trace/span_data.h"
#include "opentelemetry/exporters/memory/in_memory_span_data.h"
#include "opentelemetry/exporters/memory/in_memory_span_exporter.h"

#include <gtest/gtest.h>

using namespace opentelemetry::sdk::trace;
using opentelemetry::exporter::memory::InMemorySpanData;
using opentelemetry::exporter::memory::InMemorySpanExporter;

TEST(SimpleProcessor, ToInMemorySpanExporter)
{
  std::unique_ptr<InMemorySpanExporter> exporter(new InMemorySpanExporter());
  std::shared_ptr<InMemorySpanData> span_data = exporter.get()->GetData();
  SimpleSpanProcessor processor(std::move(exporter));

  auto recordable = processor.MakeRecordable();

  processor.OnStart(*recordable);

  ASSERT_EQ(0, span_data.get()->GetSpans().size());

  processor.OnEnd(std::move(recordable));

  ASSERT_EQ(1, span_data.get()->GetSpans().size());

  processor.Shutdown();
}
