#include "opentelemetry/sdk/trace/tracer.h"
#include "opentelemetry/sdk/trace/samplers/always_off.h"
#include "opentelemetry/sdk/trace/samplers/always_on.h"
#include "opentelemetry/sdk/trace/samplers/parent_or_else.h"
#include "opentelemetry/sdk/trace/samplers/probability.h"
#include "opentelemetry/sdk/trace/simple_processor.h"
#include "opentelemetry/sdk/trace/span_data.h"

#include <gtest/gtest.h>

using namespace opentelemetry::sdk::trace;
using opentelemetry::core::SteadyTimestamp;
using opentelemetry::core::SystemTimestamp;
namespace nostd  = opentelemetry::nostd;
namespace common = opentelemetry::common;
using opentelemetry::trace::SpanContext;

/**
 * A mock sampler that returns non-empty sampling results attributes.
 */
class MockSampler final : public Sampler
{
public:
  SamplingResult ShouldSample(const SpanContext * /*parent_context*/,
                              trace_api::TraceId /*trace_id*/,
                              nostd::string_view /*name*/,
                              trace_api::SpanKind /*span_kind*/,
                              const trace_api::KeyValueIterable & /*attributes*/) noexcept override
  {
    // Return two pairs of attributes. These attributes should be added to the span attributes
    return {Decision::RECORD_AND_SAMPLE,
            nostd::unique_ptr<const std::map<std::string, opentelemetry::common::AttributeValue>>(
                new const std::map<std::string, opentelemetry::common::AttributeValue>(
                    {{"sampling_attr1", 123}, {"sampling_attr2", "string"}}))};
  }

  std::string GetDescription() const noexcept override { return "MockSampler"; }
};

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

  ExportResult Export(const nostd::span<std::unique_ptr<Recordable>> &recordables) noexcept override
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
std::shared_ptr<opentelemetry::trace::Tracer> initTracer(
    std::shared_ptr<std::vector<std::unique_ptr<SpanData>>> &received)
{
  std::unique_ptr<SpanExporter> exporter(new MockSpanExporter(received));
  auto processor = std::make_shared<SimpleSpanProcessor>(std::move(exporter));
  return std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
}

std::shared_ptr<opentelemetry::trace::Tracer> initTracer(
    std::shared_ptr<std::vector<std::unique_ptr<SpanData>>> &received,
    std::shared_ptr<Sampler> sampler)
{
  std::unique_ptr<SpanExporter> exporter(new MockSpanExporter(received));
  auto processor = std::make_shared<SimpleSpanProcessor>(std::move(exporter));
  return std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor, sampler));
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

TEST(Tracer, StartSpanSampleOn)
{
  // create a tracer with default AlwaysOn sampler.
  std::shared_ptr<std::vector<std::unique_ptr<SpanData>>> spans_received(
      new std::vector<std::unique_ptr<SpanData>>);
  auto tracer_on = initTracer(spans_received);

  tracer_on->StartSpan("span 1")->End();

  ASSERT_EQ(1, spans_received->size());

  auto &span_data = spans_received->at(0);
  ASSERT_LT(std::chrono::nanoseconds(0), span_data->GetStartTime().time_since_epoch());
  ASSERT_LT(std::chrono::nanoseconds(0), span_data->GetDuration());
}

