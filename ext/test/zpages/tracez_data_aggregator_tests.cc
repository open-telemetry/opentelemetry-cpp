#include "opentelemetry/sdk/trace/recordable.h"
#include "opentelemetry/ext/zpages/tracez_data_aggregator.h"
#include "opentelemetry/ext/zpages/tracez_processor.h"
#include "opentelemetry/sdk/trace/tracer.h"


#include <gtest/gtest.h>

using namespace opentelemetry::sdk::trace;
using namespace opentelemetry::ext::zpages;
namespace nostd  = opentelemetry::nostd;
namespace common = opentelemetry::common;
using opentelemetry::core::SteadyTimestamp;

const std::string span_name1 = "span 1"; 
const std::string span_name2 = "span 2";
const std::string span_name3 = "span 3";

/** Test to check if data aggrgator works as expected when there are no spans **/
TEST(TracezDataAggregator, NoSpans)
{
  std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor());

  auto tracer = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
  auto tracez_data_aggregator (new TracezDataAggregator(processor));
  
  const std::map<std::string, std::unique_ptr<AggregatedSpanData>>& data = tracez_data_aggregator->GetAggregatedData();
  ASSERT_EQ(data.size(), 0); 
}

/** Test to check if data aggrgator works as expected when there are is a single running span **/
TEST(TracezDataAggregator, SingleRunningSpan)
{
  std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor());

  auto tracer = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
  auto tracez_data_aggregator (new TracezDataAggregator(processor));
  
  auto span_first  = tracer->StartSpan(span_name1);
  
  const std::map<std::string, std::unique_ptr<AggregatedSpanData>>& data = tracez_data_aggregator->GetAggregatedData();
  
  ASSERT_EQ(data.size(),1);
  ASSERT_TRUE(data.find(span_name1) != data.end());
  ASSERT_EQ(data.at(span_name1)->num_running_spans, 1);
  for(auto& latency_sample: data.at(span_name1)->latency_sample_spans)ASSERT_TRUE(latency_sample.empty());
  for(auto& completed_span_count: data.at(span_name1)->span_count_per_latency_bucket)ASSERT_EQ(completed_span_count,0);
  ASSERT_EQ(data.at(span_name1)->num_error_spans, 0);
  ASSERT_TRUE(data.at(span_name1)->error_sample_spans.empty());
}

/** Test that checks for a single error span **/
TEST(TraceZDataAggregator, SingleErrorSpan)
{
  std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor());

  auto tracer = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
  auto tracez_data_aggregator (new TracezDataAggregator(processor));
  
  opentelemetry::trace::StartSpanOptions start;
  start.start_steady_time = SteadyTimestamp(std::chrono::nanoseconds(10));

  opentelemetry::trace::EndSpanOptions end;
  end.end_steady_time = SteadyTimestamp(std::chrono::nanoseconds(40));

  tracer->StartSpan(span_name1, start)->SetStatus(opentelemetry::trace::CanonicalCode::CANCELLED,"span cancelled");
  
  const std::map<std::string, std::unique_ptr<AggregatedSpanData>>& data = tracez_data_aggregator->GetAggregatedData();
  
  ASSERT_EQ(data.size(),1);
  ASSERT_TRUE(data.find(span_name1) != data.end());
  ASSERT_EQ(data.at(span_name1)->num_running_spans, 0);
  for(auto& latency_sample: data.at(span_name1)->latency_sample_spans)ASSERT_TRUE(latency_sample.empty());
  for(auto& completed_span_count: data.at(span_name1)->span_count_per_latency_bucket)ASSERT_EQ(completed_span_count,0);
  ASSERT_EQ(data.at(span_name1)-> num_error_spans, 1);
  ASSERT_EQ(data.at(span_name1)-> error_sample_spans.size(),1);
  ASSERT_EQ(data.at(span_name1) -> error_sample_spans.front()->GetName(), span_name1);
}

