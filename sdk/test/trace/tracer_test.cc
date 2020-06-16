#include "opentelemetry/sdk/trace/tracer.h"
#include "opentelemetry/sdk/trace/simple_processor.h"
#include "opentelemetry/sdk/trace/span_data.h"

#include <gtest/gtest.h>

using namespace opentelemetry::sdk::trace;
using opentelemetry::core::SteadyTimestamp;
using opentelemetry::core::SystemTimestamp;

/**
 * A mock exporter that switches a flag once a valid recordable was received.
 */
class MockSpanExporter final : public SpanExporter
{
public:
  MockSpanExporter(std::shared_ptr<std::vector<std::unique_ptr<SpanData>>> spans_received) noexcept
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
        spans_received_->push_back(std::move(span));
      }
    }

    return ExportResult::kSuccess;
  }

  void Shutdown(std::chrono::microseconds timeout = std::chrono::microseconds(0)) noexcept override
  {}

private:
  std::shared_ptr<std::vector<std::unique_ptr<SpanData>>> spans_received_;
};

namespace
{
std::shared_ptr<Tracer> initTracer(
    std::shared_ptr<std::vector<std::unique_ptr<SpanData>>> &received)
{
  std::unique_ptr<SpanExporter> exporter(new MockSpanExporter(received));
  std::shared_ptr<SimpleSpanProcessor> processor(new SimpleSpanProcessor(std::move(exporter)));
  return std::shared_ptr<Tracer>(new Tracer(processor));
}
}  // namespace

TEST(Tracer, ToMockSpanExporter)
{
  std::shared_ptr<std::vector<std::unique_ptr<SpanData>>> spans_received(
      new std::vector<std::unique_ptr<SpanData>>);
  auto tracer = initTracer(spans_received);

  auto span_first  = tracer->StartSpan("span 1");
  auto span_second = tracer->StartSpan("span 2");

  ASSERT_EQ(0, spans_received->size());

  span_second->End();
  ASSERT_EQ(1, spans_received->size());
  ASSERT_EQ("span 2", spans_received->at(0)->GetName());

  span_first->End();
  ASSERT_EQ(2, spans_received->size());
  ASSERT_EQ("span 1", spans_received->at(1)->GetName());
}

TEST(Tracer, StartSpan)
{
  std::shared_ptr<std::vector<std::unique_ptr<SpanData>>> spans_received(
      new std::vector<std::unique_ptr<SpanData>>);
  auto tracer = initTracer(spans_received);

  tracer->StartSpan("span 1")->End();

  ASSERT_EQ(1, spans_received->size());

  auto &span_data = spans_received->at(0);
  ASSERT_LT(std::chrono::nanoseconds(0), span_data->GetStartTime().time_since_epoch());
  ASSERT_LT(std::chrono::nanoseconds(0), span_data->GetDuration());
}

TEST(Tracer, StartSpanWithOptionsTime)
{
  std::shared_ptr<std::vector<std::unique_ptr<SpanData>>> spans_received(
      new std::vector<std::unique_ptr<SpanData>>);
  auto tracer = initTracer(spans_received);

  opentelemetry::trace::StartSpanOptions start;
  start.start_system_time = SystemTimestamp(std::chrono::nanoseconds(300));
  start.start_steady_time = SteadyTimestamp(std::chrono::nanoseconds(10));

  opentelemetry::trace::EndSpanOptions end;
  end.end_steady_time = SteadyTimestamp(std::chrono::nanoseconds(40));

  tracer->StartSpan("span 1", start)->End(end);

  ASSERT_EQ(1, spans_received->size());

  auto &span_data = spans_received->at(0);
  ASSERT_EQ(std::chrono::nanoseconds(300), span_data->GetStartTime().time_since_epoch());
  ASSERT_EQ(std::chrono::nanoseconds(30), span_data->GetDuration());
}

TEST(Tracer, StartSpanWithOptionsAttributes)
{
  std::shared_ptr<std::vector<std::unique_ptr<SpanData>>> spans_received(
      new std::vector<std::unique_ptr<SpanData>>);
  auto tracer = initTracer(spans_received);

  opentelemetry::trace::StartSpanOptions start;
  std::pair<opentelemetry::nostd::string_view, opentelemetry::common::AttributeValue> attributes[] = { 
      {"attr1", 314159},
      {"attr2", false}, 
      {"attr3", "hi"} 
  };
  start.attributes = attributes;

  tracer->StartSpan("span 1", start)->End();

  ASSERT_EQ(1, spans_received->size());

  auto &span_data = spans_received->at(0);
  ASSERT_EQ(3, span_data->GetAttributes().size());
  ASSERT_EQ(314159, opentelemetry::nostd::get<int>(span_data->GetAttributes().at("attr1")));
  ASSERT_EQ(false, opentelemetry::nostd::get<bool>(span_data->GetAttributes().at("attr2")));
  ASSERT_EQ("hi", opentelemetry::nostd::get<opentelemetry::nostd::string_view>(span_data->GetAttributes().at("attr3")));
}
