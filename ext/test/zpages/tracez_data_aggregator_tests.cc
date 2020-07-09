#include "opentelemetry/sdk/trace/recordable.h"
#include "opentelemetry/ext/zpages/tracez_data_aggregator.h"
#include "opentelemetry/ext/zpages/tracez_processor.h"
#include "opentelemetry/sdk/trace/tracer.h"


#include <gtest/gtest.h>

using namespace opentelemetry::sdk::trace;
using namespace opentelemetry::ext::zpages;
namespace nostd  = opentelemetry::nostd;
namespace common = opentelemetry::common;
using opentelemetry::v0::trace::Span;
using opentelemetry::core::SteadyTimestamp;

const std::string span_name1 = "span 1"; 
const std::string span_name2 = "span 2";
const std::string span_name3 = "span 3";

/** Test to check if data aggrgator works as expected when there are no spans **/
TEST(TracezDataAggregator, NoSpans)
{
  //Setup
  std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor());
  auto tracer = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
  auto tracez_data_aggregator (new TracezDataAggregator(processor));
  
  const std::map<std::string, std::unique_ptr<AggregatedSpanData>>& data = tracez_data_aggregator->GetAggregatedData();
  ASSERT_EQ(data.size(), 0); 
}

/** Test to check if data aggrgator works as expected when there are is a single running span **/
TEST(TracezDataAggregator, SingleRunningSpan)
{
  //Setup
  std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor());
  auto tracer = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
  auto tracez_data_aggregator (new TracezDataAggregator(processor));
  
  //Start the span get the data
  auto span_first  = tracer->StartSpan(span_name1);
  const std::map<std::string, std::unique_ptr<AggregatedSpanData>>& data = tracez_data_aggregator->GetAggregatedData();
  
  //Check the size and to see if the running span count is accurate
  ASSERT_EQ(data.size(),1);
  ASSERT_TRUE(data.find(span_name1) != data.end());
  auto& aggregated_data = data.at(span_name1);
  ASSERT_EQ(aggregated_data->num_running_spans, 1);
  
  //Make sure the rest of the data is not affected
  for(auto& latency_sample: aggregated_data->latency_sample_spans)ASSERT_TRUE(latency_sample.empty());
  for(auto& completed_span_count: aggregated_data->span_count_per_latency_bucket)ASSERT_EQ(completed_span_count,0);
  ASSERT_EQ(aggregated_data->num_error_spans, 0);
  ASSERT_TRUE(aggregated_data->error_sample_spans.empty());
}

/** Test to check for single completed span **/
TEST(TraceZDataAggregator, SingleCompletedSpan)
{
  //Setup
  std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor());
  auto tracer = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
  auto tracez_data_aggregator (new TracezDataAggregator(processor));
  
  //Start and end the span at a specific time
  opentelemetry::trace::StartSpanOptions start;
  start.start_steady_time = SteadyTimestamp(std::chrono::nanoseconds(10));
  opentelemetry::trace::EndSpanOptions end;
  end.end_steady_time = SteadyTimestamp(std::chrono::nanoseconds(40));
  tracer->StartSpan(span_name1, start)->End(end);
  
  //Get the dat and make sure span name exists in the data
  const std::map<std::string, std::unique_ptr<AggregatedSpanData>>& data = tracez_data_aggregator->GetAggregatedData();
  ASSERT_EQ(data.size(), 1);
  ASSERT_TRUE(data.find(span_name1) != data.end());
  
  //Check if unrelated data is unchanged
  auto& aggregated_data = data.at(span_name1);
  ASSERT_EQ(aggregated_data->num_running_spans, 0);
  ASSERT_EQ(aggregated_data->num_error_spans,0);
  ASSERT_EQ(aggregated_data->error_sample_spans.size(),0);
  
  for(LatencyBoundaryName boundary = LatencyBoundaryName::k0MicroTo10Micro; boundary != LatencyBoundaryName::k100SecondToMax; ++boundary)
  {
    //Check if the span is correctly updated in the first boundary
    if(boundary == LatencyBoundaryName::k0MicroTo10Micro)
    {
      ASSERT_EQ(aggregated_data->span_count_per_latency_bucket[boundary], 1);
      ASSERT_EQ(aggregated_data->latency_sample_spans[boundary].size(),1);
      ASSERT_EQ(aggregated_data->latency_sample_spans[boundary].front()->GetDuration(),std::chrono::nanoseconds(30));
    }else{
    //Make sure all other information is unchanged
      ASSERT_EQ(aggregated_data->span_count_per_latency_bucket[boundary], 0);
      ASSERT_EQ(aggregated_data->latency_sample_spans[boundary].size(),0);
    }
  }
  
}

