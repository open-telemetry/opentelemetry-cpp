#include <memory>

#include <gtest/gtest.h>
#include "opentelemetry/sdk/trace/fork_aware_span_processor.h"

using namespace opentelemetry::sdk::trace;

class DummySpanProcessor : public ForkAwareSpanProcessor
{

  std::unique_ptr<Recordable> MakeRecordable() noexcept { return nullptr; }

  void OnStart(Recordable &) noexcept {}

  void OnEnd(std::unique_ptr<Recordable> &&) noexcept {}

  void ForceFlush(std::chrono::microseconds) noexcept {}

  void Shutdown(std::chrono::microseconds) noexcept {}
};

TEST(ForkAwareSpanProcessorTests, TestActiveSpanProcessorListUpdate)
{
  std::unique_ptr<DummySpanProcessor> span_processor_1(new DummySpanProcessor);
  std::unique_ptr<DummySpanProcessor> span_processor_2(new DummySpanProcessor);
  std::unique_ptr<DummySpanProcessor> span_processor_3(new DummySpanProcessor);
  span_processor_2.reset(nullptr);
  EXPECT_EQ(ForkAwareSpanProcessor::GetActiveSpanProcessorsForTesting(),
            std::vector<const ForkAwareSpanProcessor *>(
                {span_processor_3.get(), span_processor_1.get()}));
}
