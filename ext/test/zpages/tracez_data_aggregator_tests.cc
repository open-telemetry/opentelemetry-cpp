#include "opentelemetry/nostd/span.h"
#include "opentelemetry/sdk/trace/span_data.h"
#include "opentelemetry/ext/zpages/tracez_data_aggregator.h"
#include "opentelemetry/sdk/trace/tracer.h"


#include <gtest/gtest.h>

using namespace opentelemetry::sdk::trace;
using namespace opentelemetry::ext::zpages;
namespace nostd  = opentelemetry::nostd;
namespace common = opentelemetry::common;
using opentelemetry::core::SteadyTimestamp;

/**
 * A mock exporter that switches a flag once a valid recordable was received.
 */
class MockSpanExporter final : public SpanExporter {
 public:
  MockSpanExporter(std::shared_ptr<bool> span_received,
                   std::shared_ptr<bool> shutdown_called) noexcept
      : span_received_(span_received), shutdown_called_(shutdown_called) {}

  std::unique_ptr<Recordable> MakeRecordable() noexcept override {
    return std::unique_ptr<Recordable>(new SpanData);
  }

  ExportResult Export(
      const opentelemetry::nostd::span<std::unique_ptr<Recordable>> &spans) noexcept override {
    for (auto &span : spans) {
      if (span != nullptr) {
        *span_received_ = true;
      }
    }

    return ExportResult::kSuccess;
  }

  void Shutdown(std::chrono::microseconds timeout = std::chrono::microseconds(0)) noexcept override {
    *shutdown_called_ = true;
  }

 private:
  std::shared_ptr<bool> span_received_;
  std::shared_ptr<bool> shutdown_called_;
};

TEST(TracezDataAggregator, NoSpans)
{
  std::shared_ptr<bool> span_received(new bool(false));
  std::shared_ptr<bool> shutdown_called(new bool(false));
  std::unique_ptr<SpanExporter> exporter(new MockSpanExporter(span_received, shutdown_called));
  std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor(std::move(exporter)));

  auto tracer = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
  auto tracez_data_aggregator (new TracezDataAggregator(processor));
  
  const std::map<std::string, std::unique_ptr<AggregatedInformation>>& data = tracez_data_aggregator->GetAggregatedData();
  ASSERT_EQ(data.size(), 0); 
}

TEST(TracezDataAggregator, SingleRunningSpan)
{
  std::shared_ptr<bool> span_received(new bool(false));
  std::shared_ptr<bool> shutdown_called(new bool(false));
  std::unique_ptr<SpanExporter> exporter(new MockSpanExporter(span_received, shutdown_called));
  std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor(std::move(exporter)));

  auto tracer = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
  auto tracez_data_aggregator (new TracezDataAggregator(processor));
  
  auto span_first  = tracer->StartSpan("span 1");
  const std::map<std::string, std::unique_ptr<AggregatedInformation>>& data = tracez_data_aggregator->GetAggregatedData();
  ASSERT_EQ(data.size(),1);
  ASSERT_TRUE(data.find("span 1") != data.end());
  ASSERT_EQ(data.at("span 1")->num_running_spans, 1); 
}

TEST(TracezDataAggregator, MultipleRunningSpansWithSameName)
{
  std::shared_ptr<bool> span_received(new bool(false));
  std::shared_ptr<bool> shutdown_called(new bool(false));
  std::unique_ptr<SpanExporter> exporter(new MockSpanExporter(span_received, shutdown_called));
  std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor(std::move(exporter)));

  auto tracer = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
  auto tracez_data_aggregator (new TracezDataAggregator(processor));
  
  auto span_first  = tracer->StartSpan("span 1");
  auto span_second = tracer->StartSpan("span 1");
  auto span_third  = tracer->StartSpan("span 1");
  
  const std::map<std::string, std::unique_ptr<AggregatedInformation>>& data = tracez_data_aggregator->GetAggregatedData();
  ASSERT_EQ(data.size(),1);
  ASSERT_TRUE(data.find("span 1") != data.end());
  ASSERT_EQ(data.at("span 1")->num_running_spans, 3); 
}