/** Test that checks for a single error span **/
TEST(TraceZDataAggregator, SingleErrorSpan)
{
  //Setup
  std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor());
  auto tracer = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
  auto tracez_data_aggregator (new TracezDataAggregator(processor));
  
  tracer->StartSpan(span_name1)->SetStatus(opentelemetry::trace::CanonicalCode::CANCELLED,"span cancelled");
  
  const std::map<std::string, std::unique_ptr<AggregatedSpanData>>& data = tracez_data_aggregator->GetAggregatedData();
  
  //Check to see if span name can be found in aggregation
  ASSERT_EQ(data.size(),1);
  ASSERT_TRUE(data.find(span_name1) != data.end());
  auto& aggregated_data = data.at(span_name1);
  
  //Check to see if error span that is introduced updates the required field
  ASSERT_EQ(aggregated_data -> num_error_spans, 1);
  ASSERT_EQ(aggregated_data -> error_sample_spans.size(),1);
  ASSERT_EQ(aggregated_data -> error_sample_spans.front()->GetName(), span_name1);
  
  //Make sure no other information is affected
  ASSERT_EQ(aggregated_data->num_running_spans, 0);
  for(auto& latency_sample: aggregated_data->latency_sample_spans)ASSERT_TRUE(latency_sample.empty());
  for(auto& completed_span_count: aggregated_data->span_count_per_latency_bucket)ASSERT_EQ(completed_span_count,0);
  
}


/** Test to check if completed multiple running spans behaves as expected**/ 
TEST(TracezDataAggregator, MultipleRunningSpans)
{
  std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor());

  auto tracer = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
  auto tracez_data_aggregator (new TracezDataAggregator(processor));
  
  //A span name and the number of running spans to start with that span name
  std::unordered_map<std::string, int> running_span_name_to_count({
    {span_name1, 1},
    {span_name2, 2},
    {span_name3, 3},
  });
  std::vector<nostd::unique_ptr<Span>> running_span_container;
  for(auto span_name: running_span_name_to_count)
  {
    for(int count = 0; count < span_name.second; count++)running_span_container.push_back(tracer->StartSpan(span_name.first));
  }
  
  const std::map<std::string, std::unique_ptr<AggregatedSpanData>>& data = tracez_data_aggregator->GetAggregatedData();
  ASSERT_EQ(data.size(),3);
  
  //Check to see if the running span counts were updated correctly
  for(auto span_name: running_span_name_to_count)
  {
    ASSERT_TRUE(data.find(span_name.first) != data.end());
    ASSERT_EQ(data.at(span_name.first)->num_running_spans, span_name.second);
  }

  //Make sure rest of the data is unchanged
  for(auto& aggregated_data : data)
  {
    std::string span_name = aggregated_data.first;
    for(auto& latency_sample: data.at(span_name)->latency_sample_spans)ASSERT_TRUE(latency_sample.empty());
    for(auto& completed_span_count: data.at(span_name)->span_count_per_latency_bucket)ASSERT_EQ(completed_span_count,0);
    ASSERT_EQ(data.at(span_name)->num_error_spans, 0);
    ASSERT_TRUE(data.at(span_name)->error_sample_spans.empty());
  }
}



TEST(TraceZDataAggregator, AdditionToRunningSpans)
{
  //Setup
  std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor());
  auto tracer = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
  auto tracez_data_aggregator (new TracezDataAggregator(processor));
  
  //Start a span and check the data
  auto span_first = tracer->StartSpan(span_name1);
  const std::map<std::string, std::unique_ptr<AggregatedSpanData>>& temp_data = tracez_data_aggregator->GetAggregatedData();
  ASSERT_EQ(temp_data.size(),1);
  ASSERT_TRUE(temp_data.find(span_name1) != temp_data.end());
  ASSERT_EQ(temp_data.at(span_name1)->num_running_spans, 1);
  
  //Start another span and check to see if there is no double counting of spans
  auto span_second = tracer->StartSpan(span_name1);
  const std::map<std::string, std::unique_ptr<AggregatedSpanData>>& data = tracez_data_aggregator->GetAggregatedData();
  
  ASSERT_EQ(data.size(),1);
  ASSERT_TRUE(data.find(span_name1) != data.end());
  ASSERT_EQ(data.at(span_name1)->num_running_spans, 2);
  
  //Make sure rest of the data is unchanged
  for(auto& latency_sample: data.at(span_name1)->latency_sample_spans)ASSERT_TRUE(latency_sample.empty());
  for(auto& completed_span_count: data.at(span_name1)->span_count_per_latency_bucket)ASSERT_EQ(completed_span_count,0);
  ASSERT_EQ(data.at(span_name1)->num_error_spans, 0);
  ASSERT_TRUE(data.at(span_name1)->error_sample_spans.empty());
}