TEST(Tracer, StartSpanSampleOff)
{
  std::shared_ptr<std::vector<std::unique_ptr<SpanData>>> spans_received(
      new std::vector<std::unique_ptr<SpanData>>);
  // create a tracer with a custom AlwaysOff sampler.
  auto tracer_off = initTracer(spans_received, std::make_shared<AlwaysOffSampler>());

  // This span will not be recorded.
  tracer_off->StartSpan("span 2")->End();

  // The span doesn't write any span data because the sampling decision is alway NOT_RECORD.
  ASSERT_EQ(0, spans_received->size());
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

TEST(Tracer, StartSpanWithAttributes)
{
  std::shared_ptr<std::vector<std::unique_ptr<SpanData>>> spans_received_1(
      new std::vector<std::unique_ptr<SpanData>>);
  // The default tracer has empty sampling result attribute
  auto tracer_default = initTracer(spans_received_1);

  {
    tracer_default->StartSpan("span 1", {{"attr1", 314159}, {"attr2", false}, {"attr1", "string"}});

    std::map<std::string, common::AttributeValue> m;
    m["attr3"] = 3.0;
    tracer_default->StartSpan("span 2", m);
  }

  ASSERT_EQ(2, spans_received_1->size());

  auto &span_data = spans_received_1->at(0);
  ASSERT_EQ(2, span_data->GetAttributes().size());
  ASSERT_EQ("string", nostd::get<nostd::string_view>(span_data->GetAttributes().at("attr1")));
  ASSERT_EQ(false, nostd::get<bool>(span_data->GetAttributes().at("attr2")));

  auto &span_data2 = spans_received_1->at(1);
  ASSERT_EQ(1, span_data2->GetAttributes().size());
  ASSERT_EQ(3.0, nostd::get<double>(span_data2->GetAttributes().at("attr3")));

  std::shared_ptr<std::vector<std::unique_ptr<SpanData>>> spans_received_2(
      new std::vector<std::unique_ptr<SpanData>>);
  // The default tracer has two elements in the sampling result attribute
  auto tracer_mock = initTracer(spans_received_2, std::make_shared<MockSampler>());
  {
    tracer_mock->StartSpan("span 3", {{"attr1", 314159}, {"attr2", false}});
  }
  auto &span_data3 = spans_received_2->at(0);
  // Span 3 has a total of four attributes: two from StartSpan and two from the sampler.
  ASSERT_EQ(4, span_data3->GetAttributes().size());
  ASSERT_EQ(314159, nostd::get<int>(span_data3->GetAttributes().at("attr1")));
  ASSERT_EQ(false, nostd::get<bool>(span_data3->GetAttributes().at("attr2")));
  ASSERT_EQ(123, nostd::get<int>(span_data3->GetAttributes().at("sampling_attr1")));
  ASSERT_EQ("string",
            nostd::get<nostd::string_view>(span_data3->GetAttributes().at("sampling_attr2")));
}

TEST(Tracer, GetSampler)
{
  // Create a Tracer with a default AlwaysOnSampler
  std::shared_ptr<SpanProcessor> processor_1(new SimpleSpanProcessor(nullptr));
  std::shared_ptr<Tracer> tracer_on(new Tracer(std::move(processor_1)));

  auto t1 = tracer_on->GetSampler();
  ASSERT_EQ("AlwaysOnSampler", t1->GetDescription());

  // Create a Tracer with a AlwaysOffSampler
  std::shared_ptr<SpanProcessor> processor_2(new SimpleSpanProcessor(nullptr));
  std::shared_ptr<Tracer> tracer_off(
      new Tracer(std::move(processor_2), std::make_shared<AlwaysOffSampler>()));

  auto t2 = tracer_off->GetSampler();
  ASSERT_EQ("AlwaysOffSampler", t2->GetDescription());
}

TEST(Tracer, SpanSetAttribute)
{
  std::shared_ptr<std::vector<std::unique_ptr<SpanData>>> spans_received(
      new std::vector<std::unique_ptr<SpanData>>);
  auto tracer = initTracer(spans_received);

  auto span = tracer->StartSpan("span 1");

  span->SetAttribute("abc", 3.1);

  span->End();
  ASSERT_EQ(1, spans_received->size());
  auto &span_data = spans_received->at(0);
  ASSERT_EQ(3.1, nostd::get<double>(span_data->GetAttributes().at("abc")));
}

TEST(Tracer, TestAlwaysOnSampler)
{
  // Create tracers with AlwaysOn, AlwaysOff, Parent-or-else, and Probability samplers.
  std::shared_ptr<std::vector<std::unique_ptr<SpanData>>> spans_received_on(
      new std::vector<std::unique_ptr<SpanData>>);
  
  auto tracer_on = initTracer(spans_received_on);

  // Testing AlwaysOn sampler.
  // Create two spans for each tracer. Check the exported result.
  auto span_on_1 = tracer_on->StartSpan("span with AlwaysOn sampler 1");
  auto span_on_2 = tracer_on->StartSpan("span with AlwaysOn sampler 2");

  span_on_1->SetAttribute("attr1", 3.1);

  span_on_2->End();
  span_on_1->End();
  ASSERT_EQ(2, spans_received_on->size());
  auto &span_data_on_1 = spans_received_on->at(1);  // span 2 ends first.
  auto &span_data_on_2 = spans_received_on->at(0);
  ASSERT_EQ(3.1, nostd::get<double>(span_data_on_1->GetAttributes().at("attr1")));
  ASSERT_LT(std::chrono::nanoseconds(0), span_data_on_1->GetStartTime().time_since_epoch());
  ASSERT_LT(std::chrono::nanoseconds(0), span_data_on_1->GetDuration());
  ASSERT_LT(std::chrono::nanoseconds(0), span_data_on_2->GetStartTime().time_since_epoch());
  ASSERT_LT(std::chrono::nanoseconds(0), span_data_on_2->GetDuration());
}

TEST(Tracer, TestAlwaysOffSampler)
{
  std::shared_ptr<std::vector<std::unique_ptr<SpanData>>> spans_received_off(
      new std::vector<std::unique_ptr<SpanData>>);
  auto tracer_off = initTracer(spans_received_off, std::make_shared<AlwaysOffSampler>());
  // Testing AlwaysOff sampler
  auto span_off_1 = tracer_off->StartSpan("span with AlwaysOff sampler 1");
  auto span_off_2 = tracer_off->StartSpan("span with AlwaysOff sampler 2");

  span_off_1->SetAttribute("attr1", 3.1);  // Not recorded.

  span_off_2->End();
  span_off_1->End();

  // The tracer export nothing with an AlwaysOff sampler
  ASSERT_EQ(0, spans_received_off->size());
}

TEST(Tracer, TestParentOrElseSampler)
{
  std::shared_ptr<std::vector<std::unique_ptr<SpanData>>> spans_received_parent_on(
      new std::vector<std::unique_ptr<SpanData>>);

  // Current ShouldSample always pass an empty ParentContext,
  // so this sampler will work as an AlwaysOnSampler.
  auto tracer_parent_on =
      initTracer(spans_received_parent_on,
                 std::make_shared<ParentOrElseSampler>(std::make_shared<AlwaysOnSampler>()));

  // Testing ParentOrElse sampler
  auto span_parent_on_1 = tracer_parent_on->StartSpan("span with Parent-or-else sampler 1");
  auto span_parent_on_2 = tracer_parent_on->StartSpan("span with Parent-or-else sampler 2");

  span_parent_on_1->SetAttribute("attr1", 3.1);

  span_parent_on_2->End();
  span_parent_on_1->End();
  ASSERT_EQ(2, spans_received_parent_on->size());
  auto &span_data_parent_on_1 = spans_received_parent_on->at(1);
  auto &span_data_parent_on_2 = spans_received_parent_on->at(0);
  ASSERT_EQ(3.1, nostd::get<double>(span_data_parent_on_1->GetAttributes().at("attr1")));
  ASSERT_LT(std::chrono::nanoseconds(0), span_data_parent_on_1->GetStartTime().time_since_epoch());
  ASSERT_LT(std::chrono::nanoseconds(0), span_data_parent_on_1->GetDuration());
  ASSERT_LT(std::chrono::nanoseconds(0), span_data_parent_on_2->GetStartTime().time_since_epoch());
  ASSERT_LT(std::chrono::nanoseconds(0), span_data_parent_on_2->GetDuration());
}

TEST(Tracer, TestProbabilitySampler)
{
  std::shared_ptr<std::vector<std::unique_ptr<SpanData>>> spans_received_probability_on(
      new std::vector<std::unique_ptr<SpanData>>);
  std::shared_ptr<std::vector<std::unique_ptr<SpanData>>> spans_received_probability_off(
      new std::vector<std::unique_ptr<SpanData>>);

  // Current ShouldSample uses an invalid TraceId.
  // This sampler always return RECORD_AND_SAMPLE at probability 0.01
  auto tracer_probability_on =
      initTracer(spans_received_probability_on, std::make_shared<ProbabilitySampler>(0.01));
  // This sampler always return NOT_SAMPLE at probability 0
  auto tracer_probability_off =
      initTracer(spans_received_probability_off, std::make_shared<ProbabilitySampler>(0));
      
  // Testing Probablity sampler
  auto span_probability_on_1 =
      tracer_probability_on->StartSpan("span with Probability on sampler 1");
  auto span_probability_on_2 =
      tracer_probability_on->StartSpan("span with Probability on sampler 2");

  span_probability_on_1->SetAttribute("attr1", 3.1);

  span_probability_on_2->End();
  span_probability_on_1->End();
  ASSERT_EQ(2, spans_received_probability_on->size());
  auto &span_data_probability_on_1 = spans_received_probability_on->at(1);
  auto &span_data_probability_on_2 = spans_received_probability_on->at(0);
  ASSERT_EQ(3.1, nostd::get<double>(span_data_probability_on_1->GetAttributes().at("attr1")));
  ASSERT_LT(std::chrono::nanoseconds(0),
            span_data_probability_on_1->GetStartTime().time_since_epoch());
  ASSERT_LT(std::chrono::nanoseconds(0), span_data_probability_on_1->GetDuration());
  ASSERT_LT(std::chrono::nanoseconds(0),
            span_data_probability_on_2->GetStartTime().time_since_epoch());
  ASSERT_LT(std::chrono::nanoseconds(0), span_data_probability_on_2->GetDuration());

  // Testing Probablity sampler
  auto span_probability_off_1 =
      tracer_probability_off->StartSpan("span with Probability off sampler 1");
  auto span_probability_off_2 =
      tracer_probability_off->StartSpan("span with Probability off sampler 2");

  span_probability_off_1->SetAttribute("attr1", 3.1);

  span_probability_off_2->End();
  span_probability_off_1->End();
  ASSERT_EQ(0, spans_received_probability_off->size());
}