/** Test to check for single completed span **/
TEST(TraceZDataAggregator, SingleCompletedSpan)
{
  std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor());

  auto tracer = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
  auto tracez_data_aggregator (new TracezDataAggregator(processor));
  
  opentelemetry::trace::StartSpanOptions start;
  start.start_steady_time = SteadyTimestamp(std::chrono::nanoseconds(10));

  opentelemetry::trace::EndSpanOptions end;
  end.end_steady_time = SteadyTimestamp(std::chrono::nanoseconds(40));

  tracer->StartSpan(span_name1, start)->End(end);
  
  const std::map<std::string, std::unique_ptr<AggregatedSpanData>>& data = tracez_data_aggregator->GetAggregatedData();
  ASSERT_EQ(data.size(), 1);
  ASSERT_TRUE(data.find(span_name1) != data.end());
  ASSERT_EQ(data.at(span_name1)->num_running_spans, 0);
  for(LatencyBoundaryName boundary = LatencyBoundaryName::k0MicroTo10Micro; boundary != LatencyBoundaryName::k100SecondToMax; ++boundary)
  {
    if(boundary == LatencyBoundaryName::k0MicroTo10Micro)
    {
      ASSERT_EQ(data.at(span_name1)->span_count_per_latency_bucket[boundary], 1);
      ASSERT_EQ(data.at(span_name1)->latency_sample_spans[boundary].size(),1);
      ASSERT_EQ(data.at(span_name1)->latency_sample_spans[boundary].front()->GetDuration(),std::chrono::nanoseconds(30));
    }else{
      ASSERT_EQ(data.at(span_name1)->span_count_per_latency_bucket[boundary], 0);
      ASSERT_EQ(data.at(span_name1)->latency_sample_spans[boundary].size(),0);
    }
  }
  ASSERT_EQ(data.at(span_name1)->latency_sample_spans[LatencyBoundaryName::k0MicroTo10Micro].front()->GetDuration(),nanoseconds(30));
}

/** Test to check if completed spans fall in correct boundaries **/ 


TEST(TracezDataAggregator, MultipleRunningSpansWithSameName)
{
  std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor());

  auto tracer = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
  auto tracez_data_aggregator (new TracezDataAggregator(processor));
  
  auto span_first  = tracer->StartSpan(span_name1);
  auto span_second = tracer->StartSpan(span_name1);
  auto span_third  = tracer->StartSpan(span_name1);
  
  const std::map<std::string, std::unique_ptr<AggregatedSpanData>>& data = tracez_data_aggregator->GetAggregatedData();
  
  ASSERT_EQ(data.size(),1);
  ASSERT_TRUE(data.find(span_name1) != data.end());
  ASSERT_EQ(data.at(span_name1)->num_running_spans, 3);
  for(auto& latency_sample: data.at(span_name1)->latency_sample_spans)ASSERT_TRUE(latency_sample.empty());
  for(auto& completed_span_count: data.at(span_name1)->span_count_per_latency_bucket)ASSERT_EQ(completed_span_count,0);
  ASSERT_EQ(data.at(span_name1)->num_error_spans, 0);
  ASSERT_TRUE(data.at(span_name1)->error_sample_spans.empty());
}

TEST(TracezDataAggregator, MultipleRunningSpansWithDifferentNames)
{
  std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor());

  auto tracer = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
  auto tracez_data_aggregator (new TracezDataAggregator(processor));
  
  auto span_first  = tracer->StartSpan(span_name1);
  auto span_second = tracer->StartSpan(span_name2);
  auto span_third  = tracer->StartSpan(span_name3);
  auto span_fourth  = tracer->StartSpan(span_name3);
  
  const std::map<std::string, std::unique_ptr<AggregatedSpanData>>& data = tracez_data_aggregator->GetAggregatedData();
  ASSERT_EQ(data.size(),3);
  ASSERT_TRUE(data.find(span_name1) != data.end());
  ASSERT_TRUE(data.find(span_name2) != data.end());
  ASSERT_TRUE(data.find(span_name3) != data.end());
  
  ASSERT_EQ(data.at(span_name1)->num_running_spans, 1);
  ASSERT_EQ(data.at(span_name2)->num_running_spans, 1);
  ASSERT_EQ(data.at(span_name3)->num_running_spans, 2);
  
  for(auto& latency_sample: data.at(span_name1)->latency_sample_spans)ASSERT_TRUE(latency_sample.empty());
  for(auto& completed_span_count: data.at(span_name1)->span_count_per_latency_bucket)ASSERT_EQ(completed_span_count,0);
  ASSERT_EQ(data.at(span_name1)->num_error_spans, 0);
  ASSERT_TRUE(data.at(span_name1)->error_sample_spans.empty());
}

TEST(TraceZDataAggregator, AdditionToRunningSpans)
{
  std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor());

  auto tracer = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
  auto tracez_data_aggregator (new TracezDataAggregator(processor));

  auto span_first = tracer->StartSpan("span 1");
  
  const std::map<std::string, std::unique_ptr<AggregatedSpanData>>& data = tracez_data_aggregator->GetAggregatedData();
  ASSERT_EQ(data.size(),1);
  ASSERT_TRUE(data.find("span 1") != data.end());
  ASSERT_EQ(data.at("span 1")->num_running_spans,1);
  
  auto span_second = tracer->StartSpan("span 1");
  
  const std::map<std::string, std::unique_ptr<AggregatedSpanData>>& data2 = tracez_data_aggregator->GetAggregatedData();
  ASSERT_EQ(data2.size(),1);
  ASSERT_TRUE(data2.find("span 1") != data2.end());
  ASSERT_EQ(data2.at("span 1")->num_running_spans,2);
}