TEST(TraceZDataAggregator, RemovalOfRunningSpanWhenCompleted)
{
  //Setup
  std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor());
  auto tracer = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
  auto tracez_data_aggregator (new TracezDataAggregator(processor));

  opentelemetry::trace::StartSpanOptions start;
  start.start_steady_time = SteadyTimestamp(std::chrono::nanoseconds(10));
  opentelemetry::trace::EndSpanOptions end;
  end.end_steady_time = SteadyTimestamp(std::chrono::nanoseconds(40));
  
  //Start a span and make sure data is updated
  auto span_first = tracer->StartSpan(span_name1,start);
  const std::map<std::string, std::unique_ptr<AggregatedSpanData>>& temp_data = tracez_data_aggregator->GetAggregatedData();
  ASSERT_EQ(temp_data.size(),1);
  ASSERT_TRUE(temp_data.find(span_name1) != temp_data.end());
  ASSERT_EQ(temp_data.at(span_name1)->num_running_spans, 1);
  
  //End the span and make sure running span is removed and completed span is updated, there should be only one completed span
  span_first->End(end);
  const std::map<std::string, std::unique_ptr<AggregatedSpanData>>& data = tracez_data_aggregator->GetAggregatedData();
  ASSERT_EQ(data.size(),1);
  ASSERT_TRUE(data.find(span_name1) != data.end());
  
  //Check if unrelated data is unchanged
  auto& aggregated_data = data.at(span_name1);
  ASSERT_EQ(aggregated_data->num_running_spans, 0);
  ASSERT_EQ(aggregated_data->num_error_spans,0);
  ASSERT_EQ(aggregated_data->error_sample_spans.size(),0);
  
  for(LatencyBoundaryName boundary = LatencyBoundaryName::k0MicroTo10Micro; boundary != LatencyBoundaryName::k100SecondToMax; ++boundary)
  {
    //Check if the span is correctly updated in the first boundary
    if(boundary == LatencyBoundaryName::k0MicroTo10Micro)
    {
      ASSERT_EQ(aggregated_data->span_count_per_latency_bucket[boundary], 1);
      ASSERT_EQ(aggregated_data->latency_sample_spans[boundary].size(),1);
      ASSERT_EQ(aggregated_data->latency_sample_spans[boundary].front()->GetDuration(),std::chrono::nanoseconds(30));
    }else{
    //Make sure all other information is unchanged
      ASSERT_EQ(aggregated_data->span_count_per_latency_bucket[boundary], 0);
      ASSERT_EQ(aggregated_data->latency_sample_spans[boundary].size(),0);
    }
  }
}

TEST(TraceZDataAggregator, MultipleErrorSpans)
{
  std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor());
  auto tracer = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
  auto tracez_data_aggregator (new TracezDataAggregator(processor));
  
  //Container to store the span names --> error messges for the span name
  std::unordered_map<std::string, std::vector<std::string>> span_name_to_error({
    {span_name1, {"span 1 error"}},
    {span_name2, {"span 2 error 1", "span 2 error 2"}},
    {span_name3, {"span 3 error 1", "span 3 error 2", "span 3 error 3"}}
  });
  
  //Start spans with the error messages based on the map
  for(auto& span_error: span_name_to_error)
  {
    for(auto error_desc: span_error.second)tracer->StartSpan(span_error.first)->SetStatus(opentelemetry::trace::CanonicalCode::CANCELLED,error_desc);
  }

  const std::map<std::string, std::unique_ptr<AggregatedSpanData>>& data = tracez_data_aggregator->GetAggregatedData();
  ASSERT_EQ(data.size(),3);
  
  //Check if error spans were updated correctly for the different span names
  for(auto& span_error: span_name_to_error)
  {
    // First try to find the span name in aggregation, then check the count of the error spans and then check values
    ASSERT_TRUE(data.find(span_error.first) != data.end());
    
    auto& aggregated_data = data.at(span_error.first);
    
    ASSERT_EQ(aggregated_data->num_error_spans, (int)span_error.second.size()); 
    ASSERT_EQ(aggregated_data->error_sample_spans.size(),span_error.second.size());
    
    auto error_sample = aggregated_data->error_sample_spans.begin();
    for(unsigned int idx = 0; idx < span_error.second.size(); idx++)
    {
      ASSERT_EQ(span_error.second[idx], error_sample->get()->GetDescription());
      error_sample = std::next(error_sample);
    }
    std::cout << "\n";
  }
  
  //Check if remaining information remains unaffected
  for(auto& aggregated_data : data)
  {
    std::string span_name = aggregated_data.first;
    ASSERT_EQ(data.at(span_name)->num_running_spans, 0);
    for(auto& latency_sample: data.at(span_name)->latency_sample_spans)ASSERT_TRUE(latency_sample.empty());
    for(auto& completed_span_count: data.at(span_name)->span_count_per_latency_bucket)ASSERT_EQ(completed_span_count,0);
  }
}


