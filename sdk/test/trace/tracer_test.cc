#include "opentelemetry/sdk/trace/tracer.h"
#include "opentelemetry/sdk/trace/simple_processor.h"
#include "opentelemetry/sdk/trace/span_data.h"

#include <gtest/gtest.h>

using namespace opentelemetry::sdk::trace;

/**
 * A mock exporter that switches a flag once a valid recordable was received.
 */
class MockSpanExporter final : public SpanExporter
{
public:
  MockSpanExporter(std::shared_ptr<std::vector<std::string>> spans_received) noexcept
      : spans_received_(spans_received)
  {}

  std::unique_ptr<Recordable> MakeRecordable() noexcept override
  {
    return std::unique_ptr<Recordable>(new SpanData);
  }

  ExportResult Export(
      const opentelemetry::nostd::span<std::unique_ptr<Recordable>> &recordables) noexcept override
  {
    for (auto &recordable : recordables)
    {
      auto span = std::unique_ptr<SpanData>(static_cast<SpanData *>(recordable.release()));
      if (span != nullptr)
      {
        spans_received_->push_back(std::string(span->GetName()));
      }
    }

    return ExportResult::kSuccess;
  }

  void Shutdown(std::chrono::microseconds timeout = std::chrono::microseconds(0)) noexcept override
  {}

private:
  std::shared_ptr<std::vector<std::string>> spans_received_;
};

TEST(Tracer, ToMockSpanExporter)
{
  std::shared_ptr<std::vector<std::string>> spans_received(new std::vector<std::string>);
  std::unique_ptr<SpanExporter> exporter(new MockSpanExporter(spans_received));
  std::shared_ptr<SimpleSpanProcessor> processor(new SimpleSpanProcessor(std::move(exporter)));
  std::shared_ptr<Tracer> tracer(new Tracer(processor));

  auto span_first  = tracer->StartSpan("span 1");
  auto span_second = tracer->StartSpan("span 2");

  ASSERT_EQ(0, spans_received->size());

  span_second->End();
  ASSERT_EQ(1, spans_received->size());
  ASSERT_EQ("span 2", spans_received->at(0));

  span_first->End();
  ASSERT_EQ(2, spans_received->size());
  ASSERT_EQ("span 1", spans_received->at(1));
}