TEST(TracezDataAggregator, MultipleRunningSpansWithDifferentNames)
{
  std::shared_ptr<bool> span_received(new bool(false));
  std::shared_ptr<bool> shutdown_called(new bool(false));
  std::unique_ptr<SpanExporter> exporter(new MockSpanExporter(span_received, shutdown_called));
  std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor(std::move(exporter)));

  auto tracer = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
  auto tracez_data_aggregator (new TracezDataAggregator(processor));
  
  auto span_first  = tracer->StartSpan("span 1");
  auto span_second = tracer->StartSpan("span 2");
  auto span_third  = tracer->StartSpan("span 3");
  auto span_fourth  = tracer->StartSpan("span 3");
  
  const std::map<std::string, std::unique_ptr<AggregatedInformation>>& data = tracez_data_aggregator->GetAggregatedData();
  ASSERT_EQ(data.size(),3);
  ASSERT_TRUE(data.find("span 1") != data.end());
  ASSERT_EQ(data.at("span 1")->num_running_spans, 1); 
  
  ASSERT_TRUE(data.find("span 2") != data.end());
  ASSERT_EQ(data.at("span 2")->num_running_spans, 1); 
  
  ASSERT_TRUE(data.find("span 3") != data.end());
  ASSERT_EQ(data.at("span 3")->num_running_spans, 2); 
}


TEST(TraceZDataAggregator, SingleErrorSpan)
{
  std::shared_ptr<bool> span_received(new bool(false));
  std::shared_ptr<bool> shutdown_called(new bool(false));
  std::unique_ptr<SpanExporter> exporter(new MockSpanExporter(span_received, shutdown_called));
  std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor(std::move(exporter)));

  auto tracer = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
  auto tracez_data_aggregator (new TracezDataAggregator(processor));
  
  opentelemetry::trace::StartSpanOptions start;
  start.start_steady_time = SteadyTimestamp(std::chrono::nanoseconds(10));

  opentelemetry::trace::EndSpanOptions end;
  end.end_steady_time = SteadyTimestamp(std::chrono::nanoseconds(40));

  tracer->StartSpan("span 1", start)->SetStatus(opentelemetry::trace::CanonicalCode::CANCELLED,"span cancelled");
  
  const std::map<std::string, std::unique_ptr<AggregatedInformation>>& data = tracez_data_aggregator->GetAggregatedData();
  ASSERT_EQ(data.size(),1);
  ASSERT_TRUE(data.find("span 1") != data.end());
  ASSERT_EQ(data.size(),1);
  ASSERT_EQ(data.at("span 1")->num_error_spans, 1); 
  ASSERT_EQ(data.at("span 1")->error_sample_spans.front()->GetDescription(), "span cancelled");
}

TEST(TraceZDataAggregator, MultipleErrorSpanSameName)
{
  std::shared_ptr<bool> span_received(new bool(false));
  std::shared_ptr<bool> shutdown_called(new bool(false));
  std::unique_ptr<SpanExporter> exporter(new MockSpanExporter(span_received, shutdown_called));
  std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor(std::move(exporter)));

  auto tracer = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
  auto tracez_data_aggregator (new TracezDataAggregator(processor));
  
  tracer->StartSpan("span 1")->SetStatus(opentelemetry::trace::CanonicalCode::CANCELLED,"span cancelled");
  tracer->StartSpan("span 1")->SetStatus(opentelemetry::trace::CanonicalCode::UNKNOWN,"span unknown");
  
  const std::map<std::string, std::unique_ptr<AggregatedInformation>>& data = tracez_data_aggregator->GetAggregatedData();
  ASSERT_EQ(data.size(),1);
  ASSERT_TRUE(data.find("span 1") != data.end());
  ASSERT_EQ(data.size(),1);
  ASSERT_EQ(data.at("span 1")->num_error_spans, 2); 
  ASSERT_EQ(data.at("span 1")->error_sample_spans.size(), 2);
  ASSERT_EQ(data.at("span 1")->error_sample_spans.begin()->get()->GetDescription(), "span cancelled");
  ASSERT_EQ(std::next(data.at("span 1")->error_sample_spans.begin())->get()->GetDescription(), "span unknown");
}