TEST(TraceZDataAggregator, MultipleErrorSpanSameName)
{
  std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor());

  auto tracer = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
  auto tracez_data_aggregator (new TracezDataAggregator(processor));
  
  tracer->StartSpan("span 1")->SetStatus(opentelemetry::trace::CanonicalCode::CANCELLED,"span cancelled");
  tracer->StartSpan("span 1")->SetStatus(opentelemetry::trace::CanonicalCode::UNKNOWN,"span unknown");
  
  const std::map<std::string, std::unique_ptr<AggregatedSpanData>>& data = tracez_data_aggregator->GetAggregatedData();
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
  std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor());

  auto tracer = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
  auto tracez_data_aggregator (new TracezDataAggregator(processor));
  
  tracer->StartSpan("span 1")->SetStatus(opentelemetry::trace::CanonicalCode::CANCELLED,"span cancelled");
  tracer->StartSpan("span 2")->SetStatus(opentelemetry::trace::CanonicalCode::UNKNOWN,"span unknown");
  tracer->StartSpan("span 3")->SetStatus(opentelemetry::trace::CanonicalCode::INVALID_ARGUMENT,"span argument invalid");
  
  const std::map<std::string, std::unique_ptr<AggregatedSpanData>>& data = tracez_data_aggregator->GetAggregatedData();
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
  std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor());

  auto tracer = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
  auto tracez_data_aggregator (new TracezDataAggregator(processor));
  
  tracer->StartSpan("span 1")->SetStatus(opentelemetry::trace::CanonicalCode::CANCELLED,"span cancelled");
  tracer->StartSpan("span 1")->SetStatus(opentelemetry::trace::CanonicalCode::UNKNOWN,"span unknown");
  tracer->StartSpan("span 1")->SetStatus(opentelemetry::trace::CanonicalCode::INVALID_ARGUMENT,"span invalid");
  tracer->StartSpan("span 1")->SetStatus(opentelemetry::trace::CanonicalCode::DEADLINE_EXCEEDED,"span deadline exceeded");
  tracer->StartSpan("span 1")->SetStatus(opentelemetry::trace::CanonicalCode::NOT_FOUND,"span not found");
  tracer->StartSpan("span 1")->SetStatus(opentelemetry::trace::CanonicalCode::ALREADY_EXISTS,"span already exists");
  
  const std::map<std::string, std::unique_ptr<AggregatedSpanData>>& data = tracez_data_aggregator->GetAggregatedData();
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




TEST(TraceZDataAggregator, MultipleCompletedSpanSameName)
{
  std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor());

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
  
  const std::map<std::string, std::unique_ptr<AggregatedSpanData>>& data = tracez_data_aggregator->GetAggregatedData();
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
  std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor());

  auto tracer = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
  auto tracez_data_aggregator (new TracezDataAggregator(processor));

  opentelemetry::trace::StartSpanOptions start;
  start.start_steady_time = SteadyTimestamp(std::chrono::nanoseconds(5));
  
  auto span_first  = tracer->StartSpan("span 1",start);
  
  start.start_steady_time = SteadyTimestamp(std::chrono::nanoseconds(10));

  opentelemetry::trace::EndSpanOptions end;
  end.end_steady_time = SteadyTimestamp(std::chrono::nanoseconds(40));

  tracer->StartSpan("span 1", start)->End(end);
  
  const std::map<std::string, std::unique_ptr<AggregatedSpanData>>& data = tracez_data_aggregator->GetAggregatedData();
  ASSERT_EQ(data.size(),1);
  ASSERT_TRUE(data.find("span 1") != data.end());
  ASSERT_EQ(data.at("span 1")->span_count_per_latency_bucket[0], 1); 
  ASSERT_EQ(data.at("span 1")->latency_sample_spans[0].front()->GetDuration(),std::chrono::nanoseconds(30));
  ASSERT_EQ(data.at("span 1")->num_running_spans,1);
  
  span_first->End(end);
  const std::map<std::string, std::unique_ptr<AggregatedSpanData>>& data2 = tracez_data_aggregator->GetAggregatedData();
  ASSERT_EQ(data2.size(),1);
  ASSERT_TRUE(data2.find("span 1") != data2.end());
  ASSERT_EQ(data2.at("span 1")->span_count_per_latency_bucket[0], 2); 
  ASSERT_EQ(data2.at("span 1")->num_running_spans,0);
}

