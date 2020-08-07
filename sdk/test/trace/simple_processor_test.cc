#include "opentelemetry/sdk/trace/simple_processor.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/sdk/trace/span_data.h"

// Import for CMake
// #include
// "../../../exporters/memory/include/opentelemetry/exporters/memory/in_memory_span_exporter.h"

// Import for Bazel
#include "opentelemetry/exporters/memory/in_memory_span_exporter.h"

#include <gtest/gtest.h>

using namespace opentelemetry::sdk::trace;
using opentelemetry::exporter::memory::InMemorySpanExporter;

using namespace testing;

class SimpleProcessorTestPeer : public ::testing::Test
{
public:
  InMemorySpanExporter GetExporter(SimpleSpanProcessor &processor)
  {
    return *((InMemorySpanExporter *) processor.exporter_.get());
  }
};

TEST_F(SimpleProcessorTestPeer, ToInMemorySpanExporter)
{
  std::unique_ptr<InMemorySpanExporter> exporter_ptr(new InMemorySpanExporter());
  SimpleSpanProcessor processor(std::move(exporter_ptr));

  auto recordable = processor.MakeRecordable();

  processor.OnStart(*recordable);

  InMemorySpanExporter exporter = GetExporter(processor);
  ASSERT_EQ(0, exporter.GetData().get()->GetSpans().size());

  processor.OnEnd(std::move(recordable));

  exporter = GetExporter(processor);
  ASSERT_EQ(1, exporter.GetData().get()->GetSpans().size());

  processor.Shutdown();
}