TEST(TraceZDataAggregator, MultipleErrorSpanDifferentName)
{
  std::shared_ptr<bool> span_received(new bool(false));
  std::shared_ptr<bool> shutdown_called(new bool(false));
  std::unique_ptr<SpanExporter> exporter(new MockSpanExporter(span_received, shutdown_called));
  std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor(std::move(exporter)));

  auto tracer = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
  auto tracez_data_aggregator (new TracezDataAggregator(processor));
  
  tracer->StartSpan("span 1")->SetStatus(opentelemetry::trace::CanonicalCode::CANCELLED,"span cancelled");
  tracer->StartSpan("span 2")->SetStatus(opentelemetry::trace::CanonicalCode::UNKNOWN,"span unknown");
  tracer->StartSpan("span 3")->SetStatus(opentelemetry::trace::CanonicalCode::INVALID_ARGUMENT,"span argument invalid");
  
  const std::map<std::string, std::unique_ptr<AggregatedInformation>>& data = tracez_data_aggregator->GetAggregatedData();
  ASSERT_EQ(data.size(),3);
  ASSERT_TRUE(data.find("span 1") != data.end());
  ASSERT_TRUE(data.find("span 2") != data.end());
  ASSERT_TRUE(data.find("span 3") != data.end());
  
  ASSERT_EQ(data.at("span 1")->num_error_spans, 1); 
  ASSERT_EQ(data.at("span 1")->error_sample_spans.begin()->get()->GetDescription(), "span cancelled");
  ASSERT_EQ(data.at("span 2")->num_error_spans, 1); 
  ASSERT_EQ(data.at("span 2")->error_sample_spans.begin()->get()->GetDescription(), "span unknown");
  ASSERT_EQ(data.at("span 3")->num_error_spans, 1); 
  ASSERT_EQ(data.at("span 3")->error_sample_spans.begin()->get()->GetDescription(), "span argument invalid");
}

TEST(TraceZDataAggregator, ErrorSpansAtCapacity)
{
  std::shared_ptr<bool> span_received(new bool(false));
  std::shared_ptr<bool> shutdown_called(new bool(false));
  std::unique_ptr<SpanExporter> exporter(new MockSpanExporter(span_received, shutdown_called));
  std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor(std::move(exporter)));

  auto tracer = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
  auto tracez_data_aggregator (new TracezDataAggregator(processor));
  
  tracer->StartSpan("span 1")->SetStatus(opentelemetry::trace::CanonicalCode::CANCELLED,"span cancelled");
  tracer->StartSpan("span 1")->SetStatus(opentelemetry::trace::CanonicalCode::UNKNOWN,"span unknown");
  tracer->StartSpan("span 1")->SetStatus(opentelemetry::trace::CanonicalCode::INVALID_ARGUMENT,"span invalid");
  tracer->StartSpan("span 1")->SetStatus(opentelemetry::trace::CanonicalCode::DEADLINE_EXCEEDED,"span deadline exceeded");
  tracer->StartSpan("span 1")->SetStatus(opentelemetry::trace::CanonicalCode::NOT_FOUND,"span not found");
  tracer->StartSpan("span 1")->SetStatus(opentelemetry::trace::CanonicalCode::ALREADY_EXISTS,"span already exists");
  
  const std::map<std::string, std::unique_ptr<AggregatedInformation>>& data = tracez_data_aggregator->GetAggregatedData();
  ASSERT_EQ(data.size(),1);
  ASSERT_TRUE(data.find("span 1") != data.end());
  ASSERT_EQ(data.at("span 1")->num_error_spans, 6);
  ASSERT_EQ(data.at("span 1")->error_sample_spans.size(), 5);
  
  auto error_sample = data.at("span 1")->error_sample_spans.begin();
  ASSERT_EQ(error_sample->get()->GetDescription(), "span unknown");
  error_sample = std::next(error_sample);
  ASSERT_EQ(error_sample->get()->GetDescription(), "span invalid");
  error_sample = std::next(error_sample);
  ASSERT_EQ(error_sample->get()->GetDescription(), "span deadline exceeded");
  error_sample = std::next(error_sample);
  ASSERT_EQ(error_sample->get()->GetDescription(), "span not found");
  error_sample = std::next(error_sample);
  ASSERT_EQ(error_sample->get()->GetDescription(), "span already exists");
}

TEST(TraceZDataAggregator, SingleCompletedSpan)
{
  std::shared_ptr<bool> span_received(new bool(false));
  std::shared_ptr<bool> shutdown_called(new bool(false));
  std::unique_ptr<SpanExporter> exporter(new MockSpanExporter(span_received, shutdown_called));
  std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor(std::move(exporter)));

  auto tracer = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
  auto tracez_data_aggregator (new TracezDataAggregator(processor));
  
  opentelemetry::trace::StartSpanOptions start;
  start.start_steady_time = SteadyTimestamp(std::chrono::nanoseconds(10));

  opentelemetry::trace::EndSpanOptions end;
  end.end_steady_time = SteadyTimestamp(std::chrono::nanoseconds(40));

  tracer->StartSpan("span 1", start)->End(end);
  
  const std::map<std::string, std::unique_ptr<AggregatedInformation>>& data = tracez_data_aggregator->GetAggregatedData();
  ASSERT_EQ(data.size(), 1);
  ASSERT_TRUE(data.find("span 1") != data.end());
  ASSERT_EQ(data.at("span 1")->num_running_spans, 0); 
  ASSERT_EQ(data.at("span 1")->span_count_per_latency_bucket[0], 1); 
  ASSERT_EQ(data.at("span 1")->latency_sample_spans[0].size(),1);
  ASSERT_EQ(data.at("span 1")->latency_sample_spans[0].front()->GetDuration(),std::chrono::nanoseconds(30));
}


