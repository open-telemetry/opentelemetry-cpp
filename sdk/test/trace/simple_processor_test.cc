#include "src/trace/simple_processor.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/sdk/trace/span_data.h"

#include <gtest/gtest.h>

using namespace opentelemetry::sdk::trace;

/**
 * A mock exporter that switches a flag once a valid recordable was received.
 */
class MockSpanExporter final : public SpanExporter
{
public:
  MockSpanExporter(std::shared_ptr<bool> span_received,
                   std::shared_ptr<bool> shutdown_called) noexcept
      : span_received_(span_received), shutdown_called_(shutdown_called)
  {}

  std::unique_ptr<Recordable> MakeRecordable() noexcept override
  {
    return std::unique_ptr<Recordable>(new SpanData);
  }

  ExportResult Export(
      opentelemetry::nostd::span<std::unique_ptr<Recordable>> &spans) noexcept override
  {
    for (auto &span : spans)
    {
      if (span != nullptr)
      {
        *span_received_ = true;
      }
    }

    return ExportResult::kSuccess;
  }

  void Shutdown(std::chrono::microseconds timeout = std::chrono::microseconds(0)) noexcept override
  {
    *shutdown_called_ = true;
  }

private:
  std::shared_ptr<bool> span_received_;
  std::shared_ptr<bool> shutdown_called_;
};

TEST(SimpleSpanProcessor, ToMockSpanExporter)
{
  std::shared_ptr<bool> span_received(new bool(false));
  std::shared_ptr<bool> shutdown_called(new bool(false));
  std::unique_ptr<SpanExporter> exporter(new MockSpanExporter(span_received, shutdown_called));
  SimpleSpanProcessor processor(std::move(exporter));

  auto recordable = processor.MakeRecordable();

  processor.OnStart(*recordable);
  ASSERT_EQ(*span_received, false);

  processor.OnEnd(std::move(recordable));
  ASSERT_EQ(*span_received, true);

  processor.Shutdown();
  ASSERT_EQ(*shutdown_called, true);
}