TEST(TraceZDataAggregator, ErrorSpansOverCapacity)
{
  //Set up
  std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor());
  auto tracer = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
  auto tracez_data_aggregator (new TracezDataAggregator(processor));
  
  //Create error spans with the descriptions in the vector
  std::vector<std::string> span_error_descriptions = {"error span 1","error span 2","error span 3","error span 4","error span 5","error span 6"};
  for(auto span_error_description: span_error_descriptions) tracer->StartSpan(span_name1)->SetStatus(opentelemetry::trace::CanonicalCode::CANCELLED,span_error_description);
  
  //Fetch data and check if span name is spresent
  const std::map<std::string, std::unique_ptr<AggregatedSpanData>>& data = tracez_data_aggregator->GetAggregatedData();
  ASSERT_EQ(data.size(),1);
  ASSERT_TRUE(data.find(span_name1) != data.end());
  
  // Check if error spans are updated according to spans started
  auto& aggregated_data = data.at(span_name1);
  ASSERT_EQ(aggregated_data->num_error_spans, 6);
  ASSERT_EQ(aggregated_data->error_sample_spans.size(), 5);

  // Check if the latest 5 error spans exist out of the total 6 that were introduced
  auto error_sample = aggregated_data->error_sample_spans.begin();
  for(unsigned int idx = 1; idx < span_error_descriptions.size(); idx++)
  {
    ASSERT_EQ(error_sample->get()->GetDescription(), span_error_descriptions[idx]);
    error_sample = std::next(error_sample);
  }
  
  //Make sure other data is unchaged
  for(auto& latency_sample: aggregated_data->latency_sample_spans)ASSERT_TRUE(latency_sample.empty());
  for(auto& completed_span_count: aggregated_data->span_count_per_latency_bucket)ASSERT_EQ(completed_span_count,0);
  ASSERT_EQ(aggregated_data->num_running_spans, 0);
}


TEST(TraceZDataAggregator, CompletedSampleSpansOverCapacity)
{
  //Setup
  std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor());
  auto tracer = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
  auto tracez_data_aggregator (new TracezDataAggregator(processor));
  
  opentelemetry::trace::StartSpanOptions start;
  opentelemetry::trace::EndSpanOptions end;
  
  //Start and end 6 spans with the same name that fall into the first latency bucket
  std::vector<std::pair<nanoseconds,nanoseconds>> timestamps = {
    make_pair(nanoseconds(1),nanoseconds(1)),
    make_pair(nanoseconds(1),nanoseconds(2)),
    make_pair(nanoseconds(1),nanoseconds(3)),
    make_pair(nanoseconds(1),nanoseconds(4)),
    make_pair(nanoseconds(1),nanoseconds(5)),
    make_pair(nanoseconds(1),nanoseconds(6))
  };
  for(auto timestamp: timestamps)
  {
    start.start_steady_time = SteadyTimestamp(timestamp.first);
    end.end_steady_time = SteadyTimestamp(timestamp.second);
    tracer->StartSpan(span_name1, start)->End(end);
  }
  
  const std::map<std::string, std::unique_ptr<AggregatedSpanData>>& data = tracez_data_aggregator->GetAggregatedData();
  ASSERT_EQ(data.size(),1);
  ASSERT_TRUE(data.find(span_name1) != data.end());
  
  
  auto& aggregated_data = data.at(span_name1);
  
  ASSERT_EQ(aggregated_data->latency_sample_spans[LatencyBoundaryName::k0MicroTo10Micro].size(), 5);
  ASSERT_EQ(aggregated_data->span_count_per_latency_bucket[LatencyBoundaryName::k0MicroTo10Micro],6);
  auto latency_sample = aggregated_data->latency_sample_spans[LatencyBoundaryName::k0MicroTo10Micro].begin();
  for(unsigned int idx = 1; idx < timestamps.size(); idx++)
  {
    ASSERT_EQ(latency_sample->get()->GetDuration().count(), timestamps[idx].second.count()-timestamps[idx].first.count());
    latency_sample = std::next(latency_sample);
  }
  
  //Make sure all other information is unchanged
  for(LatencyBoundaryName boundary = LatencyBoundaryName::k10MicroTo100Micro; boundary != LatencyBoundaryName::k100SecondToMax; ++boundary)
  {
      ASSERT_EQ(aggregated_data->span_count_per_latency_bucket[boundary], 0);
      ASSERT_EQ(aggregated_data->latency_sample_spans[boundary].size(),0);
  }
  ASSERT_EQ(aggregated_data->num_running_spans, 0);
  ASSERT_EQ(aggregated_data->num_error_spans, 0);
  ASSERT_EQ(aggregated_data->error_sample_spans.size(), 0);
  
  
  
}



TEST(TraceZDataAggregator, MultipleCompletedSpan)
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