TEST(TraceZDataAggregator, MultipleCompletedSpanSameName)
{
  std::shared_ptr<bool> span_received(new bool(false));
  std::shared_ptr<bool> shutdown_called(new bool(false));
  std::unique_ptr<SpanExporter> exporter(new MockSpanExporter(span_received, shutdown_called));
  std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor(std::move(exporter)));

  auto tracer = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
  auto tracez_data_aggregator (new TracezDataAggregator(processor));
  
  opentelemetry::trace::StartSpanOptions start;
  start.start_steady_time = SteadyTimestamp(std::chrono::nanoseconds(10));

  opentelemetry::trace::EndSpanOptions end;
  end.end_steady_time = SteadyTimestamp(std::chrono::nanoseconds(40));

  tracer->StartSpan("span 1", start)->End(end);
  
  start.start_steady_time = SteadyTimestamp(std::chrono::nanoseconds(1));
  end.end_steady_time = SteadyTimestamp(std::chrono::nanoseconds(100));
  
  tracer->StartSpan("span 1", start)->End(end);
  
  start.start_steady_time = SteadyTimestamp(std::chrono::nanoseconds(1));
  end.end_steady_time = SteadyTimestamp(std::chrono::nanoseconds(10001));
  
  tracer->StartSpan("span 1", start)->End(end);
  
  const std::map<std::string, std::unique_ptr<AggregatedInformation>>& data = tracez_data_aggregator->GetAggregatedData();
  ASSERT_EQ(data.size(),1);
  ASSERT_TRUE(data.find("span 1") != data.end());
  ASSERT_EQ(data.at("span 1")->span_count_per_latency_bucket[0], 2); 
  ASSERT_EQ(data.at("span 1")->latency_sample_spans[0].size(),2);
  ASSERT_EQ(data.at("span 1")->span_count_per_latency_bucket[1], 1); 
  ASSERT_EQ(data.at("span 1")->latency_sample_spans[1].size(),1);
  ASSERT_EQ(data.at("span 1")->latency_sample_spans[0].front()->GetDuration(),std::chrono::nanoseconds(30));
  ASSERT_EQ(data.at("span 1")->latency_sample_spans[1].front()->GetDuration(),std::chrono::nanoseconds(10000));
}


TEST(TraceZDataAggregator, RunningAndCompletedSpan)
{
  std::shared_ptr<bool> span_received(new bool(false));
  std::shared_ptr<bool> shutdown_called(new bool(false));
  std::unique_ptr<SpanExporter> exporter(new MockSpanExporter(span_received, shutdown_called));
  std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor(std::move(exporter)));

  auto tracer = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
  auto tracez_data_aggregator (new TracezDataAggregator(processor));

  opentelemetry::trace::StartSpanOptions start;
  start.start_steady_time = SteadyTimestamp(std::chrono::nanoseconds(5));
  
  auto span_first  = tracer->StartSpan("span 1",start);
  
  start.start_steady_time = SteadyTimestamp(std::chrono::nanoseconds(10));

  opentelemetry::trace::EndSpanOptions end;
  end.end_steady_time = SteadyTimestamp(std::chrono::nanoseconds(40));

  tracer->StartSpan("span 1", start)->End(end);
  
  const std::map<std::string, std::unique_ptr<AggregatedInformation>>& data = tracez_data_aggregator->GetAggregatedData();
  ASSERT_EQ(data.size(),1);
  ASSERT_TRUE(data.find("span 1") != data.end());
  ASSERT_EQ(data.at("span 1")->span_count_per_latency_bucket[0], 1); 
  ASSERT_EQ(data.at("span 1")->latency_sample_spans[0].front()->GetDuration(),std::chrono::nanoseconds(30));
  ASSERT_EQ(data.at("span 1")->num_running_spans,1);
  
  span_first->End(end);
  const std::map<std::string, std::unique_ptr<AggregatedInformation>>& data2 = tracez_data_aggregator->GetAggregatedData();
  ASSERT_EQ(data2.size(),1);
  ASSERT_TRUE(data2.find("span 1") != data2.end());
  ASSERT_EQ(data2.at("span 1")->span_count_per_latency_bucket[0], 2); 
  ASSERT_EQ(data2.at("span 1")->num_running_spans,0);
}

